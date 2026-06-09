#include "TextureManager.hpp"
#include <iostream>

bool TextureManager::loadTowerTexture(TowerType type) {
    // Si ya está cargada, no hacer nada
    if (towerTextures.find(type) != towerTextures.end()) {
        return true;
    }

    auto texture = std::make_shared<sf::Texture>();
    std::string path = getTowerTexturePath(type);

    if (!texture->loadFromFile(path)) {
        std::cerr << "[TEXTURE ERROR] No se pudo cargar: " << path << std::endl;
        return false;
    }

    towerTextures[type] = texture;
    std::cout << "[TEXTURE] Cargada: " << getTowerName(type) << " (" << path << ")" << std::endl;
    return true;
}

sf::Texture* TextureManager::getTowerTexture(TowerType type) {
    auto it = towerTextures.find(type);
    if (it != towerTextures.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool TextureManager::loadAllTowerTextures() {
    std::cout << "\n[TEXTURE] Cargando texturas de torres..." << std::endl;
    
    bool allLoaded = true;
    allLoaded &= loadTowerTexture(TowerType::NIÑO_PALO);
    allLoaded &= loadTowerTexture(TowerType::VIEJO_MACHETE);
    allLoaded &= loadTowerTexture(TowerType::TAQUERO);
    allLoaded &= loadTowerTexture(TowerType::ABUELITA);
    allLoaded &= loadTowerTexture(TowerType::DON_COHETES);
    allLoaded &= loadTowerTexture(TowerType::ORGANILLERO);
    allLoaded &= loadTowerTexture(TowerType::RASPADERO);

    if (allLoaded) {
        std::cout << "[TEXTURE] ✓ Todas las texturas cargadas exitosamente\n" << std::endl;
    } else {
        std::cerr << "[TEXTURE] ✗ Error cargando texturas\n" << std::endl;
    }

    return allLoaded;
}

void TextureManager::clear() {
    towerTextures.clear();
    std::cout << "[TEXTURE] Recursos limpiados" << std::endl;
}
