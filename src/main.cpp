/// Tower Defense Mexicano
/// Juego de tower defense con temática mexicana
/// Basado en Bloons Tower Defense pero con piñatas y personajes mexicanos

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "GameWindow.hpp"
#include "GameState.hpp"

int main() {
    try {
        std::cout << "=== Tower Defense Mexicano ===" << std::endl;
        std::cout << "Inicializando juego..." << std::endl;

        // Crear ventana del juego
        GameWindow window(1280, 720, "Tower Defense Mexicano - Fase 1");

        // Estado del juego
        GameState currentState = GameState::MENU;

        // Variables del juego
        float playerMoney = 500.0f;
        int playerLives = 100;
        int currentRound = 1;

        std::cout << "\n--- Controles Debug ---" << std::endl;
        std::cout << "ESC: Cerrar juego" << std::endl;
        std::cout << "1-6: Cambiar estado" << std::endl;
        std::cout << "M: +100 dinero | L: -1 vida" << std::endl;
        std::cout << "----------------------------\n" << std::endl;

        // Bucle principal
        while (window.isOpen()) {
            // Procesar eventos
            while (auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
                else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyEvent->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
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
                    else if (keyEvent->code == sf::Keyboard::Key::M) {
                        playerMoney += 100;
                        std::cout << "[DEBUG] Money: " << playerMoney << std::endl;
                    }
                    else if (keyEvent->code == sf::Keyboard::Key::L) {
                        playerLives -= 1;
                        std::cout << "[DEBUG] Lives: " << playerLives << std::endl;
                    }
                }
            }

            // Renderizar
            window.clear(sf::Color::Black);

            // Dibujar fondo según estado
            sf::RectangleShape bg(window.getSizeF());
            switch (currentState) {
                case GameState::MENU:
                    bg.setFillColor(sf::Color::Blue);
                    break;
                case GameState::ROUND_ACTIVE:
                    bg.setFillColor(sf::Color::Green);
                    break;
                case GameState::ROUND_PAUSE:
                    bg.setFillColor(sf::Color::Yellow);
                    break;
                case GameState::COLLECTING_COINS:
                    bg.setFillColor(sf::Color::Magenta);
                    break;
                case GameState::VICTORY:
                    bg.setFillColor(sf::Color::Cyan);
                    break;
                case GameState::DEFEAT:
                    bg.setFillColor(sf::Color::Red);
                    break;
            }
            window.draw(bg);
            window.display();
        }

        std::cout << "\nJuego cerrado correctamente." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
