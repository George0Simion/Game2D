#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Entity {
public:
    enum Direction { Up, Left, Down, Right };
    enum Action { Walking, Slashing, Thrusting, Spellcasting, Shooting };

    Entity(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    float getX();
    float getY();
    void setX(float p_x);
    void setY(float p_y);

    SDL_Texture* getTex();
    SDL_Rect getCurrentFrame();

    void update(float deltaTime);
    void startAnimation();
    void stopAnimation();
    void setDirection(Direction dir);
    void setAction(Action act);

private:
    float x, y;
    bool moving;

    SDL_Texture* tex;
    SDL_Rect currentFrame;

    int numFrames;
    int currentFrameIndex;
    float animationSpeed;
    float animationTimer;
    Direction direction;
    Action action;

    static const int FRAME_WIDTH = 64;
    static const int FRAME_HEIGHT = 64;
};

#endif
