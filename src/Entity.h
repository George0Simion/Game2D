#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Entity {
public:
    enum Direction { Up, Left, Down, Right };
    enum Action { Walking, Slashing, Thrusting, Spellcasting, Shooting, ArrowFlyingUp, ArrowFlyingDown, ArrowFlyingLeft, ArrowFlyingRight };

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
    bool isMoving() const;
    Action getAction() const;
    Direction getDirection() const;
    void shootArrow(Direction dir);

    void setRunning(bool running);
    bool isRunning() const;

    bool isArrowActive() const;
    float getArrowX() const;
    float getArrowY() const;
    SDL_Rect getArrowFrame() const;

private:
    float x, y;
    bool moving;
    bool running;

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

    bool arrowActive;
    float arrowX, arrowY;
    float arrowSpeed;
    float arrowMaxDistance;
    float arrowTravelDistance;
    Direction arrowDirection;
};

#endif
