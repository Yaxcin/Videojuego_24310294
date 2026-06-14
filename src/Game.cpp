#include "Game.hpp"
#include "Entity.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace {
    constexpr int MAX_ROUNDS = 15;

    struct WavePreview {
        int total = 0;
        int engrudo = 0;
        int arcilla = 0;
        int fruta = 0;
        int revelacion = 0;
        int hipnotizadora = 0;
    };

    enum class MenuAction {
        None,
        Play,
        Characters,
        Options,
        Exit
    };

    struct MenuButton {
        MenuAction action;
        sf::FloatRect bounds;
        sf::FloatRect hoverBounds;
    };

    const std::vector<MenuButton>& getMenuButtons() {
        static const std::vector<MenuButton> buttons = {
            {MenuAction::Play, {{980.f, 190.f}, {275.f, 85.f}}, {{1000.f, 205.f}, {252.f, 58.f}}},
            {MenuAction::Characters, {{980.f, 292.f}, {275.f, 82.f}}, {{1002.f, 306.f}, {250.f, 58.f}}},
            {MenuAction::Options, {{975.f, 400.f}, {285.f, 84.f}}, {{1000.f, 416.f}, {252.f, 58.f}}},
            {MenuAction::Exit, {{1005.f, 508.f}, {235.f, 85.f}}, {{1023.f, 528.f}, {220.f, 56.f}}}
        };
        return buttons;
    }

    MenuAction getMenuActionAt(float x, float y) {
        for (const auto& button : getMenuButtons()) {
            if (button.bounds.contains({x, y})) return button.action;
        }
        return MenuAction::None;
    }

    const sf::FloatRect* getMenuActionBounds(MenuAction action) {
        for (const auto& button : getMenuButtons()) {
            if (button.action == action) return &button.hoverBounds;
        }
        return nullptr;
    }

    sf::ConvexShape makeMenuHoverShape(const sf::FloatRect& bounds) {
        const float bevel = std::min(bounds.size.y * 0.34f, 22.f);
        const float x = bounds.position.x;
        const float y = bounds.position.y;
        const float w = bounds.size.x;
        const float h = bounds.size.y;

        sf::ConvexShape shape(8);
        shape.setPoint(0, {x + bevel, y});
        shape.setPoint(1, {x + w - bevel, y});
        shape.setPoint(2, {x + w, y + bevel});
        shape.setPoint(3, {x + w, y + h - bevel});
        shape.setPoint(4, {x + w - bevel, y + h});
        shape.setPoint(5, {x + bevel, y + h});
        shape.setPoint(6, {x, y + h - bevel});
        shape.setPoint(7, {x, y + bevel});
        return shape;
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

    int getTowerLimit(TowerType type) {
        switch (type) {
            case TowerType::NINO_PALO: return 8;
            case TowerType::VIEJO_MACHETE: return 4;
            case TowerType::TAQUERO: return 5;
            case TowerType::ABUELITA: return 2;
            case TowerType::DON_COHETES: return 3;
            case TowerType::ORGANILLERO: return 2;
            case TowerType::RASPADERO: return 3;
            default: return 0;
        }
    }

    float distanceBetween(const sf::Vector2f& a, const sf::Vector2f& b) {
        sf::Vector2f diff = a - b;
        return std::sqrt(diff.x * diff.x + diff.y * diff.y);
    }

    PinataType getSpawnTypeForRound(int round, int spawnedIndex) {
        if (round >= 12 && spawnedIndex % 8 == 0) return PinataType::HIPNOTIZADORA;
        if (round >= 9 && spawnedIndex % 6 == 0) return PinataType::REVELACION;
        if (round >= 6 && spawnedIndex % 5 == 0) return PinataType::FRUTA;
        if (round >= 4 && spawnedIndex % 4 == 0) return PinataType::ARCILLA;
        if (round >= 2 && spawnedIndex % 5 == 0) return PinataType::ARCILLA;
        return PinataType::ENGRUDO;
    }

    int getEnemyCountForRound(int round) {
        return 4 + round * 2 + std::max(0, round - 5);
    }

    float getSpawnIntervalForRound(int round) {
        return std::max(0.48f, 1.55f - round * 0.07f);
    }

    int getRoundClearBonus(int round) {
        return 45 + round * 8;
    }

    WavePreview getWavePreviewForRound(int round) {
        WavePreview preview;
        preview.total = getEnemyCountForRound(round);

        for (int remaining = preview.total; remaining > 0; --remaining) {
            switch (getSpawnTypeForRound(round, remaining)) {
                case PinataType::ENGRUDO:
                    preview.engrudo++;
                    break;
                case PinataType::ARCILLA:
                    preview.arcilla++;
                    break;
                case PinataType::FRUTA:
                    preview.fruta++;
                    break;
                case PinataType::REVELACION:
                    preview.revelacion++;
                    break;
                case PinataType::HIPNOTIZADORA:
                    preview.hipnotizadora++;
                    break;
                default:
                    break;
            }
        }

        return preview;
    }

    const char* getPinataDebugName(PinataType type) {
        switch (type) {
            case PinataType::ENGRUDO: return "Engrudo";
            case PinataType::ARCILLA: return "Arcilla";
            case PinataType::REVELACION: return "Revelacion";
            case PinataType::FRUTA: return "Fruta";
            case PinataType::HIPNOTIZADORA: return "Hipnotizadora";
            case PinataType::BEBE_ROSA: return "Bebe rosa";
            case PinataType::BEBE_AZUL: return "Bebe azul";
            default: return "Desconocida";
        }
    }
}

Game::Game(unsigned int w, unsigned int h) 
    : width(w), height(h), 
      window(sf::VideoMode(sf::Vector2u(w, h)), "Tower Defense Mexicano"),
      currentState(GameState::MENU),
      deltaTime(0.0f),
      gameSpeedMultiplier(1.f),
      playerMoney(150.0f),
      playerLives(100),
      currentRound(1),
      enemiesLeftToSpawn(0),
      spawnTimer(0.f),
      spawnInterval(1.5f),
      map(w, h),
      selectedTowerType(TowerType::NINO_PALO),
      towerSelected(false),
      roundPreviewVisible(false),
      tutorialOfferedThisSession(false),
      tutorialChoiceVisible(false),
      tutorialWaveActive(false),
      tutorialStep(0),
      tutorialSpawnTimer(0.f),
      tutorialSpawnInterval(1.f),
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
    if (!chanclaProjectileTexture.loadFromFile("assets/textures/projectiles/CHANCLA.png")) {
        std::cerr << "[TEXTURE] No se pudo cargar CHANCLA.png" << std::endl;
    }
    if (!rocketProjectileTexture.loadFromFile("assets/textures/projectiles/COHETE.png")) {
        std::cerr << "[TEXTURE] No se pudo cargar COHETE.png" << std::endl;
    }
    if (!iceProjectileTexture.loadFromFile("assets/textures/projectiles/HIELO_RASPADO.png")) {
        std::cerr << "[TEXTURE] No se pudo cargar HIELO_RASPADO.png" << std::endl;
    }
    std::cout << "[GAME] Initialized: "     << width << "x" << height << std::endl;
}

void Game::run() {
    std::cout << "[GAME] Starting main loop..." << std::endl;
    while (window.isOpen()) {
        float rawDeltaTime = gameClock.restart().asSeconds();
        handleEvents();
        deltaTime = rawDeltaTime * ((currentState == GameState::ROUND_ACTIVE) ? gameSpeedMultiplier : 1.f);
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
                roundPreviewVisible = true;
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
            else if (keyEvent->code == sf::Keyboard::Key::Tab) {
                toggleGameSpeed();
            }
            else if (keyEvent->code == sf::Keyboard::Key::E) {
                spawnDebugPinata(PinataType::ENGRUDO);
            }
            else if (keyEvent->code == sf::Keyboard::Key::R) {
                spawnDebugPinata(PinataType::ARCILLA);
            }
            else if (keyEvent->code == sf::Keyboard::Key::T) {
                spawnDebugPinata(PinataType::REVELACION);
            }
            else if (keyEvent->code == sf::Keyboard::Key::F) {
                spawnDebugPinata(PinataType::FRUTA);
            }
            else if (keyEvent->code == sf::Keyboard::Key::H) {
                spawnDebugPinata(PinataType::HIPNOTIZADORA);
            }
            else if (keyEvent->code == sf::Keyboard::Key::B) {
                spawnDebugPinata(PinataType::BEBE_ROSA);
                spawnDebugPinata(PinataType::BEBE_AZUL);
            }
            else if (keyEvent->code == sf::Keyboard::Key::Space) {
                if (currentState == GameState::TUTORIAL) {
                    if (tutorialChoiceVisible) {
                        skipTutorial();
                    } else if (tutorialWaveActive) {
                        std::cout << "[TUTORIAL] Termina la mini oleada primero" << std::endl;
                    } else if (tutorialStep == 2) {
                        startTutorialWave({
                            PinataType::ENGRUDO,
                            PinataType::ENGRUDO
                        });
                    } else if (tutorialStep == 5) {
                        startTutorialWave({
                            PinataType::ARCILLA,
                            PinataType::FRUTA
                        });
                    } else if (tutorialStep == 7) {
                        startTutorialWave({
                            PinataType::ARCILLA,
                            PinataType::REVELACION
                        });
                    } else if (tutorialStep == 9) {
                        startTutorialWave({
                            PinataType::HIPNOTIZADORA,
                            PinataType::ENGRUDO,
                            PinataType::ENGRUDO
                        });
                    } else if (tutorialStep >= 10) {
                        finishTutorial();
                    }
                } else if (currentState == GameState::ROUND_PAUSE) {
                    if (roundPreviewVisible) {
                        roundPreviewVisible = false;
                        std::cout << "[UI] Resumen cerrado. Puedes colocar torres" << std::endl;
                    } else {
                        startWave();
                    }
                } else if (currentState == GameState::MENU) {
                    enterPlayFromMenu();
                }
            }
            else if (keyEvent->code == sf::Keyboard::Key::Enter) {
                if (currentState == GameState::TUTORIAL) {
                    if (tutorialChoiceVisible) {
                        startTutorial();
                    } else if (!tutorialWaveActive && tutorialStep == 3) {
                        tutorialStep = 4;
                    }
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
                enterPlayFromMenu();
                std::cout << "[MENU] Iniciar fiesta" << std::endl;
                break;
            case MenuAction::Characters:
                std::cout << "[MENU] Personajes aun no implementado" << std::endl;
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
            bool editTowers = canEditTowers();
            if (mouseEvent->button == sf::Mouse::Button::Left && isSpeedButtonAt(mx, my)) {
                toggleGameSpeed();
                return;
            }

            if (mouseEvent->button == sf::Mouse::Button::Right) {
                towerSelected = false;
                clearTowerSelection();
                std::cout << "[UI] Seleccion cancelada" << std::endl;
            }
            else if (mouseEvent->button == sf::Mouse::Button::Left) {
                std::cout << "[MOUSE] x=" << mx << " y=" << my << std::endl;
                if (mx >= mapWidth) {
                    if (!editTowers) {
                        std::cout << "[UI] Ahora no puedes seleccionar torres" << std::endl;
                        return;
                    }
                    // Click en panel - seleccionar torre
                    TowerType tipos[] = {
                        TowerType::NINO_PALO, TowerType::VIEJO_MACHETE,
                        TowerType::TAQUERO, TowerType::ABUELITA,
                        TowerType::DON_COHETES, TowerType::ORGANILLERO,
                        TowerType::RASPADERO
                    };
                    int slot = static_cast<int>(my / 95.f);
                    if (slot >= 0 && slot < 7) {
                        if (!isTutorialTowerAllowed(tipos[slot])) {
                            std::cout << "[TUTORIAL] Esa torre se vera mas adelante" << std::endl;
                            return;
                        }
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
                    if (currentState == GameState::ROUND_PAUSE && roundPreviewVisible) {
                        std::cout << "[UI] Cierra el resumen con SPACE antes de colocar torres" << std::endl;
                        return;
                    }
                    auto clickedTower = findTowerAt(mx, my);
                    if (editTowers && selectedPlacedTower) {
                        if (clickedTower == selectedPlacedTower) {
                            clearTowerSelection();
                            std::cout << "[UI] Torre deseleccionada" << std::endl;
                        } else {
                            moveSelectedTower(mx, my);
                        }
                    } else if (editTowers && clickedTower) {
                        towerSelected = false;
                        clearTowerSelection();
                        selectedPlacedTower = clickedTower;
                        selectedPlacedTower->setSelected(true);
                        std::cout << "[UI] Torre colocada seleccionada para mover" << std::endl;
                    } else if (towerSelected && editTowers) {
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
        case GameState::TUTORIAL:
            updateTutorial();
            break;
        case GameState::ROUND_ACTIVE: {
    // Spawnear enemigos automaticamente
    if (enemiesLeftToSpawn > 0) {
        spawnTimer -= deltaTime;
        if (spawnTimer <= 0.f) {
            auto spawnType = getSpawnTypeForRound(currentRound, enemiesLeftToSpawn);
            auto newEnemy = std::make_shared<Pinata>(
                map.getWaypoints(),
                spawnType,
                currentRound
            );
            enemies.push_back(newEnemy);
            enemiesLeftToSpawn--;
            spawnTimer = spawnInterval;
            std::cout << "[WAVE] Pinata spawned. Quedan: " << enemiesLeftToSpawn << std::endl;
        }
    }
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) enemy->update(deltaTime);
        if (enemy && enemy->hasReachedEnd()) damagePlayer(enemy->getEscapeDamage());
    }
    if (playerLives <= 0) {
        playerLives = 0;
        currentState = GameState::DEFEAT;
        towerSelected = false;
        clearTowerSelection();
        std::cout << "[GAME] Derrota" << std::endl;
        break;
    }
    updateTowerRangeVisuals();
    updateHypnosisAuras();
    for (auto& tower : towers) {
        if (!tower) continue;

        float fireRateMultiplier = 1.f;
        if (!tower->isSupportTower()) {
            float supportRangeScale = getOrganilleroRangeScale();
            for (const auto& support : towers) {
                if (support && support->isSupportTower() && support->isInScaledRange(tower->getPosition(), supportRangeScale)) {
                    float baseBuff = (tower->getType() == TowerType::ABUELITA) ? 1.05f : 1.35f;
                    fireRateMultiplier = 1.f + (baseBuff - 1.f) * support->getSupportMultiplierScale();
                    break;
                }
            }
        }
        tower->combat(deltaTime, enemies, fireRateMultiplier, &towers);
        if (auto projectile = tower->consumeProjectileRequest()) {
            float projectileSpeed = 520.f;
            if (projectile->type == ProjectileType::Chancla) projectileSpeed = 760.f;
            if (projectile->type == ProjectileType::Rocket) projectileSpeed = 430.f;
            if (projectile->type == ProjectileType::Ice) projectileSpeed = 500.f;
            projectiles.push_back(Projectile{
                projectile->startPosition,
                projectile->target,
                projectile->damage,
                projectileSpeed,
                0.f,
                projectile->type,
                projectile->splashRadius,
                true
            });
        }
    }
    updateProjectiles();
    std::vector<std::shared_ptr<Pinata>> spawnedPinatas;
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [this, &spawnedPinatas](const std::shared_ptr<Pinata>& e) {
                if (!e) return true;
                if (e->isAlive()) return false;
                if (!e->hasReachedEnd()) {
                    if (e->getType() == PinataType::REVELACION) {
                        spawnedPinatas.push_back(std::make_shared<Pinata>(
                            map.getWaypoints(),
                            PinataType::BEBE_ROSA,
                            currentRound,
                            e->getPosition() + sf::Vector2f(-12.f, 0.f),
                            e->getCurrentWaypointIndex()
                        ));
                        spawnedPinatas.push_back(std::make_shared<Pinata>(
                            map.getWaypoints(),
                            PinataType::BEBE_AZUL,
                            currentRound,
                            e->getPosition() + sf::Vector2f(12.f, 0.f),
                            e->getCurrentWaypointIndex()
                        ));
                        std::cout << "[PINATA] Revelacion genero 2 pinatas bebe" << std::endl;
                    }
                    if (e->getType() == PinataType::FRUTA) {
                        float slowRadius = e->isSlowed() ? 85.f : 150.f;
                        float slowDuration = e->isSlowed() ? 2.5f : 5.f;
                        slowTowersNear(e->getPosition(), slowRadius, 0.5f, slowDuration);
                        std::cout << "[PINATA] Fruta ralentizo torres cercanas" << std::endl;
                    }
                    addPlayerMoney(static_cast<float>(e->getReward()));
                    std::cout << "[REWARD] +" << e->getReward()
                              << " dulces. Total: " << playerMoney << std::endl;
                }
                return true;
            }),
        enemies.end()
    );
    enemies.insert(enemies.end(), spawnedPinatas.begin(), spawnedPinatas.end());
    // Oleada terminada
    if (enemiesLeftToSpawn == 0 && enemies.empty()) {
        if (currentRound >= MAX_ROUNDS) {
            currentState = GameState::VICTORY;
            towerSelected = false;
            clearTowerSelection();
            std::cout << "[GAME] Victoria! Completaste " << MAX_ROUNDS << " rondas" << std::endl;
            break;
        }

        addPlayerMoney(static_cast<float>(getRoundClearBonus(currentRound)));
        currentRound++;
        currentState = GameState::ROUND_PAUSE;
        roundPreviewVisible = true;
        std::cout << "[WAVE] Oleada completada! Ronda: " << currentRound << std::endl;
    }
    break;
    }
        default:
            break;
    }

    bool playSupportLoops = currentState == GameState::ROUND_ACTIVE || (currentState == GameState::TUTORIAL && tutorialWaveActive);
    for (auto& tower : towers) {
        if (!tower) continue;
        tower->setLoopAnimationActive(playSupportLoops);
        tower->updateLoopAnimation(deltaTime);
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

        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
        sf::Vector2u menuSize = menuTexture.getSize();
        float menuX = mousePos.x * static_cast<float>(menuSize.x) / static_cast<float>(width);
        float menuY = mousePos.y * static_cast<float>(menuSize.y) / static_cast<float>(height);

        if (const sf::FloatRect* hoverBounds = getMenuActionBounds(getMenuActionAt(menuX, menuY))) {
            float scaleX = static_cast<float>(width) / static_cast<float>(menuSize.x);
            float scaleY = static_cast<float>(height) / static_cast<float>(menuSize.y);

            sf::FloatRect scaledBounds({
                hoverBounds->position.x * scaleX,
                hoverBounds->position.y * scaleY
            }, {
                hoverBounds->size.x * scaleX,
                hoverBounds->size.y * scaleY
            });

            sf::ConvexShape hover = makeMenuHoverShape(scaledBounds);
            hover.setFillColor(sf::Color(255, 230, 80, 45));
            hover.setOutlineColor(sf::Color(255, 245, 120, 220));
            hover.setOutlineThickness(3.f);
            window.draw(hover);
        }

        window.display();
        return;
    }

    map.render(window);
    updateTowerRangeVisuals();

    for (const auto& tower : towers) {
        if (tower && tower != selectedPlacedTower) tower->render(window);
    }

    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) enemy->render(window);
    }

    renderProjectiles();

    renderTowerPreview();
    renderPanel();
    renderHUD();
    renderRoundPreview();
    renderTutorial();

    window.display();
}

void Game::updateProjectiles() {
    for (auto& explosion : explosionEffects) {
        explosion.timer -= deltaTime;
    }
    explosionEffects.erase(
        std::remove_if(explosionEffects.begin(), explosionEffects.end(),
            [](const ExplosionEffect& explosion) {
                return explosion.timer <= 0.f;
            }),
        explosionEffects.end()
    );

    for (auto& impact : impactEffects) {
        impact.timer -= deltaTime;
    }
    impactEffects.erase(
        std::remove_if(impactEffects.begin(), impactEffects.end(),
            [](const ImpactEffect& impact) {
                return impact.timer <= 0.f;
            }),
        impactEffects.end()
    );

    for (auto& projectile : projectiles) {
        if (!projectile.alive) continue;

        auto target = projectile.target.lock();
        if (!target || !target->isAlive()) {
            projectile.alive = false;
            continue;
        }

        sf::Vector2f targetPosition = target->getPosition();
        sf::Vector2f direction = targetPosition - projectile.position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        float travel = projectile.speed * deltaTime;

        if (distance <= 12.f || distance <= travel) {
            if (projectile.type == ProjectileType::Rocket) {
                sf::Vector2f impactPosition = target->getPosition();
                for (auto& enemy : enemies) {
                    if (!enemy || !enemy->isAlive()) continue;
                    if (distanceBetween(enemy->getPosition(), impactPosition) <= projectile.splashRadius) {
                        enemy->takeDamage(projectile.damage);
                    }
                }
                explosionEffects.push_back(ExplosionEffect{
                    impactPosition,
                    projectile.splashRadius,
                    0.28f,
                    0.28f
                });
            } else {
                target->takeDamage(projectile.damage);
                if (projectile.type == ProjectileType::Ice) {
                    target->applySlow(0.5f, 2.f);
                }
                impactEffects.push_back(ImpactEffect{
                    target->getPosition(),
                    projectile.type,
                    0.22f,
                    0.22f,
                    projectile.rotation
                });
            }
            projectile.alive = false;
            continue;
        }

        direction /= distance;
        projectile.rotation = sf::degrees(std::atan2(direction.y, direction.x)).asDegrees();
        projectile.position += direction * travel;
    }

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& projectile) {
                return !projectile.alive;
            }),
        projectiles.end()
    );
}

void Game::renderProjectiles() {
    for (const auto& explosion : explosionEffects) {
        float progress = explosion.timer / explosion.duration;
        float radius = explosion.radius * (1.1f - progress * 0.15f);

        sf::CircleShape blast(radius);
        blast.setOrigin({radius, radius});
        blast.setPosition(explosion.position);
        blast.setFillColor(sf::Color(255, 115, 35, static_cast<std::uint8_t>(95 * progress)));
        blast.setOutlineColor(sf::Color(255, 225, 90, static_cast<std::uint8_t>(210 * progress)));
        blast.setOutlineThickness(4.f);
        window.draw(blast);

        sf::CircleShape core(radius * 0.35f);
        core.setOrigin({radius * 0.35f, radius * 0.35f});
        core.setPosition(explosion.position);
        core.setFillColor(sf::Color(255, 240, 140, static_cast<std::uint8_t>(180 * progress)));
        window.draw(core);
    }

    for (const auto& impact : impactEffects) {
        float progress = impact.timer / impact.duration;
        float age = 1.f - progress;
        float radians = impact.rotation * 3.14159265f / 180.f;
        sf::Vector2f forward(std::cos(radians), std::sin(radians));
        sf::Vector2f right(-forward.y, forward.x);

        if (impact.type == ProjectileType::Chancla) {
            sf::CircleShape ring(12.f + age * 12.f, 18);
            ring.setOrigin({12.f + age * 12.f, 12.f + age * 12.f});
            ring.setPosition(impact.position);
            ring.setFillColor(sf::Color(255, 255, 255, 0));
            ring.setOutlineColor(sf::Color(255, 245, 185, static_cast<std::uint8_t>(210 * progress)));
            ring.setOutlineThickness(3.f);
            window.draw(ring);

            for (int i = -1; i <= 1; ++i) {
                sf::RectangleShape spark({20.f, 4.f});
                spark.setOrigin({10.f, 2.f});
                spark.setPosition(impact.position + right * static_cast<float>(i) * (7.f + age * 8.f));
                spark.setRotation(sf::degrees(impact.rotation + static_cast<float>(i) * 45.f));
                spark.setFillColor(sf::Color(255, 220, 120, static_cast<std::uint8_t>(230 * progress)));
                window.draw(spark);
            }
            continue;
        }

        if (impact.type == ProjectileType::Ice) {
            sf::CircleShape frost(10.f + age * 16.f, 20);
            frost.setOrigin({10.f + age * 16.f, 10.f + age * 16.f});
            frost.setPosition(impact.position);
            frost.setFillColor(sf::Color(120, 220, 255, static_cast<std::uint8_t>(65 * progress)));
            frost.setOutlineColor(sf::Color(210, 250, 255, static_cast<std::uint8_t>(190 * progress)));
            frost.setOutlineThickness(2.f);
            window.draw(frost);

            for (int i = -2; i <= 2; ++i) {
                sf::RectangleShape shard({5.f, 14.f});
                shard.setOrigin({2.5f, 7.f});
                shard.setPosition(impact.position + right * static_cast<float>(i) * (6.f + age * 7.f) - forward * (8.f + age * 10.f));
                shard.setRotation(sf::degrees(impact.rotation + 90.f + static_cast<float>(i) * 18.f));
                shard.setFillColor(sf::Color(230, 255, 255, static_cast<std::uint8_t>(210 * progress)));
                window.draw(shard);
            }
            continue;
        }

        sf::CircleShape splash(9.f + age * 10.f, 16);
        splash.setOrigin({9.f + age * 10.f, 9.f + age * 10.f});
        splash.setPosition(impact.position);
        splash.setFillColor(sf::Color(205, 255, 70, static_cast<std::uint8_t>(70 * progress)));
        splash.setOutlineColor(sf::Color(80, 150, 35, static_cast<std::uint8_t>(180 * progress)));
        splash.setOutlineThickness(2.f);
        window.draw(splash);

        for (int i = -1; i <= 1; ++i) {
            sf::CircleShape drop(3.f, 10);
            drop.setOrigin({3.f, 3.f});
            drop.setPosition(impact.position - forward * (10.f + age * 13.f) + right * static_cast<float>(i) * (8.f + age * 6.f));
            drop.setFillColor(sf::Color(230, 255, 115, static_cast<std::uint8_t>(230 * progress)));
            window.draw(drop);
        }
    }

    for (const auto& projectile : projectiles) {
        if (!projectile.alive) continue;

        if (projectile.type == ProjectileType::Chancla && chanclaProjectileTexture.getSize().x > 0) {
            sf::Sprite chancla(chanclaProjectileTexture);
            auto size = chanclaProjectileTexture.getSize();
            chancla.setOrigin({static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f});
            chancla.setPosition(projectile.position);
            chancla.setRotation(sf::degrees(projectile.rotation));
            window.draw(chancla);
            continue;
        }

        if (projectile.type == ProjectileType::Rocket && rocketProjectileTexture.getSize().x > 0) {
            sf::CircleShape smoke(7.f);
            smoke.setOrigin({7.f, 7.f});
            smoke.setPosition(projectile.position - sf::Vector2f(std::cos(projectile.rotation * 3.14159265f / 180.f), std::sin(projectile.rotation * 3.14159265f / 180.f)) * 18.f);
            smoke.setFillColor(sf::Color(255, 165, 70, 90));
            window.draw(smoke);

            sf::Sprite rocket(rocketProjectileTexture);
            auto size = rocketProjectileTexture.getSize();
            rocket.setOrigin({static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f});
            rocket.setPosition(projectile.position);
            rocket.setRotation(sf::degrees(projectile.rotation + 35.f));
            window.draw(rocket);
            continue;
        }

        if (projectile.type == ProjectileType::Ice && iceProjectileTexture.getSize().x > 0) {
            sf::Sprite ice(iceProjectileTexture);
            auto size = iceProjectileTexture.getSize();
            ice.setOrigin({static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f});
            ice.setPosition(projectile.position);
            ice.setRotation(sf::degrees(projectile.rotation));
            window.draw(ice);
            continue;
        }

        sf::CircleShape trail(5.f);
        trail.setOrigin({5.f, 5.f});
        trail.setPosition(projectile.position + sf::Vector2f(-7.f, 2.f));
        trail.setFillColor(sf::Color(175, 255, 95, 90));
        window.draw(trail);

        sf::CircleShape lime(6.f, 16);
        lime.setOrigin({6.f, 6.f});
        lime.setPosition(projectile.position);
        lime.setFillColor(sf::Color(190, 255, 80));
        lime.setOutlineColor(sf::Color(70, 135, 35));
        lime.setOutlineThickness(2.f);
        window.draw(lime);

        sf::CircleShape pulp(2.f, 10);
        pulp.setOrigin({2.f, 2.f});
        pulp.setPosition(projectile.position + sf::Vector2f(1.f, -1.f));
        pulp.setFillColor(sf::Color(245, 255, 180, 210));
        window.draw(pulp);
    }
}

void Game::spawnDebugPinata(PinataType type) {
    enemies.push_back(std::make_shared<Pinata>(map.getWaypoints(), type, currentRound));
    std::cout << "[DEBUG] Spawn " << getPinataDebugName(type)
              << ". Total: " << enemies.size() << std::endl;
}

int Game::countTowersOfType(TowerType type) const {
    int total = 0;
    for (const auto& tower : towers) {
        if (tower && tower->getType() == type) {
            total++;
        }
    }
    return total;
}

void Game::toggleGameSpeed() {
    gameSpeedMultiplier = (gameSpeedMultiplier > 1.f) ? 1.f : 2.f;
    std::cout << "[UI] Velocidad x" << static_cast<int>(gameSpeedMultiplier) << std::endl;
}

bool Game::isSpeedButtonAt(float x, float y) const {
    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;
    sf::FloatRect speedButton({mapWidth - 145.f, 5.f}, {130.f, 30.f});
    return speedButton.contains({x, y});
}

void Game::placeTower(float x, float y) {
    if (!canEditTowers()) {
        std::cout << "[UI] No puedes colocar torres durante la oleada" << std::endl;
        return;
    }
    if (roundPreviewVisible) {
        std::cout << "[UI] Cierra el resumen con SPACE antes de colocar torres" << std::endl;
        return;
    }

    sf::Vector2f towerPosition{x, y};
    if (!isValidTowerPosition(towerPosition)) return;
    if (!isTutorialTowerAllowed(selectedTowerType)) {
        std::cout << "[TUTORIAL] Usa la torre indicada por el tutorial" << std::endl;
        return;
    }

    int currentTypeCount = countTowersOfType(selectedTowerType);
    int towerLimit = getTowerLimit(selectedTowerType);
    if (currentTypeCount >= towerLimit) {
        std::cout << "[UI] Limite de " << getTowerName(selectedTowerType)
                  << " alcanzado (" << currentTypeCount << "/" << towerLimit << ")" << std::endl;
        return;
    }

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
        if (currentState == GameState::TUTORIAL) {
            if (tutorialStep == 1) {
                tutorialStep = 2;
            } else if (tutorialStep == 4 &&
                       countTowersOfType(TowerType::RASPADERO) > 0 &&
                       countTowersOfType(TowerType::DON_COHETES) > 0) {
                tutorialStep = 5;
            } else if (tutorialStep == 6 &&
                       countTowersOfType(TowerType::VIEJO_MACHETE) > 0 &&
                       countTowersOfType(TowerType::TAQUERO) > 0) {
                tutorialStep = 7;
            } else if (tutorialStep == 8 &&
                       countTowersOfType(TowerType::ABUELITA) > 0 &&
                       countTowersOfType(TowerType::ORGANILLERO) > 0) {
                tutorialStep = 9;
            }
        }
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

bool Game::isValidTowerPosition(const sf::Vector2f& position, const std::shared_ptr<Tower>& ignoredTower, bool showMessage) const {
    if (map.isOnPath(position)) {
        if (showMessage) std::cout << "[UI] No puedes colocar torres sobre el camino" << std::endl;
        return false;
    }

    constexpr float minTowerDistance = 55.f;
    for (const auto& existingTower : towers) {
        if (!existingTower || existingTower == ignoredTower) continue;
        if (distanceBetween(existingTower->getPosition(), position) < minTowerDistance) {
            if (showMessage) std::cout << "[UI] No puedes colocar torres tan juntas" << std::endl;
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

void Game::slowTowersNear(const sf::Vector2f& position, float radius, float multiplier, float duration) {
    for (auto& tower : towers) {
        if (!tower) continue;
        if (distanceBetween(tower->getPosition(), position) <= radius) {
            tower->applyAttackSlow(multiplier, duration);
        }
    }
}

void Game::updateTowerRangeVisuals() {
    float organilleroRangeScale = getOrganilleroRangeScale();
    for (auto& tower : towers) {
        if (!tower) continue;
        tower->setRangeVisualScale(tower->isSupportTower() ? organilleroRangeScale : 1.f);
    }
}

void Game::updateHypnosisAuras() {
    constexpr float hypnosisRadius = 190.f;
    float supportRangeScale = getOrganilleroRangeScale();

    for (auto& tower : towers) {
        if (tower) tower->setHypnotized(false);
    }

    for (const auto& enemy : enemies) {
        if (!enemy || !enemy->isAlive() || enemy->getType() != PinataType::HIPNOTIZADORA) continue;

        for (auto& tower : towers) {
            if (!tower || tower->isImmuneToHypnosis()) continue;
            if (tower->isHypnosisProtected()) continue;
            bool protectedByOrganillero = false;
            for (const auto& support : towers) {
                if (support && support->isSupportTower() && support->isInScaledRange(tower->getPosition(), supportRangeScale)) {
                    protectedByOrganillero = true;
                    break;
                }
            }
            if (protectedByOrganillero) continue;

            if (distanceBetween(tower->getPosition(), enemy->getPosition()) <= hypnosisRadius) {
                tower->setHypnotized(true);
            }
        }
    }
}

bool Game::hasActiveHypnotizer() const {
    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive() && enemy->getType() == PinataType::HIPNOTIZADORA) {
            return true;
        }
    }
    return false;
}

float Game::getOrganilleroRangeScale() const {
    return hasActiveHypnotizer() ? 0.7f : 1.f;
}

bool Game::canEditTowers() const {
    if (currentState == GameState::ROUND_PAUSE) return !roundPreviewVisible;
    if (currentState == GameState::TUTORIAL) return !tutorialChoiceVisible && !tutorialWaveActive && tutorialStep >= 1 && tutorialStep <= 9;
    return false;
}

bool Game::isTutorialTowerAllowed(TowerType type) const {
    if (currentState != GameState::TUTORIAL || tutorialChoiceVisible) return true;
    if (tutorialStep <= 3) return type == TowerType::NINO_PALO;
    if (tutorialStep == 4 || tutorialStep == 5) {
        return type == TowerType::RASPADERO || type == TowerType::DON_COHETES;
    }
    if (tutorialStep == 6 || tutorialStep == 7) {
        return type == TowerType::VIEJO_MACHETE || type == TowerType::TAQUERO;
    }
    if (tutorialStep == 8 || tutorialStep == 9) {
        return type == TowerType::ABUELITA || type == TowerType::ORGANILLERO;
    }
    return true;
}

void Game::resetPlaySession() {
    playerMoney = 150.f;
    playerLives = 100;
    currentRound = 1;
    enemiesLeftToSpawn = 0;
    spawnTimer = 0.f;
    spawnInterval = 1.5f;
    gameSpeedMultiplier = 1.f;
    selectedTowerType = TowerType::NINO_PALO;
    towerSelected = false;
    roundPreviewVisible = false;
    clearTowerSelection();
    enemies.clear();
    towers.clear();
    projectiles.clear();
    explosionEffects.clear();
    impactEffects.clear();
    tutorialWaveActive = false;
    tutorialStep = 0;
    tutorialSpawnTimer = 0.f;
    tutorialSpawnQueue.clear();
}

void Game::enterPlayFromMenu() {
    resetPlaySession();
    if (!tutorialOfferedThisSession) {
        currentState = GameState::TUTORIAL;
        tutorialChoiceVisible = true;
        tutorialStep = 0;
        std::cout << "[TUTORIAL] Pregunta inicial" << std::endl;
        return;
    }

    currentState = GameState::ROUND_PAUSE;
    roundPreviewVisible = true;
}

void Game::startTutorial() {
    tutorialOfferedThisSession = true;
    tutorialChoiceVisible = false;
    tutorialWaveActive = false;
    tutorialStep = 1;
    playerMoney = 75.f;
    selectedTowerType = TowerType::NINO_PALO;
    towerSelected = true;
    std::cout << "[TUTORIAL] Iniciado" << std::endl;
}

void Game::skipTutorial() {
    tutorialOfferedThisSession = true;
    tutorialChoiceVisible = false;
    tutorialWaveActive = false;
    tutorialStep = 0;
    currentState = GameState::ROUND_PAUSE;
    roundPreviewVisible = true;
    playerMoney = 150.f;
    std::cout << "[TUTORIAL] Saltado" << std::endl;
}

void Game::finishTutorial() {
    tutorialOfferedThisSession = true;
    tutorialChoiceVisible = false;
    tutorialWaveActive = false;
    tutorialStep = 0;
    currentState = GameState::ROUND_PAUSE;
    roundPreviewVisible = true;
    playerMoney = 150.f;
    playerLives = 100;
    currentRound = 1;
    enemiesLeftToSpawn = 0;
    enemies.clear();
    towers.clear();
    projectiles.clear();
    explosionEffects.clear();
    impactEffects.clear();
    towerSelected = false;
    clearTowerSelection();
    std::cout << "[TUTORIAL] Completado. Ronda 1 lista" << std::endl;
}

void Game::startTutorialWave(const std::vector<PinataType>& types) {
    tutorialSpawnQueue = types;
    tutorialWaveActive = true;
    tutorialSpawnTimer = 0.f;
    tutorialSpawnInterval = 0.95f;
    towerSelected = false;
    clearTowerSelection();
    std::cout << "[TUTORIAL] Mini oleada iniciada" << std::endl;
}

void Game::updateTutorial() {
    if (tutorialChoiceVisible) return;

    if (tutorialWaveActive && !tutorialSpawnQueue.empty()) {
        tutorialSpawnTimer -= deltaTime;
        if (tutorialSpawnTimer <= 0.f) {
            PinataType type = tutorialSpawnQueue.front();
            tutorialSpawnQueue.erase(tutorialSpawnQueue.begin());
            enemies.push_back(std::make_shared<Pinata>(map.getWaypoints(), type, 0));
            tutorialSpawnTimer = tutorialSpawnInterval;
        }
    }

    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) enemy->update(deltaTime);
        if (enemy && enemy->hasReachedEnd()) damagePlayer(enemy->getEscapeDamage());
    }

    updateTowerRangeVisuals();
    updateHypnosisAuras();
    for (auto& tower : towers) {
        if (!tower) continue;

        float fireRateMultiplier = 1.f;
        if (!tower->isSupportTower()) {
            float supportRangeScale = getOrganilleroRangeScale();
            for (const auto& support : towers) {
                if (support && support->isSupportTower() && support->isInScaledRange(tower->getPosition(), supportRangeScale)) {
                    float baseBuff = (tower->getType() == TowerType::ABUELITA) ? 1.05f : 1.35f;
                    fireRateMultiplier = 1.f + (baseBuff - 1.f) * support->getSupportMultiplierScale();
                    break;
                }
            }
        }
        tower->combat(deltaTime, enemies, fireRateMultiplier, &towers);
        if (auto projectile = tower->consumeProjectileRequest()) {
            float projectileSpeed = 520.f;
            if (projectile->type == ProjectileType::Chancla) projectileSpeed = 760.f;
            if (projectile->type == ProjectileType::Rocket) projectileSpeed = 430.f;
            if (projectile->type == ProjectileType::Ice) projectileSpeed = 500.f;
            projectiles.push_back(Projectile{
                projectile->startPosition,
                projectile->target,
                projectile->damage,
                projectileSpeed,
                0.f,
                projectile->type,
                projectile->splashRadius,
                true
            });
        }
    }
    updateProjectiles();

    std::vector<std::shared_ptr<Pinata>> spawnedPinatas;
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [this, &spawnedPinatas](const std::shared_ptr<Pinata>& e) {
                if (!e) return true;
                if (e->isAlive()) return false;
                if (!e->hasReachedEnd()) {
                    if (e->getType() == PinataType::REVELACION) {
                        spawnedPinatas.push_back(std::make_shared<Pinata>(
                            map.getWaypoints(),
                            PinataType::BEBE_ROSA,
                            0,
                            e->getPosition() + sf::Vector2f(-12.f, 0.f),
                            e->getCurrentWaypointIndex()
                        ));
                        spawnedPinatas.push_back(std::make_shared<Pinata>(
                            map.getWaypoints(),
                            PinataType::BEBE_AZUL,
                            0,
                            e->getPosition() + sf::Vector2f(12.f, 0.f),
                            e->getCurrentWaypointIndex()
                        ));
                    }
                    if (e->getType() == PinataType::FRUTA) {
                        slowTowersNear(e->getPosition(), 2000.f, 0.5f, 2.6f);
                    }
                    addPlayerMoney(static_cast<float>(e->getReward()));
                }
                return true;
            }),
        enemies.end()
    );
    enemies.insert(enemies.end(), spawnedPinatas.begin(), spawnedPinatas.end());

    if (tutorialWaveActive && tutorialSpawnQueue.empty() && enemies.empty()) {
        tutorialWaveActive = false;
        if (tutorialStep == 2) {
            playerMoney = std::max(playerMoney, 320.f);
            tutorialStep = 3;
        } else if (tutorialStep == 5) {
            playerMoney = std::max(playerMoney, 225.f);
            tutorialStep = 6;
        } else if (tutorialStep == 7) {
            playerMoney = std::max(playerMoney, 410.f);
            tutorialStep = 8;
        } else if (tutorialStep == 9) {
            tutorialStep = 10;
        }
    }
}

void Game::renderTutorial() {
    if (currentState != GameState::TUTORIAL) return;

    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;

    if (tutorialChoiceVisible) {
        sf::RectangleShape overlay({mapWidth, static_cast<float>(height)});
        overlay.setFillColor(sf::Color(0, 0, 0, 130));
        window.draw(overlay);

        sf::RectangleShape box({560.f, 220.f});
        box.setOrigin({280.f, 110.f});
        box.setPosition({mapWidth / 2.f, static_cast<float>(height) / 2.f});
        box.setFillColor(sf::Color(18, 18, 18, 230));
        box.setOutlineColor(sf::Color(255, 230, 90, 210));
        box.setOutlineThickness(3.f);
        window.draw(box);

        sf::Text title(hudFont, "Primera vez en la fiesta?", 30);
        title.setFillColor(sf::Color::White);
        title.setPosition({mapWidth / 2.f - 220.f, static_cast<float>(height) / 2.f - 75.f});
        window.draw(title);

        sf::Text body(hudFont, "Aprende colocando personajes y jugando mini rondas.", 21);
        body.setFillColor(sf::Color(230, 230, 230));
        body.setPosition({mapWidth / 2.f - 220.f, static_cast<float>(height) / 2.f - 20.f});
        window.draw(body);

        sf::Text hint(hudFont, "ENTER: Tutorial    SPACE: Saltar", 24);
        hint.setFillColor(sf::Color::Yellow);
        hint.setPosition({mapWidth / 2.f - 220.f, static_cast<float>(height) / 2.f + 45.f});
        window.draw(hint);
        return;
    }

    std::string title = "Tutorial";
    std::string body = "";
    std::string body2 = "";
    std::string hint = "";

    switch (tutorialStep) {
        case 1:
            title = "Paso 1: Coloca tu primer personaje";
            body = "Selecciona al Nino con Palo y ponlo sobre el pasto.";
            body2 = "El camino debe quedar libre.";
            hint = "El boton del Nino esta resaltado. Click en el mapa para colocarlo.";
            break;
        case 2:
            title = "Paso 2: Inicia una mini oleada";
            body = tutorialWaveActive
                ? "Las pinatas estan entrando. Mira como tu personaje las ataca."
                : "Bien. Ahora presiona SPACE para romper unas pinatas de engrudo.";
            body2 = tutorialWaveActive ? "" : "Seran pocas para que veas el combate sin presion.";
            hint = tutorialWaveActive ? "Espera a que termine la mini oleada." : "SPACE: Iniciar mini oleada";
            break;
        case 3:
            title = "Paso 3: Dulces y movimiento";
            body = "Cada pinata derrotada da dulces.";
            body2 = "Entre rondas puedes seleccionar una torre y moverla.";
            hint = "Prueba mover una torre si quieres. ENTER: Continuar";
            break;
        case 4:
            title = "Paso 4: Roles especiales";
            body = "Coloca un Raspadero y un Don Cohetes.";
            body2 = "Uno ralentiza y el otro pega en area.";
            hint = "Tienes dulces para comprar ambos.";
            break;
        case 5:
            title = "Paso 5: Arcilla y Fruta";
            body = tutorialWaveActive
                ? "Arcilla resiste. Fruta ralentiza torres al romperse."
                : "Ahora veremos una mini oleada con Arcilla y Fruta.";
            body2 = tutorialWaveActive ? "En tutorial la fruta afectara torres para que se note." : "";
            hint = tutorialWaveActive ? "Observa los efectos especiales." : "SPACE: Iniciar mini oleada especial";
            break;
        case 6:
            title = "Paso 6: Dano directo";
            body = "Coloca al Viejo con Machete y al Taquero.";
            body2 = "El Viejo ayuda contra Arcilla; el Taquero lanza limones.";
            hint = "Ambos estan desbloqueados ahora.";
            break;
        case 7:
            title = "Paso 7: Revelacion";
            body = tutorialWaveActive
                ? "La pinata de Revelacion se rompe en pinatas bebe."
                : "Ahora prueba contra Arcilla y Revelacion.";
            body2 = tutorialWaveActive ? "Observa si aparecen las pinatas bebe." : "";
            hint = tutorialWaveActive ? "Espera a que termine la mini oleada." : "SPACE: Iniciar mini oleada";
            break;
        case 8:
            title = "Paso 8: Apoyo y control";
            body = "Coloca una Abuelita y un Organillero.";
            body2 = "La Abuelita cubre todo el mapa; el Organillero protege y acelera.";
            hint = "Ambos estan desbloqueados ahora.";
            break;
        case 9:
            title = "Paso 9: Hipnotizadora";
            body = tutorialWaveActive
                ? "La Hipnotizadora hace fallar ataques si no hay proteccion."
                : "Ahora veremos una Hipnotizadora.";
            body2 = tutorialWaveActive ? "Organillero y Abuelita ayudan contra esa amenaza." : "";
            hint = tutorialWaveActive ? "Espera a que termine la mini oleada." : "SPACE: Iniciar mini oleada";
            break;
        default:
            title = "Tutorial completado";
            body = "Ya sabes colocar, iniciar oleadas, ganar dulces y reconocer roles especiales.";
            body2 = "Ahora empieza la partida normal.";
            hint = "SPACE: Empezar la ronda 1";
            break;
    }

    sf::RectangleShape box({700.f, 152.f});
    box.setPosition({24.f, static_cast<float>(height) - 176.f});
    box.setFillColor(sf::Color(18, 18, 18, 225));
    box.setOutlineColor(sf::Color(255, 230, 90, 190));
    box.setOutlineThickness(2.f);
    window.draw(box);

    sf::Text titleText(hudFont, title, 24);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition({44.f, static_cast<float>(height) - 158.f});
    window.draw(titleText);

    sf::Text bodyText(hudFont, body, 18);
    bodyText.setFillColor(sf::Color(225, 225, 225));
    bodyText.setPosition({44.f, static_cast<float>(height) - 118.f});
    window.draw(bodyText);

    if (!body2.empty()) {
        sf::Text bodyText2(hudFont, body2, 18);
        bodyText2.setFillColor(sf::Color(225, 225, 225));
        bodyText2.setPosition({44.f, static_cast<float>(height) - 92.f});
        window.draw(bodyText2);
    }

    sf::Text hintText(hudFont, hint, 18);
    hintText.setFillColor(sf::Color::Yellow);
    hintText.setPosition({44.f, static_cast<float>(height) - 56.f});
    window.draw(hintText);

    if (tutorialStep == 1) {
        sf::RectangleShape highlight({188.f, 88.f});
        highlight.setPosition({mapWidth + 6.f, 5.f});
        highlight.setFillColor(sf::Color(255, 230, 80, 35));
        highlight.setOutlineColor(sf::Color(255, 245, 120, 230));
        highlight.setOutlineThickness(4.f);
        window.draw(highlight);
    } else if (tutorialStep == 4 || tutorialStep == 6 || tutorialStep == 8) {
        auto drawSlotHighlight = [this, mapWidth](int slot, const sf::Color& fill, const sf::Color& outline) {
            sf::RectangleShape highlight({188.f, 88.f});
            highlight.setPosition({mapWidth + 6.f, static_cast<float>(slot) * 95.f + 5.f});
            highlight.setFillColor(fill);
            highlight.setOutlineColor(outline);
            highlight.setOutlineThickness(3.f);
            window.draw(highlight);
        };

        if (tutorialStep == 6) {
            drawSlotHighlight(1, sf::Color(255, 230, 80, 28), sf::Color(255, 245, 120, 210));
            drawSlotHighlight(2, sf::Color(120, 255, 120, 28), sf::Color(160, 255, 160, 210));
            return;
        }

        if (tutorialStep == 8) {
            drawSlotHighlight(3, sf::Color(255, 230, 80, 28), sf::Color(255, 245, 120, 210));
            drawSlotHighlight(5, sf::Color(120, 220, 255, 28), sf::Color(160, 240, 255, 210));
            return;
        }

        sf::RectangleShape rocketHighlight({188.f, 88.f});
        rocketHighlight.setPosition({mapWidth + 6.f, 4.f * 95.f + 5.f});
        rocketHighlight.setFillColor(sf::Color(255, 230, 80, 28));
        rocketHighlight.setOutlineColor(sf::Color(255, 245, 120, 210));
        rocketHighlight.setOutlineThickness(3.f);
        window.draw(rocketHighlight);

        sf::RectangleShape iceHighlight({188.f, 88.f});
        iceHighlight.setPosition({mapWidth + 6.f, 6.f * 95.f + 5.f});
        iceHighlight.setFillColor(sf::Color(120, 220, 255, 28));
        iceHighlight.setOutlineColor(sf::Color(160, 240, 255, 210));
        iceHighlight.setOutlineThickness(3.f);
        window.draw(iceHighlight);
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
        bool allowed = isTutorialTowerAllowed(tipos[i]);

        sf::RectangleShape slot(sf::Vector2f(188.f, 88.f));
        slot.setPosition({mapWidth + 6.f, slotY});
        slot.setFillColor(allowed ? sf::Color(50, 50, 50) : sf::Color(28, 28, 28));
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
            if (!allowed) spr.setColor(sf::Color(95, 95, 95, 170));
            window.draw(spr);
        }

        sf::Text costText(hudFont, "$" + std::to_string(getTowerCost(tipos[i])), 20);
        costText.setFillColor(allowed ? sf::Color(255, 215, 0) : sf::Color(120, 120, 120));
        costText.setPosition({mapWidth + 105.f, slotY + 22.f});
        window.draw(costText);

        int placedCount = countTowersOfType(tipos[i]);
        int towerLimit = getTowerLimit(tipos[i]);
        sf::Text limitText(hudFont, std::to_string(placedCount) + "/" + std::to_string(towerLimit), 16);
        limitText.setFillColor(!allowed ? sf::Color(100, 100, 100) : (placedCount >= towerLimit ? sf::Color(255, 120, 120) : sf::Color(210, 210, 210)));
        limitText.setPosition({mapWidth + 108.f, slotY + 52.f});
        window.draw(limitText);
    }
}

void Game::renderTowerPreview() {
    if (!canEditTowers()) return;
    if (roundPreviewVisible) return;
    if (!towerSelected && !selectedPlacedTower) return;

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;

    if (mousePos.x >= mapWidth) return;

    std::shared_ptr<Tower> ignoredTower = selectedPlacedTower;
    bool valid = isValidTowerPosition(mousePos, ignoredTower, false);
    sf::Color previewColor = valid
        ? sf::Color(80, 255, 120, 150)
        : sf::Color(255, 80, 80, 150);

    TowerType previewType = selectedPlacedTower ? selectedPlacedTower->getType() : selectedTowerType;
    float previewRange = selectedPlacedTower ? selectedPlacedTower->getRange() : 0.f;

    sf::Texture* tex = TextureManager::getInstance().getTowerTexture(previewType);
    if (tex) {
        sf::Sprite preview(*tex);
        preview.setOrigin({32.f, 32.f});
        preview.setPosition(mousePos);
        preview.setColor(previewColor);
        window.draw(preview);
    } else {
        sf::CircleShape placeholder(24.f);
        placeholder.setOrigin({24.f, 24.f});
        placeholder.setPosition(mousePos);
        placeholder.setFillColor(previewColor);
        window.draw(placeholder);
    }

    if (previewRange <= 0.f) {
        switch (previewType) {
            case TowerType::NINO_PALO: previewRange = 120.f; break;
            case TowerType::VIEJO_MACHETE: previewRange = 90.f; break;
            case TowerType::TAQUERO: previewRange = 150.f; break;
            case TowerType::ABUELITA: previewRange = 2000.f; break;
            case TowerType::DON_COHETES: previewRange = 204.f; break;
            case TowerType::ORGANILLERO: previewRange = 170.f; break;
            case TowerType::RASPADERO: previewRange = 160.f; break;
            default: previewRange = 120.f; break;
        }
    }

    if (previewRange < 1000.f) {
        sf::CircleShape rangePreview(previewRange);
        rangePreview.setOrigin({previewRange, previewRange});
        rangePreview.setPosition(mousePos);
        rangePreview.setFillColor(valid ? sf::Color(80, 255, 120, 25) : sf::Color(255, 80, 80, 25));
        rangePreview.setOutlineColor(valid ? sf::Color(80, 255, 120, 120) : sf::Color(255, 80, 80, 120));
        rangePreview.setOutlineThickness(2.f);
        window.draw(rangePreview);
    }
}

void Game::renderRoundPreview() {
    if (currentState != GameState::ROUND_PAUSE || !roundPreviewVisible) return;

    WavePreview preview = getWavePreviewForRound(currentRound);
    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;

    sf::RectangleShape overlay({mapWidth, static_cast<float>(height)});
    overlay.setPosition({0.f, 0.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 145));
    window.draw(overlay);

    sf::RectangleShape box({520.f, 430.f});
    box.setOrigin({260.f, 215.f});
    box.setPosition({mapWidth / 2.f, static_cast<float>(height) / 2.f});
    box.setFillColor(sf::Color(20, 20, 20, 210));
    box.setOutlineColor(sf::Color(255, 255, 255, 120));
    box.setOutlineThickness(2.f);
    window.draw(box);

    float x = mapWidth / 2.f - 220.f;
    float y = static_cast<float>(height) / 2.f - 175.f;

    sf::Text title(hudFont, "Proxima ronda " + std::to_string(currentRound) + "/" + std::to_string(MAX_ROUNDS), 30);
    title.setFillColor(sf::Color::White);
    title.setPosition({x, y});
    window.draw(title);

    sf::Text moneyText(hudFont, "Dulces: $" + std::to_string(static_cast<int>(playerMoney)), 22);
    moneyText.setFillColor(sf::Color(255, 215, 0));
    moneyText.setPosition({x, y + 52.f});
    window.draw(moneyText);

    std::vector<std::string> lines = {
        "Total de pinatas: " + std::to_string(preview.total),
        "Engrudo: " + std::to_string(preview.engrudo),
        "Arcilla: " + std::to_string(preview.arcilla),
        "Fruta: " + std::to_string(preview.fruta),
        "Revelacion: " + std::to_string(preview.revelacion),
        "Hipnotizadora: " + std::to_string(preview.hipnotizadora)
    };

    for (size_t i = 0; i < lines.size(); ++i) {
        sf::Text line(hudFont, lines[i], 22);
        line.setFillColor(sf::Color::White);
        line.setPosition({x, y + 100.f + static_cast<float>(i) * 34.f});
        window.draw(line);
    }

    sf::Text hint(hudFont, "SPACE para preparar defensas", 24);
    hint.setFillColor(sf::Color::Yellow);
    hint.setPosition({x, y + 335.f});
    window.draw(hint);
}

void Game::updateDebugInfo() {
    std::ostringstream oss;
    oss << "State: ";
    switch (currentState) {
        case GameState::MENU: oss << "MENU"; break;
        case GameState::TUTORIAL: oss << "TUTORIAL"; break;
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
    if (currentState == GameState::VICTORY || currentState == GameState::DEFEAT) return;

    towerSelected = false;
    clearTowerSelection();
    currentState = GameState::ROUND_ACTIVE;
    enemiesLeftToSpawn = getEnemyCountForRound(currentRound);
    spawnInterval = getSpawnIntervalForRound(currentRound);
    spawnTimer = 0.f;
    std::cout << "[WAVE] Oleada " << currentRound << " iniciada! Enemigos: " 
              << enemiesLeftToSpawn << std::endl;
}
void Game::renderHUD() {
    // Barra de fondo
    float mapWidth = static_cast<float>(width) - PANEL_WIDTH;
    sf::RectangleShape hudBar(sf::Vector2f(mapWidth, 40.f));
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
    sf::Text roundText(hudFont, "Ronda: " + std::to_string(currentRound) + "/" + std::to_string(MAX_ROUNDS), 20);
    roundText.setFillColor(sf::Color::White);
    roundText.setPosition({320.f, 8.f});
    window.draw(roundText);

    // Mensaje de estado
    std::string msg = "";
    if (currentState == GameState::TUTORIAL) {
        if (tutorialChoiceVisible) {
            msg = "ENTER: Tutorial | SPACE: Saltar";
        } else if (tutorialWaveActive) {
            msg = "Tutorial: mini oleada";
        } else {
            msg = "Tutorial interactivo";
        }
    } else if (currentState == GameState::MENU || currentState == GameState::ROUND_PAUSE) {
        msg = (currentState == GameState::ROUND_PAUSE && roundPreviewVisible)
            ? "SPACE: Preparar defensas"
            : "SPACE: Iniciar oleada";
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

    sf::RectangleShape speedButton({130.f, 30.f});
    speedButton.setPosition({mapWidth - 145.f, 5.f});
    speedButton.setFillColor(gameSpeedMultiplier > 1.f ? sf::Color(90, 120, 40, 230) : sf::Color(45, 45, 45, 230));
    speedButton.setOutlineColor(sf::Color(255, 255, 255, 140));
    speedButton.setOutlineThickness(1.f);
    window.draw(speedButton);

    sf::Text speedText(hudFont, "VEL x" + std::to_string(static_cast<int>(gameSpeedMultiplier)), 18);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition({mapWidth - 125.f, 10.f});
    window.draw(speedText);
}
