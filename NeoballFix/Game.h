#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include "Structs.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

struct Particle {
    float x, y;
    float velX, velY;
    float life;
    Color color;
};

struct PowerUp {
    SDL_FRect rect;
    PowerUpType type;
    bool active;
};

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    
    // Resource Management
    void loadTextures();
    SDL_Texture* loadTexture(const char* file);

    // Helpers
    void resetBall();
    void nextLevel();
    void loadLevel(int levelIndex);
    void createBrick(float x, float y, int type);
    void spawnParticles(float x, float y, Color c);
    void trySpawnItem(float x, float y);
    
    // Rendering Helpers
    void drawNumber(int number, float x, float y, float scale);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = false;
    Uint64 lastTime = 0;

    // --- Texturen ---
    SDL_Texture* texBg = nullptr;
    SDL_Texture* texPaddle = nullptr;
    SDL_Texture* texBall = nullptr;
    SDL_Texture* texBrickWood = nullptr;
    SDL_Texture* texBrickStone = nullptr;
    SDL_Texture* texBrickGold = nullptr;
    SDL_Texture* texBrickGreen = nullptr;
    SDL_Texture* texItemPower = nullptr;
    SDL_Texture* texItemPoint = nullptr;

    Paddle* paddle = nullptr;
    
    std::vector<Ball> balls;
    std::vector<Brick> bricks;
    std::vector<Particle> particles;
    std::vector<PowerUp> powerups;
    std::vector<PointItem> pointItems;

    float shakeTime = 0.0f; 

    int lives;
    bool ballStuckToPaddle;
    bool isGameOver;
    int score;
    int currentLevelIndex;
};