#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Menu.h"
#include "World.h"

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() { return isRunning; }

    bool isRunning;
    bool isMenuOpen;

    SDL_Window* window;
    SDL_Renderer* renderer;

private:
    Uint32 lastTime;
    float deltaTime;

    std::vector<Entity*> entitiesToRemove;
    std::vector<std::unique_ptr<Entity>> entities;
    SDL_Texture* loadTexture(const char* fileName);

    Player* player;
    Menu* menu;
    World* world;

    SDL_Rect camera;

    void processInput();
    SDL_Texture* spriteSheet;

    void spawnEnemy();

    void resolveCollision(Player& player, Enemy& enemy);
    void adjustPositionOnCollision(Player& player, Enemy& enemy);
    void applyDamage(Entity& attacker, Entity& target, int damage);

    void renderHealthBar(int x, int y, int currentHealth, int maxHealth);
};

#endif