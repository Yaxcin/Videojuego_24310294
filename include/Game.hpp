#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "GameState.hpp"
#include <vector>
#include <memory>
#include <optional>
#include "Map.hpp"
#include "Enemies/Pinata.hpp"
#include "Towers/Tower.hpp"
#include "Towers/NinoPalo.hpp"
#include "Towers/ViejoMachete.hpp"
#include "Towers/Taquero.hpp"
#include "Towers/Abuelita.hpp"
#include "Towers/DonCohetes.hpp"
#include "Towers/Organillero.hpp"
#include "Towers/Raspadero.hpp"
#include "TextureManager.hpp"

// Forward declarations
class Entity;

class Game {
public:
    Game(unsigned int width = 1280, unsigned int height = 720);
    ~Game() = default;

    // Método principal - ejecuta el bucle del juego
    void run();

    // Getters
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    GameState getCurrentState() const { return currentState; }
    float getDeltaTime() const { return deltaTime; }
    float getPlayerMoney() const { return playerMoney; }
    int getPlayerLives() const { return playerLives; }
    int getCurrentRound() const { return currentRound; }

    // Setters
    void setPlayerMoney(float money) { playerMoney = money; }
    void addPlayerMoney(float amount) { playerMoney += amount; }
    void setPlayerLives(int lives) { playerLives = lives; }
    void damagePlayer(int damage = 1) { playerLives -= damage; }
    void setState(GameState newState) { currentState = newState; }

private:
    unsigned int width;
    unsigned int height;
    sf::RenderWindow window;

    GameState currentState;
    float deltaTime;
    sf::Clock gameClock;

    float playerMoney;
    int playerLives;
    int currentRound;
    int enemiesLeftToSpawn;
    float spawnTimer;
    float spawnInterval;

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Pinata>> enemies;
    std::vector<std::shared_ptr<Tower>> towers;
    Map map;

    TowerType selectedTowerType;
    bool towerSelected;
    static constexpr float PANEL_WIDTH = 200.f;

    void handleEvents();
    void update();
    void render();
    void updateDebugInfo();
    void placeTower(float x, float y);
    void renderPanel();
    void startWave();
    void renderHUD();   

    sf::Font hudFont;
    std::string debugInfo;  
    sf::Texture menuTexture;
    sf::Sprite* menuSprite = nullptr;
};

#endif
