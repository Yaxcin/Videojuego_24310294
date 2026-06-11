#ifndef VIEJO_MACHETE_HPP
#define VIEJO_MACHETE_HPP

#include "Towers/Tower.hpp"

class ViejoMachete : public Tower {
public:
    ViejoMachete(float x, float y)
        : Tower(x, y, TowerType::VIEJO_MACHETE, 90.f, 35, 0.8f, 100) {}
};

#endif
