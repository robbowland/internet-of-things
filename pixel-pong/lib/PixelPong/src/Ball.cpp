#include "Ball.h"

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
 * @param initalPosition The starting position of the Ball.
 * @param initalVelocity The starting velocity of the Ball.
 */
Ball::Ball(Position initialPosition, Velocity initialVelocity)
    : position(initialPosition),
      velocity(initialVelocity) {}

/**
 * @brief Update the Ball's Position based on its Velocity.
 */
void Ball::updatePosition()
{
  Position newPosition = {
      position.x + velocity.x,
      position.y + velocity.y};

  this->position = newPosition;
}

/**
 * _____________ GETTERS
 */

/**
 * @brief Get the current position of the ball.
 * 
 * @return The current position of the ball.
 */
Position Ball::getPosition()
{
  return position;
}

/**
 * @brief Get the current velocity of the ball.
 * 
 * @return The current velocity of the ball.
 */
Velocity Ball::getVelocity()
{
  return velocity;
}

/**
 * _____________ SETTERS
 */

/**
 * @brief Set the Ball's Position.
 * 
 * @param newPosition The new Position of the Ball.
 */
void Ball::setPosition(Position newPosition)
{
  this->position = newPosition;
}

/**
 * @brief Set the Ball's Velocity.
 * 
 * @param newVelocity The new Velocity of the Ball.
 */
void Ball::setVelocity(Velocity newVelocity)
{
  this->velocity = newVelocity;
}

/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/
