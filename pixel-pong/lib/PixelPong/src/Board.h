#ifndef PONGBOARD_H
#define PONGBOARD_H

#include "Helpers.h"
#include "Ball.h"
#include "Paddle.h"

/**
 * ==================================================================================================================
 * ~                                                  CLASS                                                    
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
 * @brief Class representing the board entity in the game of Pong.
 *    This is the environemnt that the game takes place in.
 */
class Board
{
public:
  /**
   * @brief Class constructor.
   * 
   * @param xDim The X-dimension of the board.
   *    Should be equal to the number of pixels
   *    in the x-dimension of the pixel matrix.
   * @param yDim The Y-dimension of the board.
   *    Should be equal to the number of pixels
   *    in the x-dimension of the pixel matrix.
   * @param ball The ball object that will be used.
   * @param paddle1 One of the paddles to be used.
   * @param paddle2 One of the paddles to be used.
   */
  Board(
      int xDim,
      int yDim,
      Ball &ball,
      Paddle &paddle1,
      Paddle &paddle2);

  /**
   * @brief Check whether a given position is a collision
   *    with the boundary (top & bottom) of the board.
   * 
   * @return Whether or not the position would result in a collision.
   */
  bool checkBoundaryCollision(Position position);

  /**
   * @brief Check whether a given position is a win state
   *     (left & right) of the board.
   * 
   * @return Whether or not the position would result in a win state.
   */
  bool checkWinState(Position position);

  /**
   * _____________ GETTERS
   */

  /**
   * @brief Get the X-dimension of the board in pixels.
   * 
   * @return The pixel X-dimension of the board.
   */
  int getXDim();

  /**
   * @brief Get the X-dimension of the board in pixels.
   * 
   * @return The pixel X-dimension of the board.
   */
  int getYDim();

private:
  /**
   * _____________ MEMEBER VARIABLES
   */
  /**
   * @brief The X-Dimension of the board in pixels.
   */
  int xDim;

  /**
   * @brief The Y-Dimension of the board in pixels.
   */
  int yDim;

  /**
 * @brief The ball to be used.
 */
  Ball &ball;

  /**
 * @brief One of the paddles to be used.
 */
  Paddle &paddle1;

  /**
 * @brief One of the paddles to be used.
 */
  Paddle &paddle2;
  /**
   * _____________ METHODS
   */
};

/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/

#endif // PONGBOARD_H
