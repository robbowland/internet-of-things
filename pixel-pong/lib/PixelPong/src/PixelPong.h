#ifndef PONG_H
#define PONG_H

#include "Helpers.h"
#include "Ball.h"
#include "Paddle.h"
#include "Board.h"
#include <Adafruit_NeoMatrix.h>

/**
 * ==================================================================================================================
 * ~                                                  CLASS                                                    
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
 * @brief Main manager class for Pong. Handles rendering and game rules.
 */
class PixelPong
{
public:
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
  PixelPong(
      Adafruit_NeoMatrix &pixelMatrix,
      Board &board,
      Ball &ball,
      Paddle &paddle1,
      Paddle &paddle2);

  /**
   * @brief Renders the current game state onto the pixel matrix.
   * 
   * @param ballColour The RBG colour values of the ball.
   * @param paddle1Colour The RBG colour values of paddle 1.
   * @param paddle2Colour The RBG colour values of paddle 2.
   * @note RBG values are from 0 to 255
   */
  void render(std::tuple<uint16_t, uint16_t, uint16_t> ballColour, std::tuple<uint16_t, uint16_t, uint16_t> paddle1Colour, std::tuple<uint16_t, uint16_t, uint16_t> paddle2Colour);

  /**
   * @brief Coordinate game elements to handle updating of game state, 
   *    collisions, win states, etc.
   *    Main function of the class, used to advance the game.
   */
  bool handle();

  /**
   * _____________ GETTERS
   */

  /**
   * @brief Get the current count for the nuber of 
   *    times the ball has collided with a paddle.
   * 
   * @return The number of time the ball has collided with a paddle
   */
  int getPaddleCollisionCount();

  /**
   * _____________ SETTERS
   */

  /**
   * @brief Manually set the count for the number of
   *    times the ball has collided with a paddle.
   * 
   * @param The new collistion count.
   */
  void setCollisionCount(int collisionCount);

private:
  /**
   * _____________ MEMEBER VARIABLES
   */

  /**
   * @brief The pixel matrix on which rendering will occur.
   */
  Adafruit_NeoMatrix &pixelMatrix;

  /**
   * @brief The pong board.
   */
  Board &board;

  /**
   * @brief The pong ball.
   */
  Ball &ball;

  /**
   * @brief One of the pong paddles.
   */
  Paddle &paddle1;

  /**
   * @brief One of the pong paddles.
   */
  Paddle &paddle2;

  /**
   * @brief The number of times the ball has collided with a paddle.
   */
  int paddleCollisionCounter;

  /**
   * _____________ METHODS
   */

  /**
   * @brief Render the ball on the pixel matrix.
   * 
   * @param The ball to render.
  */
  void renderBall(Ball &ball, std::tuple<uint16_t, uint16_t, uint16_t> colour);

  /**
   * @brief Render the paddle on the pixel matrix.
   * 
   * @param The paddle to render.
  */
  void renderPaddle(Paddle &paddle, std::tuple<uint16_t, uint16_t, uint16_t> colour);

  /**
   * @brief Handle to ball hitting the (upper & lower) boundaries
   *    of the board. Updates the balls velocity appropriately.
   */
  void handleBoundaryCollision(Velocity ballVelocity);

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
  void handlePaddleCollision(Velocity ballVelocity, CollisionRegion collisionRegion);
};

/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/

#endif // PONG_H
