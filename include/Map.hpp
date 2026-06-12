#ifndef MAP_HPP
#define MAP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Map {
public:
    Map(unsigned int windowWidth, unsigned int windowHeight);

    // Obtener waypoints del camino
    const std::vector<sf::Vector2f>& getWaypoints() const { return waypoints; }

    // Renderizar el mapa (camino + fondo)
    void render(sf::RenderWindow& window) const;

    // Validar si una posicion esta sobre el camino
    bool isOnPath(const sf::Vector2f& point) const;

    // Ancho del camino visual
    static constexpr float PATH_WIDTH = 76.0f;

private:
    std::vector<sf::Vector2f> waypoints;
    unsigned int winW, winH;
    sf::Texture backgroundTexture;
    std::unique_ptr<sf::Sprite> backgroundSprite;

    void initWaypoints();
    void initBackground();
    void drawPath(sf::RenderWindow& window) const;
};

#endif
