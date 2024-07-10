#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>

class Entity {
public:
    Entity(int x, int y, int w, int h);              /* Constructor initializing the entity's position and size */
    void update();                                   /* Updates the entity */
    void render(SDL_Renderer* renderer);             /* Renders the entity */
    void move(int dx, int dy);                       /* Method to move the entity */

private:
    SDL_Rect rect;                                   /* Rectangle defining the entity's position and size */
};

#endif // ENTITY_H
