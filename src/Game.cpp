#include "Game.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), isFullscreen(false) {}                              /* Constructor care intitializeaza membrii */

Game::~Game() {
    cleanup();                                                                                      /* Destructor numit cleanup */
}

bool Game::init(const char* title, int width, int height) {                                         /* Initializare */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);          /* Creaza window-ul */
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);                            /* Creaza render-ul */
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    isRunning = true;
    entities.push_back(Entity(100, 100, 50, 50));  // Example entity

    SDL_GetWindowPosition(window, &windowRect.x, &windowRect.y);                                    /* Save initial window size and position */
    SDL_GetWindowSize(window, &windowRect.w, &windowRect.h);

    return true;
}

void Game::run() {
    while (isRunning) {
        handleEvents();                 /* Handles SDL events, such as quitting the game */
        update();                       /* Updates all entities */
        render();                       /* Clears the screen, renders entities, and presents the renderer */
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    entities[0].move(0, -10);
                    break;
                case SDLK_s:
                    entities[0].move(0, 10);
                    break;
                case SDLK_a:
                    entities[0].move(-10, 0);
                    break;
                case SDLK_d:
                    entities[0].move(10, 0);
                    break;
                case SDLK_f:  // Press 'F' to toggle fullscreen
                    toggleFullscreen();
                    break;
            }
        }
    }
}

void Game::update() {
    // Update entities
    for (auto& entity : entities) {
        entity.update();
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render entities
    for (auto& entity : entities) {
        entity.render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::toggleFullscreen() {
    isFullscreen = !isFullscreen;
    if (isFullscreen) {
        SDL_GetWindowPosition(window, &windowRect.x, &windowRect.y);
        SDL_GetWindowSize(window, &windowRect.w, &windowRect.h);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, windowRect.w, windowRect.h);
        SDL_SetWindowPosition(window, windowRect.x, windowRect.y);
    }
}
