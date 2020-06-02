/**
 * @file Bolt.h
 *
 * Defines a single bit of laser shot from a cannon on the ship.
 */

#ifndef BOLT_H_2020_05_31
#define BOLT_H_2020_05_31

#include "GraphObj.h"

class Bolt : public GraphObj
{
public:

  struct Config
  {
    sf::Color color;
    float size = 0;
  };

  Bolt(const Config &config);

  void onOutOfBounds(UpdateContext *context) override;
};

#endif