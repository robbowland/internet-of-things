#ifndef PONGHELPERS_H
#define PONGHELPERS_H

#include <stdlib.h>
#include <Arduino.h>
#include <random>
#include <vector>

/**
 * @brief Structure for representing the position of an entity.
 */
struct Position
{
  int x; /// The X position of the entity.
  int y; /// The Y position of the entity.

  Position(int x_, int y_)
  {
    x = x_;
    y = y_;
  }
};

// TODO - look at adding magnitude to this to speed up the ball.
/**
 * @brief Structure for repesenting the velocity of an entity.
 */
struct Velocity
{
  int x; /// The X component direction
  int y; /// The Y component direction

  Velocity(int x_, int y_)
  {
    x = x_;
    y = y_;
  }
};

int getNotSoRandomElement(std::vector<int> options);

/**
 * @brief An enum for defined surface orientation
 *    for assisting in clarity during collision
 *    calculations.
 */
enum CollisionSurfaceOrientation
{
  VERTICAL,
  HORIZONTAL
};

/**
 * @brief Calculate the velocity of an entity after it
 *    has collided with a surface.
 * 
 * @param velocity The velocity of the entity.
 * @param surfaceOrientation Whether the collision surface is 
 *    oriented verticall or horizontally to the entity.
 * @param random Whether or not the randomly select one of the 
 *    velocity components (not entirely random as this was 
 *    designed for Pong so it ensures the ball will stay 
 *    reachable by the paddles)
 * 
 * @return Velocity The new velocity after rebounding.
 */
Velocity reboundVelocity(Velocity velocity, CollisionSurfaceOrientation surfaceOrientation, bool random = false);

#endif //PONGHELPERS_H
