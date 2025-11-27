#pragma once
#include <SDL3/SDL.h>

class Paddle {
public:
    Paddle(float x, float y);
    
    void update(float dt, int windowWidth);
    void setWidth(float w);
    SDL_FRect getRect() const { return rect; }

private:
    SDL_FRect rect;
    float targetWidth;
    float speed;
};