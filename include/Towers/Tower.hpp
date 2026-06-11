#ifndef TOWER_HPP
#define TOWER_HPP

#include "Entity.hpp"
#include "Enemies/Pinata.hpp"
#include "TowerType.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Tower : public Entity {
public:
    Tower(float x, float y, TowerType type, float range, int damage, float fireRate, int cost);

    void update(float /*deltaTime*/) override {}
    void combat(float deltaTime, std::vector<std::shared_ptr<Pinata>>& enemies);
    void render(sf::RenderWindow& window) const override;

    int getCost() const { return cost; }
    float getRange() const { return range; }
    TowerType getType() const { return towerType; }
    void setSelected(bool s) { selected = s; }

protected:
    TowerType towerType;
    float range;
    int damage;
    float fireRate;
    float fireCooldown;
    int cost;
    bool selected;

    sf::Texture texture;
    std::unique_ptr<sf::Sprite> sprite;
    sf::CircleShape rangeCircle;

    std::shared_ptr<Pinata> findTarget(std::vector<std::shared_ptr<Pinata>>& enemies);
    virtual void attack(std::shared_ptr<Pinata>& target);
    void initSprite();
};

#endif