#include "Paddle.h"

/**
 * ==================================================================================================================
 * ~                                                  CLASS                                                    
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
   * @brief Class constructor.
   * 
   * @param size The size of the Paddle in pixels.
   *    i.e. size = 3 is 3 pixels.
   * @param anchor The pixel to be to position the paddle. 
   *    Counted form the BOTTOM of the paddle.
   *    e.g. for a paddle of size 3:
   *         anchor = 0 would mean the bottom pixel is used when positioning.
   *         anchor = 1 would the center pixel is used when positioning.
   * @param initalPosition The starting position of the Paddle.
   * @param regions The hit regions of the paddle - these can
   *    be used to influence the behaviour of the Ball depending on 
   *    region it hits.
   *    @see Ball::Ball
   *    @see HitRegions
   */
Paddle::Paddle(
    int size,
    int anchor,
    Position initialPosition,
    HitRegions regions)
    : size(size), anchor(anchor), position(initialPosition), regions(regions) {}

/**
 * @brief Check when or not an entity being in a given position 
 *    would result in a collision with the panel and return
 *    the collision region.
 * 
 * @param position The postion of the entity for which
 *    the collision will be checked.
 * 
 * @return CollisionRegion Whether or not a collision 
 *    occured and the region.
 *    @see CollisionRegion
 */
CollisionRegion Paddle::checkPaddleCollision(Position position)
{
  // Differen X, collision impossible
  if (position.x != this->position.x)
  {
    // No collision
    return NO_COLLISION;
  }

  int paddleYMax = this->position.y + (this->size - 1 - this->anchor);
  int paddleYMin = paddleYMax - (this->size - 1);
  if (position.y <= paddleYMax && position.y >= paddleYMin)
  {
    return getCollisionHitRegion(position);
  }
  // No collision
  return NO_COLLISION;
}

/**
 * _____________ GETTERS
 */
/**
 * @brief Get the Paddle's size.
 */
int Paddle::getSize()
{
  return this->size;
}

/**
 * @brief Get the Paddle's current anchor point.
 */
int Paddle::getAnchorIndex()
{
  return this->anchor;
}

/**
 * @brief Get the Paddle's current Position.
 */
Position Paddle::getPosition()
{
  return this->position;
}

/**
 * _____________ SETTERS
 */

/**
 * @brief Set the Paddles Size.
 * 
 * @param newSize The new size of the Paddle.
 */
void Paddle::setSze(int newSize)
{
  this->size = newSize;
}

/**
 * @brief Set the Paddles Anchor point.
 * 
 * @param newAnchor The new anchor position of the Paddle.
 *    @see Paddle::Paddle
 */
void Paddle::setAnchor(int newAnchor)
{
  this->anchor = newAnchor;
}

/**
 * @brief Set the Paddle's Position.
 * 
 * @param newPosition The new Positon of the Paddle.
 */
void Paddle::setPosition(Position newPosition)
{
  this->position = newPosition;
}

/**
 * <                               PRIVATE
 * ---------------------------------------
*/
/**
 * @brief Get the hit region that a collision occured. Assumes
 *    that a collision will definitely occur so does not check.
 * 
 * @param position The postion of the entity for which
 *    the collision region will be determined.
 * 
 * @return The collision region.
 *    @see Paddle::CollisionRegion
 */
CollisionRegion Paddle::getCollisionHitRegion(Position position)
{
  int paddleYMax = this->position.y + (this->size - 1 - this->anchor);
  if (position.y <= paddleYMax && position.y > paddleYMax - regions.topSize)
  {
    // Top regions
    return TOP;
  }
  else if (position.y <= paddleYMax - regions.topSize && position.y > paddleYMax - (regions.topSize + regions.middleSize))
  {
    // Middle region
    return MIDDLE;
  }
  else
  {
    // Bottom region
    return BOTTOM;
  }
}
/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/
