#include "Entity.h"
#include <iostream>

Entity::Entity(int x, int y, int w, int h, const std::string& imagePath, SDL_Renderer* renderer) : renderer(renderer) {                        /* Constructor initializing the entity's position and size */
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    // Load the image
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        texture = nullptr;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        }
    }
}

Entity::~Entity() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Entity::update() {
    // Placeholder for update logic
}

void Entity::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Entity::move(int dx, int dy) {
    rect.x += dx;
    rect.y += dy;
}