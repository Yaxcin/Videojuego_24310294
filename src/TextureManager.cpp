#include "TextureManager.hpp"
#include <iostream>

bool TextureManager::loadTowerTexture(TowerType type) {
    if (towerTextures.find(type) != towerTextures.end()) return true;

    auto texture = std::make_shared<sf::Texture>();
    std::string path = getTowerTexturePath(type);

    if (!texture->loadFromFile(path)) {
        std::cerr << "[TEXTURE ERROR] No se pudo cargar: " << path << std::endl;
        return false;
    }

    towerTextures[type] = texture;
    std::cout << "[TEXTURE] Cargada: " << getTowerName(type) << std::endl;
    return true;
}

sf::Texture* TextureManager::getTowerTexture(TowerType type) {
    auto it = towerTextures.find(type);
    if (it != towerTextures.end()) return it->second.get();
    return nullptr;
}

bool TextureManager::loadAllTowerTextures() {
    std::cout << "[TEXTURE] Cargando texturas..." << std::endl;
    bool ok = true;
    ok &= loadTowerTexture(TowerType::NINO_PALO);
    ok &= loadTowerTexture(TowerType::VIEJO_MACHETE);
    ok &= loadTowerTexture(TowerType::TAQUERO);
    ok &= loadTowerTexture(TowerType::ABUELITA);
    ok &= loadTowerTexture(TowerType::DON_COHETES);
    ok &= loadTowerTexture(TowerType::ORGANILLERO);
    ok &= loadTowerTexture(TowerType::RASPADERO);
    return ok;
}

void TextureManager::clear() {
    towerTextures.clear();
}
