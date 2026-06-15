#include "Enemies/Pinata.hpp"
#include <algorithm>
#include <cmath>

namespace {
    struct PinataStats {
        float speed;
        int health;
        int reward;
    };

    PinataStats getStatsForType(PinataType type) {
        switch (type) {
            case PinataType::ENGRUDO: return {88.f, 55, 8};
            case PinataType::ARCILLA: return {68.f, 150, 14};
            case PinataType::REVELACION: return {85.f, 220, 22};
            case PinataType::FRUTA: return {82.f, 120, 12};
            case PinataType::HIPNOTIZADORA: return {78.f, 180, 16};
            case PinataType::BEBE_ROSA: return {95.f, 45, 4};
            case PinataType::BEBE_AZUL: return {95.f, 45, 4};
            default: return {90.f, 70, 8};
        }
    }

    int getRewardForRound(int baseReward, int round) {
        if (round >= 11) return std::max(1, baseReward * 65 / 100);
        if (round >= 7) return std::max(1, baseReward * 80 / 100);
        return baseReward;
    }
}

Pinata::Pinata(const std::vector<sf::Vector2f>& waypoints, PinataType type, int round)
    : Pinata(waypoints, type, round, waypoints[0], 1) {
}

Pinata::Pinata(
    const std::vector<sf::Vector2f>& waypoints,
    PinataType type,
    int round,
    const sf::Vector2f& startPosition,
    size_t waypointIndex
)
    : Entity(waypoints[0].x, waypoints[0].y),
      type(type),
      speed((getStatsForType(type).speed + static_cast<float>(round) * 2.f) * 1.10f),
      speedMultiplier(1.f),
      slowTimer(0.f),
      health(getStatsForType(type).health + round * 6),
      maxHealth(getStatsForType(type).health + round * 6),
      reward(getRewardForRound(getStatsForType(type).reward, round)),
      reachedEnd(false),
      waypoints(waypoints),
      currentWaypoint(std::max<size_t>(1, waypointIndex)) {
    position = startPosition;

    // Visual temporal: cuadro de colores
    shape.setSize({30.f, 30.f});
    shape.setOrigin({15.f, 15.f});
    shape.setFillColor(getBaseColor());
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
    initSprite();
}

void Pinata::moveTowardsWaypoint(float deltaTime) {
    if (currentWaypoint >= waypoints.size()) {
        reachedEnd = true;
        alive = false;
        return;
    }

    sf::Vector2f target = waypoints[currentWaypoint];
    sf::Vector2f dir = target - position;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    if (dist < 5.f) {
        currentWaypoint++;
        return;
    }

    // Normalizar y mover
    dir /= dist;
    position += dir * speed * speedMultiplier * deltaTime;
}

void Pinata::update(float deltaTime) {
    if (!alive) return;
    if (slowTimer > 0.f) {
        slowTimer -= deltaTime;
        if (slowTimer <= 0.f) {
            speedMultiplier = 1.f;
            applyVisualColor(getBaseColor());
        }
    }
    moveTowardsWaypoint(deltaTime);
    shape.setPosition(position);
    if (sprite) {
        sprite->setPosition(position);
    }
}

void Pinata::applySlow(float multiplier, float duration) {
    if (multiplier > speedMultiplier && slowTimer > 0.f) return;
    speedMultiplier = multiplier;
    slowTimer = duration;
    applyVisualColor(sf::Color(80, 190, 255));
}

sf::Color Pinata::getBaseColor() const {
    switch (type) {
        case PinataType::ENGRUDO: return sf::Color(220, 50, 50);
        case PinataType::ARCILLA: return sf::Color(70, 170, 80);
        case PinataType::REVELACION: return sf::Color(245, 210, 60);
        case PinataType::FRUTA: return sf::Color(245, 130, 40);
        case PinataType::HIPNOTIZADORA: return sf::Color(150, 80, 220);
        case PinataType::BEBE_ROSA: return sf::Color(255, 120, 190);
        case PinataType::BEBE_AZUL: return sf::Color(80, 150, 255);
        default: return sf::Color(220, 50, 50);
    }
}

const char* Pinata::getTexturePath() const {
    switch (type) {
        case PinataType::ENGRUDO:
            return "assets/textures/pinatas/engrudo.png";
        case PinataType::ARCILLA:
            return "assets/textures/pinatas/arcilla.png";
        case PinataType::REVELACION:
            return "assets/textures/pinatas/revelacion.png";
        case PinataType::FRUTA:
            return "assets/textures/pinatas/fruta.png";
        case PinataType::HIPNOTIZADORA:
            return "assets/textures/pinatas/hipnotizadora.png";
        case PinataType::BEBE_ROSA:
            return "assets/textures/pinatas/bebe_rosa.png";
        case PinataType::BEBE_AZUL:
            return "assets/textures/pinatas/bebe_azul.png";
        default:
            return "assets/textures/pinatas/engrudo.png";
    }
}

void Pinata::initSprite() {
    if (!texture.loadFromFile(getTexturePath())) return;

    sprite = std::make_unique<sf::Sprite>(texture);
    const auto size = texture.getSize();
    sprite->setOrigin({static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f});
    sprite->setPosition(position);

    const float maxDimension = static_cast<float>(std::max(size.x, size.y));
    if (maxDimension > 0.f) {
        const float scale = 48.f / maxDimension;
        sprite->setScale({scale, scale});
    }

}

void Pinata::applyVisualColor(const sf::Color& color) {
    shape.setFillColor(color);
    if (!sprite) return;

    if (slowTimer > 0.f) {
        sprite->setColor(sf::Color(130, 215, 255));
    } else {
        sprite->setColor(sf::Color::White);
    }
}

float Pinata::getPathProgress() const {
    if (waypoints.empty()) return 0.f;
    if (currentWaypoint >= waypoints.size()) {
        return static_cast<float>(waypoints.size());
    }

    const sf::Vector2f previous = waypoints[currentWaypoint - 1];
    const sf::Vector2f target = waypoints[currentWaypoint];
    const sf::Vector2f segment = target - previous;
    const sf::Vector2f traveled = position - previous;

    float segmentLength = std::sqrt(segment.x * segment.x + segment.y * segment.y);
    if (segmentLength == 0.f) return static_cast<float>(currentWaypoint);

    float traveledLength = std::sqrt(traveled.x * traveled.x + traveled.y * traveled.y);
    float segmentProgress = std::clamp(traveledLength / segmentLength, 0.f, 1.f);
    return static_cast<float>(currentWaypoint) + segmentProgress;
}

int Pinata::getEscapeDamage() const {
    switch (type) {
        case PinataType::ENGRUDO: return 1;
        case PinataType::BEBE_ROSA: return 1;
        case PinataType::BEBE_AZUL: return 1;
        case PinataType::FRUTA: return 2;
        case PinataType::ARCILLA: return 3;
        case PinataType::REVELACION: return 4;
        case PinataType::HIPNOTIZADORA: return 5;
        default: return 1;
    }
}

void Pinata::render(sf::RenderWindow& window) const {
    if (!alive) return;
    if (sprite) {
        window.draw(*sprite);
    } else {
        window.draw(shape);
    }

    // Barra de vida
    float healthPct = static_cast<float>(health) / maxHealth;
    sf::RectangleShape bgBar({30.f, 5.f});
    bgBar.setFillColor(sf::Color::Red);
    bgBar.setPosition({position.x - 15.f, position.y - 25.f});
    window.draw(bgBar);

    sf::RectangleShape hpBar({30.f * healthPct, 5.f});
    hpBar.setFillColor(sf::Color::Green);
    hpBar.setPosition({position.x - 15.f, position.y - 25.f});
    window.draw(hpBar);
}
