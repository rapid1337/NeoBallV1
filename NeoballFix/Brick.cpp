#include "Brick.h"

Brick::Brick(float x, float y, int hp, int t, Color c) 
    : health(hp), type(t), color(c), active(true), zOffset(0.0f) {
    rect = {x, y, 60.0f, 30.0f};
}

void Brick::hit() {
    health--;
    zOffset = 5.0f; // Hüpft bei Treffer
    if (health <= 0) active = false;
}

void Brick::updateAnimation(float dt) {
    if (zOffset > 0) {
        zOffset -= 10.0f * dt;
        if (zOffset < 0) zOffset = 0;
    }
}