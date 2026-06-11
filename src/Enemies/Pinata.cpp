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
            case PinataType::ENGRUDO: return {90.f, 70, 8};
            case PinataType::ARCILLA: return {68.f, 150, 14};
            case PinataType::REVELACION: return {85.f, 220, 22};
            case PinataType::FRUTA: return {82.f, 120, 12};
            case PinataType::HIPNOTIZADORA: return {78.f, 140, 16};
            case PinataType::BEBE_ROSA: return {95.f, 45, 4};
            case PinataType::BEBE_AZUL: return {95.f, 45, 4};
            default: return {90.f, 70, 8};
        }
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
      speed(getStatsForType(type).speed + static_cast<float>(round) * 2.f),
      speedMultiplier(1.f),
      slowTimer(0.f),
      health(getStatsForType(type).health + round * 12),
      maxHealth(getStatsForType(type).health + round * 12),
      reward(getStatsForType(type).reward),
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
            shape.setFillColor(getBaseColor());
        }
    }
    moveTowardsWaypoint(deltaTime);
    shape.setPosition(position);
}

void Pinata::applySlow(float multiplier, float duration) {
    if (multiplier > speedMultiplier && slowTimer > 0.f) return;
    speedMultiplier = multiplier;
    slowTimer = duration;
    shape.setFillColor(sf::Color(80, 190, 255));
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

void Pinata::render(sf::RenderWindow& window) const {
    if (!alive) return;
    window.draw(shape);

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
