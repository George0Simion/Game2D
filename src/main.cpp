#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (game.init("Simple Game", 1400, 1000)) {
        game.run();                                     /* Initializam window-ul si rulam daca s-a initializat cu succes */
    }

    game.cleanup();

    return 0;
}
