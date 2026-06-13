#ifndef DON_COHETES_HPP
#define DON_COHETES_HPP

#include "Towers/Tower.hpp"

class DonCohetes : public Tower {
public:
    DonCohetes(float x, float y)
        : Tower(x, y, TowerType::DON_COHETES, 204.f, 10, 0.65f, 200) {}
};

#endif
