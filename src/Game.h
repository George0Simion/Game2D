#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "Entity.h"
#include "Menu.h"

class Game {
public:
    Game();
    ~Game();                                                                    /* Constructor and deconstructor */

    void init(const char* title, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() { return isRunning; }                                        /* Game methods */

    bool isRunning;
    bool isMenuOpen;

    SDL_Window* window;
    SDL_Renderer* renderer;                                                     /* Window variables */

private:
    Uint32 lastTime;
    float deltaTime;                                                            /* Time for the smooth movement */

    std::vector<Entity> entities;                                               /* Entities vector */
    SDL_Texture* loadTexture(const char* fileName);

    Menu* menu;                                                                 /* Menu child */

    void processInput();
};

#endif
