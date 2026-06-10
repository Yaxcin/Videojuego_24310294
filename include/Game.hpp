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
    // Window setup
    unsigned int width;
    unsigned int height;
    sf::RenderWindow window;

    // Game state
    GameState currentState;
    float deltaTime;
    sf::Clock gameClock;

    // Player stats
    float playerMoney;
    int playerLives;
    int currentRound;

    // Entities (para Fase 2 y 3)
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Pinata>> enemies;
    Map map;

    // Métodos privados
    void handleEvents();
    void update();
    void render();
    void updateDebugInfo();

    // Debug
    sf::Font debugFont;
    std::string debugInfo;
};

#endif
