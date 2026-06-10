#include "Map.hpp"
#include <cmath>

Map::Map(unsigned int windowWidth, unsigned int windowHeight)
    : winW(windowWidth), winH(windowHeight) {
    initWaypoints();
}

void Map::initWaypoints() {
    // Camino en zigzag estilo Bloons, de izquierda a derecha
    // Ajustado a resolución 1280x720
    float w = static_cast<float>(winW);
    float h = static_cast<float>(winH);

    waypoints = {
        {-50.f,        h * 0.20f},   // Entrada (fuera de pantalla)
        {w * 0.20f,    h * 0.20f},   // Tramo 1 - horizontal
        {w * 0.20f,    h * 0.75f},   // Tramo 2 - baja
        {w * 0.45f,    h * 0.75f},   // Tramo 3 - horizontal
        {w * 0.45f,    h * 0.25f},   // Tramo 4 - sube
        {w * 0.70f,    h * 0.25f},   // Tramo 5 - horizontal
        {w * 0.70f,    h * 0.75f},   // Tramo 6 - baja
        {w + 50.f,     h * 0.75f},   // Salida (fuera de pantalla)
    };
}

void Map::drawPath(sf::RenderWindow& window) const {
    if (waypoints.size() < 2) return;

    for (size_t i = 0; i + 1 < waypoints.size(); ++i) {
        sf::Vector2f a = waypoints[i];
        sf::Vector2f b = waypoints[i + 1];

        sf::Vector2f dir = b - a;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len == 0.f) continue;

        // Rectángulo orientado entre dos waypoints
        sf::RectangleShape segment(sf::Vector2f(len, PATH_WIDTH));
        segment.setOrigin({0.f, PATH_WIDTH / 2.f});
        segment.setPosition(a);
        segment.setRotation(sf::radians(std::atan2(dir.y, dir.x)));
        segment.setFillColor(sf::Color(180, 140, 80));       // Color tierra/arena
        segment.setOutlineColor(sf::Color(120, 90, 40));
        segment.setOutlineThickness(2.f);
        window.draw(segment);
    }

    // Dibujar círculos en las esquinas para que no queden huecos
    for (const auto& wp : waypoints) {
        sf::CircleShape corner(PATH_WIDTH / 2.f);
        corner.setOrigin({PATH_WIDTH / 2.f, PATH_WIDTH / 2.f});
        corner.setPosition(wp);
        corner.setFillColor(sf::Color(180, 140, 80));
        window.draw(corner);
    }
}

void Map::render(sf::RenderWindow& window) const {
    // Fondo verde (pasto mexicano 🌿)
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(winW), static_cast<float>(winH)));
    bg.setFillColor(sf::Color(60, 120, 40));
    window.draw(bg);

    // Camino
    drawPath(window);
}