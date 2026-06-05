#include "Game.hpp"
#include "Entity.hpp"
#include <iostream>
#include <sstream>

Game::Game(unsigned int w, unsigned int h) 
    : width(w), height(h), 
      window(sf::VideoMode(sf::Vector2u(w, h)), "Tower Defense Mexicano"),
      currentState(GameState::MENU),
      deltaTime(0.0f),
      playerMoney(500.0f),      // Dinero inicial
      playerLives(100),          // Vidas iniciales
      currentRound(1) {
    
    window.setFramerateLimit(60);
    std::cout << "[GAME] Initialized: " << width << "x" << height << std::endl;
}

void Game::run() {
    std::cout << "[GAME] Starting main loop..." << std::endl;

    while (window.isOpen()) {
        // Calcular deltaTime
        deltaTime = gameClock.restart().asSeconds();

        // Procesar eventos
        handleEvents();

        // Actualizar lógica
        update();

        // Renderizar
        render();
    }

    std::cout << "[GAME] Main loop ended, window closed." << std::endl;
}

void Game::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            // Teclas de debug
            if (keyEvent->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
            // Cambiar estados (debug)
            else if (keyEvent->code == sf::Keyboard::Key::Num1) {
                currentState = GameState::MENU;
                std::cout << "[STATE] Changed to MENU" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num2) {
                currentState = GameState::ROUND_ACTIVE;
                std::cout << "[STATE] Changed to ROUND_ACTIVE" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num3) {
                currentState = GameState::ROUND_PAUSE;
                std::cout << "[STATE] Changed to ROUND_PAUSE" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num4) {
                currentState = GameState::COLLECTING_COINS;
                std::cout << "[STATE] Changed to COLLECTING_COINS" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num5) {
                currentState = GameState::VICTORY;
                std::cout << "[STATE] Changed to VICTORY" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num6) {
                currentState = GameState::DEFEAT;
                std::cout << "[STATE] Changed to DEFEAT" << std::endl;
            }
            // Debug dinero y vidas
            else if (keyEvent->code == sf::Keyboard::Key::M) {
                addPlayerMoney(100);
                std::cout << "[DEBUG] +100 Money. Total: " << playerMoney << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::L) {
                damagePlayer(1);
                std::cout << "[DEBUG] -1 Life. Total: " << playerLives << std::endl;
            }
        }
    }
}

void Game::update() {
    // Actualizar lógica según estado actual
    switch (currentState) {
        case GameState::MENU:
            // TODO: Menú principal
            break;
        case GameState::ROUND_ACTIVE:
            // TODO: Actualizar enemigos, torres, proyectiles
            // TODO: Detectar colisiones
            break;
        case GameState::ROUND_PAUSE:
            // TODO: Permitir compra/venta de torres
            break;
        case GameState::COLLECTING_COINS:
            // TODO: Recolectar monedas de piñata "Bolo Bolo"
            break;
        case GameState::VICTORY:
            // TODO: Mostrar pantalla de victoria
            break;
        case GameState::DEFEAT:
            // TODO: Mostrar pantalla de derrota
            break;
    }

    // Actualizar entidades
    for (auto& entity : entities) {
        if (entity && entity->isAlive()) {
            entity->update(deltaTime);
        }
    }

    // Actualizar info de debug
    updateDebugInfo();
}

void Game::render() {
    window.clear(sf::Color::Black);

    // Renderizar fondo simple según estado
    switch (currentState) {
        case GameState::MENU: {
            sf::RectangleShape bg(sf::Vector2f(width, height));
            bg.setFillColor(sf::Color::Blue);
            window.draw(bg);
            break;
        }
        case GameState::ROUND_ACTIVE: {
            sf::RectangleShape bg(sf::Vector2f(width, height));
            bg.setFillColor(sf::Color::Green);
            window.draw(bg);
            break;
        }
        case GameState::ROUND_PAUSE: {
            sf::RectangleShape bg(sf::Vector2f(width, height));
            bg.setFillColor(sf::Color::Yellow);
            window.draw(bg);
            break;
        }
        case GameState::COLLECTING_COINS: {
            sf::RectangleShape bg(sf::Vector2f(width, height));
            bg.setFillColor(sf::Color::Magenta);
            window.draw(bg);
            break;
        }
        case GameState::VICTORY: {
            sf::RectangleShape bg(sf::Vector2f(width, height));
            bg.setFillColor(sf::Color::Cyan);
            window.draw(bg);
            break;
        }
        case GameState::DEFEAT: {
            sf::RectangleShape bg(sf::Vector2f(width, height));
            bg.setFillColor(sf::Color::Red);
            window.draw(bg);
            break;
        }
    }

    // Renderizar entidades
    for (const auto& entity : entities) {
        if (entity && entity->isAlive()) {
            entity->render(window);
        }
    }

    // Renderizar debug info (texto simple sin cargar fuente por ahora)
    // TODO: Cargar fuente y mostrar HUD

    window.display();
}

void Game::updateDebugInfo() {
    std::ostringstream oss;
    oss << "State: ";
    switch (currentState) {
        case GameState::MENU: oss << "MENU"; break;
        case GameState::ROUND_ACTIVE: oss << "ROUND_ACTIVE"; break;
        case GameState::ROUND_PAUSE: oss << "ROUND_PAUSE"; break;
        case GameState::COLLECTING_COINS: oss << "COLLECTING_COINS"; break;
        case GameState::VICTORY: oss << "VICTORY"; break;
        case GameState::DEFEAT: oss << "DEFEAT"; break;
    }
    oss << " | Round: " << currentRound 
        << " | Money: " << playerMoney 
        << " | Lives: " << playerLives 
        << " | FPS: " << (1.0f / deltaTime);
    
    debugInfo = oss.str();
    std::cout << debugInfo << std::endl;
}
