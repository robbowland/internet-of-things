#include "PixelPong.h"
#include <iostream>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <tuple>
/**
 * ==================================================================================================================
 * ~                                                  CLASS                                                    
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
 * >                                PUBLIC
 * ---------------------------------------
*/
/**
 * @brief Class constructor.
 * 
 * @param pixelMatrix The Adafruit_NeoMatrix object that will be used for 
 *    rendering.
 * @param board The pong game board.
 * @param ball The pong ball.
 * @param paddle1 One of the pong paddles.
 * @param paddle2 One of the pong paddles.
 */
PixelPong::PixelPong(
    Adafruit_NeoMatrix &pixelMatrix,
    Board &board,
    Ball &ball,
    Paddle &paddle1,
    Paddle &paddle2)
    : pixelMatrix(pixelMatrix), board(board), ball(ball), paddle1(paddle1), paddle2(paddle2) {}

/**
   * @brief Renders the current game state onto the pixel matrix.
   * 
   * @param ballColour The RBG colour values of the ball.
   * @param paddle1Colour The RBG colour values of paddle 1.
   * @param paddle2Colour The RBG colour values of paddle 2.
   * @note RBG values are from 0 to 255
   */
void PixelPong::render(std::tuple<uint16_t, uint16_t, uint16_t> ballColour, std::tuple<uint16_t, uint16_t, uint16_t> paddle1Colour, std::tuple<uint16_t, uint16_t, uint16_t> paddle2Colour)
{
  pixelMatrix.clear();
  renderBall(ball, ballColour);
  renderPaddle(paddle1, paddle1Colour);
  renderPaddle(paddle2, paddle2Colour);
  pixelMatrix.show();
}

/**
 * @brief Coordinate game elements to handle updating of game state, 
 *    collisions, win states, etc.
 *    Main function of the class, used to advance the game.
 */
bool PixelPong::handle()
{
  // Before any updates
  Position initialBallPosition = ball.getPosition();
  Velocity initialBallVelocity = ball.getVelocity();
  // After updates
  ball.updatePosition();
  Position newBallPosition = ball.getPosition();

  if (board.checkWinState(newBallPosition))
  {
    return true;
  }

  // Check if new location is a collision with bard boundaries
  if (board.checkBoundaryCollision(newBallPosition))
  {
    handleBoundaryCollision(initialBallVelocity);
    // Change position and update to simulate a rebound
    ball.setPosition(initialBallPosition);
    ball.updatePosition();
    // For the edge case that it hits the corner of the paddle, so the paddle collision check can catch it.
    newBallPosition = ball.getPosition();
  }

  // Ball is moving diagonally - need to do some corrections for visuals
  if (initialBallVelocity.y != 0)
  {
    // Check for lateral collision - move ball one step to left for right
    Position lateralBallPosition = Position(initialBallPosition.x + initialBallVelocity.x, initialBallPosition.y);

    // Check for edge collision
    CollisionRegion paddle1CollisionRegion = paddle1.checkPaddleCollision(lateralBallPosition);
    CollisionRegion paddle2CollisionRegion = paddle2.checkPaddleCollision(lateralBallPosition);
    // If this position caused a collision, use this one instead of the ball.updatePostion one.
    // This prevents the ball from 'phasing' to an unexpected position.
    if (paddle1CollisionRegion != NO_COLLISION || paddle2CollisionRegion != NO_COLLISION)
    {
      newBallPosition = lateralBallPosition;
    }
  }
  // Check for collision with either of the paddles
  CollisionRegion paddle1CollisionRegion = paddle1.checkPaddleCollision(newBallPosition);
  CollisionRegion paddle2CollisionRegion = paddle2.checkPaddleCollision(newBallPosition);
  if (paddle1CollisionRegion != NO_COLLISION || paddle2CollisionRegion != NO_COLLISION)
  {
    // If there's been a collision handle it.
    if (paddle1CollisionRegion != NO_COLLISION)
    {
      Serial.println("PADDLE1 COLLISION");
      handlePaddleCollision(initialBallVelocity, paddle1CollisionRegion);
      ball.setPosition(initialBallPosition);
      // Edge case handling
      if (initialBallPosition.y > 0 && initialBallPosition.y < board.getYDim() - 1)
      {
        ball.updatePosition();
      }
      this->paddleCollisionCounter++;
    }
    if (paddle2CollisionRegion != NO_COLLISION)
    {
      Serial.println("PADDLE1 COLLISION");
      handlePaddleCollision(initialBallVelocity, paddle2CollisionRegion);
      ball.setPosition(initialBallPosition);
      // Edge case handling
      if (initialBallPosition.y > 0 && initialBallPosition.y < board.getYDim() - 1)
      {
        ball.updatePosition();
      }
      this->paddleCollisionCounter++;
    }
  }
  return false;
}

/**
 * _____________ GETTERS
 */

/**
 * @brief Get the current count for the nuber of 
 *    times the ball has collided with a paddle.
 * 
 * @return The number of time the ball has collided with a paddle
 */
int PixelPong::getPaddleCollisionCount()
{
  return this->paddleCollisionCounter;
}

/**
 * _____________ SETTERS
 */
/**
 * @brief Manually set the count for the number of
 *    times the ball has collided with a paddle.
 * 
 * @param The new collistion count.
 */
void PixelPong::setCollisionCount(int collisionCount)
{
  this->paddleCollisionCounter = collisionCount;
}
/**
 * <                               PRIVATE
 * ---------------------------------------
*/

/**
 * @brief Handle to ball hitting the (upper & lower) boundaries
 *    of the board. Updates the balls velocity appropriately.
 */
void PixelPong::handleBoundaryCollision(Velocity ballVelocity)
{
  ball.setVelocity(reboundVelocity(ballVelocity, HORIZONTAL));
}

/**
 * @brief Handle the collision of the ball with one of the paddles.
 *    Updates the ball appropidately in accordance to its velocity
 *    and the region of the paddle hit.
 *    Hitting TOP or BOTTOM regions will result in a reandom rebound.
 *    Hitting the MIDDLE region will result in a deterministic rebound.
 * 
 * @param ballVelocity The current velocity of the ball.
 * @param collisionRegion The region of the paddle that the ball collided with.
 */
void PixelPong::handlePaddleCollision(Velocity ballVelocity, CollisionRegion collisionRegion)
{
  switch (collisionRegion)
  {
  case TOP:
    Serial.println("TOP");
    ball.setVelocity(reboundVelocity(ballVelocity, VERTICAL, true));
    break;

  case MIDDLE:
    Serial.println("MIDDLE");
    ball.setVelocity(reboundVelocity(ballVelocity, VERTICAL, false));
    break;

  case BOTTOM:
    Serial.println("BOTTOM");
    ball.setVelocity(reboundVelocity(ballVelocity, VERTICAL, true));
    break;

  case NO_COLLISION:
    break;
  }
}

/**
 * @brief Render the ball on the pixel matrix.
 * 
 * @param The ball to render.
*/
void PixelPong::renderBall(Ball &ball, std::tuple<uint16_t, uint16_t, uint16_t> colour)
{
  pixelMatrix.drawPixel(ball.getPosition().x, ball.getPosition().y, pixelMatrix.Color(std::get<0>(colour), std::get<1>(colour), std::get<2>(colour)));
}

/**
 * @brief Render the paddle on the pixel matrix.
 * 
 * @param The paddle to render.
*/
void PixelPong::renderPaddle(Paddle &paddle, std::tuple<uint16_t, uint16_t, uint16_t> colour)
{
  int paddleYMax = paddle.getPosition().y + (paddle.getSize() - 1 - paddle.getAnchorIndex());
  int paddleYMin = paddleYMax - (paddle.getSize() - 1);

  pixelMatrix.writeFillRect(paddle.getPosition().x, paddleYMin, 1, paddleYMax - paddleYMin + 1, pixelMatrix.Color(std::get<0>(colour), std::get<1>(colour), std::get<2>(colour)));
}
/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/
