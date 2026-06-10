#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Map {
public:
    Map(unsigned int windowWidth, unsigned int windowHeight);

    // Obtener waypoints del camino
    const std::vector<sf::Vector2f>& getWaypoints() const { return waypoints; }

    // Renderizar el mapa (camino + fondo)
    void render(sf::RenderWindow& window) const;

    // Ancho del camino visual
    static constexpr float PATH_WIDTH = 60.0f;

private:
    std::vector<sf::Vector2f> waypoints;
    unsigned int winW, winH;

    void initWaypoints();
    void drawPath(sf::RenderWindow& window) const;
};

#endif