#ifndef TAQUERO_HPP
#define TAQUERO_HPP

#include "Towers/Tower.hpp"

class Taquero : public Tower {
public:
    Taquero(float x, float y)
        : Tower(x, y, TowerType::TAQUERO, 150.f, 12, 1.8f, 125) {}
};

#endif
