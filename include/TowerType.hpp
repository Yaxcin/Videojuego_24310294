#ifndef TOWER_TYPE_HPP
#define TOWER_TYPE_HPP

#include <string>

// Enum de tipos de torres/personajes
enum class TowerType {
    NIÑO_PALO,           // Niño con palo - Básico
    VIEJO_MACHETE,       // Viejo con machete - Intermedio
    TAQUERO,             // Taquero
    ABUELITA,            // Abuelita (Vieja con chancla)
    DON_COHETES,         // Don de los cohetes
    ORGANILLERO,         // Organillero
    RASPADERO            // Raspadero
};

// Función auxiliar para obtener el nombre del archivo de sprite
inline std::string getTowerTexturePath(TowerType type) {
    switch (type) {
        case TowerType::NIÑO_PALO:
            return "assets/textures/Niño con palo.png";
        case TowerType::VIEJO_MACHETE:
            return "assets/textures/Viejo con machete.png";
        case TowerType::TAQUERO:
            return "assets/textures/Taquero.png";
        case TowerType::ABUELITA:
            return "assets/textures/Veija con chancla.png";
        case TowerType::DON_COHETES:
            return "assets/textures/El don de los cohetes.png";
        case TowerType::ORGANILLERO:
            return "assets/textures/Organillero.png";
        case TowerType::RASPADERO:
            return "assets/textures/Raspadero.png";
        default:
            return "";
    }
}

// Función auxiliar para obtener el nombre del tipo
inline std::string getTowerName(TowerType type) {
    switch (type) {
        case TowerType::NIÑO_PALO:
            return "Niño con Palo";
        case TowerType::VIEJO_MACHETE:
            return "Viejo con Machete";
        case TowerType::TAQUERO:
            return "Taquero";
        case TowerType::ABUELITA:
            return "Abuelita";
        case TowerType::DON_COHETES:
            return "Don de los Cohetes";
        case TowerType::ORGANILLERO:
            return "Organillero";
        case TowerType::RASPADERO:
            return "Raspadero";
        default:
            return "Unknown";
    }
}

#endif
