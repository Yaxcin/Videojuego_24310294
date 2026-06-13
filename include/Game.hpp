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

struct Projectile {
    sf::Vector2f position;
    std::weak_ptr<Pinata> target;
    int damage;
    float speed;
    float rotation;
    ProjectileType type;
    float splashRadius;
    bool alive;
};

struct ExplosionEffect {
    sf::Vector2f position;
    float radius;
    float timer;
    float duration;
};

class Game {
public:
    Game(unsigned int width = 1280, unsigned int height = 720);
    ~Game() = default;

    // Metodo principal: ejecuta el bucle del juego
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
    float gameSpeedMultiplier;

    float playerMoney;
    int playerLives;
    int currentRound;
    int enemiesLeftToSpawn;
    float spawnTimer;
    float spawnInterval;

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Pinata>> enemies;
    std::vector<std::shared_ptr<Tower>> towers;
    std::vector<Projectile> projectiles;
    std::vector<ExplosionEffect> explosionEffects;
    Map map;

    TowerType selectedTowerType;
    bool towerSelected;
    bool roundPreviewVisible;
    std::shared_ptr<Tower> selectedPlacedTower;
    static constexpr float PANEL_WIDTH = 200.f;

    void handleEvents();
    void update();
    void render();
    void updateProjectiles();
    void renderProjectiles();
    void updateDebugInfo();
    void spawnDebugPinata(PinataType type);
    int countTowersOfType(TowerType type) const;
    void toggleGameSpeed();
    bool isSpeedButtonAt(float x, float y) const;
    void placeTower(float x, float y);
    void moveSelectedTower(float x, float y);
    bool isValidTowerPosition(const sf::Vector2f& position, const std::shared_ptr<Tower>& ignoredTower = nullptr, bool showMessage = true) const;
    std::shared_ptr<Tower> findTowerAt(float x, float y) const;
    void clearTowerSelection();
    void slowTowersNear(const sf::Vector2f& position, float radius, float multiplier, float duration);
    void updateTowerRangeVisuals();
    void updateHypnosisAuras();
    bool hasActiveHypnotizer() const;
    float getOrganilleroRangeScale() const;
    void renderPanel();
    void renderTowerPreview();
    void renderRoundPreview();
    void startWave();
    void renderHUD();   

    sf::Font hudFont;
    std::string debugInfo;  
    sf::Texture menuTexture;
    sf::Texture chanclaProjectileTexture;
    sf::Texture rocketProjectileTexture;
    sf::Texture iceProjectileTexture;
    sf::Sprite* menuSprite = nullptr;
};

#endif
