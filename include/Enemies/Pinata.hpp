#ifndef PINATA_HPP
#define PINATA_HPP

#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum class PinataType {
    ENGRUDO,
    ARCILLA,
    REVELACION,
    FRUTA,
    HIPNOTIZADORA,
    BEBE_ROSA,
    BEBE_AZUL
};

class Pinata : public Entity {
public:
    Pinata(const std::vector<sf::Vector2f>& waypoints, PinataType type = PinataType::ENGRUDO, int round = 1);
    Pinata(
        const std::vector<sf::Vector2f>& waypoints,
        PinataType type,
        int round,
        const sf::Vector2f& startPosition,
        size_t waypointIndex
    );

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getHealth() const { return health; }
    void takeDamage(int damage) { health -= damage; if (health <= 0) alive = false; }
    void applySlow(float multiplier, float duration);
    bool isSlowed() const { return slowTimer > 0.f; }
    float getPathProgress() const;
    bool hasReachedEnd() const { return reachedEnd; }
    int getReward() const { return reward; }
    int getEscapeDamage() const;
    PinataType getType() const { return type; }
    size_t getCurrentWaypointIndex() const { return currentWaypoint; }

protected:
    PinataType type;
    float speed;
    float speedMultiplier;
    float slowTimer;
    int health;
    int maxHealth;
    int reward;
    bool reachedEnd;

    // Waypoints del camino
    const std::vector<sf::Vector2f>& waypoints;
    size_t currentWaypoint;

    sf::RectangleShape shape;
    sf::Texture texture;
    std::unique_ptr<sf::Sprite> sprite;

    void moveTowardsWaypoint(float deltaTime);
    sf::Color getBaseColor() const;
    const char* getTexturePath() const;
    void initSprite();
    void applyVisualColor(const sf::Color& color);
};

#endif
