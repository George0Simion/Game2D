#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

class Game;  // Forward declaration

class Menu {
public:
    enum MenuState {
        NONE,
        MAIN_MENU,
        RESPAWN_MENU
    };

    Menu(Game* game);                           /* Child menu */

    void handleInput(SDL_Event& event);
    void render();                              /* Methods for the menu */
    void setState(MenuState state);

private:
    bool mouseDown;

    Game* game;
    SDL_Rect closeButton;
    SDL_Rect fullscreenButton;
    SDL_Rect respawnButton;

    MenuState state;
};

#endif
