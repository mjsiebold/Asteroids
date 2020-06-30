/**
 * @file Asteroid.h
 *
 * A random-shaped rock that when shot breaks up into pieces.
 */

#ifndef ASTERIOD_H_2020_06_7
#define ASTERIOD_H_2020_06_7

#include "VolatileObj.h"

class Asteroid : public VolatileObj
{
public:

  static constexpr float kDefaultMinChildSize = 25.0F;
  static const sf::Color kDefaultColor;
 
  struct Config
  {
    float maxSize = 0;
    float minSize = 0;
    float minChildSize = kDefaultMinChildSize;
    sf::Color color;
  };

  Asteroid(const Config &config);

  bool explodesOnDeath() const override { return true; }

  std::list<std::shared_ptr<GraphObj>> explode() override;

  void knockAsteriod(float minLinearSpeed, float maxLinearSpeed);

  void disintegrate() override
  {
    mChildrenAllowed = false;
    kill();
  }

private:
  bool mChildrenAllowed = true;
  float mRemainingLifeSeconds = 0;
  float mMinChildSize = 0;
};

#endif