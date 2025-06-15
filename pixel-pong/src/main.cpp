#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Ball.h>
#include <Board.h>
#include <Paddle.h>
#include <PixelPong.h>
#include <ProjectThing.h>
#include <tuple>
/**
 *       DEFINITIONS & DECLARATIONS
 * ===============================
 */
// ====== DEFINITIONS
//_______ Ultrasonic Sensors
// Sensor 1
#define ECHO_PIN1 33
#define TRIGGER_PIN1 15
// Sensor 2
#define ECHO_PIN2 12
#define TRIGGER_PIN2 27
//_______ Buttons
#define PLAY_PAUSE_RESTART_BTN_PIN 21
#define BRIGHTNESS_LOWER_BTN_PIN 17
#define BRIGHTNESS_RAISE_BTN_PIN 16
//_______ LED Matrix
#define LED_MATRIX_PIN 13
#define NUM_PIXELS 64     // Total number of pixels in the entire matrix
#define MIN_BRIGHTNESS 10 // Seems to not come back on if it goes to 0
#define MAX_BRIGHTNESS 250
#define DEFAULT_BRIGHTNESS 25
#define BRIGHTNESS_STEP 15 // Works best when using this value to configure the others to avoid going out of range.
//_______ Game Speed Logic
#define INITIAL_STATE_UPDATE_DELAY 1000        // ms delay between each game state update, mainly governs ball speed
#define MIN_STATE_UPDATE_DELAY 100             // The mimimum allowed ms delay, should not be less than the render delay.
#define STATE_UPDATE_DELAY_REDUCTION_FACTOR 50 // How much the delay is reduced by per ball to paddle collision.
#define RENDER_DELAY 50                        // ms delay between rerendering the scene
//_______ Game Variables
// Board size only seems to work if the dimensions are equal to the number of x and y pixels on the new matrix
#define GAME_BOARD_X 8                  // Size of the game board x dimension in pixels
#define GAME_BOARD_Y 8                  // Size of the game board y dimension in pixels
#define GAME_PADDLE_SIZE 3              // Size of the paddles in pixels
#define GAME_PADDLE_ANCHOR 1            // The pixel that is used to specify anchor positon @see Paddle::Paddle
#define GAME_PADDLE_HIT_REGIONS 1, 1, 1 // The HitRegions for the paddles @see Paddle::HitRegions
//_______ Game Starting States
#define INITIAL_BALL_POSITION 4, 2
#define INITIAL_BALL_VELOCITY -1, 0
#define INITIAL_PADDLE_POSITION1 0, 3
#define INITIAL_PADDLE_POSITION2 7, 3
//_______ Game Controls
#define CONTROL_HEIGHT_LOWER 5     // cm for which any lower value will be classed as the lower state.
#define CONTROL_HEIGHT_INCREMENT 5 // cm value indicated the size of the region corresponding to each paddle position.                            \
                                   // i.e if Ultrasonic sensor height < CONTROL_HEIGHT_LOWER paddle will be in the first state                    \
                                   //     if CONTROL_HEIGHT_LOWER <= Ultrasonic sensor height < CONTROL_HEIGHT_LOWER + n*CONTROL_HEIGHT_INCREMENT \
                                   //     paddle will be in state n.

// ====== DECLARATIONS

//_______ Game Elements
// The pixel matrix
Adafruit_NeoMatrix pixelMatrix(GAME_BOARD_X, GAME_BOARD_Y, LED_MATRIX_PIN,
                               NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                               NEO_GRB + NEO_KHZ800);
// Ball
Ball ball({INITIAL_BALL_POSITION}, {INITIAL_BALL_VELOCITY});
// Paddles
Paddle paddle1(GAME_PADDLE_SIZE,
               GAME_PADDLE_ANCHOR,
               {INITIAL_PADDLE_POSITION1},
               {GAME_PADDLE_HIT_REGIONS});
Paddle paddle2(GAME_PADDLE_SIZE,
               GAME_PADDLE_ANCHOR,
               {INITIAL_PADDLE_POSITION2},
               {GAME_PADDLE_HIT_REGIONS});
// Board
Board board(GAME_BOARD_X, GAME_BOARD_Y, ball, paddle1, paddle2);
// Game Manager
PixelPong pong(pixelMatrix, board, ball, paddle1, paddle2);
//_______ Flags
// These all signal pending requests from the tasks or interrupt
//that are too be acted on in the main loop
volatile bool render = false;
volatile bool updateState = false;
volatile bool gameOver = false;
volatile bool paused = true;
volatile bool showPausedVisual = true;
volatile bool brightnessChanged = false;
volatile bool restart = false;
//_______ Tasks & Interrupts
// - Tasks
static TimerHandle_t gameEngine = NULL; // Handles updating game state, ball position, collistions etc
void updateBoardState(TimerHandle_t xTimer);
static TimerHandle_t renderEngine = NULL; // Handles visual rendering and player controls indirectly
void renderScene(TimerHandle_t xTimer);
// - Interrupts
void IRAM_ATTR playPauseRestart();
void IRAM_ATTR raiseBrightness();
void IRAM_ATTR lowerBrightness();
//_______ Functions
void controlPaddlePosition(Paddle &paddle, int triggerPin, int echoPin, std::vector<int> validPositions);
void renderPausedVisual(int x1, int x2, int yMin, int yMax);
void renderWinVisual(Velocity finalBallVelocity);
void resetGame();
std::vector<int> getValidPaddlePositions(Board &board, Paddle &paddle);
// ______ Variables
int ballDelay = INITIAL_STATE_UPDATE_DELAY;
volatile int ledBrightness = DEFAULT_BRIGHTNESS;
std::vector<int> validPaddlePositions;

/**
 *                            SETUP
 * ===============================
 */
void setup()
{
  Serial.begin(115200);

  // BUTTONS
  // Play/ Pause/ Restart Button
  pinMode(PLAY_PAUSE_RESTART_BTN_PIN, INPUT_PULLUP);
  attachInterrupt(PLAY_PAUSE_RESTART_BTN_PIN, playPauseRestart, FALLING);
  // Brightness down
  pinMode(BRIGHTNESS_LOWER_BTN_PIN, INPUT_PULLUP);
  attachInterrupt(BRIGHTNESS_LOWER_BTN_PIN, lowerBrightness, FALLING);
  // Brightness up
  pinMode(BRIGHTNESS_RAISE_BTN_PIN, INPUT_PULLUP);
  attachInterrupt(BRIGHTNESS_RAISE_BTN_PIN, raiseBrightness, FALLING);
  // ULTRASONIC SENSORS (Controllers)
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIGGER_PIN1, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(TRIGGER_PIN2, OUTPUT);
  // LED MATRIX (Display)
  pixelMatrix.begin();
  pixelMatrix.setBrightness(DEFAULT_BRIGHTNESS);
  pixelMatrix.show();
  // Get valid paddle positionl (can use either as paddles are intended to be the same)
  validPaddlePositions = getValidPaddlePositions(board, paddle1);

  // Create and start Task Timers
  renderEngine = xTimerCreate(
      "renderingEngine",
      RENDER_DELAY / portTICK_PERIOD_MS,
      pdTRUE,
      (void *)0,
      renderScene);
  gameEngine = xTimerCreate(
      "gameEngine",
      INITIAL_STATE_UPDATE_DELAY / portTICK_PERIOD_MS,
      pdTRUE,
      (void *)1,
      updateBoardState);
  xTimerStart(renderEngine, portMAX_DELAY);
  xTimerStart(gameEngine, portMAX_DELAY);
}

/**
 *                            LOOP
 * ===============================
 */
void loop()
{
  if (restart)
  {
    restart = false;
    resetGame();
  }
  // Adjust the brightness if there is a pending change.
  if (brightnessChanged)
  {
    brightnessChanged = false;
    pixelMatrix.setBrightness(ledBrightness);
    pixelMatrix.show();
  }
  // Render the paused visual if necessary
  if (showPausedVisual)
  {
    renderPausedVisual(2, 5, 2, 5);
    showPausedVisual = false;
  }

  // Check whether the game is in a paused state (i.e. no updates)
  if (!paused && !gameOver)
  {
    // Re-render the scene if there is a pending change.
    if (render)
    {
      render = false;
      controlPaddlePosition(paddle1, TRIGGER_PIN1, ECHO_PIN1, validPaddlePositions);
      controlPaddlePosition(paddle2, TRIGGER_PIN2, ECHO_PIN2, validPaddlePositions);
      pong.render(std::make_tuple(BALL_COLOUR_RGB), std::make_tuple(PADDLE1_COLOUR_RBG), std::make_tuple(PADDLE2_COLOUR_RGB));
    }

    // Update game states if there is a pending change.
    if (updateState)
    {
      updateState = false;
      bool ballInWinState = pong.handle();
      if (ballInWinState)
      {
        gameOver = true;
        Serial.println("Game Over!");
        renderWinVisual(ball.getVelocity());
      }
      // Alter the speed of the ball based on number of paddle collisions.
      if (ballDelay > MIN_STATE_UPDATE_DELAY)
      {
        int numCollisions = pong.getPaddleCollisionCount();
        ballDelay = (INITIAL_STATE_UPDATE_DELAY - (STATE_UPDATE_DELAY_REDUCTION_FACTOR * numCollisions));
        if (ballDelay < MIN_STATE_UPDATE_DELAY)
        {
          ballDelay = MIN_STATE_UPDATE_DELAY;
        }
        xTimerChangePeriod(gameEngine, ballDelay / portTICK_PERIOD_MS, portMAX_DELAY);
      }
    }
  }
}

/**
 *                           OTHER
 * ===============================
 */

//======= GAME MANAGEMENT
/**
 * @brief Handles user control of paddles.
 *    Reads the position of the ultrasonic sensor controller 
 *    and updates the paddel accordingly.
 * 
 * @param paddle The paddle to be updated.
 * @param triggerPin The trigger pin number for the ultrasonic sensor controller
 *    corresponding to the given paddle.
 * @param echoPin The echo pin number for the ultrasonic sensor controller
 *    correspinding to the given paddle.
 */
void controlPaddlePosition(Paddle &paddle, int triggerPin, int echoPin, std::vector<int> validPositions)
{
  // Read distance from the Ultrasonic Sensor
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  // Update paddle position accoringly
  int paddleXPosition = paddle.getPosition().x;
  if (distance < CONTROL_HEIGHT_LOWER)
  {
    paddle.setPosition({paddleXPosition, validPositions[0]});
  }
  else if (distance >= CONTROL_HEIGHT_LOWER * validPositions.size())
  {
    paddle.setPosition({paddleXPosition, validPositions[validPositions.size() - 1]});
  }
  else
  {
    int rounded = ((distance + CONTROL_HEIGHT_INCREMENT - 1) / CONTROL_HEIGHT_INCREMENT) * CONTROL_HEIGHT_INCREMENT;
    int positionIndex = (rounded / CONTROL_HEIGHT_INCREMENT) - 1;
    paddle.setPosition({paddleXPosition, validPositions[positionIndex]});
  }
}

/**
 * @brief Rest the game to the starting state.
 */
void resetGame()
{
  pixelMatrix.clear();
  pixelMatrix.show();
  ballDelay = INITIAL_STATE_UPDATE_DELAY;
  ball.setPosition({INITIAL_BALL_POSITION});
  ball.setVelocity({INITIAL_BALL_VELOCITY});
  paddle1.setPosition({INITIAL_PADDLE_POSITION1});
  paddle2.setPosition({INITIAL_PADDLE_POSITION2});
  pong.setCollisionCount(0);
  render = false;
  updateState = false;
  gameOver = false;
  paused = true;
  showPausedVisual = true;
  brightnessChanged = false;
}

// ===== VISUALS
/**
 * @brief Renders two lines (the paused symbol) to 
 *    indicate the game is paused.
 * 
 * @param x1 The X-coordinate of the first line.
 * @param x2 The X-coordinate of the second line.
 * @param yMin The minimum Y-coordinate of the line.
 * @param yMax The maximum Y-coordinate of the line.
 */
void renderPausedVisual(int x1, int x2, int yMin, int yMax)
{
  for (int i = yMin; i <= yMax; i++)
  {
    pixelMatrix.drawPixel(x1, i, pixelMatrix.Color(PAUSE_COLOUR_RGB));
    pixelMatrix.drawPixel(x2, i, pixelMatrix.Color(PAUSE_COLOUR_RGB));
  }
  pixelMatrix.show();
}

/**
 * @brief Renders a win visual indicating which player won.
 * 
 * @param finalBallVelocity The final ball velocity before entering the win state.
 *    Use to determin which side won.
 */
void renderWinVisual(Velocity finalBallVelocity)
{
  // Left side won
  if (finalBallVelocity.x == -1)
  {
    pixelMatrix.writeFillRect(0, 0, GAME_BOARD_X / 2, GAME_BOARD_Y, pixelMatrix.Color(255, 0, 0));
    pixelMatrix.writeFillRect(GAME_BOARD_X / 2, 0, GAME_BOARD_X / 2, GAME_BOARD_Y, pixelMatrix.Color(0, 255, 0));
  }
  // right side won
  if (finalBallVelocity.x == 1)
  {
    pixelMatrix.writeFillRect(0, 0, GAME_BOARD_X / 2, GAME_BOARD_Y, pixelMatrix.Color(0, 255, 0));
    pixelMatrix.writeFillRect(GAME_BOARD_X / 2, 0, GAME_BOARD_X / 2, GAME_BOARD_Y, pixelMatrix.Color(255, 0, 0));
  }
  pixelMatrix.show();
}

// ====== TASKS
/**
 * @brief Task for flagging board state updates.
 */
void updateBoardState(TimerHandle_t xTimer)
{
  updateState = true;
}

/**
 * @brief Task for flagging rendering updates.
 */
void renderScene(TimerHandle_t xTimer)
{
  render = true;
}

// ======= INTERRUPS
/**
 * @brief Interrupt handler for playing, pausing & restarting the game.
 */
void IRAM_ATTR playPauseRestart()
{
  // Debouncing
  static uint32_t playPauseRestartLastMillis = 0;
  if (millis() - playPauseRestartLastMillis > 500)
  {
    playPauseRestartLastMillis = millis();
    if (gameOver)
    {
      restart = true;
    }
    else
    {
      paused = !paused;
      showPausedVisual = !showPausedVisual;
    }
  }
}

/**
 * @brief Interrupt handler for lowering LED brightness.
 */
void IRAM_ATTR lowerBrightness()
{
  // Debouncing
  static uint32_t lowerBrightnessLastMillis = 0;
  if (millis() - lowerBrightnessLastMillis > 250)
  {
    lowerBrightnessLastMillis = millis();
    if (ledBrightness > MIN_BRIGHTNESS)
    {
      ledBrightness -= BRIGHTNESS_STEP;
      brightnessChanged = true;
    }
  }
  Serial.println(ledBrightness);
}

/**
 * @brief Interrupt handler for rasing LED brightness.
 */
void IRAM_ATTR raiseBrightness()
{
  // Debouncing
  static uint32_t raiseBrightnessLastMillis = 0;
  if (millis() - raiseBrightnessLastMillis > 250)
  {
    raiseBrightnessLastMillis = millis();
    if (ledBrightness < MAX_BRIGHTNESS)
    {
      ledBrightness += BRIGHTNESS_STEP;
      brightnessChanged = true;
    }
  }
}

/**
 * @brief Calculate the valid Y coordinate for paddles based on their size,
 *    anchor point and the board size.
 * 
 * @param board The game board.
 * @param paddle A paddle used in the game (assumes both game paddles are the same)
 * 
 * @return A vector of the valid Y coordinates.
 */
std::vector<int> getValidPaddlePositions(Board &board, Paddle &paddle)
{
  int maxY = (board.getYDim() - 1) - (paddle.getSize() - 1 - paddle.getAnchorIndex());
  int minY = paddle.getAnchorIndex();
  int yRange = (maxY - minY) + 1;

  std::vector<int> validePositions(yRange);
  std::generate(validePositions.begin(), validePositions.end(), [n = minY]() mutable { return n++; });

  return validePositions;
}
