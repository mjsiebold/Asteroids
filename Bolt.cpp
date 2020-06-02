/**
 * @file Bolt.cpp
 *
 * Implements a simple ship for the user.
 */

#include "Bolt.h"

Bolt::Bolt(const Config &config) : GraphObj()
{
  sf::VertexArray spike(sf::Triangles, 3);
  spike[0].position = sf::Vector2f(config.size / 2, 0);
  spike[1].position = sf::Vector2f(-config.size / 2, -config.size / 4);
  spike[2].position = sf::Vector2f(-config.size / 2, config.size / 4);
  spike[0].color = sf::Color::White;
  spike[1].color = config.color;
  spike[2].color = config.color;
  mModelShapes.push_back(spike);
  mCollisionRadius = config.size;
}

void Bolt::onOutOfBounds(UpdateContext *context)
{
  mIsAlive = false;
}

