#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>

// Clase base para todas las entidades del juego (pinatas, torres, proyectiles)
class Entity {
public:
    Entity(float x = 0.0f, float y = 0.0f);
    virtual ~Entity() = default;

    // Metodos virtuales para actualizacion y renderizado
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;

    // Getters y setters
    sf::Vector2f getPosition() const { return position; }
    void setPosition(float x, float y) { position = {x, y}; }
    void setPosition(const sf::Vector2f& pos) { position = pos; }

    sf::Vector2f getVelocity() const { return velocity; }
    void setVelocity(float vx, float vy) { velocity = {vx, vy}; }
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }

    float getRadius() const { return radius; }
    void setRadius(float r) { radius = r; }

    bool isAlive() const { return alive; }
    void setAlive(bool state) { alive = state; }

protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;           // Para colisiones circulares
    bool alive;
};

#endif
