#include "Paddle.h"

Paddle::Paddle(float x, float y) {
    rect = {x, y, 100.0f, 20.0f};
    targetWidth = 100.0f;
    speed = 500.0f;
}

void Paddle::update(float dt, int windowWidth) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        rect.x -= speed * dt;
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        rect.x += speed * dt;
    }

    // Begrenzung
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > (float)windowWidth) rect.x = (float)windowWidth - rect.w;

    // Animation der Breite
    if (rect.w != targetWidth) {
        rect.w = rect.w + (targetWidth - rect.w) * 5.0f * dt;
    }
}

void Paddle::setWidth(float w) {
    targetWidth = w;
}