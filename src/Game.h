#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <algorithm>
#include <limits>
#include <cmath>
#include <vector>
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Menu.h"
#include "World.h"
#include "MazeGenerator.h"
#include "PathfindingManager.h"  // Ensure this is included

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
    void resetGame(bool resetDungeon = true);
    void applyDamage(Entity& attacker, Entity& target, int damage);
    bool isPlayerDeathAnimationFinished() const;
    void removeDeadEntities();
    void updateCamera(float playerX, float playerY);
    void enterDungeon();
    bool checkDungeonEntrance();
    void exitDungeon();
    bool checkDungeonExit();
    void transitionToNextLevel();
    bool checkNextLevelDoor();
    void startLevel(int difficulty);
    bool isWall(float x, float y);
    int getDungeonWidth() const;
    int getDungeonHeight() const;

    bool isRunning;
    bool isMenuOpen;

    SDL_Window* window;
    SDL_Renderer* renderer;

    const std::vector<std::vector<int>>& getDungeonMaze() const { return dungeonMaze; }

private:
    Uint32 lastTime;
    float deltaTime;

    Uint32 deathTime;
    const Uint32 DEATH_DELAY = 2000;

    std::vector<Entity*> entitiesToRemove;
    std::vector<std::unique_ptr<Entity>> entities;

    SDL_Texture* loadTexture(const char* fileName);
    SDL_Texture* spriteSheet;
    void loadHUDTexture();
    SDL_Texture* hudTexture;

    Player* player;
    Menu* menu;
    World* world;
    SDL_Rect camera;

    void processInput();

    void spawnEnemy();
    void resolveCollision(Player& player, Enemy& enemy);
    void adjustPositionOnCollision(Player& player, Enemy& enemy);
    void renderHealthBar(int x, int y, int currentHealth, int maxHealth);
    void updateSpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities);
    void updateEnemySpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities);
    void renderHUD();
    void renderCooldowns();
    void renderText(const char* text, int x, int y, SDL_Color color);
    void renderSmallText(const char* text, int x, int y, SDL_Color color);
    bool isFacing(Entity& entity, Entity& target);
    void spawnEnemiesInDungeon(int numberOfEnemies);
    bool areAllEnemiesCleared() const;

    TTF_Font* font;
    TTF_Font* smallFont;

    bool isPlayerInDungeon;
    SDL_Rect dungeonEntrance;
    SDL_Rect dungeonExit;
    float lastPlayerX, lastPlayerY;

    MazeGenerator* mazeGenerator;
    std::vector<std::vector<int>> dungeonMaze;
    PathfindingManager pathfindingManager;  // Ensure PathfindingManager is included before its use here

    int difficulty;

    friend class Player;
};

#endif
