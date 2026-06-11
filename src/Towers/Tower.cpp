#include "Towers/Tower.hpp"
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

void Tower::combat(float deltaTime, std::vector<std::shared_ptr<Pinata>>& enemies) {
    if (fireCooldown > 0.f) {
        fireCooldown -= deltaTime;
        return;
    }
    auto target = findTarget(enemies);
    if (target) {
        attack(target);
        fireCooldown = 1.f / fireRate;
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

void Tower::attack(std::shared_ptr<Pinata>& target) {
    target->takeDamage(damage);
}

void Tower::render(sf::RenderWindow& window) const {
    if (selected) {
        window.draw(rangeCircle);
    }
    if (sprite) {
        window.draw(*sprite);
    }
}  
