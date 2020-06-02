/**
 * @file Fragment.h
 *
 * Defines a piece of an explosion.
 */

#ifndef FRAGMENT_H_2020_05_31
#define FRAGMENT_H_2020_05_31

#include "GraphObj.h"

class Fragment : public GraphObj
{
public:

  struct Config
  {
    sf::Color color;
    float size = 0;
    bool isFire = false;
    float lifespanSeconds;
  };

  Fragment(const Config &config);

  void onOutOfBounds(UpdateContext *context) override;

  void update(sf::Time deltaT, UpdateContext *context) override;

private:
  float mRemainingLifeSeconds = 0;
};

#endif