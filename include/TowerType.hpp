#ifndef TOWER_TYPE_HPP
#define TOWER_TYPE_HPP

#include <string>

enum class TowerType {
    NINO_PALO,
    VIEJO_MACHETE,
    TAQUERO,
    ABUELITA,
    DON_COHETES,
    ORGANILLERO,
    RASPADERO
};

inline std::string getTowerTexturePath(TowerType type) {
    switch (type) {
        case TowerType::NINO_PALO:      return "assets/textures/NINO_PALO.png";
        case TowerType::VIEJO_MACHETE:  return "assets/textures/VIEJO_MACHETE.png";
        case TowerType::TAQUERO:        return "assets/textures/TAQUERO.png";
        case TowerType::ABUELITA:       return "assets/textures/ABUELITA.png";
        case TowerType::DON_COHETES:    return "assets/textures/DON_COHETES.png";
        case TowerType::ORGANILLERO:    return "assets/textures/ORGANILLERO.png";
        case TowerType::RASPADERO:      return "assets/textures/RASPADERO.png";
        default:                        return "";
    }
}

inline std::string getTowerName(TowerType type) {
    switch (type) {
        case TowerType::NINO_PALO:      return "Nino con Palo";
        case TowerType::VIEJO_MACHETE:  return "Viejo con Machete";
        case TowerType::TAQUERO:        return "Taquero";
        case TowerType::ABUELITA:       return "Abuelita";
        case TowerType::DON_COHETES:    return "Don de los Cohetes";
        case TowerType::ORGANILLERO:    return "Organillero";
        case TowerType::RASPADERO:      return "Raspadero";
        default:                        return "Unknown";
    }
}

#endif
