#include "Menu.h"
#include "Game.h"

Menu::Menu(Game* game) : game(game) {
    closeButton = { 350, 250, 100, 50 };  // Adjust position and size as needed
    fullscreenButton = { 350, 320, 100, 50 };  // Adjust position and size as needed
}

void Menu::handleInput(SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            game->isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                game->isMenuOpen = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= closeButton.x && x <= closeButton.x + closeButton.w &&
                    y >= closeButton.y && y <= closeButton.y + closeButton.h) {
                    game->isRunning = false;  // Close the game
                }
                if (x >= fullscreenButton.x && x <= fullscreenButton.x + fullscreenButton.w &&
                    y >= fullscreenButton.y && y <= fullscreenButton.y + fullscreenButton.h) {
                    Uint32 fullscreenFlag = SDL_GetWindowFlags(game->window) & SDL_WINDOW_FULLSCREEN;
                    SDL_SetWindowFullscreen(game->window, fullscreenFlag ? 0 : SDL_WINDOW_FULLSCREEN);
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
    SDL_Rect overlay = { 0, 0, 1600, 1200 };
    SDL_RenderFillRect(game->renderer, &overlay);

    // Render menu background
    SDL_SetRenderDrawColor(game->renderer, 200, 200, 200, 255);  // Light grey
    SDL_Rect menuRect = { 300, 200, 200, 200 };
    SDL_RenderFillRect(game->renderer, &menuRect);

    // Render buttons
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);  // Red
    SDL_RenderFillRect(game->renderer, &closeButton);
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);  // Green
    SDL_RenderFillRect(game->renderer, &fullscreenButton);

    // Optionally render button labels (e.g., using SDL_ttf for text rendering)
}

