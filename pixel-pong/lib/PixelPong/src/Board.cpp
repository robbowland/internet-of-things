#include "Board.h"

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
   * @param xDim The X-dimension of the board.
   *    Should be equal to the number of pixels
   *    in the x-dimension of the pixel matrix.
   * @param yDim The Y-dimension of the board.
   *    Should be equal to the number of pixels
   *    in the x-dimension of the pixel matrix.
   */
Board::Board(
    int xDim,
    int yDim,
    Ball &ball,
    Paddle &paddle1,
    Paddle &paddle2)
    : xDim(xDim), yDim(yDim), ball(ball), paddle1(paddle1), paddle2(paddle2) {}


bool Board::checkBoundaryCollision(Position position)
{
  // Is the position within the horizontal 'play' region i.e not in the winstate regions on either side
  if (position.x >= 0 || position.x <= xDim - 1)
  {
    // If position is outside horizontal play region, indicates boundary collision
    if (position.y > yDim - 1 || position.y < 0)
    {
      return true;
    }
  }
  return false;
}

bool Board::checkWinState(Position position)
{
  // TODO - may need to tweak this for handling corners
  // Is the position within the vertical 'play' region i.e not in the winstate regions on either side
  if (position.y >= 0 || position.y <= yDim - 1)
  {
    // If position is outside horizontal play region, indicates win collision
    if (position.x > xDim - 1 || position.x < 0)
    {
      return true;
    }
  }
  return false;
}

/**
 * _____________ GETTERS
 */

/**
 * @brief Get the X-dimension of the board in pixels.
 * 
 * @return The pixel X-dimension of the board.
 */
int Board::getXDim()
{
  return xDim;
}

/**
 * @brief Get the X-dimension of the board in pixels.
 * 
 * @return The pixel X-dimension of the board.
 */
int Board::getYDim()
{
  return yDim;
}


/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/
