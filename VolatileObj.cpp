/**
 * @file VolatileObj.cpp
 *
 * Implements an object that can explode.
 *
 * The base explosion throws only fragments - for display only.
 */

#include "VolatileObj.h"
#include "Fragment.h"

static const int kMinExplosionFragments = 15;
static const int kMaxExplosionFragments = 30;
static const float kMinExplosionSpeed = 500;
static const float kMaxExplosionSpeed = 1000;
static const float kMinBreakupSpeed = 50;
static const float kMaxBreakupSpeed = 250;
static const float kMinExplosionRadialSpeed = 0;
static const float kMaxExplosionRadialSpeed = 2 * PI * 6;
static const float kMinBreakupRadialSpeed = 0;
static const float kMaxBreakupRadialSpeed = 2 * PI * 2;
static const float kMinBodyFragmentSize = 5;
static const float kMaxBodyFragmentSize = 50;
static const float kMinFireFragmentSize = 5;
static const float kMaxFireFragmentSize = 20;
static const float kMinFragmentLifeSeconds = 0.5;
static const float kMaxFragmentLifeSeconds = 2;
static const sf::Color kFireColor = kOrange;

void VolatileObj::throwObjRand(std::shared_ptr<GraphObj> obj, ThrowStyle throwStyle)
{
  float fragSpeed = 0;
  float fragRadialSpeed = 0;
  float angle = 0;

  if (throwStyle == ThrowStyle::Breakup)
  {
    fragSpeed = randFloat(kMinBreakupSpeed, kMaxBreakupSpeed);
    fragRadialSpeed = randFloat(kMinBreakupRadialSpeed, kMaxBreakupRadialSpeed);
    angle = randFloat(0, 2 * PI);
  }
  else
  {
    fragSpeed = randFloat(kMinExplosionSpeed, kMaxExplosionSpeed);
    fragRadialSpeed = randFloat(kMinExplosionRadialSpeed, kMaxExplosionRadialSpeed);
    angle = randFloat(0, 2 * PI);
  }

  if (randInt(0, 1))
  {
    fragRadialSpeed *= -1;
  }

  obj->setPosition(getPosition());
  obj->setOrientation(angle);
  obj->setRadialVelocity(fragRadialSpeed);
  obj->setLinearVelocity(getLinearVelocity() +
    obj->getDirectionVector() * fragSpeed);
}

std::list<std::shared_ptr<GraphObj>> VolatileObj::explode()
{
  std::list<std::shared_ptr<GraphObj>> ejecta;

  int fragments = randInt(kMinExplosionFragments, kMaxExplosionFragments);
  for (int fragIndex = 0; fragIndex < fragments; fragIndex++)
  {
    Fragment::Config fragmentConfig;
    fragmentConfig.lifespanSeconds = randFloat(kMinFragmentLifeSeconds, kMaxFragmentLifeSeconds);

    if (mExplodeStyle == ExplodeStyle::FireAndFragments && rand() % 3 == 0)
    {
      fragmentConfig.color = getMainColor();
      fragmentConfig.size = randFloat(kMinBodyFragmentSize, kMaxBodyFragmentSize);
      fragmentConfig.isFire = false;
    }
    else
    {
      fragmentConfig.color = kFireColor;
      fragmentConfig.size = randFloat(kMinFireFragmentSize, kMaxFireFragmentSize);
      fragmentConfig.isFire = true;
    }

    auto fragment = std::make_shared<Fragment>(fragmentConfig);
    throwObjRand(fragment);

    ejecta.push_back(fragment);
  }
  kill();

  return ejecta;
}