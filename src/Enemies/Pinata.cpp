#include "Enemies/Pinata.hpp"
#include <algorithm>
#include <cmath>

Pinata::Pinata(const std::vector<sf::Vector2f>& waypoints, float speed, int health)
    : Entity(waypoints[0].x, waypoints[0].y),
      speed(speed),
      speedMultiplier(1.f),
      slowTimer(0.f),
      health(health),
      maxHealth(health),
      reward(10),
      reachedEnd(false),
      waypoints(waypoints),
      currentWaypoint(1) {

    // Visual temporal — cuadro de colores
    shape.setSize({30.f, 30.f});
    shape.setOrigin({15.f, 15.f});
    shape.setFillColor(sf::Color(220, 50, 50));
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
        }
    }
    moveTowardsWaypoint(deltaTime);
    shape.setPosition(position);
}

void Pinata::applySlow(float multiplier, float duration) {
    if (multiplier > speedMultiplier && slowTimer > 0.f) return;
    speedMultiplier = multiplier;
    slowTimer = duration;
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
