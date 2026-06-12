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
    void combat(float deltaTime, std::vector<std::shared_ptr<Pinata>>& enemies, float fireRateMultiplier = 1.f);
    void render(sf::RenderWindow& window) const override;

    int getCost() const { return cost; }
    float getRange() const { return range; }
    TowerType getType() const { return towerType; }
    bool isSupportTower() const { return towerType == TowerType::ORGANILLERO; }
    bool isAttackSlowed() const { return attackSlowTimer > 0.f; }
    bool isHypnotized() const { return hypnotized; }
    bool isImmuneToHypnosis() const { return towerType == TowerType::ABUELITA || towerType == TowerType::ORGANILLERO; }
    float getSupportMultiplierScale() const { return isAttackSlowed() ? attackSlowMultiplier : 1.f; }
    bool isInRange(const sf::Vector2f& targetPosition) const;
    void moveTo(float x, float y);
    void setSelected(bool s) { selected = s; }
    void applyAttackSlow(float multiplier, float duration);
    void setHypnotized(bool value);

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
    sf::Vector2f lastTargetPosition;
    float attackEffectTimer;
    float areaEffectTimer;
    float attackSlowTimer;
    float attackSlowMultiplier;
    bool hypnotized;

    std::shared_ptr<Pinata> findTarget(std::vector<std::shared_ptr<Pinata>>& enemies);
    std::shared_ptr<Pinata> findMostAdvancedTarget(std::vector<std::shared_ptr<Pinata>>& enemies);
    bool attackArea(std::shared_ptr<Pinata>& target, std::vector<std::shared_ptr<Pinata>>& enemies);
    virtual bool attack(std::shared_ptr<Pinata>& target);
    bool missesFromHypnosis() const;
    void renderAttackEffect(sf::RenderWindow& window) const;
    sf::Color getAttackEffectColor() const;
    void initSprite();
};

#endif
