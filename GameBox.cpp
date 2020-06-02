/**
 * @file GameBox.cpp
 *
 * Defines the game core, including the game bounds and the set of graphic objects.
 */

#include "GameBox.h"
#include <SFML/System/Clock.hpp>
#include "Fragment.h"

#include <stdlib.h>
#include <unordered_map>

static const int kMinExplosionFragments = 15;
static const int kMaxExplosionFragments = 30;
static const float kMinExplosionSpeed =  500;
static const float kMaxExplosionSpeed = 1000;
static const float kMinExplosionRadialSpeed = 0;
static const float kMaxExplosionRadialSpeed = 2 * PI * 6;
static const float kMinBodyFragmentSize = 5;
static const float kMaxBodyFragmentSize = 50;
static const float kMinFireFragmentSize = 5;
static const float kMaxFireFragmentSize = 20;
static const float kMinFragmentLifeSeconds = 0.5;
static const float kMaxFragmentLifeSeconds = 2;
static const sf::Color kFireColor = kOrange;

static int randInt(int min, int max)
{
  if (max > min)
  {
    return rand() % (max - min) + min;
  }
  return min;
}

void GameBox::checkForCollisions(GraphObj::UpdateContext *context)
{
  // TODO: use a more efficient method to check
  //       for collisions.
  //

  // Divide into teams, and filter out things that don't collide.

  std::list<std::shared_ptr<GraphObj>> collideCandidates;
  for (auto &obj : mObjects)
  {
    if (obj && obj->isAlive() && obj->canCollide())
    {
      collideCandidates.push_back(obj);
    }
  }

  // Inefficient O(N^2) search
  auto i = collideCandidates.begin();
  while (i != collideCandidates.end())
  {
    bool collision = false;
    auto j = collideCandidates.begin();
    while (j != collideCandidates.end())
    {
      if (i != j)
      {
        auto oi = *i;
        auto oj = *j;
        if (oi && oj)
        {
          if (oi->collidesWith(*oj, context))
          {
            oi->onCollision();
            oj->onCollision();
            j = collideCandidates.erase(j);

            // Only allow one collision
            collision = true;
            break;
          }
        }
      }
      ++j;
    }
    if (collision)
    {
      i = collideCandidates.erase(i);
    }
    else
    {
      ++i;
    }
  }
}

void GameBox::update(sf::RenderWindow &win)
{
  sf::Time currentTime = mClock.getElapsedTime();
  sf::Time deltaTime = sf::Time::Zero;

  if (mLastUpdateTimeValid)
  {
    deltaTime = currentTime - mLastUpdateTime;
  }

  GraphObj::UpdateContext context;
  context.spaceLimits = win.getSize();

  auto objIter = mObjects.begin();
  while (objIter != mObjects.end())
  {
    bool isAlive = false;
    auto obj = *objIter;
    if (obj && obj->isAlive())
    {
      isAlive = true;
    }
    if (!isAlive)
    {
      if (obj)
      {
        if (obj->explodesOnDeath())
        {
          explode(obj);
        }
      }
      // Remove inactive objects
      objIter = mObjects.erase(objIter);
    }
    else
    {
      obj->update(deltaTime, &context);
      obj->render(win);
      ++objIter;
    }
  }

  checkForCollisions(&context);

  for (auto obj : context.spawnList)
  {
    mObjects.push_back(obj);
  }

  mLastUpdateTimeValid = true;
  mLastUpdateTime = currentTime;
}

void GameBox::add(std::shared_ptr<GraphObj> obj)
{
  if (obj)
  {
    mObjects.push_back(obj);
  }
}

void GameBox::explode(std::shared_ptr<GraphObj> obj)
{
  if (obj)
  {
    int fragments = randInt(kMinExplosionFragments, kMaxExplosionFragments);
    for (int fragIndex = 0; fragIndex < fragments; fragIndex++)
    {
      float fragSpeed = randFloat(kMinExplosionSpeed, kMaxExplosionSpeed);
      float fragRadialSpeed = randFloat(kMinExplosionRadialSpeed, kMaxExplosionRadialSpeed);
      float angle = randFloat(0, 2 * PI);

      Fragment::Config fragmentConfig;
      fragmentConfig.lifespanSeconds = randFloat(kMinFragmentLifeSeconds, kMaxFragmentLifeSeconds);
      if (rand() % 3 == 0)
      {
        fragmentConfig.color = obj->getMainColor();
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
      fragment->setPosition(obj->getPosition());
      fragment->setOrientation(angle);
      fragment->setRadialVelocity(fragRadialSpeed);
      fragment->setLinearVelocity(obj->getLinearVelocity() + 
        fragment->getDirectionVector() * fragSpeed);

      mObjects.push_back(fragment);
    }
    obj->kill();
  }
}

bool GameBox::isPresent(std::shared_ptr<GraphObj> obj)
{
  for (auto &nextObj : mObjects)
  {
    if (nextObj == obj)
    {
      return true;
    }
  }
  return false;
}