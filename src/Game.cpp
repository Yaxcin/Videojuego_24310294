#include "Game.hpp"
#include "Entity.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace {
    enum class MenuAction {
        None,
        Play,
        Shop,
        Options,
        Exit
    };

    MenuAction getMenuActionAt(float x, float y) {
        const sf::FloatRect playButton({960.f, 350.f}, {270.f, 90.f});
        const sf::FloatRect shopButton({980.f, 485.f}, {225.f, 80.f});
        const sf::FloatRect optionsButton({970.f, 585.f}, {240.f, 75.f});
        const sf::FloatRect exitButton({980.f, 685.f}, {230.f, 70.f});

        if (playButton.contains({x, y})) return MenuAction::Play;
        if (shopButton.contains({x, y})) return MenuAction::Shop;
        if (optionsButton.contains({x, y})) return MenuAction::Options;
        if (exitButton.contains({x, y})) return MenuAction::Exit;
        return MenuAction::None;
    }

    int getTowerCost(TowerType type) {
        switch (type) {
            case TowerType::NINO_PALO: return 75;
            case TowerType::VIEJO_MACHETE: return 100;
            case TowerType::TAQUERO: return 125;
            case TowerType::ABUELITA: return 250;
            case TowerType::DON_COHETES: return 200;
            case TowerType::ORGANILLERO: return 160;
            case TowerType::RASPADERO: return 120;
            default: return 0;
        }
    }

    float distanceBetween(const sf::Vector2f& a, const sf::Vector2f& b) {
        sf::Vector2f diff = a - b;
        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
    }

    PinataType getSpawnTypeForRound(int round, int spawnedIndex) {
        if (round >= 5 && spawnedIndex % 11 == 0) return PinataType::HIPNOTIZADORA;
        if (round >= 4 && spawnedIndex % 7 == 0) return PinataType::REVELACION;
        if (round >= 3 && spawnedIndex % 5 == 0) return PinataType::FRUTA;
        if (round >= 2 && spawnedIndex % 3 == 0) return PinataType::ARCILLA;
        return PinataType::ENGRUDO;
    }
}

Game::Game(unsigned int w, unsigned int h) 
    : width(w), height(h), 
      window(sf::VideoMode(sf::Vector2u(w, h)), "Tower Defense Mexicano"),
      currentState(GameState::MENU),
      deltaTime(0.0f),
      playerMoney(500.0f),
      playerLives(100),
      currentRound(1),
      enemiesLeftToSpawn(0),
      spawnTimer(0.f),
      spawnInterval(1.5f),
      map(w, h),
      selectedTowerType(TowerType::NINO_PALO),
      towerSelected(false),
      selectedPlacedTower(nullptr) {
    
    window.setFramerateLimit(60);
    TextureManager::getInstance().loadAllTowerTextures();
    if (!hudFont.openFromFile("assets/fonts/arial.ttf")) {
    std::cerr << "[FONT] No se pudo cargar la fuente" << std::endl;
   }
   if (menuTexture.loadFromFile("assets/textures/MENU.png")) {
    menuSprite = new sf::Sprite(menuTexture);
    // Escalar para cubrir 1280x720
    float scaleX = static_cast<float>(width) / menuTexture.getSize().x;
    float scaleY = static_cast<float>(height) / menuTexture.getSize().y;
    menuSprite->setScale({scaleX, scaleY});
    }
    std::cout << "[GAME] Initialized: "     << width << "x" << height << std::endl;
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
                enemies.push_back(std::make_shared<Pinata>(map.getWaypoints(), PinataType::ENGRUDO, currentRound));
                std::cout << "[DEBUG] Pinata spawned. Total: " << enemies.size() << std::endl;
            }
            else if (keyEvent->code == sf::Keyboard::Key::Space) {
             if (currentState == GameState::ROUND_PAUSE || currentState == GameState::MENU) {
             startWave();
                 }
            }
        }
        else if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
    sf::Vector2f mousePos = window.mapPixelToCoords(mouseEvent->position);
    float mx = mousePos.x;
    float my = mousePos.y;

    if (currentState == GameState::MENU) {
        sf::Vector2u menuSize = menuTexture.getSize();
        float menuX = mx * static_cast<float>(menuSize.x) / static_cast<float>(width);
        float menuY = my * static_cast<float>(menuSize.y) / static_cast<float>(height);
        MenuAction action = getMenuActionAt(menuX, menuY);
        switch (action) {
            case MenuAction::Play:
                currentState = GameState::ROUND_PAUSE;
                std::cout << "[MENU] Iniciar fiesta" << std::endl;
                break;
            case MenuAction::Shop:
                std::cout << "[MENU] Tienda aun no implementada" << std::endl;
                break;
            case MenuAction::Options:
                std::cout << "[MENU] Opciones aun no implementadas" << std::endl;
                break;
            case MenuAction::Exit:
                window.close();
                break;
            case MenuAction::None:
                break;
        }
        return;
    }

    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;

            if (mouseEvent->button == sf::Mouse::Button::Right) {
                towerSelected = false;
                clearTowerSelection();
                std::cout << "[UI] Seleccion cancelada" << std::endl;
            }
            else if (mouseEvent->button == sf::Mouse::Button::Left) {
                std::cout << "[MOUSE] x=" << mx << " y=" << my << std::endl;
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
                        clearTowerSelection();
                        if (towerSelected && selectedTowerType == tipos[slot]) {
                            towerSelected = false;
                            std::cout << "[UI] Torre deseleccionada" << std::endl;
                        } else {
                            selectedTowerType = tipos[slot];
                            towerSelected = true;
                            std::cout << "[UI] Seleccionada: " << getTowerName(selectedTowerType) << std::endl;
                        }
                    }
                } else {
                    auto clickedTower = findTowerAt(mx, my);
                    if (currentState == GameState::ROUND_PAUSE && selectedPlacedTower) {
                        if (clickedTower == selectedPlacedTower) {
                            clearTowerSelection();
                            std::cout << "[UI] Torre deseleccionada" << std::endl;
                        } else {
                            moveSelectedTower(mx, my);
                        }
                    } else if (currentState == GameState::ROUND_PAUSE && clickedTower) {
                        towerSelected = false;
                        clearTowerSelection();
                        selectedPlacedTower = clickedTower;
                        selectedPlacedTower->setSelected(true);
                        std::cout << "[UI] Torre colocada seleccionada para mover" << std::endl;
                    } else if (towerSelected) {
                        // Click en mapa - colocar torre
                        placeTower(mx, my);
                    }
                }
            }
        }
    }
}

void Game::update() {
    switch (currentState) {
        case GameState::ROUND_ACTIVE:
    // Spawnear enemigos automaticamente
    if (enemiesLeftToSpawn > 0) {
        spawnTimer -= deltaTime;
        if (spawnTimer <= 0.f) {
            enemies.push_back(std::make_shared<Pinata>(
                map.getWaypoints(),
                getSpawnTypeForRound(currentRound, enemiesLeftToSpawn),
                currentRound
            ));
            enemiesLeftToSpawn--;
            spawnTimer = spawnInterval;
            std::cout << "[WAVE] Pinata spawned. Quedan: " << enemiesLeftToSpawn << std::endl;
        }
    }
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) enemy->update(deltaTime);
        if (enemy && enemy->hasReachedEnd()) damagePlayer(1);
    }
    for (auto& tower : towers) {
        if (!tower) continue;

        float fireRateMultiplier = 1.f;
        if (!tower->isSupportTower()) {
            for (const auto& support : towers) {
                if (support && support->isSupportTower() && support->isInRange(tower->getPosition())) {
                    fireRateMultiplier = (tower->getType() == TowerType::ABUELITA) ? 1.05f : 1.35f;
                    break;
                }
            }
        }
        tower->combat(deltaTime, enemies, fireRateMultiplier);
    }
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [this](const std::shared_ptr<Pinata>& e) {
                if (!e) return true;
                if (e->isAlive()) return false;
                if (!e->hasReachedEnd()) {
                    addPlayerMoney(static_cast<float>(e->getReward()));
                    std::cout << "[REWARD] +" << e->getReward()
                              << " dulces. Total: " << playerMoney << std::endl;
                }
                return true;
            }),
        enemies.end()
    );
    // Oleada terminada
    if (enemiesLeftToSpawn == 0 && enemies.empty()) {
        currentRound++;
        addPlayerMoney(50 + currentRound * 10);
        currentState = GameState::ROUND_PAUSE;
        std::cout << "[WAVE] Oleada completada! Ronda: " << currentRound << std::endl;
    }
    break;
        default:
            break;
    }

    for (auto& entity : entities) {
        if (entity && entity->isAlive()) entity->update(deltaTime);
    }

    updateDebugInfo();
}

void Game::render() {
    window.clear(sf::Color::Black);
    if (currentState == GameState::MENU && menuSprite) {
    window.draw(*menuSprite);
    window.display();
    return;
}

    map.render(window);

    for (const auto& tower : towers) {
        if (tower) tower->render(window);
    }

    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) enemy->render(window);
    }

    renderPanel();
    renderHUD();

    window.display();
}

void Game::placeTower(float x, float y) {
    if (currentState != GameState::ROUND_PAUSE) {
        std::cout << "[UI] No puedes colocar torres durante la oleada" << std::endl;
        return;
    }

    sf::Vector2f towerPosition{x, y};
    if (!isValidTowerPosition(towerPosition)) return;

    std::shared_ptr<Tower> tower;
    int cost = 0;

    switch (selectedTowerType) {
        case TowerType::NINO_PALO:
            tower = std::make_shared<NinoPalo>(x, y);
            break;
        case TowerType::VIEJO_MACHETE:
            tower = std::make_shared<ViejoMachete>(x, y);
            break;
        case TowerType::TAQUERO:
            tower = std::make_shared<Taquero>(x, y);
            break;
        case TowerType::ABUELITA:
            tower = std::make_shared<Abuelita>(x, y);
            break;
        case TowerType::DON_COHETES:
            tower = std::make_shared<DonCohetes>(x, y);
            break;
        case TowerType::ORGANILLERO:
            tower = std::make_shared<Organillero>(x, y);
            break;
        case TowerType::RASPADERO:
            tower = std::make_shared<Raspadero>(x, y);
            break;
        default:
            std::cout << "[UI] Torre aun no implementada" << std::endl;
            return;
    }

    cost = tower->getCost();

    if (playerMoney >= cost) {
        playerMoney -= cost;
        towers.push_back(tower);
        std::cout << "[TOWER] Colocada en (" << x << ", " << y << "). Dinero: " << playerMoney << std::endl;
    } else {
        std::cout << "[UI] Dinero insuficiente!" << std::endl;
    }
}

void Game::moveSelectedTower(float x, float y) {
    if (!selectedPlacedTower) return;

    sf::Vector2f newPosition{x, y};
    if (!isValidTowerPosition(newPosition, selectedPlacedTower)) return;

    selectedPlacedTower->moveTo(x, y);
    std::cout << "[TOWER] Movida a (" << x << ", " << y << ")" << std::endl;
}

bool Game::isValidTowerPosition(const sf::Vector2f& position, const std::shared_ptr<Tower>& ignoredTower) const {
    if (map.isOnPath(position)) {
        std::cout << "[UI] No puedes colocar torres sobre el camino" << std::endl;
        return false;
    }

    constexpr float minTowerDistance = 55.f;
    for (const auto& existingTower : towers) {
        if (!existingTower || existingTower == ignoredTower) continue;
        if (distanceBetween(existingTower->getPosition(), position) < minTowerDistance) {
            std::cout << "[UI] No puedes colocar torres tan juntas" << std::endl;
            return false;
        }
    }

    return true;
}

std::shared_ptr<Tower> Game::findTowerAt(float x, float y) const {
    sf::Vector2f clickPosition{x, y};
    constexpr float selectRadius = 34.f;

    for (auto it = towers.rbegin(); it != towers.rend(); ++it) {
        if (*it && distanceBetween((*it)->getPosition(), clickPosition) <= selectRadius) {
            return *it;
        }
    }

    return nullptr;
}

void Game::clearTowerSelection() {
    if (selectedPlacedTower) {
        selectedPlacedTower->setSelected(false);
        selectedPlacedTower = nullptr;
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

        sf::Text costText(hudFont, "$" + std::to_string(getTowerCost(tipos[i])), 20);
        costText.setFillColor(sf::Color(255, 215, 0));
        costText.setPosition({mapWidth + 105.f, slotY + 32.f});
        window.draw(costText);
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

void Game::startWave() {
    towerSelected = false;
    clearTowerSelection();
    currentState = GameState::ROUND_ACTIVE;
    enemiesLeftToSpawn = 5 + currentRound * 3;
    spawnInterval = std::max(0.4f, 1.5f - currentRound * 0.1f);
    spawnTimer = 0.f;
    std::cout << "[WAVE] Oleada " << currentRound << " iniciada! Enemigos: " 
              << enemiesLeftToSpawn << std::endl;
}
void Game::renderHUD() {
    // Barra de fondo
    sf::RectangleShape hudBar(sf::Vector2f(static_cast<float>(width) - PANEL_WIDTH, 40.f));
    hudBar.setPosition({0.f, 0.f});
    hudBar.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(hudBar);

    // Dinero
    sf::Text moneyText(hudFont, "$" + std::to_string(static_cast<int>(playerMoney)), 20);
    moneyText.setFillColor(sf::Color(255, 215, 0)); // dorado
    moneyText.setPosition({10.f, 8.f});
    window.draw(moneyText);

    // Vidas
    sf::Text livesText(hudFont, "Vidas: " + std::to_string(playerLives), 20);
    livesText.setFillColor(sf::Color(255, 80, 80)); // rojo
    livesText.setPosition({150.f, 8.f});
    window.draw(livesText);

    // Ronda
    sf::Text roundText(hudFont, "Ronda: " + std::to_string(currentRound), 20);
    roundText.setFillColor(sf::Color::White);
    roundText.setPosition({320.f, 8.f});
    window.draw(roundText);

    // Mensaje de estado
    std::string msg = "";
    if (currentState == GameState::MENU || currentState == GameState::ROUND_PAUSE) {
        msg = "SPACE: Iniciar oleada";
    } else if (currentState == GameState::ROUND_ACTIVE) {
        msg = "Enemigos: " + std::to_string(enemies.size() + enemiesLeftToSpawn);
    } else if (currentState == GameState::DEFEAT) {
        msg = "DERROTA - ESC para salir";
    } else if (currentState == GameState::VICTORY) {
        msg = "VICTORIA!";
    }

    if (!msg.empty()) {
        sf::Text stateText(hudFont, msg, 20);
        stateText.setFillColor(sf::Color::Yellow);
        stateText.setPosition({480.f, 8.f});
        window.draw(stateText);
    }
}
