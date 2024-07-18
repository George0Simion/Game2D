#include "Entity.h"

const int FRAME_WIDTH = 64;
const int FRAME_HEIGHT = 64;

Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex, int p_numFrames, float p_animationSpeed)
: x(p_x), y(p_y), tex(p_tex), numFrames(p_numFrames), animationSpeed(p_animationSpeed), arrowActive(false), arrowSpeed(400.0f), arrowMaxDistance(800.0f), arrowTravelDistance(0.0f), moving(false), running(false), direction(Down), action(Walking) {
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

SDL_Rect Entity::getArrowFrame() const {
    if (arrowActive) {
        int arrowRowOffset = 0;
        switch (arrowDirection) {
            case Up:    arrowRowOffset = 20; break;
            case Down:  arrowRowOffset = 22; break;
            case Left:  arrowRowOffset = 21; break;
            case Right: arrowRowOffset = 23; break;
        }
        return {0, FRAME_HEIGHT * arrowRowOffset, FRAME_WIDTH, FRAME_HEIGHT};
    }
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
            case ArrowFlyingUp:
            case ArrowFlyingDown:
            case ArrowFlyingLeft:
            case ArrowFlyingRight:
                numFrames = 1; // 1 frame per arrow flying direction
                break;
            default:
                numFrames = 9; // 9 frames for walking
                break;
        }
        currentFrame = {0, direction * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
    }
}

void Entity::update(float deltaTime) {
    float actualSpeed = running ? 1.5f * arrowSpeed : arrowSpeed;  // Adjust speed for running
    float actualAnimationSpeed = running ? animationSpeed / 1.5f : animationSpeed;  // Adjust animation speed for running

    // Update arrow position independently
    if (arrowActive) {
        float distance = arrowSpeed * deltaTime;
        arrowTravelDistance += distance;

        switch (arrowDirection) {
            case Up:    arrowY -= distance; break;
            case Down:  arrowY += distance; break;
            case Left:  arrowX -= distance; break;
            case Right: arrowX += distance; break;
        }

        if (arrowTravelDistance >= arrowMaxDistance) {
            arrowActive = false;
        }
    }

    // Update character animation only if moving or performing an action
    if (moving || action != Walking) {
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
                case ArrowFlyingUp:
                case ArrowFlyingDown:
                case ArrowFlyingLeft:
                case ArrowFlyingRight:
                    actionOffset = 5; break;
            }

            currentFrame.x = currentFrameIndex * FRAME_WIDTH;
            currentFrame.y = (direction + actionOffset * 4) * FRAME_HEIGHT;

            if ((action == Slashing && currentFrameIndex == numFrames - 1) ||
                (action == Spellcasting && currentFrameIndex == numFrames - 1) ||
                (action == Shooting && currentFrameIndex == numFrames - 1) ||
                (action == ArrowFlyingUp || action == ArrowFlyingDown || 
                 action == ArrowFlyingLeft || action == ArrowFlyingRight)) {
                stopAnimation();
                setAction(Walking);
            }
        }
    }
}

bool Entity::isMoving() const {
    return moving;
}

Entity::Action Entity::getAction() const {
    return action;
}

Entity::Direction Entity::getDirection() const {
    return direction;
}

void Entity::shootArrow(Direction dir) {
    if (!arrowActive) {
        arrowActive = true;
        float xOffset = 0;
        float yOffset = 0;

        switch (dir) {
            case Up:
                xOffset = FRAME_WIDTH / 2;  // Center horizontally
                yOffset = -FRAME_HEIGHT / 4;  // Slightly up from the middle
                break;
            case Down:
                xOffset = FRAME_WIDTH / 2;  // Center horizontally
                yOffset = FRAME_HEIGHT / 2 + 25;  // Slightly down from the middle
                break;
            case Left:
                xOffset = -FRAME_WIDTH / 4;  // Slightly left from the middle
                yOffset = FRAME_HEIGHT / 2;  // Center vertically
                break;
            case Right:
                xOffset = FRAME_WIDTH / 2 + 30;  // Slightly right from the middle
                yOffset = FRAME_HEIGHT / 2;  // Center vertically
                break;
        }

        arrowX = x + xOffset;
        arrowY = y + yOffset;
        arrowDirection = dir;
        arrowTravelDistance = 0.0f;
    }
}

void Entity::setRunning(bool running) {
    this->running = running;
}

bool Entity::isRunning() const {
    return running;
}

bool Entity::isArrowActive() const {
    return arrowActive;
}

float Entity::getArrowX() const {
    return arrowX;
}

float Entity::getArrowY() const {
    return arrowY;
}