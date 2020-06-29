/**
 * @file GraphObj.h
 *
 * Defines the base object for the simple shape-based 2D graphic game objects.
 * These objects have a velocity, an orientation, a series of shapes that when
 * rendered displays them, and a collision radius.
 */

#ifndef GRAPH_OBJ_H_2020_05_30
#define GRAPH_OBJ_H_2020_05_30

#include <list>
#include <memory>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <assert.h>

struct CollisionEnvelope
{
  CollisionEnvelope(sf::Vector2f c, float r) : center(c), radius(r) {}
  sf::Vector2f center;
  float radius = 0;
};

struct AngleFactors
{
  AngleFactors(float angleRadians) 
  : sinFactor(sin(angleRadians)), cosFactor(cos(angleRadians)) {}
  float sinFactor;
  float cosFactor;
};

#ifndef PI
static const float PI = 3.1415927F;
#endif

static const sf::Color kLightBlue(0x80, 0x80, 0xFF);
static const sf::Color kOrange(255, 145, 0);
static const sf::Color kLightGray(224, 224, 224);
static const sf::Color kMediumGray(0x80, 0x80, 0x80);
static const sf::Color kDarkGray(0x60, 0x60, 0x60);

static float randFloat()
{
  return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

static int randInt(int min, int max)
{
  if (max > min)
  {
    return rand() % (max - min + 1) + min;
  }
  return min;
}

static float randFloat(float min, float max)
{
  return min + randFloat() * (max - min);
}

class GraphObj
{
public:

  struct UpdateContext
  {
    sf::Vector2u spaceLimits; // Limits of the region of space
    std::list<std::shared_ptr<GraphObj>> spawnList; // Allows an object to post new spawned objects
  };

  GraphObj() {}
  virtual ~GraphObj() {}

  virtual void render(sf::RenderWindow &win);

  virtual void update(sf::Time deltaT, UpdateContext *context);

  // Returns true if this object should be in the game, false
  // if it is dead and should be cleared from the game.
  bool isAlive() const { return mIsAlive; }
  void kill() { mIsAlive = false; }
  void revive() { mIsAlive = true; }

  virtual std::list<std::shared_ptr<GraphObj>> explode() { return std::list<std::shared_ptr<GraphObj>>(); }

  virtual bool explodesOnDeath() const { return false; }

  virtual CollisionEnvelope getCollisionEnvelope() const
  {
    return CollisionEnvelope(mCenterPt, mCollisionRadius);
  }

  virtual bool collidesWith(const GraphObj &obj, UpdateContext *context);

  virtual void onCollision()
  {
    kill();
  }

  void setPosition(sf::Vector2f pos) { mCenterPt = pos; }
  void setOrientation(float angleRadians) { mAngleRadians = angleRadians;  }
  void setLinearVelocity(sf::Vector2f linearVelocity) { mLinearVelocity = linearVelocity; }
  void setRadialVelocity(float radialVelocity) { mRadialVelocity = radialVelocity; }
  void setTeam(int team) { mTeam = team; }

  sf::Vector2f getPosition() const { return mCenterPt; }
  sf::Vector2f getLinearVelocity() const { return mLinearVelocity; }
  float getRadialVelocity() const { return mRadialVelocity; }
  float getAngle() const { return mAngleRadians; }
  sf::Vector2f getDirectionVector() const
  {
    AngleFactors angleFactors(mAngleRadians);
    return sf::Vector2f(angleFactors.cosFactor, angleFactors.sinFactor);
  }
  int getTeam() const { return mTeam; }

  sf::Color getMainColor() { return mMainColor; }

  bool canCollide() const { return mCollisionRadius > 0; }

protected:

  struct Shape
  {
    Shape(const sf::VertexArray &_vertices, bool _isVisible = true) 
      : vertices(_vertices), isVisible(_isVisible) {}
    sf::VertexArray vertices;
    bool isVisible = true;
  };

  sf::Vector2f modelToWorld(sf::Vector2f pt, AngleFactors angleFact) const
  {
    return mCenterPt + sf::Vector2f(pt.x * angleFact.cosFactor - pt.y * angleFact.sinFactor, pt.x * angleFact.sinFactor + pt.y * angleFact.cosFactor);
  }
  sf::Vector2f modelToWorld(sf::Vector2f pt) const
  {
    AngleFactors angleFactors(mAngleRadians);
    return mCenterPt + sf::Vector2f(pt.x * angleFactors.cosFactor - pt.y * angleFactors.sinFactor, pt.x * angleFactors.sinFactor + pt.y * angleFactors.cosFactor);
  }
  void changeModelToWorld(sf::VertexArray *va, AngleFactors angleFact);

  virtual void onOutOfBounds(UpdateContext *context);

  sf::Vector2f mLinearVelocity;
  float mRadialVelocity = 0;

  // This is relative to a 0,0 center point.
  std::vector<Shape> mModelShapes;

  sf::Vector2f mCenterPt;
  float mAngleRadians = 0;
  float mCollisionRadius = 0;
  bool mIsAlive = true;
  sf::Color mMainColor = kDarkGray;

  // Only objects on different teams are considered for collision.
  int mTeam = 0;
};

#endif