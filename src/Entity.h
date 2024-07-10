#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Entity {
public:
    Entity(int x, int y, int w, int h, const std::string& imagePath, SDL_Renderer* renderer);              /* Constructor initializing the entity's position and size */
    ~Entity();
    void update();                                                                                         /* Updates the entity */
    void render(SDL_Renderer* renderer);                                                                   /* Renders the entity */
    void move(int dx, int dy);                                                                             /* Method to move the entity */

private:
    SDL_Rect rect;                                   /* Rectangle defining the entity's position and size */
    SDL_Texture* texture;
    SDL_Renderer* renderer;
};

#endif // ENTITY_H
