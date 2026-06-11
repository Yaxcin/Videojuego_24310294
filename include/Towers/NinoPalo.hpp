#ifndef NINO_PALO_HPP
#define NINO_PALO_HPP

#include "Towers/Tower.hpp"

class NinoPalo : public Tower {
public:
    NinoPalo(float x, float y)
        : Tower(x, y, TowerType::NINO_PALO, 120.f, 15, 1.2f, 75) {}
};

#endif