#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>
#include "Entity.h"

class Game {
public:
    Game();                             /* Constructor */
    ~Game();                            /* Destructor */

    bool init(const char* title, int width, int height);            /* Metoda pentru initalizarea jocului */
    void run();                                                     /* Runs the game loop */
    void cleanup();                                                 /* Cleanup resources */
    void toggleFullscreen();                                        /* Method that makes the window full screen */

private:
    void handleEvents();                                            /* Handles user input and events */
    void update();                                                  /* Update the game stats */
    void render();                                                  /* Renders the game */

    bool isRunning;
    bool isFullscreen;
    SDL_Window* window;                                             /* Pointer la SDL window */
    SDL_Renderer* renderer;                                         /* Pointer la SDL render */
    std::vector<Entity> entities;                                   /* Container for game entities */
    SDL_Rect windowRect;                                            /* To store window size and position before fullscreen */
};

#endif // GAME_H
