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

  std::list <std::shared_ptr<GraphObj>> totalEjecta;
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
          auto ejecta = obj->explode();
          totalEjecta.splice(totalEjecta.end(), ejecta);
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

  mObjects.splice(mObjects.end(), totalEjecta);

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

void GameBox::remove(std::shared_ptr<GraphObj> obj)
{
  if (obj)
  {
    for (std::list<std::shared_ptr<GraphObj>>::iterator nextObj = mObjects.begin();
      nextObj != mObjects.end(); 
      ++nextObj)
    {
      if (obj == *nextObj)
      {
        mObjects.erase(nextObj);
        break;
      }
    }
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