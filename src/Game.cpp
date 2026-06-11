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
      map(w, h),
      towerSelected(false),
      selectedTowerType(TowerType::NINO_PALO) {
    
    window.setFramerateLimit(60);
    TextureManager::getInstance().loadAllTowerTextures();
    std::cout << "[GAME] Initialized: " << width << "x" << height << std::endl;
}

void Game::run() {
    std::cout << "[GAME] Starting main loop..." << std::endl;
    while (window.isOpen()) {
        deltaTime = gameClock.restart().asSeconds();
        handleEvents();
        update();
        render();
    }
    std::cout << "[GAME] Main loop ended." << std::endl;
}

void Game::handleEvents() {
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
                std::cout << "[STATE] MENU" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num2) {
                currentState = GameState::ROUND_ACTIVE;
                std::cout << "[STATE] ROUND_ACTIVE" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num3) {
                currentState = GameState::ROUND_PAUSE;
                std::cout << "[STATE] ROUND_PAUSE" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num4) {
                currentState = GameState::COLLECTING_COINS;
                std::cout << "[STATE] COLLECTING_COINS" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num5) {
                currentState = GameState::VICTORY;
                std::cout << "[STATE] VICTORY" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Num6) {
                currentState = GameState::DEFEAT;
                std::cout << "[STATE] DEFEAT" << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::M) {
                addPlayerMoney(100);
                std::cout << "[DEBUG] +100 Money. Total: " << playerMoney << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::L) {
                damagePlayer(1);
                std::cout << "[DEBUG] -1 Life. Total: " << playerLives << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::E) {
                enemies.push_back(std::make_shared<Pinata>(map.getWaypoints()));
                std::cout << "[DEBUG] Pinata spawned. Total: " << enemies.size() << std::endl;
            }
        }
        else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            float mx = static_cast<float>(mouseEvent->position.x);
            float my = static_cast<float>(mouseEvent->position.y);
            float mapWidth = static_cast<float>(width) - PANEL_WIDTH;

            if (mouseEvent->button == sf::Mouse::Button::Left) {
                if (mx >= mapWidth) {
                    // Click en panel - seleccionar torre
                    TowerType tipos[] = {
                        TowerType::NINO_PALO, TowerType::VIEJO_MACHETE,
                        TowerType::TAQUERO, TowerType::ABUELITA,
                        TowerType::DON_COHETES, TowerType::ORGANILLERO,
                        TowerType::RASPADERO
                    };
                    int slot = static_cast<int>(my / 95.f);
                    if (slot >= 0 && slot < 7) {
                        selectedTowerType = tipos[slot];
                        towerSelected = true;
                        std::cout << "[UI] Seleccionada: " << getTowerName(selectedTowerType) << std::endl;
                    }
                } else if (towerSelected) {
                    // Click en mapa - colocar torre
                    placeTower(mx, my);
                }
            }
        }
    }
}

void Game::update() {
    switch (currentState) {
        case GameState::ROUND_ACTIVE:
            // Actualizar enemigos
            for (auto& enemy : enemies) {
                if (enemy && enemy->isAlive()) enemy->update(deltaTime);
                if (enemy && enemy->hasReachedEnd()) {
                    damagePlayer(1);
                }
            }
            // Limpiar enemigos muertos/llegaron al final
            enemies.erase(
                std::remove_if(enemies.begin(), enemies.end(),
                    [](const std::shared_ptr<Pinata>& e) { return !e || !e->isAlive(); }),
                enemies.end()
            );
            // Torres atacan
            for (auto& tower : towers) {
                if (tower) tower->combat(deltaTime, enemies);
            }
            break;
        case GameState::MENU:
        case GameState::ROUND_PAUSE:
        case GameState::COLLECTING_COINS:
        case GameState::VICTORY:
        case GameState::DEFEAT:
            break;
    }

    for (auto& entity : entities) {
        if (entity && entity->isAlive()) entity->update(deltaTime);
    }

    updateDebugInfo();
}

void Game::render() {
    window.clear(sf::Color::Black);

    map.render(window);

    for (const auto& tower : towers) {
        if (tower) tower->render(window);
    }

    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) enemy->render(window);
    }

    renderPanel();

    window.display();
}

void Game::placeTower(float x, float y) {
    std::shared_ptr<Tower> tower;
    int cost = 0;

    switch (selectedTowerType) {
        case TowerType::NINO_PALO:
            tower = std::make_shared<NinoPalo>(x, y);
            cost = 75;
            break;
        default:
            std::cout << "[UI] Torre aun no implementada" << std::endl;
            return;
    }

    if (playerMoney >= cost) {
        playerMoney -= cost;
        towers.push_back(tower);
        std::cout << "[TOWER] Colocada en (" << x << ", " << y << "). Dinero: " << playerMoney << std::endl;
    } else {
        std::cout << "[UI] Dinero insuficiente!" << std::endl;
    }
}

void Game::renderPanel() {
    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;

    sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, static_cast<float>(height)));
    panel.setPosition({mapWidth, 0.f});
    panel.setFillColor(sf::Color(20, 20, 20, 230));
    window.draw(panel);

    TowerType tipos[] = {
        TowerType::NINO_PALO, TowerType::VIEJO_MACHETE,
        TowerType::TAQUERO, TowerType::ABUELITA,
        TowerType::DON_COHETES, TowerType::ORGANILLERO,
        TowerType::RASPADERO
    };

    for (int i = 0; i < 7; i++) {
        float slotY = i * 95.f + 5.f;
        bool isSelected = towerSelected && selectedTowerType == tipos[i];

        sf::RectangleShape slot(sf::Vector2f(188.f, 88.f));
        slot.setPosition({mapWidth + 6.f, slotY});
        slot.setFillColor(sf::Color(50, 50, 50));
        if (isSelected) {
            slot.setOutlineColor(sf::Color::Yellow);
            slot.setOutlineThickness(3.f);
        }
        window.draw(slot);

        sf::Texture* tex = TextureManager::getInstance().getTowerTexture(tipos[i]);
        if (tex) {
            sf::Sprite spr(*tex);
            spr.setOrigin({32.f, 32.f});
            spr.setPosition({mapWidth + 50.f, slotY + 44.f});
            window.draw(spr);
        }
    }
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
        << " | Lives: " << playerLives;
    debugInfo = oss.str();
}
