#ifndef PONGBALL_H
#define PONGBALL_H

#include "Helpers.h"

/**
 * ==================================================================================================================
 * ~                                                  CLASS                                                    
 * ------------------------------------------------------------------------------------------------------------------
*/

/**
 * @brief Class representing the ball entity in the game of Pong.
 */
class Ball
{
public:
  /**
   * @brief Class constructor.
   * 
   * @param initalPosition The starting position of the Ball.
   * @param initalVelocity The starting velocity of the Ball.
   */
  Ball(
      Position initialPosition,
      Velocity initialVelocity);

  /**
   * @brief Update the Ball's Position based on its Velocity.
   */
  void updatePosition();

  /**
   * _____________ GETTERS
   */

  /**
   * @brief Get the Ball's current Position.
   */
  Position getPosition();

  /**
   * @brief Get the Ball's current Velocity.
   */
  Velocity getVelocity();

  /**
   * _____________ SETTERS
   */

  /**
   * @brief Set the Ball's Position.
   * 
   * @param newPosition The new Position of the Ball.
   */
  void setPosition(Position newPosition);

  /**
   * @brief Set the Ball's Velocity.
   * 
   * @param newVelocity The new Velocity of the Ball.
   */
  void setVelocity(Velocity newVelocity);

private:
  /**
   * _____________ MEMEBER VARIABLES
   */
  /**
   * @brief The current Position of the Ball.
   */
  Position position;

  /**
   * @brief The current Velocity of the Ball.
   */
  Velocity velocity;

  /**
   * _____________ METHODS
   */
};

/**
 * ----------------------------------------------------------------------------------------------------------------- 
 * =================================================================================================================
*/

#endif // PONGBALL_H
