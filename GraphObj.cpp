/**
 * @file GraphObj.cpp
 *
 * Defines the base object for the simple shape-based 2D graphic game objects.
 * These objects have a velocity, an orientation, a series of shapes that when
 * rendered displays them, and a collision radius.
 */

#include "GraphObj.h"

#include <SFML/Graphics.hpp>

void GraphObj::changeModelToWorld(sf::VertexArray *va, AngleFactors angleFact)
{
  if (va != nullptr)
  {
    for (unsigned int index = 0; index < va->getVertexCount(); index++)
    {
      (*va)[index].position = modelToWorld((*va)[index].position, angleFact);
    }
  }
}

void GraphObj::render(sf::RenderWindow &win)
{
  AngleFactors angleFact(mAngleRadians);
  for (auto &shape : mModelShapes)
  {
    if (shape.isVisible)
    {
      Shape worldShape = shape; 
      changeModelToWorld(&worldShape.vertices, angleFact);
      win.draw(worldShape.vertices);
    }
  }
}

void GraphObj::onOutOfBounds(UpdateContext *context) 
{
  if (context)
  {
    if (mCenterPt.x < 0)
    {
      mCenterPt.x += context->spaceLimits.x;
    }
    else if (mCenterPt.x > context->spaceLimits.x)
    {
      mCenterPt.x -= context->spaceLimits.x;
    }
    if (mCenterPt.y < 0)
    {
      mCenterPt.y += context->spaceLimits.y;
    }
    else if (mCenterPt.y > context->spaceLimits.y)
    {
      mCenterPt.y -= context->spaceLimits.y;
    }
  }
}

void GraphObj::update(sf::Time deltaT, UpdateContext *context)
{
  if (context)
  {
    float deltaSeconds = deltaT.asSeconds();
    mCenterPt += mLinearVelocity * deltaSeconds;
    mAngleRadians += mRadialVelocity * deltaSeconds;
    if (mAngleRadians > 2 * PI)
    {
      mAngleRadians -= 2 * PI;
    }
    if (mAngleRadians < -2 * PI)
    {
      mAngleRadians += 2 * PI;
    }

    if (mCenterPt.x < 0 ||
        mCenterPt.x > context->spaceLimits.x ||
        mCenterPt.y < 0 ||
        mCenterPt.y > context->spaceLimits.y)
    {
      onOutOfBounds(context);
    }
  }
}


// Inefficient
// TODO: improve
bool GraphObj::collidesWith(const GraphObj &obj, UpdateContext *context)
{
  if (isAlive() && canCollide() && 
      obj.isAlive() && obj.canCollide() && 
      getTeam() != obj.getTeam())
  {
    sf::Vector2f delta = mCenterPt - obj.mCenterPt;
    if (context)
    {
      // Assume wrapping, in which case if we're over a half span away,
      // we're closer to the wrapped distance.
      if (delta.x > context->spaceLimits.x / 2)
      {
        delta.x -= context->spaceLimits.x;
      }
      if (delta.y > context->spaceLimits.y / 2)
      {
        delta.y -= context->spaceLimits.y;
      }
    }
    float distanceSq = delta.x * delta.x + delta.y * delta.y;
    float minDistance = mCollisionRadius + obj.mCollisionRadius;
    if (distanceSq < minDistance * minDistance)
    {
      return true;
    }
  }
  return false;
}


