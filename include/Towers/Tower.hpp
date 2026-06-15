#ifndef TOWER_HPP
#define TOWER_HPP

#include "Entity.hpp"
#include "Enemies/Pinata.hpp"
#include "TowerType.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <optional>

enum class ProjectileType {
    Lime,
    Chancla,
    Rocket,
    Ice
};

struct ProjectileRequest {
    sf::Vector2f startPosition;
    std::shared_ptr<Pinata> target;
    int damage;
    ProjectileType type;
    float splashRadius;
};

class Tower : public Entity {
public:
    Tower(float x, float y, TowerType type, float range, int damage, float fireRate, int cost);

    void update(float /*deltaTime*/) override {}
    void combat(
        float deltaTime,
        std::vector<std::shared_ptr<Pinata>>& enemies,
        float fireRateMultiplier = 1.f,
        const std::vector<std::shared_ptr<Tower>>* allTowers = nullptr
    );
    void render(sf::RenderWindow& window) const override;

    int getCost() const { return cost; }
    float getRange() const { return range; }
    TowerType getType() const { return towerType; }
    bool isSupportTower() const { return towerType == TowerType::ORGANILLERO; }
    bool isAttackSlowed() const { return attackSlowTimer > 0.f; }
    bool isHypnotized() const { return hypnotized && hypnosisTimer > 0.f; }
    bool isHypnosisProtected() const { return hypnosisProtectionTimer > 0.f; }
    bool isImmuneToHypnosis() const { return towerType == TowerType::ABUELITA || towerType == TowerType::ORGANILLERO; }
    float getSupportMultiplierScale() const { return isAttackSlowed() ? attackSlowMultiplier : 1.f; }
    bool isInRange(const sf::Vector2f& targetPosition) const;
    bool isInScaledRange(const sf::Vector2f& targetPosition, float rangeScale) const;
    void moveTo(float x, float y);
    void setSelected(bool s) { selected = s; }
    void setRangeVisualScale(float scale) { rangeVisualScale = scale; }
    void applyAttackSlow(float multiplier, float duration);
    void setHypnotized(bool value);
    void applyHypnosisProtection(float duration);
    std::optional<ProjectileRequest> consumeProjectileRequest();
    void setLoopAnimationActive(bool active);
    void updateLoopAnimation(float deltaTime);

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
    sf::Texture attackTexture;
    bool hasAttackAnimation;
    bool attackAnimationActive;
    float attackAnimationTimer;
    float attackAnimationDuration;
    int attackAnimationFrame;
    int attackAnimationFrameCount;
    sf::Texture loopTexture;
    bool hasLoopAnimation;
    bool loopAnimationActive;
    float loopAnimationTimer;
    float loopAnimationDuration;
    int loopAnimationFrame;
    int loopAnimationFrameCount;
    sf::CircleShape rangeCircle;
    float rangeVisualScale;
    sf::Vector2f lastTargetPosition;
    float attackEffectTimer;
    float areaEffectTimer;
    float supportActionCooldown;
    float attackSlowTimer;
    float attackSlowMultiplier;
    float hypnosisProtectionTimer;
    float hypnosisTimer;
    bool hypnotized;
    std::optional<ProjectileRequest> pendingProjectileRequest;

    std::shared_ptr<Pinata> findTarget(std::vector<std::shared_ptr<Pinata>>& enemies);
    std::shared_ptr<Pinata> findMostAdvancedTarget(std::vector<std::shared_ptr<Pinata>>& enemies);
    std::shared_ptr<Tower> findNearestHypnotizedTower(const std::vector<std::shared_ptr<Tower>>& allTowers);
    bool clearHypnosisFromTower(const std::shared_ptr<Tower>& target);
    bool attackArea(std::shared_ptr<Pinata>& target, std::vector<std::shared_ptr<Pinata>>& enemies);
    virtual bool attack(std::shared_ptr<Pinata>& target);
    bool missesFromHypnosis() const;
    void renderAttackEffect(sf::RenderWindow& window) const;
    sf::Color getAttackEffectColor() const;
    void initSprite();
    void initAttackAnimation();
    void initLoopAnimation();
    void startAttackAnimation();
    void updateAttackAnimation(float deltaTime);
    void setAttackAnimationFrame(int frame);
    void setLoopAnimationFrame(int frame);
    void resetIdleSprite();
};

#endif
