#ifndef PINATA_HPP
#define PINATA_HPP

#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

class Pinata : public Entity {
public:
    Pinata(const std::vector<sf::Vector2f>& waypoints, float speed = 80.0f, int health = 100);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    int getHealth() const { return health; }
    void takeDamage(int damage) { health -= damage; if (health <= 0) alive = false; }
    void applySlow(float multiplier, float duration);
    float getPathProgress() const;
    bool hasReachedEnd() const { return reachedEnd; }
    int getReward() const { return reward; }

protected:
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

    sf::RectangleShape shape; // Visual temporal hasta tener sprite

    void moveTowardsWaypoint(float deltaTime);
};

#endif
