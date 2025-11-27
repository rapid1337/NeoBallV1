#include "Ball.h"

Ball::Ball(float x, float y, float vx, float vy) 
    : velX(vx), velY(vy), active(true), isFireball(false) {
    rect = {x, y, 16.0f, 16.0f};
}

void Ball::update(float dt) {
    rect.x += velX * dt;
    rect.y += velY * dt;

    // Wandkollisionen (Links/Rechts/Oben)
    if (rect.x < 0) { rect.x = 0; velX *= -1; }
    if (rect.x > 800 - rect.w) { rect.x = 800 - rect.w; velX *= -1; } 
    if (rect.y < 0) { rect.y = 0; velY *= -1; }
    
    // Ball fällt unten raus -> inaktiv
    if (rect.y > 600) active = false; 
}