#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>

/**
 * Wrapper para sf::RenderWindow que proporciona una interfaz simplificada
 * Compatible con SFML 3.x
 */
class GameWindow {
public:
    GameWindow(unsigned int width, unsigned int height, const std::string& title)
        : frameRate(60), currentFPS(0.0f) {
        window.create(sf::VideoMode(sf::Vector2u(width, height)), title);
        window.setFramerateLimit(frameRate);
    }

    ~GameWindow() = default;

    // Ventana
    bool isOpen() const {
        return window.isOpen();
    }

    void close() {
        window.close();
    }

    void clear(const sf::Color& color = sf::Color::Black) {
        window.clear(color);
    }

    void display() {
        window.display();
    }

    // Eventos (SFML 3.x usa std::optional<Event>)
    std::optional<sf::Event> pollEvent() {
        return window.pollEvent();
    }

    // Renderizado
    void draw(const sf::Shape& shape) {
        window.draw(shape);
    }

    void draw(const sf::Sprite& sprite) {
        window.draw(sprite);
    }

    void draw(const sf::Text& text) {
        window.draw(text);
    }

    void draw(const sf::Drawable& drawable) {
        window.draw(drawable);
    }

    // Informacion
    sf::Vector2u getSize() const {
        return window.getSize();
    }

    sf::Vector2f getSizeF() const {
        return sf::Vector2f(static_cast<float>(window.getSize().x),
                           static_cast<float>(window.getSize().y));
    }

    void setFramerateLimit(unsigned int limit) {
        frameRate = limit;
        window.setFramerateLimit(limit);
    }

    unsigned int getFramerateLimit() const {
        return frameRate;
    }

    // Vista/Camara
    void setView(const sf::View& view) {
        window.setView(view);
    }

    sf::View getView() const {
        return window.getView();
    }

    // Acceso directo a la ventana (si se necesita)
    sf::RenderWindow& getWindow() {
        return window;
    }

    const sf::RenderWindow& getWindow() const {
        return window;
    }

private:
    sf::RenderWindow window;
    unsigned int frameRate;
    float currentFPS;
};

#endif // GAMEWINDOW_HPP
