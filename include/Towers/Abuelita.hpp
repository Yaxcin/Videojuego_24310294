#ifndef ABUELITA_HPP
#define ABUELITA_HPP

#include "Towers/Tower.hpp"

class Abuelita : public Tower {
public:
    Abuelita(float x, float y)
        : Tower(x, y, TowerType::ABUELITA, 2000.f, 60, 0.25f, 250) {}
};

#endif
