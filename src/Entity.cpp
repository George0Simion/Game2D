#include "Entity.h"

/* Entity constructor with the initial positions */
Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex, int p_frameWidth, int p_frameHeight, int p_numFrames, float p_animationSpeed)
    : x(p_x), y(p_y), tex(p_tex), frameWidth(p_frameWidth), frameHeight(p_frameHeight), numFrames(p_numFrames), animationSpeed(p_animationSpeed), animationTimer(0.0f), currentFrameIndex(0), moving(false), direction(Down) {
    currentFrame = {0, 0, frameWidth, frameHeight};
}

/* Methods for the entity */
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
    currentFrame.y = direction * frameHeight;
}

void Entity::update(float deltaTime) {
    if (!moving) return;

    animationTimer += deltaTime;

    if (animationTimer >= animationSpeed) {
        animationTimer = 0.0f;
        currentFrameIndex = (currentFrameIndex + 1) % numFrames;
        currentFrame.x = currentFrameIndex * frameWidth;
        currentFrame.y = direction * frameHeight;
    }
}
