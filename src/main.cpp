#include "Game.h"

Game* game = nullptr;                                     /* Game pointer */

int main(int argc, char* argv[]) {
    game = new Game();                                    /* Alocating memory for the game */

    game->init("Game Window", 1920, 1080, false);         /* Method for initializing the game */

    while (game->running()) {
        game->handleEvents();
        game->update();
        game->render();
    }                                                     /* Game loop */

    game->clean();                                        /* Cleaning the memmory */

    return 0;
}
