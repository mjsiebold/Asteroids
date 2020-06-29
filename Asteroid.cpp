/**
 * @file Asteroids.cpp
 *
 * Implements a simple ship for the user.
 */

#include "Asteroid.h"

static const float kMinSideRatio = 0.80F;
static const float kMaxSideRatio = 1.30F;
static const int kMinAsteroidPoints = 6;
static const int kMaxAsteroidPoints = 12;
static const float kMinAngleRatio = -0.33F;
static const float kMaxAngleRatio = 0.33F;

static const float kMinSize = 25;
static const float kMaxChildSizeRatio = 0.75F;
static const float kMinChildSizeRatio = 0.1F;
static const int kMaxChildTries = 15;

Asteroid::Asteroid(const Config &config) : VolatileObj()
{
  int pointCount = randInt(kMinAsteroidPoints, kMaxAsteroidPoints);

  // The triagle fan has a central point (the first point)
  // and the rest points circle the original.  To complete
  // the asteroid, we connect the circle with the last
  // point matching the first point (past the central):
  // body[0]: Central point
  // body[1]: First circling point
  // body[pointCount]: Last unique circling point
  // body[pointCount + 1]: Connecting point (same as body[1])
  //
  int totalPointCount = pointCount + 2;

  sf::VertexArray body(sf::TriangleFan, totalPointCount);
  body[0].position = sf::Vector2f(0, 0);
  double nextAngle = 0;
  double deltaAngle = 2 * PI / pointCount;
  double size = randFloat(config.minSize, config.maxSize);
  for (int i = 0; i < pointCount; i++)
  {
    double angle = nextAngle + randFloat(kMinAngleRatio, kMaxAngleRatio) * deltaAngle;
    double len = size * randFloat(kMinSideRatio, kMaxSideRatio);
    double x = cos(angle) * len;
    double y = sin(angle) * len;
    body[(int)(i + 1)].position = sf::Vector2f((float)x, (float)y);
    nextAngle += deltaAngle;
  }
  body[(int)(pointCount + 1)].position = body[1].position;
  for (int i = 0; i < totalPointCount; i++)
  {
    body[i].color = config.color;
  }
  mMainColor = config.color;
  mModelShapes.push_back(body);
  
  // TODO: larger asteroids, with large variances in side radius,
  //       may need something more precise when detecting a collision...
  mCollisionRadius = (float)size;

  mExplodeStyle = ExplodeStyle::FireOnly;
}

std::list<std::shared_ptr<GraphObj>> Asteroid::explode()
{
  // Try breaking up into an approximately similar volume
  std::list<std::shared_ptr<GraphObj>> ejecta;

  float remainingVolume = mCollisionRadius * mCollisionRadius;

  Asteroid::Config config;
  config.color = mMainColor;
  config.minSize = mCollisionRadius * kMinChildSizeRatio;
  config.maxSize = mCollisionRadius * kMaxChildSizeRatio;

  for (int tries = 0; tries < kMaxChildTries; tries++)
  {
    auto obj = std::make_shared<Asteroid>(config);
    float size = obj->mCollisionRadius;
    if (size < kMinSize)
    {
      break;
    }
    float volume = size * size;
    remainingVolume -= volume;
    if (remainingVolume < 0)
    {
      break;
    }
    throwObjRand(obj, ThrowStyle::Breakup);
    // Space the objects out 
    obj->setPosition(obj->getPosition() + obj->getDirectionVector() * (mCollisionRadius / 2));

    // Reduce the rotation by the size
    obj->setRadialVelocity(obj->getRadialVelocity() * (kMinSize / size));
    ejecta.push_back(obj);
  }

  auto explodeEjecta = VolatileObj::explode();
  ejecta.splice(ejecta.end(), explodeEjecta);
  return ejecta;
}


