#ifndef RASPADERO_HPP
#define RASPADERO_HPP

#include "Towers/Tower.hpp"

class Raspadero : public Tower {
public:
    Raspadero(float x, float y)
        : Tower(x, y, TowerType::RASPADERO, 160.f, 8, 1.3f, 120) {}
};

#endif
