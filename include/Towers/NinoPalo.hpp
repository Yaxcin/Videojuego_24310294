#ifndef NINO_PALO_HPP
#define NINO_PALO_HPP

#include "Towers/Tower.hpp"

class NinoPalo : public Tower {
public:
    NinoPalo(float x, float y)
        : Tower(x, y, TowerType::NINO_PALO, 125.f, 30, 1.25f, 75) {}
};

#endif
