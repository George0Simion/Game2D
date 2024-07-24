#include "Menu.h"
#include "Game.h"

Menu::Menu(Game* game) : game(game), state(NONE) {
    closeButton = { 350, 250, 100, 50 };  // Adjust position and size as needed
    fullscreenButton = { 350, 320, 100, 50 };  // Adjust position and size as needed
    respawnButton = { 350, 320, 100, 50 };  // Adjust position and size as needed
}

void Menu::handleInput(SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            game->isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                if (state == MAIN_MENU) {
                    game->isMenuOpen = false;
                    state = NONE;
                } else if (state == NONE) {
                    game->isMenuOpen = true;
                    state = MAIN_MENU;
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;

                if (state == MAIN_MENU) {
                    if (x >= closeButton.x && x <= closeButton.x + closeButton.w &&
                        y >= closeButton.y && y <= closeButton.y + closeButton.h) {
                        game->isRunning = false;  // Close the game
                    }
                    if (x >= fullscreenButton.x && x <= fullscreenButton.x + fullscreenButton.w) {
                        Uint32 fullscreenFlag = SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN;
                        SDL_SetWindowFullscreen(game->window, fullscreenFlag ? 0 : SDL_WINDOW_FULLSCREEN);
                    }
                } else if (state == RESPAWN_MENU) {
                    if (x >= closeButton.x && x <= closeButton.x + closeButton.w &&
                        y >= closeButton.y && y <= closeButton.y + closeButton.h) {
                        game->isRunning = false;  // Close the game
                    }
                    if (x >= respawnButton.x && x <= respawnButton.x + respawnButton.w &&
                        y >= respawnButton.y && y <= respawnButton.y + respawnButton.h) {
                        game->resetGame();  // Respawn the game
                        game->isMenuOpen = false;  // Close the menu
                    }
                }
            }
            break;
        default:
            break;
    }
}

void Menu::render() {
    // Render semi-transparent overlay
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 128);  // Semi-transparent black
    SDL_Rect overlay = { 0, 0, 1920, 1080 };
    SDL_RenderFillRect(game->renderer, &overlay);

    // Render menu background
    SDL_SetRenderDrawColor(game->renderer, 200, 200, 200, 255);  // Light grey
    SDL_Rect menuRect = { 300, 200, 200, 200 };
    SDL_RenderFillRect(game->renderer, &menuRect);

    // Render close button
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);  // Red
    SDL_RenderFillRect(game->renderer, &closeButton);

    if (state == MAIN_MENU) {
        // Render fullscreen button
        SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);  // Green
        SDL_RenderFillRect(game->renderer, &fullscreenButton);
    } else if (state == RESPAWN_MENU) {
        // Render respawn button
        SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);  // Green
        SDL_RenderFillRect(game->renderer, &respawnButton);
    }
}

void Menu::setState(MenuState state) {
    this->state = state;
}
