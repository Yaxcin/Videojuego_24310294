#include "Entity.hpp"

Entity::Entity(float x, float y)
    : position(x, y), velocity(0.0f, 0.0f), radius(10.0f), alive(true) {
}
