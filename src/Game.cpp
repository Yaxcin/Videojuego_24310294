#include "Game.hpp"
#include "Entity.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

Game::Game(unsigned int w, unsigned int h) 
    : width(w), height(h), 
      window(sf::VideoMode(sf::Vector2u(w, h)), "Tower Defense Mexicano"),
      currentState(GameState::MENU),
      deltaTime(0.0f),
      playerMoney(500.0f),
      playerLives(100),
      currentRound(1),
      map(w, h) {
    
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
            else if (keyEvent->code == sf::Keyboard::Key::E) {
               enemies.push_back(std::make_shared<Pinata>(map.getWaypoints()));
               std::cout << "[DEBUG] Spawned pinata. Total: " << enemies.size() << std::endl;
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
    // Actualizar enemigos
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->update(deltaTime);
        }
        // Si llegó al final, quitar vida
        if (enemy && enemy->hasReachedEnd()) {
            damagePlayer(1);
        }
    }
    // Limpiar enemigos muertos
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::shared_ptr<Pinata>& e) { return !e || !e->isAlive(); }),
        enemies.end()
    );
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

    // Renderizar mapa (fondo + camino)
    map.render(window);
    // Renderizar enemigos
for (const auto& enemy : enemies) {
    if (enemy && enemy->isAlive()) {
        enemy->render(window);
    }
}

    // Renderizar entidades
    for (const auto& entity : entities) {
        if (entity && entity->isAlive()) {
            entity->render(window);
        }
    }

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
