#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

class Game;  // Forward declaration

class Menu {
public:
    Menu(Game* game);
    void handleInput();
    void render();

private:
    bool mouseDown;

    Game* game;
    SDL_Rect closeButton;
    SDL_Rect fullscreenButton;
};

#endif
