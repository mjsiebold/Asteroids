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

  struct Config
  {
    float maxSize = 0;
    float minSize = 0;
    sf::Color color;
  };

  Asteroid(const Config &config);

  bool explodesOnDeath() const override { return true; }

  std::list<std::shared_ptr<GraphObj>> explode() override;

  //void update(sf::Time deltaT, UpdateContext *context) override;

private:
  float mRemainingLifeSeconds = 0;
};

#endif