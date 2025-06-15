#include "Helpers.h"
#include <chrono>

/**
 * @brief Turns out getting random numbers in C++ is a serious pain.
 *    So this will be good enough - I'm sure it's not a great method
 *    but it was a massive pain trying to try and get the other random 
 *    methods to work.
 * 
 * @param options The options from which a 'random' element will be selected.
 * 
 * @return The 'randomly' selected element.
 */
int getNotSoRandomElement(std::vector<int> options)
{
  auto duration = std::chrono::system_clock::now().time_since_epoch();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  int index = millis % options.size();
  return options[index];
}

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
Velocity reboundVelocity(Velocity velocity, CollisionSurfaceOrientation surfaceOrientation, bool random)
{
  switch (surfaceOrientation)
  {
  case VERTICAL:
    if (random)
    {
      std::vector<int> options = {-1, 0, 1};
      int element = getNotSoRandomElement(options);
      return Velocity(velocity.x * -1, element);
      break;
    }
    return Velocity(velocity.x * -1, velocity.y);
    break;

  case HORIZONTAL:
    if (random)
    {
      int options[3] = {-1, 1}; // Can't remove the X componment as ball will become unreachable.
      int index = rand() % 1;
      int randomDirection = options[index];
      return Velocity(randomDirection, velocity.y * -1);
      break;
    }
    return Velocity(velocity.x, velocity.y * -1);
    break;
  }
  return velocity;
}
