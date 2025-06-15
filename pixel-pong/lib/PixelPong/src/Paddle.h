#ifndef PONGPADDLE_H
#define PONGPADDLE_H

#include "Helpers.h"

/**
 * ==================================================================================================================
 * ~                                               STRUCTS                                                      
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
 * @brief Structure for repesenting the HitRegiong dimensions of a Paddle.
 *    Each hit region will result in a different behaviour.
 *    HitRegions or specified as pixels counting from the TOP of the paddle
 *    and the sum should be equal to the size of the paddle.
 *    e.g. for a Paddle of size 3 we could have:
 * 
 *          topSize = 1, middleSize = 1, bottomSize = 1
 * 
 *          Giving a paddle with 3 hit regions.
 *          
 *          OR if we only want the behaviour of the middle seciont.
 *          
 *          topSize = 0, middleSize = 3, bottomSize = 0
 *        
 * 
 *   Regions must be defined in order of desired appearance from top to bottom.
 */
struct HitRegions
{
  int topSize;    /// The size of the top paddle region.
  int middleSize; /// The change in X-position per 'timestep' of the game.
  int bottomSize; /// The change in Y-position per 'timestep' of the game.
};

/**
 * @brief Enum for possible collisions with the paddle.
 */
enum CollisionRegion
{
  NO_COLLISION,
  TOP,
  MIDDLE,
  BOTTOM
};

/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/

/**
 * ==================================================================================================================
 * ~                                                  CLASS                                                    
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
 * @brief Class representing the paddle entity in the game of Pong.
 */
class Paddle
{
public:
  /**
   * @brief Class constructor.
   * 
   * @param size The size of the Paddle in pixels.
   *    i.e. size = 3 is 3 pixels.
   * @param anchor The pixel to be to position the paddle. 
   *    Counted form the TOP of the paddle.
   *    e.g. for a paddle of size 3:
   *         anchor = 0 would mean the top pixel is used when positioning.
   *         anchor = 1 would the center pixel is used when positioning.
   * @param initalPosition The starting position of the Paddle.
   * @param regions The hit regions of the paddle - these can
   *    be used to influence the behaviour of the Ball depending on 
   *    region it hits.
   *    @see Ball::Ball
   *    @see HitRegions
   */
  Paddle(
      int size,
      int anchor,
      Position initialPosition,
      HitRegions regions);

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
  CollisionRegion checkPaddleCollision(Position position);

  /**
   * _____________ GETTERS
   */

  /**
   * @brief Get the Paddle's size.
   */
  int getSize();

  /**
   * @brief Get the Paddle's current anchor point.
   */
  int getAnchorIndex();

  /**
   * @brief Get the Paddle's current Position.
   */
  Position getPosition();

  /**
   * _____________ SETTERS
   */
  /**
   * @brief Set the Paddles Size.
   * 
   * @param newSize The new size of the Paddle.
   */
  void setSze(int newSize);

  /**
   * @brief Set the Paddles Anchor point.
   * 
   * @param newAnchor The new anchor position of the Paddle.
   *    @see Paddle::Paddle
   */
  void setAnchor(int newAnchor);

  /**
   * @brief Set the Paddle's Position.
   * 
   * @param newPosition The new Positon of the Paddle.
   */
  void setPosition(Position newPosition);

  /**
   * @brief Set the Paddle's HitRegions.
   * 
   * @param newHitRegions The new HitRegaions of the Paddle.
   */
  void setHitRegions(HitRegions newHitRegions);

private:
  /**
   * _____________ MEMEBER VARIABLES
   */

  /**
   * @brief The size of the paddle in pixels.
   */
  int size;

  /**
   * @brief The pixel of the Paddle that to be used
   *    for positioning.
   */
  int anchor;

  /**
   * @brief The current position of the Paddle (anchor).
   */
  Position position;

  /**
   * @brief The deined HitRegions of the paddle.
   */
  HitRegions regions;

  /**
   * _____________ METHODS
   */

  /**
   * @brief Get the hit region that a collision occured. Assumes
   *    that a collision will definitely occur so does not check.
   * 
   * @param position The postion of the entity for which
   *    the collision region will be determined.
   * 
   * @return CollisionRegion The collision region.
   *     @see CollisionRegion
   */
  CollisionRegion getCollisionHitRegion(Position position);
};

/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/

#endif // PONGPADDLE_H
