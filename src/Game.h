// Game.h
#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>
#include <cmath>
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Menu.h"
#include "World.h"

class Game {
public:
    Game();
    ~Game();                                                                                    /* Constructor and deconstructor */

    void init(const char* title, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() { return isRunning; }
    void resetGame();
    void applyDamage(Entity& attacker, Entity& target, int damage);
    bool isPlayerDeathAnimationFinished() const;
    void removeDeadEntities();                                                                  /* Game thods */
    void updateCamera(float playerX, float playerY);

    bool isRunning;
    bool isMenuOpen;                                                                            /* Window sem - variables */

    SDL_Window* window;
    SDL_Renderer* renderer;                                                                     /* Window variables */

private:
    Uint32 lastTime;
    float deltaTime;                                                                            /* Time variables for smooth movement */

    Uint32 deathTime;
    const Uint32 DEATH_DELAY = 2000;                                                            /* Death dealy time for the death animation */

    std::vector<Entity*> entitiesToRemove;
    std::vector<std::unique_ptr<Entity>> entities;                                              /* Alive and dead entities */

    SDL_Texture* loadTexture(const char* fileName);
    SDL_Texture* spriteSheet;                                                                   /* Texture variables */
    void loadHUDTexture();
    SDL_Texture* hudTexture; 

    Player* player;
    Menu* menu;
    World* world;
    SDL_Rect camera;                                                                           /* Player, menu and world variables */

    void processInput();

    void spawnEnemy();
    void resolveCollision(Player& player, Enemy& enemy);
    void adjustPositionOnCollision(Player& player, Enemy& enemy);
    void renderHealthBar(int x, int y, int currentHealth, int maxHealth);
    void updateSpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities);
    void updateEnemySpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities); 
    void renderHUD();

    friend class Player;
};

#endif