#include "Towers/Tower.hpp"
#include <algorithm>
#include <cmath>

Tower::Tower(float x, float y, TowerType type, float range, int damage, float fireRate, int cost)
    : Entity(x, y), towerType(type), range(range), damage(damage),
      fireRate(fireRate), fireCooldown(0.f), cost(cost), selected(false) {

    rangeCircle.setRadius(range);
    rangeCircle.setOrigin({range, range});
    rangeCircle.setPosition(position);
    rangeCircle.setFillColor(sf::Color(255, 255, 255, 30));
    rangeCircle.setOutlineColor(sf::Color(255, 255, 255, 100));
    rangeCircle.setOutlineThickness(1.f);

    initSprite();
}

void Tower::initSprite() {
    std::string path = getTowerTexturePath(towerType);
    if (texture.loadFromFile(path)) {
        sprite = std::make_unique<sf::Sprite>(texture);
        sprite->setOrigin(sf::Vector2f(32.f, 32.f)); // centro 64x64
        sprite->setPosition(position);
    }
}

void Tower::combat(float deltaTime, std::vector<std::shared_ptr<Pinata>>& enemies, float fireRateMultiplier) {
    if (isSupportTower()) return;

    float effectiveMultiplier = std::max(1.f, fireRateMultiplier);
    if (fireCooldown > 0.f) {
        fireCooldown -= deltaTime * effectiveMultiplier;
        return;
    }

    auto target = (towerType == TowerType::ABUELITA)
        ? findMostAdvancedTarget(enemies)
        : findTarget(enemies);

    if (target) {
        if (towerType == TowerType::DON_COHETES) {
            attackArea(target, enemies);
        } else {
            attack(target);
        }
        fireCooldown = 1.f / fireRate;
    }
}

bool Tower::isInRange(const sf::Vector2f& targetPosition) const {
    sf::Vector2f diff = targetPosition - position;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    return dist <= range;
}

void Tower::moveTo(float x, float y) {
    position = {x, y};
    rangeCircle.setPosition(position);
    if (sprite) {
        sprite->setPosition(position);
    }
}

std::shared_ptr<Pinata> Tower::findTarget(std::vector<std::shared_ptr<Pinata>>& enemies) {
    std::shared_ptr<Pinata> nearest = nullptr;
    float minDist = range;

    for (auto& e : enemies) {
        if (!e || !e->isAlive()) continue;
        sf::Vector2f diff = e->getPosition() - position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= minDist) {
            minDist = dist;
            nearest = e;
        }
    }
    return nearest;
}

std::shared_ptr<Pinata> Tower::findMostAdvancedTarget(std::vector<std::shared_ptr<Pinata>>& enemies) {
    std::shared_ptr<Pinata> mostAdvanced = nullptr;
    float bestProgress = -1.f;

    for (auto& e : enemies) {
        if (!e || !e->isAlive() || !isInRange(e->getPosition())) continue;
        float progress = e->getPathProgress();
        if (progress > bestProgress) {
            bestProgress = progress;
            mostAdvanced = e;
        }
    }
    return mostAdvanced;
}

void Tower::attack(std::shared_ptr<Pinata>& target) {
    target->takeDamage(damage);
    if (towerType == TowerType::RASPADERO) {
        target->applySlow(0.5f, 2.f);
    }
}

void Tower::attackArea(std::shared_ptr<Pinata>& target, std::vector<std::shared_ptr<Pinata>>& enemies) {
    constexpr float splashRadius = 70.f;
    constexpr int maxTargets = 10;
    int hits = 0;

    for (auto& e : enemies) {
        if (!e || !e->isAlive()) continue;
        sf::Vector2f diff = e->getPosition() - target->getPosition();
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= splashRadius) {
            e->takeDamage(damage);
            hits++;
            if (hits >= maxTargets) return;
        }
    }
}

void Tower::render(sf::RenderWindow& window) const {
    if (selected) {
        window.draw(rangeCircle);
    }
    if (sprite) {
        window.draw(*sprite);
    }
}  
