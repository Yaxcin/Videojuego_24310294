#ifndef ORGANILLERO_HPP
#define ORGANILLERO_HPP

#include "Towers/Tower.hpp"

class Organillero : public Tower {
public:
    Organillero(float x, float y)
        : Tower(x, y, TowerType::ORGANILLERO, 170.f, 0, 1.f, 160) {}
};

#endif
