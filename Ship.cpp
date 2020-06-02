/**
 * @file Ship.cpp
 *
 * Implements a simple ship for the user.
 */

#include "Ship.h"
#include "Bolt.h"

static const float kControlRotationsPerSecond = 0.75F;
static const float kControlThrustShipLenPerSecSquared = 4.0F;
static const float kMaxVelocityPerShipLen = 10;
static const float kBoltVelocityPerShipLen = 20;
static const float kBoltSizeFraction = 0.33F;
static const float kMaxFireRate = 3;
static const float kMaxHead2HeadFireRate = 1;
static sf::Color kBoltColor(0x80, 0xFF, 0xFF);

Ship::Ship(const Config &config) : GraphObj(), mConfig(config)
{
  float wingEndX = -mConfig.sizeRadius * 0.5F;
  float wingEndY = mConfig.sizeRadius * 0.87F;
  float windowMinY = mConfig.sizeRadius * 0.167F;
  float windowMaxY = mConfig.sizeRadius * 0.333F;
  float windowEndX = mConfig.sizeRadius * 0.5F;
  float windowStartX = mConfig.sizeRadius * 0.167F;
  float baseStartX = wingEndX + mConfig.sizeRadius * 0.167F;
  float baseEndX = wingEndX;
  float baseY = mConfig.sizeRadius * 0.45F;
  float nozzleStartX = wingEndX;
  float nozzleEndX = nozzleStartX - mConfig.sizeRadius * 0.167F;
  float nozzleMinY = mConfig.sizeRadius * 0.22F;
  float nozzleMaxY = mConfig.sizeRadius * 0.25F;
  float thrustStartX = nozzleEndX;
  float thrustEndX = nozzleEndX - mConfig.sizeRadius * 0.4F;
  float thrustY = mConfig.sizeRadius * 0.2F;

  sf::Vector2f vFront(mConfig.sizeRadius, 0);
  sf::Vector2f vWing(-mConfig.sizeRadius * 0.5F, mConfig.sizeRadius * 0.87F);

  sf::VertexArray shipBody(sf::Triangles, 3);
  shipBody[0].position = sf::Vector2f(mConfig.sizeRadius, 0);
  shipBody[1].position = sf::Vector2f(wingEndX, -wingEndY);
  shipBody[2].position = sf::Vector2f(wingEndX, wingEndY);
  shipBody[0].color = kMediumGray;
  shipBody[1].color = kMediumGray;
  shipBody[2].color = kMediumGray;
  mCannonModelPt = shipBody[0].position;
  mMainColor = kMediumGray;

  sf::VertexArray shipWindow(sf::Quads, 4);
  shipWindow[0].position = sf::Vector2f(windowEndX, windowMinY);
  shipWindow[1].position = sf::Vector2f(windowEndX, -windowMinY);
  shipWindow[2].position = sf::Vector2f(windowStartX, -windowMaxY);
  shipWindow[3].position = sf::Vector2f(windowStartX, windowMaxY);
  shipWindow[0].color = sf::Color::Blue;
  shipWindow[1].color = sf::Color::Blue;
  shipWindow[2].color = kLightBlue;
  shipWindow[3].color = kLightBlue;

  sf::VertexArray shipBase(sf::Quads, 4);
  shipBase[0].position = sf::Vector2f(baseStartX, baseY);
  shipBase[1].position = sf::Vector2f(baseStartX, -baseY);
  shipBase[2].position = sf::Vector2f(baseEndX, -baseY);
  shipBase[3].position = sf::Vector2f(baseEndX, baseY);
  shipBase[0].color = config.baseColor;
  shipBase[1].color = config.baseColor;
  shipBase[2].color = config.baseColor;
  shipBase[3].color = config.baseColor;

  sf::VertexArray shipNozzle(sf::Quads, 4);
  shipNozzle[0].position = sf::Vector2f(nozzleEndX, nozzleMinY);
  shipNozzle[1].position = sf::Vector2f(nozzleEndX, -nozzleMinY);
  shipNozzle[2].position = sf::Vector2f(nozzleStartX, -nozzleMaxY);
  shipNozzle[3].position = sf::Vector2f(nozzleStartX, nozzleMaxY);
  shipNozzle[0].color = kDarkGray;
  shipNozzle[1].color = kDarkGray;
  shipNozzle[2].color = kDarkGray;
  shipNozzle[3].color = kDarkGray;

  sf::VertexArray shipExhaust(sf::Triangles, 3);
  shipExhaust[0].position = sf::Vector2f(thrustEndX, 0);
  shipExhaust[1].position = sf::Vector2f(thrustStartX, -thrustY);
  shipExhaust[2].position = sf::Vector2f(thrustStartX, thrustY);
  shipExhaust[0].color = kOrange;
  shipExhaust[1].color = sf::Color::Red;
  shipExhaust[2].color = sf::Color::Red;

  mModelShapes.push_back(shipBody);
  mModelShapes.push_back(shipWindow);
  mModelShapes.push_back(shipBase);
  mModelShapes.push_back(shipNozzle);
  mModelShapes.push_back(shipExhaust);
  mExaustIndex = mModelShapes.size() - 1;

  mMaxVelocity = kMaxVelocityPerShipLen * mConfig.sizeRadius;
  mBoltSpeed = kBoltVelocityPerShipLen * mConfig.sizeRadius;
  mBoltConfig.color = kBoltColor;
  mBoltConfig.size = kBoltSizeFraction * mConfig.sizeRadius;

  mCollisionRadius = mConfig.sizeRadius;

  mFirePeriod = 1.0F / kMaxFireRate;
  if (config.headToHead)
  {
    mFirePeriod = 1.0F / kMaxHead2HeadFireRate;
  }
}

void Ship::render(sf::RenderWindow &win)
{
  GraphObj::render(win);
}

void Ship::update(sf::Time deltaT, UpdateContext *context)
{
  float deltaSeconds = deltaT.asSeconds();
  if (mControls.rotateLeft)
  {
    mRadialVelocity = -kControlRotationsPerSecond * 2 * PI;
  }
  else if (mControls.rotateRight)
  {
    mRadialVelocity = kControlRotationsPerSecond * 2 * PI;
  }
  else
  {
    mRadialVelocity = 0;
  }

  if (mMaxVelocity == 0)
  {
    mLinearVelocity = sf::Vector2f(0, 0);
  }
  else
  {
    if (mControls.thrust)
    {
      float thrustDelta = deltaSeconds * kControlThrustShipLenPerSecSquared * mConfig.sizeRadius; 
      float dx = cos(mAngleRadians) * thrustDelta;
      float dy = sin(mAngleRadians) * thrustDelta;
      mLinearVelocity.x += dx;
      mLinearVelocity.y += dy;

      float velocity = sqrt(mLinearVelocity.x * mLinearVelocity.x + mLinearVelocity.y * mLinearVelocity.y);
      if (velocity > mMaxVelocity)
      {
        // Clip the velocity, but don't change the direction of travel.

        sf::Vector2f unitVelocity(mLinearVelocity.x / velocity, mLinearVelocity.y / velocity);
        mLinearVelocity = unitVelocity * mMaxVelocity;
      }

      mModelShapes[mExaustIndex].isVisible = true;
    }
    else
    {
      mModelShapes[mExaustIndex].isVisible = false;
    }
  }

  if (mFireWaitTime > 0)
  {
    mFireWaitTime -= deltaSeconds;
    if (mFireWaitTime < 0)
    {
      mFireWaitTime = 0;
    }
  }

  if (mControls.fire && mFireWaitTime <= 0)
  {
    auto bolt = std::make_shared<Bolt>(mBoltConfig);
    bolt->setPosition(modelToWorld(mCannonModelPt));
    bolt->setOrientation(mAngleRadians);
    bolt->setLinearVelocity(getDirectionVector() * mBoltSpeed);
    bolt->setTeam(mTeam); // Make sure we aren't blown up by our own fire
    context->spawnList.push_back(bolt);
    mFireWaitTime = mFirePeriod;
  }

  GraphObj::update(deltaT, context);
}
