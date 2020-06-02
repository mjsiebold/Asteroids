/**
 * @file Bolt.cpp
 *
 * Implements a simple ship for the user.
 */

#include "Fragment.h"

static const float kMinSideRatio = 0.25F;
static const float kMaxSideRatio = 0.75F;

static float randSide(const Fragment::Config &config)
{
  return randFloat(config.size * kMinSideRatio, config.size * kMaxSideRatio);
}

Fragment::Fragment(const Config &config) : GraphObj()
{
  if (config.isFire)
  {
    sf::VertexArray spike(sf::Triangles, 3);
    spike[0].position = sf::Vector2f(config.size / 2, 0);
    spike[1].position = sf::Vector2f(-config.size / 2, -config.size / 4);
    spike[2].position = sf::Vector2f(-config.size / 2, config.size / 4);
    spike[0].color = sf::Color::White;
    spike[1].color = config.color;
    spike[2].color = config.color;
    mModelShapes.push_back(spike);
  }
  else
  {
    sf::VertexArray spike(sf::Quads, 4);
    spike[0].position = sf::Vector2f(randSide(config), randSide(config));
    spike[1].position = sf::Vector2f(randSide(config), -randSide(config));
    spike[2].position = sf::Vector2f(-randSide(config), -randSide(config));
    spike[3].position = sf::Vector2f(-randSide(config), randSide(config));
    spike[0].color = config.color;
    spike[1].color = config.color;
    spike[2].color = config.color;
    spike[3].color = config.color;
    mModelShapes.push_back(spike);
  }
  mRemainingLifeSeconds = config.lifespanSeconds;

  // No collision radius - fragments only for show.
  mCollisionRadius = 0;
}

void Fragment::onOutOfBounds(UpdateContext *context)
{
  mIsAlive = false;
}

void Fragment::update(sf::Time deltaT, UpdateContext *context)
{
  if (mRemainingLifeSeconds > 0)
  {
    mRemainingLifeSeconds -= deltaT.asSeconds();
  }

  if (mRemainingLifeSeconds <= 0)
  {
    mRemainingLifeSeconds = 0;
    mIsAlive = false;
  }

  GraphObj::update(deltaT, context);
}
