#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>
#include "TowerType.hpp"

// Gestor centralizado de texturas
class TextureManager {
public:
    // Singleton
    static TextureManager& getInstance() {
        static TextureManager instance;
        return instance;
    }

    // Cargar textura de torre por tipo
    bool loadTowerTexture(TowerType type);
    
    // Obtener textura cargada
    sf::Texture* getTowerTexture(TowerType type);

    // Cargar todas las texturas de torres
    bool loadAllTowerTextures();

    // Limpiar recursos
    void clear();

private:
    TextureManager() = default;
    ~TextureManager() = default;

    // Mapa de texturas cargadas
    std::map<TowerType, std::shared_ptr<sf::Texture>> towerTextures;

    // Prevenir copia
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
};

#endif
