#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>

Game::Game() : lives(3), ballStuckToPaddle(true), isGameOver(false), score(0), currentLevelIndex(1) {}

Game::~Game() {
    if (texBg) SDL_DestroyTexture(texBg);
    if (texPaddle) SDL_DestroyTexture(texPaddle);
    if (texBall) SDL_DestroyTexture(texBall);
    if (texBrickWood) SDL_DestroyTexture(texBrickWood);
    if (texBrickStone) SDL_DestroyTexture(texBrickStone);
    if (texBrickGold) SDL_DestroyTexture(texBrickGold);
    if (texBrickGreen) SDL_DestroyTexture(texBrickGreen);
    if (texItemPower) SDL_DestroyTexture(texItemPower);
    if (texItemPoint) SDL_DestroyTexture(texItemPoint);

    delete paddle;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Game::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;
    window = SDL_CreateWindow(title, width, height, 0);
    renderer = SDL_CreateRenderer(window, NULL);
    
    srand((unsigned int)time(0));
    
    loadTextures();

    paddle = new Paddle((float)width / 2.0f - 50.0f, (float)height - 50.0f);
    
    lives = 3;
    score = 0;
    currentLevelIndex = 1; 
    isGameOver = false;
    
    loadLevel(currentLevelIndex);
    resetBall();
    
    isRunning = true;
    return true;
}

SDL_Texture* Game::loadTexture(const char* file) {
    char path[256];
    snprintf(path, sizeof(path), "graphics/%s", file);
    
    SDL_Surface* surf = SDL_LoadBMP(path);
    if (!surf) {
        SDL_Log("Info: Bild %s nicht gefunden. Nutze Fallback-Grafik.", path);
        return nullptr;
    }
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    return tex;
}

void Game::loadTextures() {
    texBg = loadTexture("bg.bmp");
    texPaddle = loadTexture("paddle.bmp");
    texBall = loadTexture("ball.bmp");
    texBrickWood = loadTexture("brick_wood.bmp");
    texBrickStone = loadTexture("brick_stone.bmp");
    texBrickGold = loadTexture("brick_gold.bmp");
    texBrickGreen = loadTexture("brick_green.bmp");
    texItemPower = loadTexture("item_power.bmp");
    texItemPoint = loadTexture("item_point.bmp");
}

void Game::resetBall() {
    balls.clear();
    paddle->setWidth(100.0f); 
    SDL_FRect pRect = paddle->getRect();
    Ball b(pRect.x + pRect.w/2.0f - 8.0f, pRect.y - 20.0f, 0.0f, 0.0f); 
    balls.push_back(b);
    ballStuckToPaddle = true;
}

void Game::createBrick(float x, float y, int type) {
    int health = (type == 1) ? 1 : (type == 2 ? 2 : 3);
    
    float r, g, b;
    // Fallback Farben
    if (type == 1) { r=0.8f; g=0.5f; b=0.2f; } 
    else if (type == 2) { r=0.5f; g=0.5f; b=0.6f; } 
    else if (type == 3) { r=0.9f; g=0.8f; b=0.1f; } 
    else { r=0.2f; g=0.8f; b=0.2f; } 

    bricks.push_back(Brick(x, y, health, type, {r, g, b, 1.0f}));
}

void Game::loadLevel(int levelIndex) {
    bricks.clear(); powerups.clear(); pointItems.clear(); particles.clear();
    float bW = 60.0f; 
    
    if (levelIndex == 1) { // Pyramide
        for (int row = 0; row < 8; row++) {
            float offsetX = (8 - row) * (bW / 2.0f); 
            for (int col = 0; col <= row; col++) 
                createBrick(150.0f + offsetX + col * 70.0f, 50.0f + row * 40.0f, (row % 2) + 1);
        }
    } else if (levelIndex == 2) { // Haus
        for(int x=0; x<5; x++) for(int y=2; y<6; y++) 
            if(x==0 || x==4 || y==2 || y==5) createBrick(200.0f + x*70.0f, 50.0f + y*40.0f, 2);
        createBrick(340.0f, 50.0f, 3); createBrick(270.0f, 90.0f, 2); createBrick(410.0f, 90.0f, 2);
        createBrick(270.0f, 170.0f, 1); createBrick(410.0f, 170.0f, 1); createBrick(340.0f, 210.0f, 1);
        createBrick(270.0f, 250.0f, 1); createBrick(410.0f, 250.0f, 1);
    } else if (levelIndex == 3) { // Flugzeug
        for(int y=0; y<8; y++) createBrick(350.0f, 50.0f + y*40.0f, 2);
        for(int x=0; x<7; x++) createBrick(140.0f + x*70.0f, 170.0f, 3);
        createBrick(280.0f, 330.0f, 1); createBrick(420.0f, 330.0f, 1); createBrick(350.0f, 20.0f, 3);
    } else if (levelIndex == 4) { // Zug
        for(int i=0; i<4; i++) createBrick(100.0f + i*100.0f, 400.0f, 2);
        for(int x=0; x<7; x++) createBrick(80.0f + x*70.0f, 360.0f, 2);
        for(int x=4; x<7; x++) for(int y=0; y<3; y++) createBrick(80.0f + x*70.0f, 240.0f + y*40.0f, 1);
        for(int x=0; x<4; x++) for(int y=1; y<3; y++) createBrick(80.0f + x*70.0f, 240.0f + y*40.0f, 3);
        createBrick(150.0f, 200.0f, 2);
    }
}

void Game::nextLevel() {
    currentLevelIndex++;
    if (currentLevelIndex > 4) currentLevelIndex = 1; 
    loadLevel(currentLevelIndex);
    resetBall();
}

void Game::trySpawnItem(float x, float y) {
    if ((powerups.size() + pointItems.size()) >= 4) return; 
    int roll = rand() % 100;
    
    if (roll < 20) {
        PowerUp pu; pu.rect = {x, y, 20.0f, 20.0f}; pu.active = true;
        pu.type = (PowerUpType)(rand() % 3);
        powerups.push_back(pu);
    } else if (roll < 60) {
        PointItem pi; pi.x = x; pi.y = y; pi.active = true;
        int pRoll = rand() % 100;
        if (pRoll < 10) { pi.value = 50; pi.radius = 6.0f; pi.velY = 250.0f; pi.color = {1,0,0,1}; }
        else if (pRoll < 35) { pi.value = 25; pi.radius = 9.0f; pi.velY = 180.0f; pi.color = {1,0.5f,0,1}; }
        else if (pRoll < 65) { pi.value = 20; pi.radius = 10.0f; pi.velY = 150.0f; pi.color = {1,1,0,1}; }
        else { pi.value = 10; pi.radius = 14.0f; pi.velY = 100.0f; pi.color = {0.2f,1,0.2f,1}; }
        pointItems.push_back(pi);
    }
}

void Game::spawnParticles(float x, float y, Color c) {
    for (int i = 0; i < 6; i++) {
        Particle p; p.x = x; p.y = y;
        p.velX = (float)(rand() % 200 - 100); p.velY = (float)(rand() % 200 - 100);
        p.life = 1.0f; p.color = c;
        particles.push_back(p);
    }
}

void Game::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) isRunning = false;
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (isGameOver) {
                if (event.key.key == SDLK_SPACE) init("NeoBall", 800, 600); 
            } else {
                if (ballStuckToPaddle && event.key.key == SDLK_SPACE) {
                    ballStuckToPaddle = false;
                    // Reduzierte Geschwindigkeit wie gewünscht (~360 statt 400)
                    if (!balls.empty()) balls[0].setVelocity(0.0f, -360.0f);
                }
            }
        }
    }
}

void Game::update(float dt) {
    if (isGameOver) return;
    paddle->update(dt, 800);

    if (ballStuckToPaddle && !balls.empty()) {
        SDL_FRect pRect = paddle->getRect();
        SDL_FRect bRect = balls[0].getRect();
        balls[0].setPosition(pRect.x + (pRect.w/2.0f)-(bRect.w/2.0f), pRect.y - bRect.h);
    }

    for (auto& pi : pointItems) {
        if (!pi.active) continue;
        pi.y += pi.velY * dt;
        SDL_FRect pRect = paddle->getRect();
        float cx = std::max(pRect.x, std::min(pi.x, pRect.x + pRect.w));
        float cy = std::max(pRect.y, std::min(pi.y, pRect.y + pRect.h));
        if (((pi.x-cx)*(pi.x-cx) + (pi.y-cy)*(pi.y-cy)) < (pi.radius*pi.radius)) {
            pi.active = false; score += pi.value; 
        }
        if (pi.y > 600) pi.active = false;
    }

    for (auto& p : powerups) {
        if (!p.active) continue;
        p.rect.y += 150.0f * dt;
        SDL_FRect pRect = paddle->getRect();
        if (SDL_HasRectIntersectionFloat(&p.rect, &pRect)) {
            p.active = false;
            if (p.type == PU_WIDE) paddle->setWidth(150.0f);
            if (p.type == PU_FIRE) for(auto& b : balls) b.setFireball(true);
            if (p.type == PU_MULTI) {
                ballStuckToPaddle = false;
                if (!balls.empty()) balls[0].setVelocity(balls[0].getRect().x>400?-180.0f:180.0f, -360.0f);
                balls.push_back(Ball(pRect.x, pRect.y-30.0f, -180.0f, -360.0f));
                balls.push_back(Ball(pRect.x, pRect.y-30.0f, 180.0f, -360.0f));
            }
        }
        if (p.rect.y > 600) p.active = false;
    }

    bool anyBallActive = false;
    int activeBricksCount = 0;
    for(const auto& br : bricks) if(br.isActive()) activeBricksCount++;

    for (auto& ball : balls) {
        if (!ball.isActive()) continue;
        anyBallActive = true;
        if (!ballStuckToPaddle) ball.update(dt);
        
        SDL_FRect bRect = ball.getRect();
        SDL_FRect pRect = paddle->getRect();

        // Paddle Kollision FIX
        if (!ballStuckToPaddle && SDL_HasRectIntersectionFloat(&bRect, &pRect)) {
            // Nur abprallen, wenn er nach UNTEN fliegt
            if (ball.getVelY() > 0) {
                // Anti-Tunneling: Ball hart auf Paddelkante setzen
                ball.setPosition(bRect.x, pRect.y - bRect.h - 0.1f);
                ball.invertY();
                float hitPos = (bRect.x + bRect.w/2.0f) - (pRect.x + pRect.w/2.0f);
                ball.setVelX(hitPos * 4.5f); 
            }
        }

        for (auto& brick : bricks) {
            if (!brick.isActive()) continue;
            brick.updateAnimation(dt);
            SDL_FRect brRect = brick.getRect();
            if (SDL_HasRectIntersectionFloat(&bRect, &brRect)) {
                if(!ball.isFire()) ball.invertY(); 
                brick.hit(); 
                if (!brick.isActive()) {
                    shakeTime = 0.1f;
                    score += 10; 
                    spawnParticles(brRect.x + brRect.w/2.0f, brRect.y + brRect.h/2.0f, brick.getColor());
                    trySpawnItem(brRect.x + brRect.w/2.0f, brRect.y); 
                    activeBricksCount--;
                }
                break; 
            }
        }
    }

    // Nur nächstes Level, wenn ALLE Steine weg sind
    if (activeBricksCount == 0 && !bricks.empty()) { nextLevel(); return; }

    if (!anyBallActive && !ballStuckToPaddle) {
        lives--;
        if (lives > 0) resetBall(); else isGameOver = true;
    }

    for (auto& p : particles) {
        p.x += p.velX * dt; p.y += p.velY * dt; p.velY += 300.0f * dt; p.life -= 2.0f * dt;
    }
    if (shakeTime > 0) shakeTime -= dt;
}

void Game::drawNumber(int number, float x, float y, float scale) {
    std::string s = std::to_string(number);
    float cursorX = x;
    for (char c : s) {
        int digit = c - '0';
        float w = 10.0f * scale; float h = 20.0f * scale; float t = 2.0f * scale;  
        SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, 1);
        bool segs[7] = {false};
        if(digit!=1 && digit!=4) segs[0]=true;
        if(digit!=1 && digit!=2 && digit!=3 && digit!=7) segs[1]=true;
        if(digit!=5 && digit!=6) segs[2]=true;
        if(digit!=0 && digit!=1 && digit!=7) segs[3]=true;
        if(digit==0 || digit==2 || digit==6 || digit==8) segs[4]=true;
        if(digit!=2) segs[5]=true;
        if(digit!=1 && digit!=4 && digit!=7) segs[6]=true;
        SDL_FRect r;
        if(segs[0]) { r={cursorX, y, w, t}; SDL_RenderFillRect(renderer, &r); }
        if(segs[1]) { r={cursorX, y, t, h/2}; SDL_RenderFillRect(renderer, &r); }
        if(segs[2]) { r={cursorX+w-t, y, t, h/2}; SDL_RenderFillRect(renderer, &r); }
        if(segs[3]) { r={cursorX, y+h/2-t/2, w, t}; SDL_RenderFillRect(renderer, &r); }
        if(segs[4]) { r={cursorX, y+h/2, t, h/2}; SDL_RenderFillRect(renderer, &r); }
        if(segs[5]) { r={cursorX+w-t, y+h/2, t, h/2}; SDL_RenderFillRect(renderer, &r); }
        if(segs[6]) { r={cursorX, y+h-t, w, t}; SDL_RenderFillRect(renderer, &r); }
        cursorX += w + 5.0f * scale;
    }
}

void Game::run() {
    lastTime = SDL_GetTicks();
    while (isRunning) {
        Uint64 current = SDL_GetTicks();
        float deltaTime = (current - lastTime) / 1000.0f;
        lastTime = current;
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::render() {
    SDL_SetRenderDrawColorFloat(renderer, 0.1f, 0.1f, 0.15f, 1.0f);
    SDL_RenderClear(renderer);

    if (texBg) {
        SDL_RenderTexture(renderer, texBg, NULL, NULL); 
    }

    float sX = (shakeTime>0) ? (float)(rand()%6-3) : 0.0f;
    float sY = (shakeTime>0) ? (float)(rand()%6-3) : 0.0f;

    auto ApplyShake = [&](SDL_FRect r) { 
        r.x += sX; r.y += sY; return r; 
    };

    if (!isGameOver) {
        // Bricks
        for (const auto& b : bricks) {
            if (b.isActive()) {
                SDL_FRect br = ApplyShake(b.getRect());
                br.x += b.getZOffset() * 0.5f; 
                br.y += b.getZOffset() * 0.5f;

                SDL_Texture* t = nullptr;
                if (b.getType() == 1) t = texBrickWood;
                else if (b.getType() == 2) t = texBrickStone;
                else if (b.getType() == 3) t = texBrickGold;
                else t = texBrickGreen;

                if (t) {
                    SDL_RenderTexture(renderer, t, NULL, &br);
                } else {
                    Color c = b.getColor();
                    SDL_SetRenderDrawColorFloat(renderer, c.r, c.g, c.b, c.a);
                    SDL_RenderFillRect(renderer, &br);
                }
            }
        }

        // Paddle
        SDL_FRect pr = ApplyShake(paddle->getRect());
        if (texPaddle) SDL_RenderTexture(renderer, texPaddle, NULL, &pr);
        else {
            SDL_SetRenderDrawColorFloat(renderer, 0.6f, 0.6f, 0.8f, 1.0f);
            SDL_RenderFillRect(renderer, &pr);
        }

        // Items
        for(const auto& pi : pointItems) {
            if(pi.active) {
                SDL_FRect ir = {pi.x - pi.radius, pi.y - pi.radius, pi.radius*2, pi.radius*2};
                if(texItemPoint) SDL_RenderTexture(renderer, texItemPoint, NULL, &ir);
                else {
                    Color c = pi.color; SDL_SetRenderDrawColorFloat(renderer, c.r, c.g, c.b, c.a);
                    SDL_RenderFillRect(renderer, &ir);
                }
            }
        }

        // PowerUps
        for (const auto& p : powerups) {
            if(p.active) {
                SDL_FRect pr = p.rect;
                if(texItemPower) SDL_RenderTexture(renderer, texItemPower, NULL, &pr);
                else {
                    Color c = {0,1,0,1}; if(p.type==PU_FIRE) c={1,0,0,1}; if(p.type==PU_WIDE) c={0,0,1,1};
                    SDL_SetRenderDrawColorFloat(renderer, c.r, c.g, c.b, 1);
                    SDL_RenderFillRect(renderer, &pr);
                }
            }
        }

        // Ball
        for (const auto& b : balls) {
            if (!b.isActive()) continue;
            SDL_FRect br = b.getRect();
            if (texBall) {
                if(b.isFire()) SDL_SetTextureColorMod(texBall, 255, 100, 100);
                else SDL_SetTextureColorMod(texBall, 255, 255, 255);
                SDL_RenderTexture(renderer, texBall, NULL, &br);
            } else {
                Color bc = b.isFire() ? Color{1, 0.2f, 0, 1} : Color{1, 1, 1, 1};
                SDL_SetRenderDrawColorFloat(renderer, bc.r, bc.g, bc.b, 1.0f);
                SDL_RenderFillRect(renderer, &br);
            }
        }

        // Partikel
        for (const auto& p : particles) {
            if (p.life > 0) {
                SDL_SetRenderDrawColorFloat(renderer, p.color.r, p.color.g, p.color.b, p.life);
                SDL_FRect pr = {p.x, p.y, 4.0f, 4.0f};
                SDL_RenderFillRect(renderer, &pr);
            }
        }
    } else {
        // Game Over Overlay
        SDL_SetRenderDrawColorFloat(renderer, 1, 0, 0, 0.5f);
        SDL_RenderFillRect(renderer, NULL); 
    }

    // UI: Leben
    for(int i=0; i < lives; i++) {
        SDL_FRect heart = { 10.0f + i * 25.0f, 570.0f, 20.0f, 20.0f };
        if(texBall) SDL_RenderTexture(renderer, texBall, NULL, &heart);
        else {
            SDL_SetRenderDrawColorFloat(renderer, 0.8f, 0.2f, 0.2f, 1);
            SDL_RenderFillRect(renderer, &heart);
        }
    }
    
    drawNumber(score, 650.0f, 20.0f, 1.5f);
    drawNumber(currentLevelIndex, 20.0f, 20.0f, 1.0f);

    SDL_RenderPresent(renderer);
}