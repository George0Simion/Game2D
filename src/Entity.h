#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Entity {
public:
    Entity(float p_x, float p_y, SDL_Texture* p_tex);                   /* Constructor */

    float getX();
    float getY();
    void setX(float p_x);
    void setY(float p_y);                                               /* Methods */

    SDL_Texture* getTex();
    SDL_Rect getCurrentFrame();                                         /* Methods for the entity renderer */

private:
    float x, y;

    SDL_Texture* tex;
    SDL_Rect currentFrame;
};

#endif
