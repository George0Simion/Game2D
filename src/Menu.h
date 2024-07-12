#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

class Game;  // Forward declaration

class Menu {
public:
    Menu(Game* game);                           /* Child menu */

    void handleInput(SDL_Event& event);
    void render();                              /* Methods for the menu */

private:
    bool mouseDown;

    Game* game;
    SDL_Rect closeButton;
    SDL_Rect fullscreenButton;
};

#endif
