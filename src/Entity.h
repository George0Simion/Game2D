#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Entity {
public:
    enum Direction { Up, Left, Down, Right }; // Corrected directions

    Entity(float p_x, float p_y, SDL_Texture* p_tex, int frameWidth, int frameHeight, int numFrames, float animationSpeed);  /* Constructor */

    float getX();
    float getY();
    void setX(float p_x);
    void setY(float p_y);

    SDL_Texture* getTex();
    SDL_Rect getCurrentFrame();  /* Methods for the entity renderer */
    
    void update(float deltaTime);  /* Method to update the animation */
    void startAnimation();
    void stopAnimation();
    void setDirection(Direction dir);

private:
    float x, y;
    bool moving;

    SDL_Texture* tex;
    SDL_Rect currentFrame;

    int frameWidth, frameHeight;
    int numFrames;
    int currentFrameIndex;
    float animationSpeed;
    float animationTimer;
    Direction direction;
};

#endif
