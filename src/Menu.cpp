#include "Menu.h"
#include "Game.h"

Menu::Menu(Game* game) : game(game), state(NONE) {
    // Centering the buttons inside the square
    closeButton = { 885, 490, 150, 60 };  // Positioned in the middle horizontally and vertically
    fullscreenButton = { 885, 570, 150, 60 };  // Positioned below the close button
    respawnButton = { 885, 570, 150, 60 };  // Positioned similarly for respawn menu
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
                    if (x >= fullscreenButton.x && x <= fullscreenButton.x + fullscreenButton.w &&
                        y >= fullscreenButton.y && y <= fullscreenButton.y + fullscreenButton.h) {
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
    // Step 1: Render a much darker dimming overlay for the game screen
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 220);  // Darker semi-transparent black for more intense dimming
    SDL_RenderCopy(game->renderer, game->lightingManager->getDimmingTexture(), NULL, NULL);

    // Step 2: Render a larger and much darker dimmed square in the center of the screen
    SDL_SetRenderDrawColor(game->renderer, 30, 30, 30, 255);  // Very dark square
    SDL_Rect menuRect = { 760, 400, 400, 300 };  // Positioned at the center of a 1920x1080 window
    SDL_RenderFillRect(game->renderer, &menuRect);

    // Step 3: Render the close (Exit) button
    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);  // Red color for the close button
    SDL_RenderFillRect(game->renderer, &closeButton);

    // Step 4: Render the fullscreen button or respawn button depending on the state
    if (state == MAIN_MENU) {
        SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);  // Green color for the fullscreen button
        SDL_RenderFillRect(game->renderer, &fullscreenButton);

        // Render text over buttons in MAIN_MENU (with black text)
        SDL_Color black = {0, 0, 0, 255};  // Black color for text
        game->renderText("Exit", closeButton.x + 45, closeButton.y + 15, black);
        game->renderText("Fullscreen", fullscreenButton.x + 10, fullscreenButton.y + 15, black);
    } else if (state == RESPAWN_MENU) {
        SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);  // Green color for the respawn button
        SDL_RenderFillRect(game->renderer, &respawnButton);

        // Render text over buttons in RESPAWN_MENU (with black text)
        SDL_Color black = {0, 0, 0, 255};  // Black color for text
        game->renderText("Exit", closeButton.x + 45, closeButton.y + 15, black);
        game->renderText("Respawn", respawnButton.x + 25, respawnButton.y + 15, black);
    }
}

void Menu::setState(MenuState state) {
    this->state = state;
}
