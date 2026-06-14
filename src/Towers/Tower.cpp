#include "Towers/Tower.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace {
    std::mt19937& rng() {
        static std::random_device seed;
        static std::mt19937 generator(seed());
        return generator;
    }

    int getEffectiveDamage(TowerType towerType, int baseDamage, const std::shared_ptr<Pinata>& target) {
        if (!target || target->getType() != PinataType::ARCILLA) {
            return baseDamage;
        }

        if (towerType == TowerType::VIEJO_MACHETE) {
            return std::max(1, static_cast<int>(baseDamage * 1.25f));
        }

        if (towerType == TowerType::ABUELITA || towerType == TowerType::DON_COHETES) {
            return baseDamage;
        }

        return std::max(1, static_cast<int>(baseDamage * 0.6f));
    }
}

Tower::Tower(float x, float y, TowerType type, float range, int damage, float fireRate, int cost)
    : Entity(x, y), towerType(type), range(range), damage(damage),
      fireRate(fireRate), fireCooldown(0.f), cost(cost), selected(false),
      hasAttackAnimation(false), attackAnimationActive(false), attackAnimationTimer(0.f),
      attackAnimationDuration(0.48f), attackAnimationFrame(0), attackAnimationFrameCount(0),
      hasLoopAnimation(false), loopAnimationActive(false), loopAnimationTimer(0.f),
      loopAnimationDuration(0.8f), loopAnimationFrame(0), loopAnimationFrameCount(0),
      rangeVisualScale(1.f), lastTargetPosition(x, y), attackEffectTimer(0.f), areaEffectTimer(0.f),
      supportActionCooldown(0.f),
      attackSlowTimer(0.f), attackSlowMultiplier(1.f), hypnosisProtectionTimer(0.f),
      hypnotized(false) {

    rangeCircle.setRadius(range);
    rangeCircle.setOrigin({range, range});
    rangeCircle.setPosition(position);
    rangeCircle.setFillColor(sf::Color(255, 255, 255, 30));
    rangeCircle.setOutlineColor(sf::Color(255, 255, 255, 100));
    rangeCircle.setOutlineThickness(1.f);

    initSprite();
    initAttackAnimation();
    initLoopAnimation();
}

void Tower::initSprite() {
    std::string path = getTowerTexturePath(towerType);
    if (texture.loadFromFile(path)) {
        sprite = std::make_unique<sf::Sprite>(texture);
        sprite->setOrigin(sf::Vector2f(32.f, 32.f)); // centro 64x64
        sprite->setPosition(position);
    }
}

void Tower::initAttackAnimation() {
    std::string path;
    switch (towerType) {
        case TowerType::NINO_PALO:
            path = "assets/textures/animations/NINO_PALO_ATTACK.png";
            break;
        case TowerType::VIEJO_MACHETE:
            path = "assets/textures/animations/VIEJO_MACHETE_ATTACK.png";
            break;
        case TowerType::TAQUERO:
            path = "assets/textures/animations/TAQUERO_ATTACK.png";
            break;
        case TowerType::ABUELITA:
            path = "assets/textures/animations/ABUELITA_ATTACK.png";
            break;
        case TowerType::DON_COHETES:
            path = "assets/textures/animations/DON_COHETES_ATTACK.png";
            break;
        case TowerType::RASPADERO:
            path = "assets/textures/animations/RASPADERO_ATTACK.png";
            break;
        default:
            return;
    }

    if (!attackTexture.loadFromFile(path)) return;

    hasAttackAnimation = true;
    attackAnimationFrameCount = 4;
}

void Tower::initLoopAnimation() {
    if (towerType != TowerType::ORGANILLERO) return;
    if (!loopTexture.loadFromFile("assets/textures/animations/ORGANILLERO_PLAYING.png")) return;

    hasLoopAnimation = true;
    loopAnimationFrameCount = 4;
}

void Tower::setLoopAnimationActive(bool active) {
    if (!hasLoopAnimation || !sprite) return;
    if (loopAnimationActive == active) return;

    loopAnimationActive = active;
    loopAnimationTimer = 0.f;
    loopAnimationFrame = -1;

    if (loopAnimationActive) {
        setLoopAnimationFrame(0);
    } else {
        resetIdleSprite();
    }
}

void Tower::updateLoopAnimation(float deltaTime) {
    if (!loopAnimationActive || !hasLoopAnimation || !sprite) return;

    loopAnimationTimer += deltaTime;
    float frameDuration = loopAnimationDuration / static_cast<float>(loopAnimationFrameCount);
    int frame = static_cast<int>(loopAnimationTimer / frameDuration) % loopAnimationFrameCount;
    setLoopAnimationFrame(frame);
}

void Tower::startAttackAnimation() {
    if (!hasAttackAnimation || !sprite) return;

    attackAnimationActive = true;
    attackAnimationTimer = attackAnimationDuration;
    attackAnimationFrame = -1;
    setAttackAnimationFrame(0);
}

void Tower::updateAttackAnimation(float deltaTime) {
    if (!attackAnimationActive || !sprite) return;

    attackAnimationTimer -= deltaTime;
    if (attackAnimationTimer <= 0.f) {
        attackAnimationActive = false;
        resetIdleSprite();
        return;
    }

    float progress = 1.f - (attackAnimationTimer / attackAnimationDuration);
    int frame = std::clamp(
        static_cast<int>(progress * static_cast<float>(attackAnimationFrameCount)),
        0,
        attackAnimationFrameCount - 1
    );
    setAttackAnimationFrame(frame);
}

void Tower::setAttackAnimationFrame(int frame) {
    if (!hasAttackAnimation || !sprite || frame == attackAnimationFrame) return;

    attackAnimationFrame = frame;
    sprite->setTexture(attackTexture, true);
    sprite->setTextureRect(sf::IntRect({frame * 64, 0}, {64, 64}));
    sprite->setOrigin(sf::Vector2f(32.f, 32.f));
    sprite->setPosition(position);
}

void Tower::setLoopAnimationFrame(int frame) {
    if (!hasLoopAnimation || !sprite || frame == loopAnimationFrame) return;

    loopAnimationFrame = frame;
    sprite->setTexture(loopTexture, true);
    sprite->setTextureRect(sf::IntRect({frame * 64, 0}, {64, 64}));
    sprite->setOrigin(sf::Vector2f(32.f, 32.f));
    sprite->setPosition(position);
}

void Tower::resetIdleSprite() {
    if (!sprite) return;

    sprite->setTexture(texture, true);
    sprite->setTextureRect(sf::IntRect({0, 0}, {64, 64}));
    sprite->setOrigin(sf::Vector2f(32.f, 32.f));
    sprite->setPosition(position);
    attackAnimationFrame = 0;
    loopAnimationFrame = 0;
}

void Tower::combat(
    float deltaTime,
    std::vector<std::shared_ptr<Pinata>>& enemies,
    float fireRateMultiplier,
    const std::vector<std::shared_ptr<Tower>>* allTowers
) {
    updateAttackAnimation(deltaTime);
    if (attackEffectTimer > 0.f) attackEffectTimer -= deltaTime;
    if (areaEffectTimer > 0.f) areaEffectTimer -= deltaTime;
    if (supportActionCooldown > 0.f) supportActionCooldown -= deltaTime;
    if (attackSlowTimer > 0.f) {
        attackSlowTimer -= deltaTime;
        if (attackSlowTimer <= 0.f) {
            attackSlowMultiplier = 1.f;
        }
    }
    if (hypnosisProtectionTimer > 0.f) {
        hypnosisProtectionTimer -= deltaTime;
    }
    if (isSupportTower()) return;

    float effectiveMultiplier = std::max(0.1f, fireRateMultiplier * attackSlowMultiplier);
    if (fireCooldown > 0.f) {
        fireCooldown -= deltaTime * effectiveMultiplier;
        return;
    }

    if (towerType == TowerType::ABUELITA && allTowers && supportActionCooldown <= 0.f) {
        auto hypnotizedTower = findNearestHypnotizedTower(*allTowers);
        if (clearHypnosisFromTower(hypnotizedTower)) {
            lastTargetPosition = hypnotizedTower->getPosition();
            attackEffectTimer = 0.18f;
            supportActionCooldown = 2.f;
            return;
        }
    }

    auto target = (towerType == TowerType::ABUELITA)
        ? findMostAdvancedTarget(enemies)
        : findTarget(enemies);

    if (target) {
        lastTargetPosition = target->getPosition();
        if (towerType == TowerType::DON_COHETES) {
            if (attackArea(target, enemies)) {
                areaEffectTimer = 0.25f;
                startAttackAnimation();
            }
        } else {
            if (attack(target)) {
                attackEffectTimer = 0.12f;
                startAttackAnimation();
            }
        }
        fireCooldown = 1.f / fireRate;
    }
}

bool Tower::isInRange(const sf::Vector2f& targetPosition) const {
    return isInScaledRange(targetPosition, 1.f);
}

bool Tower::isInScaledRange(const sf::Vector2f& targetPosition, float rangeScale) const {
    sf::Vector2f diff = targetPosition - position;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    return dist <= range * rangeScale;
}

void Tower::moveTo(float x, float y) {
    position = {x, y};
    rangeCircle.setPosition(position);
    if (sprite) {
        sprite->setPosition(position);
    }
}

void Tower::applyAttackSlow(float multiplier, float duration) {
    attackSlowMultiplier = std::min(attackSlowMultiplier, multiplier);
    attackSlowTimer = std::max(attackSlowTimer, duration);
}

void Tower::setHypnotized(bool value) {
    hypnotized = value && !isImmuneToHypnosis() && !isHypnosisProtected();
}

void Tower::applyHypnosisProtection(float duration) {
    hypnotized = false;
    hypnosisProtectionTimer = std::max(hypnosisProtectionTimer, duration);
}

std::shared_ptr<Pinata> Tower::findTarget(std::vector<std::shared_ptr<Pinata>>& enemies) {
    std::shared_ptr<Pinata> nearest = nullptr;
    float minDist = range;

    for (auto& e : enemies) {
        if (!e || !e->isAlive()) continue;
        sf::Vector2f diff = e->getPosition() - position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= minDist) {
            minDist = dist;
            nearest = e;
        }
    }
    return nearest;
}

std::shared_ptr<Pinata> Tower::findMostAdvancedTarget(std::vector<std::shared_ptr<Pinata>>& enemies) {
    std::shared_ptr<Pinata> mostAdvanced = nullptr;
    float bestProgress = -1.f;

    for (auto& e : enemies) {
        if (!e || !e->isAlive() || !isInRange(e->getPosition())) continue;
        float progress = e->getPathProgress();
        if (progress > bestProgress) {
            bestProgress = progress;
            mostAdvanced = e;
        }
    }
    return mostAdvanced;
}

std::shared_ptr<Tower> Tower::findNearestHypnotizedTower(const std::vector<std::shared_ptr<Tower>>& allTowers) {
    std::shared_ptr<Tower> nearest = nullptr;
    float minDist = range;

    for (const auto& tower : allTowers) {
        if (!tower || tower.get() == this || !tower->isHypnotized()) continue;

        sf::Vector2f diff = tower->getPosition() - position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist <= minDist) {
            minDist = dist;
            nearest = tower;
        }
    }

    return nearest;
}

bool Tower::clearHypnosisFromTower(const std::shared_ptr<Tower>& target) {
    if (!target || !target->isHypnotized()) return false;

    target->applyHypnosisProtection(5.f);
    return true;
}

std::optional<ProjectileRequest> Tower::consumeProjectileRequest() {
    auto request = pendingProjectileRequest;
    pendingProjectileRequest.reset();
    return request;
}

bool Tower::attack(std::shared_ptr<Pinata>& target) {
    if (missesFromHypnosis()) return false;

    int effectiveDamage = getEffectiveDamage(towerType, damage, target);
    if (towerType == TowerType::TAQUERO || towerType == TowerType::ABUELITA || towerType == TowerType::RASPADERO) {
        sf::Vector2f direction = target->getPosition() - position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance > 0.f) {
            direction /= distance;
        }
        ProjectileType projectileType = ProjectileType::Lime;
        if (towerType == TowerType::ABUELITA) projectileType = ProjectileType::Chancla;
        if (towerType == TowerType::RASPADERO) projectileType = ProjectileType::Ice;

        pendingProjectileRequest = ProjectileRequest{
            position + direction * 20.f + sf::Vector2f(0.f, -8.f),
            target,
            effectiveDamage,
            projectileType,
            0.f
        };
        return true;
    }

    target->takeDamage(effectiveDamage);
    return true;
}

bool Tower::attackArea(std::shared_ptr<Pinata>& target, std::vector<std::shared_ptr<Pinata>>& enemies) {
    if (missesFromHypnosis()) return false;
    (void)enemies;

    sf::Vector2f direction = target->getPosition() - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0.f) {
        direction /= distance;
    }

    pendingProjectileRequest = ProjectileRequest{
        position + direction * 22.f + sf::Vector2f(0.f, -10.f),
        target,
        damage,
        ProjectileType::Rocket,
        180.f
    };
    return true;
}

void Tower::render(sf::RenderWindow& window) const {
    if (selected || isSupportTower()) {
        float visualRange = range * rangeVisualScale;
        sf::CircleShape visualRangeCircle(visualRange);
        visualRangeCircle.setOrigin({visualRange, visualRange});
        visualRangeCircle.setPosition(position);
        visualRangeCircle.setFillColor(rangeCircle.getFillColor());
        visualRangeCircle.setOutlineColor(rangeCircle.getOutlineColor());
        visualRangeCircle.setOutlineThickness(rangeCircle.getOutlineThickness());
        window.draw(visualRangeCircle);
    }
    if (sprite) {
        window.draw(*sprite);
    }
    if (isAttackSlowed()) {
        sf::CircleShape slowMarker(34.f);
        slowMarker.setOrigin({34.f, 34.f});
        slowMarker.setPosition(position);
        slowMarker.setFillColor(sf::Color(255, 180, 40, 55));
        slowMarker.setOutlineColor(sf::Color(255, 230, 90, 180));
        slowMarker.setOutlineThickness(3.f);
        window.draw(slowMarker);
    }
    if (isHypnotized()) {
        sf::CircleShape hypnosisMarker(40.f);
        hypnosisMarker.setOrigin({40.f, 40.f});
        hypnosisMarker.setPosition(position);
        hypnosisMarker.setFillColor(sf::Color(150, 80, 220, 45));
        hypnosisMarker.setOutlineColor(sf::Color(210, 150, 255, 210));
        hypnosisMarker.setOutlineThickness(3.f);
        window.draw(hypnosisMarker);
    }
    renderAttackEffect(window);
}  

sf::Color Tower::getAttackEffectColor() const {
    switch (towerType) {
        case TowerType::NINO_PALO: return sf::Color(255, 245, 120, 210);
        case TowerType::VIEJO_MACHETE: return sf::Color(210, 230, 255, 230);
        case TowerType::TAQUERO: return sf::Color(120, 255, 120, 220);
        case TowerType::ABUELITA: return sf::Color(255, 255, 255, 240);
        case TowerType::DON_COHETES: return sf::Color(255, 120, 40, 120);
        case TowerType::RASPADERO: return sf::Color(80, 210, 255, 220);
        default: return sf::Color(255, 255, 255, 180);
    }
}

void Tower::renderAttackEffect(sf::RenderWindow& window) const {
    if (hasAttackAnimation) {
        return;
    }

    if (isSupportTower()) {
        float visualRange = range * rangeVisualScale;
        sf::CircleShape aura(visualRange);
        aura.setOrigin({visualRange, visualRange});
        aura.setPosition(position);
        aura.setFillColor(sf::Color(255, 230, 90, 18));
        aura.setOutlineColor(sf::Color(255, 230, 90, 80));
        aura.setOutlineThickness(2.f);
        window.draw(aura);
        return;
    }

    if (towerType == TowerType::DON_COHETES && areaEffectTimer > 0.f) {
        float progress = areaEffectTimer / 0.25f;
        float radius = range * (1.05f - progress * 0.15f);
        sf::CircleShape explosion(radius);
        explosion.setOrigin({radius, radius});
        explosion.setPosition(position);
        explosion.setFillColor(sf::Color(255, 120, 40, static_cast<std::uint8_t>(90 * progress)));
        explosion.setOutlineColor(sf::Color(255, 220, 80, static_cast<std::uint8_t>(190 * progress)));
        explosion.setOutlineThickness(3.f);
        window.draw(explosion);
        return;
    }

    if (attackEffectTimer <= 0.f) return;

    sf::Vector2f diff = lastTargetPosition - position;
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (length <= 0.f) return;

    sf::RectangleShape beam({length, 4.f});
    beam.setOrigin({0.f, 2.f});
    beam.setPosition(position);
    beam.setRotation(sf::radians(std::atan2(diff.y, diff.x)));
    beam.setFillColor(getAttackEffectColor());
    window.draw(beam);
}

bool Tower::missesFromHypnosis() const {
    if (!isHypnotized()) return false;

    std::uniform_real_distribution<float> chance(0.f, 1.f);
    return chance(rng()) < 0.9f;
}
