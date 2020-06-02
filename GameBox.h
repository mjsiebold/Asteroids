/**
 * @file GameBox.h
 *
 * Defines the game core, including the game bounds and the set of graphic objects.
 */

#ifndef GAME_BOX_H_2020_05_31
#define GAME_BOX_H_2020_05_31

#include <memory>
#include <list>
#include "GraphObj.h"

class GameBox
{
public:
  GameBox() {}
  virtual ~GameBox() {}

  virtual void update(sf::RenderWindow &win);

  void add(std::shared_ptr<GraphObj> obj);
  void explode(std::shared_ptr<GraphObj> obj);
  bool isPresent(std::shared_ptr<GraphObj> obj);

protected:

  void checkForCollisions(GraphObj::UpdateContext *context);

  std::list<std::shared_ptr<GraphObj>> mObjects;
  sf::Time mLastUpdateTime;
  sf::Clock mClock;
  bool mLastUpdateTimeValid = false;
};

#endif