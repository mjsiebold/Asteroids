/**
 * @file Ship.h
 *
 * Defines a ship graphic object to represent the user.
 */

#ifndef SHIP_H_2020_05_30
#define SHIP_H_2020_05_30

#include "GraphObj.h"
#include "Bolt.h"

class Ship : public GraphObj
{
public:

  struct Config
  {
    sf::Color baseColor;
    float sizeRadius = 0;
    bool headToHead; // Adjusts the fire rate and speed
                     // to make it harder to hit.
  };

  struct Controls
  {
    bool rotateLeft = false;
    bool rotateRight = false;
    bool thrust = false;
    bool fire = false;
  };

  Ship(const Config &config);

  void render(sf::RenderWindow &win) override;

  void updateControls(const Controls &controls)
  {
    mControls = controls;
  }

  void update(sf::Time deltaT, UpdateContext *context) override;

  bool explodesOnDeath() const override { return true; }

private:
  Controls mControls;
  Config mConfig;
  int mExaustIndex = 0;
  sf::Vector2f mCannonModelPt;
  float mMaxVelocity = 0;
  float mBoltSpeed = 0;
  float mFireWaitTime = 0;
  float mFirePeriod = 0;
  Bolt::Config mBoltConfig;
};

#endif