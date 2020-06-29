#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

#include <memory>

#include "GameBox.h"
#include "Ship.h"
#include "Asteroid.h"

static const int kMaxFps = 100;
static const float kShipRadius = 40.0F;
static const float kMaxAsteroidRadius = 300.0F;
static const float kMinAsteroidRadius = 25.0F;
static const int kNumPlayers = 2;
static const int kScreenMargin = 100;
static const float kRespawnSeconds = 2;
static const sf::Color kAsteroidColor(165, 42, 42);

class HeadToHeadPlayer
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
  HeadToHeadPlayer(const Config &config) : mConfig(config) {}
  void update(GameBox *box, sf::RenderWindow &win);
  void setShip(std::shared_ptr<Ship> ship) { mShip = ship; }
  sf::Color getTeamColor() { return mConfig.teamColor; }
private:
  Config mConfig;
  bool mRespawning = false;
  sf::Clock mRespawnClock;
  std::shared_ptr<Ship> mShip;
};

void HeadToHeadPlayer::update(GameBox *box, sf::RenderWindow &win)
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

static void singlePlayerGame()
{
  sf::RenderWindow window(sf::VideoMode(
    sf::VideoMode::getDesktopMode().width - kScreenMargin,
    sf::VideoMode::getDesktopMode().height - kScreenMargin),
    "Single Player");

  window.setFramerateLimit(kMaxFps);

  GameBox gameBox;
  int numPlayers = 1;

  HeadToHeadPlayer::Config playerConfig;
  playerConfig.teamColor = sf::Color::Green;
  playerConfig.leftKey = sf::Keyboard::Left;
  playerConfig.rightKey = sf::Keyboard::Right;
  playerConfig.thrustKey = sf::Keyboard::Up;
  playerConfig.fireKey = sf::Keyboard::Slash;
  playerConfig.respawnSeconds = kRespawnSeconds;

  HeadToHeadPlayer player(playerConfig);

  int teamIndex = 1; // non-default - asteriods will all be at default(zero)
  Ship::Config shipConfig;

  shipConfig.baseColor = player.getTeamColor();
  shipConfig.sizeRadius = (float)(sf::VideoMode::getDesktopMode().width / 40);
  shipConfig.headToHead = false;
  auto ship = std::make_shared<Ship>(shipConfig);
  sf::Vector2f pos((float)(window.getSize().x * (teamIndex + 1) / (numPlayers + 1)), (float)(window.getSize().y / 2));
  ship->setPosition(pos);
  ship->setOrientation((float)(-PI / 2));
  ship->setTeam(teamIndex++);
  player.setShip(ship);
  gameBox.add(ship);

  // Testing only:
  Asteroid::Config asteroidConfig = {};
  asteroidConfig.maxSize = kMaxAsteroidRadius;
  asteroidConfig.minSize = kMinAsteroidRadius;
  asteroidConfig.color = kAsteroidColor;
  auto testAsteroid = std::make_shared<Asteroid>(asteroidConfig);
  gameBox.add(testAsteroid);
  sf::Vector2f asteroidPos((float)(window.getSize().x * (teamIndex + 1) / 2), (float)(window.getSize().y / 4));
  testAsteroid->setPosition(asteroidPos);

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

    window.display();
  }
}

static void headToHeadGame()
{
  sf::RenderWindow window(sf::VideoMode(
    sf::VideoMode::getDesktopMode().width - kScreenMargin, 
    sf::VideoMode::getDesktopMode().height - kScreenMargin), 
    "Head To Head");

  window.setFramerateLimit(kMaxFps); 

  GameBox gameBox;

  HeadToHeadPlayer::Config config[kNumPlayers];
  config[0].teamColor       = sf::Color::Red;
  config[0].leftKey         = sf::Keyboard::Z;
  config[0].rightKey        = sf::Keyboard::X;
  config[0].thrustKey       = sf::Keyboard::S;
  config[0].fireKey         = sf::Keyboard::Tab;
  config[0].respawnSeconds  = kRespawnSeconds;
  config[1].teamColor = sf::Color::Green;
  config[1].leftKey = sf::Keyboard::Left;
  config[1].rightKey = sf::Keyboard::Right;
  config[1].thrustKey = sf::Keyboard::Up;
  config[1].fireKey = sf::Keyboard::Slash;
  config[1].respawnSeconds = kRespawnSeconds;

  std::vector<HeadToHeadPlayer> players =
  {
    config[0], config[1]
  };

  int teamIndex = 0;
  for (auto &player : players)
  {
    Ship::Config config;
    
    config.baseColor = player.getTeamColor();
    config.sizeRadius = (float)(sf::VideoMode::getDesktopMode().width / 40);
    config.headToHead = true;
    auto ship = std::make_shared<Ship>(config);
    sf::Vector2f pos((float)(window.getSize().x * (teamIndex + 1) / (kNumPlayers + 1)), (float)(window.getSize().y / 2));
    ship->setPosition(pos);
    ship->setOrientation((float)(-PI / 2));
    ship->setTeam(teamIndex++);
    player.setShip(ship);
    gameBox.add(ship);
  }

  // Testing only:
  Asteroid::Config asteroidConfig = {};
  asteroidConfig.maxSize = kMaxAsteroidRadius;
  asteroidConfig.minSize = kMinAsteroidRadius;
  asteroidConfig.color = kAsteroidColor;
  auto testAsteroid = std::make_shared<Asteroid>(asteroidConfig);
  gameBox.add(testAsteroid);
  sf::Vector2f asteroidPos((float)(window.getSize().x * (teamIndex + 1) / 2), (float)(window.getSize().y / 4));
  testAsteroid->setPosition(asteroidPos);

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

    for (auto &player : players)
    {
      player.update(&gameBox, window);
    }

    window.display();
  }
}

int main()
{
  singlePlayerGame();
  return 0;
}