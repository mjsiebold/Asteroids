#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

#include <memory>

#include "GameBox.h"
#include "Ship.h"
#include "Asteroid.h"
#include "SinglePlayerGame.h"

static const int kMaxFps = 100;
static const float kShipRadius = 25.0F;
static const float kMaxAsteroidRadiusWinRatio = 0.02F;
static const float kMinAsteroidRadius = 25.0F;
static const int kNumPlayers = 1;
static const int kScreenMargin = 100;
static const float kRespawnSeconds = 2;
static const sf::Color kDarkBrown(128, 64, 0);
static const sf::Color kRedBrown(165, 42, 42);
static const sf::Color kAsteroidMinColor = kDarkBrown;
static const sf::Color kAsteroidMaxColor = kRedBrown;
static const float kMinColorRatio = 0.75;
static const float kMaxColorRatio = 1.25;
static const int kPlayerTeamIndex = 0;
static const int kAsteroidTeamIndex = 2;
static const float kMaxRotationSpeed = 2 * PI * 3;
static const float kDisintegrationRadiusWinRatio = 0.20F;

class AsteroidField : public GameBox
{
public:
  struct FieldConfig
  {
    int minAsteroids = 0;
    int maxAsteroids = 0;
    float minAsteroidSize = 0;
    float maxAsteroidSize = 0;
    float maxLinearSpeed = 0;
    float maxRadialSpeed = 0;
    sf::Color minColor = kDarkBrown;
    sf::Color maxColor = kDarkBrown;
    int teamIndex = kAsteroidTeamIndex;
  };

  AsteroidField() : GameBox() {}

  void populateField(FieldConfig config, const sf::RenderWindow& window);
  void disintegrateAround(sf::Vector2f center, float radius, const sf::RenderWindow& window);

  int getAsteroidTeamCount();

protected:
  int mTeamIndex = 0;
};

void AsteroidField::populateField(FieldConfig config, const sf::RenderWindow& window)
{
  int numAsteroids = randInt(config.minAsteroids, config.maxAsteroids);

  for (int index = 0; index < numAsteroids; index++)
  {
    Asteroid::Config asteroidConfig = {};
    asteroidConfig.maxSize = config.maxAsteroidSize;
    asteroidConfig.minSize = config.minAsteroidSize;
    asteroidConfig.minChildSize = config.minAsteroidSize;
    float colorRatio = randFloat(0, 1);
    asteroidConfig.color = sf::Color(
      (int)(config.minColor.r + (config.maxColor.r - config.minColor.r) * colorRatio),
      (int)(config.minColor.g + (config.maxColor.g - config.minColor.g) * colorRatio),
      (int)(config.minColor.b + (config.maxColor.b - config.minColor.b) * colorRatio));
    auto asteroid = std::make_shared<Asteroid>(asteroidConfig);
    sf::Vector2f asteroidPos(randFloat(0, (float)window.getSize().x), randFloat(0, (float)window.getSize().y));
    asteroid->setPosition(asteroidPos);
    GraphObj::KnockConfig knockConfig;
    knockConfig.maxLinearSpeed = config.maxLinearSpeed;
    knockConfig.maxRadialSpeed = config.maxRadialSpeed;
    asteroid->knockRand(knockConfig);
    asteroid->setTeam(config.teamIndex);
    add(asteroid);
  }

  mTeamIndex = config.teamIndex;
}

void AsteroidField::disintegrateAround(sf::Vector2f center, float radius, const sf::RenderWindow& window)
{
  float doubleRadius = radius * radius;
  sf::Vector2u winSize = window.getSize();
  float doubleWinSizeX = (float)(winSize.x * winSize.x);
  float doubleWinSizeY = (float)(winSize.y * winSize.y);
  for (auto obj : mObjects)
  {
    if (obj->getTeam() != mTeamIndex)
    {
      continue;
    }
    sf::Vector2f pt = obj->getPosition();
    sf::Vector2f delta = pt - center;
    sf::Vector2f doubleDelta(delta.x * delta.x, delta.y * delta.y);

    // Wrap the disintegration around
    if (doubleDelta.x > doubleWinSizeX)
    {
      doubleDelta.x -= doubleWinSizeX;
    }
    if (doubleDelta.y > doubleWinSizeY)
    {
      doubleDelta.y -= doubleWinSizeY;
    }
    float doubleDistance = doubleDelta.x + doubleDelta.y;
    if (doubleDistance < doubleRadius)
    {
      obj->disintegrate();
    }
  }
}

int AsteroidField::getAsteroidTeamCount()
{
  int count = 0;

  for (auto obj : mObjects)
  {
    if (obj->getTeam() == mTeamIndex)
    {
      count++;
    }
  }

  return count;
}

class Player
{
public:
  struct Config
  {
    sf::Color teamColor;
    sf::Keyboard::Key leftKey;
    sf::Keyboard::Key rightKey;
    sf::Keyboard::Key thrustKey;
    sf::Keyboard::Key fireKey;
    float respawnSeconds = 0;
  };
  Player(const Config& config) : mConfig(config) { mRespawnClock.restart(); }
  void update(AsteroidField* box, sf::RenderWindow& win);
  void restart(AsteroidField* box);
  void setShip(std::shared_ptr<Ship> ship) { mShip = ship; }
  sf::Color getTeamColor() { return mConfig.teamColor; }
private:
  Config mConfig;
  bool mRespawning = true;
  sf::Clock mRespawnClock;
  std::shared_ptr<Ship> mShip;
};

void Player::restart(AsteroidField* box)
{
  box->remove(mShip);
  mRespawnClock.restart();
  mRespawning = true;
}

void Player::update(AsteroidField* box, sf::RenderWindow& win)
{
  if (box && mShip)
  {
    Ship::Controls controls;
    if (sf::Keyboard::isKeyPressed(mConfig.leftKey))
    {
      controls.rotateLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(mConfig.rightKey))
    {
      controls.rotateRight = true;
    }
    if (sf::Keyboard::isKeyPressed(mConfig.thrustKey))
    {
      controls.thrust = true;
    }
    if (sf::Keyboard::isKeyPressed(mConfig.fireKey))
    {
      controls.fire = true;
    }
    mShip->updateControls(controls);

    if (mRespawning && !box->isPresent(mShip))
    {
      if (mRespawnClock.getElapsedTime().asSeconds() > mConfig.respawnSeconds)
      {
        mShip->revive();
        mShip->setPosition(
          sf::Vector2f(
            randFloat(0, (float)win.getSize().x),
            randFloat(0, (float)win.getSize().y)
          )
        );
        box->disintegrateAround(mShip->getPosition(), kDisintegrationRadiusWinRatio * win.getSize().x, win);
        mShip->setLinearVelocity(sf::Vector2f(0, 0));
        mShip->setOrientation(randFloat(0, 2 * PI));
        mShip->setRadialVelocity(0);
        box->add(mShip);
      }
    }
    else
    {
      if (!mShip->isAlive())
      {
        mRespawning = true;
        mRespawnClock.restart();
      }
    }
  }
}

void singlePlayerGame()
{
  srand(clock());

  sf::RenderWindow window(sf::VideoMode(
    sf::VideoMode::getDesktopMode().width - kScreenMargin,
    sf::VideoMode::getDesktopMode().height - kScreenMargin),
    "Single Player");

  window.setFramerateLimit(kMaxFps);

  AsteroidField gameBox;
  int numPlayers = 1;

  Player::Config playerConfig;
  playerConfig.teamColor = sf::Color::Green;
  playerConfig.leftKey = sf::Keyboard::Left;
  playerConfig.rightKey = sf::Keyboard::Right;
  playerConfig.thrustKey = sf::Keyboard::Up;
  playerConfig.fireKey = sf::Keyboard::Slash;
  playerConfig.respawnSeconds = kRespawnSeconds;

  Player player(playerConfig);

  Ship::Config shipConfig;
  shipConfig.baseColor = player.getTeamColor();
  shipConfig.sizeRadius = (float)(sf::VideoMode::getDesktopMode().width / 40);
  shipConfig.headToHead = false;
  auto ship = std::make_shared<Ship>(shipConfig);
  sf::Vector2f pos((float)(window.getSize().x / 2), (float)(window.getSize().y / 2));
  ship->setPosition(pos);
  ship->setOrientation((float)(-PI / 2));
  ship->setTeam(kPlayerTeamIndex);
  player.setShip(ship);

  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        window.close();
      }
    }

    window.clear();

    gameBox.update(window);

    player.update(&gameBox, window);

    if (gameBox.getAsteroidTeamCount() == 0)
    {
      // Level cleared
      AsteroidField::FieldConfig fieldConfig;
      fieldConfig.maxAsteroids = 30;
      fieldConfig.minAsteroids = 20;
      fieldConfig.maxLinearSpeed = 750; // Note - this is only for small asteriods
      fieldConfig.maxRadialSpeed = 2 * PI * 5;
      fieldConfig.minAsteroidSize = 25;
      fieldConfig.maxAsteroidSize = 0.05F * window.getSize().x;
      fieldConfig.teamIndex = kAsteroidTeamIndex;
      fieldConfig.maxColor = kAsteroidMaxColor;
      fieldConfig.minColor = kAsteroidMinColor;
      gameBox.populateField(fieldConfig, window);

      player.restart(&gameBox);
    }

    window.display();
  }
}
