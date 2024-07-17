#include "Entity.h"

const int FRAME_WIDTH = 64;
const int FRAME_HEIGHT = 64;

Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex, int p_numFrames, float p_animationSpeed)
    : x(p_x), y(p_y), tex(p_tex), numFrames(p_numFrames), animationSpeed(p_animationSpeed), animationTimer(0.0f), currentFrameIndex(0), moving(false), direction(Down), action(Walking) {
    currentFrame = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
}

float Entity::getX() {
    return x;
}
float Entity::getY() {
    return y;
}
void Entity::setX(float p_x) {
    if (p_x != x) {
        x = p_x;
        startAnimation();
    } else {
        stopAnimation();
    }
}
void Entity::setY(float p_y) {
    if (p_y != y) {
        y = p_y;
        startAnimation();
    } else {
        stopAnimation();
    }
}
SDL_Texture* Entity::getTex() {
    return tex;
}
SDL_Rect Entity::getCurrentFrame() {
    return currentFrame;
}

void Entity::startAnimation() {
    moving = true;
}

void Entity::stopAnimation() {
    moving = false;
}

void Entity::setDirection(Direction dir) {
    direction = dir;
}

void Entity::setAction(Action act) {
    if (action != act) {
        action = act;
        currentFrameIndex = 0;
        animationTimer = 0.0f;

        // Adjust number of frames for each action
        switch(action) {
            case Slashing:
                numFrames = 6; // 6 frames for slashing
                break;
            case Thrusting:
                numFrames = 8; // 8 frames for thrusting
                break;
            case Spellcasting:
                numFrames = 7; // 7 frames for spellcasting
                break;
            case Shooting:
                numFrames = 13; // 13 frames for shooting
                break;
            default:
                numFrames = 9; // 9 frames for walking
                break;
        }
        currentFrame = {0, direction * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
    }
}

void Entity::update(float deltaTime) {
    if (!moving && action == Walking) return;

    animationTimer += deltaTime;

    if (animationTimer >= animationSpeed) {
        animationTimer = 0.0f;
        currentFrameIndex = (currentFrameIndex + 1) % numFrames;

        int actionOffset = 0;
        switch(action) {
            case Walking: actionOffset = 2; break;
            case Slashing: actionOffset = 3; break;
            case Thrusting: actionOffset = 1; break;
            case Spellcasting: actionOffset = 0; break;
            case Shooting: actionOffset = 4; break;
        }

        currentFrame.x = currentFrameIndex * FRAME_WIDTH;
        currentFrame.y = (direction + actionOffset * 4) * FRAME_HEIGHT;

        // Stop animation for specific actions after one complete cycle
        if ((action == Slashing && currentFrameIndex == numFrames - 1) ||
            (action == Spellcasting && currentFrameIndex == numFrames - 1) ||
            (action == Shooting && currentFrameIndex == numFrames - 1)) {
            stopAnimation();
            setAction(Walking);
        }
    }
}

bool Entity::isMoving() const {
    return moving;
}

Entity::Action Entity::getAction() const {
    return action;
}
