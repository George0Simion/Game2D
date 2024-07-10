#include "Entity.h"

Entity::Entity(int x, int y, int w, int h) {                        /* Constructor initializing the entity's position and size */
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void Entity::update() {
    // Placeholder for update logic
}

void Entity::render(SDL_Renderer* renderer) {                       /* Sets the draw color to red and fills the entity's rectangle */
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Entity::move(int dx, int dy) {
    rect.x += dx;
    rect.y += dy;
}