#include "Enemies/Pinata.hpp"
#include <cmath>

Pinata::Pinata(const std::vector<sf::Vector2f>& waypoints, float speed, int health)
    : Entity(waypoints[0].x, waypoints[0].y),
      speed(speed),
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
    position += dir * speed * deltaTime;
}

void Pinata::update(float deltaTime) {
    if (!alive) return;
    moveTowardsWaypoint(deltaTime);
    shape.setPosition(position);
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