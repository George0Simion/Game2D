#include "Entity.h"
#include "Enemy.h"
#include <iostream>

const int FRAME_WIDTH = 64;
const int FRAME_HEIGHT = 64;

Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex, int p_numFrames, float p_animationSpeed)
: x(p_x), y(p_y), tex(p_tex), numFrames(p_numFrames), animationSpeed(p_animationSpeed), arrowActive(false), spellActive(false), arrowSpeed(400.0f), spellSpeed(200.0f), spellCurveFactor(0.1f), arrowMaxDistance(800.0f), arrowTravelDistance(0.0f), moving(false), running(false), direction(Down), action(Walking), health(100) {
    currentFrame = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
}

void Entity::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        if (dynamic_cast<Enemy*>(this)) {
            markForRemoval();
        }
    }
}

void Entity::handleInput(const SDL_Event& event) {
    // Base class method is empty because it's meant to be overridden by derived classes
}

void Entity::setSpellRow(int row) {
    int spellRow = row;
    int spellFrameIndex = (SDL_GetTicks() / 50) % 6; // Assuming 6 frames for the spell animation
    currentFrame = {spellFrameIndex * FRAME_WIDTH, spellRow * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
}

SDL_Rect Entity::getSpellFrameForEnemy() const {
    if (spellActive) {
        int spellRow = 13; // Row for the enemy's spell animation
        int spellFrameIndex = (SDL_GetTicks() / 50) % 6; // Assuming 6 frames for the spell animation
        return {spellFrameIndex * FRAME_WIDTH, spellRow * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
    }
    return currentFrame;
}

void Entity::setSpellTarget(float targetX, float targetY) {
    spellActive = true;
    spellX = x + FRAME_WIDTH / 2;
    spellY = y - 40;
    spellTargetX = targetX;
    spellTargetY = targetY;
    spellStartTime = SDL_GetTicks();
    spellDuration = 4000;
}

SDL_Rect Entity::getSpellFrame() const {
    if (spellActive) {
        int spellRow = 25; // Adjust as per your sprite sheet
        int spellFrameIndex = (SDL_GetTicks() / 50) % 6; // Assuming 6 frames for the spell animation
        return {spellFrameIndex * FRAME_WIDTH, spellRow * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
    }
    return currentFrame;
}

SDL_Rect Entity::getCollisionBoundingBox() const {
    SDL_Rect boundingBox = { static_cast<int>(x) + 10, static_cast<int>(y) + 10, FRAME_WIDTH - 20, FRAME_HEIGHT - 20 };
    return boundingBox;
}

SDL_Rect Entity::getAttackBoundingBox() const {
    SDL_Rect boundingBox = getBoundingBox();
    int thrustRange = getThrustRange();

    if (action == Slashing) {
        switch (direction) {
            case Up:
                boundingBox.y -= 45; boundingBox.h += 45; break;
            case Down:
                boundingBox.h += 45; break;
            case Left:
                boundingBox.x -= 45; boundingBox.w += 45; break;
            case Right:
                boundingBox.w += 45; break;
        }
    } else if (action == Thrusting) {
        switch (direction) {
            case Up:
                boundingBox.y -= thrustRange; boundingBox.h += thrustRange; break;
            case Down:
                boundingBox.h += thrustRange; break;
            case Left:
                boundingBox.x -= thrustRange; boundingBox.w += thrustRange; break;
            case Right:
                boundingBox.w += thrustRange; break;
        }
    }

    return boundingBox;
}

SDL_Rect Entity::getBoundingBox() const {
    SDL_Rect boundingBox = { static_cast<int>(x), static_cast<int>(y), FRAME_WIDTH, FRAME_HEIGHT };

    if (action == Slashing) {
        switch (direction) {
            case Up:
            case Down:
                boundingBox.y -= FRAME_HEIGHT / 4;
                boundingBox.h += FRAME_HEIGHT / 2;
                break;
            case Left:
            case Right:
                boundingBox.x -= FRAME_WIDTH / 4;
                boundingBox.w += FRAME_WIDTH / 2;
                break;
        }
    } else if (action == Thrusting) {
        switch (direction) {
            case Up:
            case Down:
                boundingBox.y -= FRAME_HEIGHT / 8;
                boundingBox.h += FRAME_HEIGHT / 4;
                break;
            case Left:
            case Right:
                boundingBox.x -= FRAME_WIDTH / 8;
                boundingBox.w += FRAME_WIDTH / 4;
                break;
        }
    }

    return boundingBox;
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

void Entity::setAction(Action act) {
    if (action != act) {
        action = act;
        currentFrameIndex = 0;
        animationTimer = 0.0f;
        damageApplied = false;

        switch(action) {
            case Slashing:
                numFrames = 6;
                attackDelay = 300;
                break;
            case Thrusting:
                numFrames = 8;
                attackDelay = 200;
                break;
            case Spellcasting:
                numFrames = 7;
                attackDelay = 400;
                break;
            case Shooting:
                numFrames = 13;
                attackDelay = 0;
                break;
            case ArrowFlyingUp:
            case ArrowFlyingDown:
            case ArrowFlyingLeft:
            case ArrowFlyingRight:
                numFrames = 1;
                attackDelay = 0;
                break;
            default:
                numFrames = 9;
                attackDelay = 0;
                break;
        }
        currentFrame = {0, direction * FRAME_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT};
    }
}

void Entity::update(float deltaTime) {
    if (action == Dying) {
        animationTimer += deltaTime;
        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            currentFrameIndex = (currentFrameIndex + 1) % numFrames;
            currentFrame.x = currentFrameIndex * FRAME_WIDTH;
            currentFrame.y = 24 * FRAME_HEIGHT;

            if (currentFrameIndex == numFrames - 1) {
                stopAnimation();
            }
        }
        return;
    }

    float actualSpeed = running ? 1.5f * arrowSpeed : arrowSpeed;
    float actualAnimationSpeed = running ? animationSpeed / 1.5f : animationSpeed;

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

    if (moving || action != Walking) {
        animationTimer += deltaTime;

        if (animationTimer >= animationSpeed) {
            animationTimer = 0.0f;
            currentFrameIndex = (currentFrameIndex + 1) % numFrames;

            int actionOffset = getActionOffset();

            currentFrame.x = currentFrameIndex * FRAME_WIDTH;
            currentFrame.y = (direction + actionOffset * 4) * FRAME_HEIGHT;

            if ((action == Slashing && currentFrameIndex == numFrames - 1) ||
                (action == Spellcasting && currentFrameIndex == numFrames - 1) ||
                (action == Shooting && currentFrameIndex == numFrames - 1) ||
                (action == Thrusting && currentFrameIndex == numFrames - 1)) {
                stopAnimation();
                setAction(Walking);
            }
        }
    }
}

int Entity::getActionOffset() const {
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
    return actionOffset;
}

void Entity::shootArrow(Direction dir) {
    if (!arrowActive) {
        arrowActive = true;
        float xOffset = 0;
        float yOffset = 0;

        switch (dir) {
            case Up:
                xOffset = FRAME_WIDTH / 2;
                yOffset = -FRAME_HEIGHT / 4;
                break;
            case Down:
                xOffset = FRAME_WIDTH / 2;
                yOffset = FRAME_HEIGHT / 2 + 25;
                break;
            case Left:
                xOffset = -FRAME_WIDTH / 4;
                yOffset = FRAME_HEIGHT / 2;
                break;
            case Right:
                xOffset = FRAME_WIDTH / 2 + 30;
                yOffset = FRAME_HEIGHT / 2;
                break;
        }

        arrowX = x + xOffset;
        arrowY = y + yOffset;
        arrowDirection = dir;
        arrowTravelDistance = 0.0f;
    } else {
        arrowActive = false;
    }
}

int Entity::getThrustRange() const {
    return 30;
}
void Entity::deactivateSpell() {
    spellActive = false;
}
bool Entity::isSpellActive() const {
    return spellActive;
}
float Entity::getSpellX() const {
    return spellX;
}
float Entity::getSpellY() const {
    return spellY;
}
bool Entity::isMarkedForRemoval() const {
    return markedForRemoval;
}
void Entity::markForRemoval() {
    markedForRemoval = true;
}
int Entity::getHealth() const {
    return health;
}
void Entity::setHealth(int health) {
    this->health = health;
}
bool Entity::isAlive() const {
    return health > 0;
}
bool Entity::getDamageApplied() const {
    return damageApplied;
}
void Entity::setDamageApplied(bool value) {
    damageApplied = value;
}
Uint32 Entity::getAttackStartTime() const {
    return attackStartTime;
}
void Entity::setAttackStartTime(Uint32 time) {
    attackStartTime = time;
}
Uint32 Entity::getAttackDelay() const {
    return attackDelay;
}
void Entity::setAttackDelay(Uint32 delay) {
    attackDelay = delay;
}
float Entity::getX() {
    return x;
}
float Entity::getY() {
    return y;
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
bool Entity::isMoving() const {
    return moving;
}
Entity::Action Entity::getAction() const {
    return action;
}
Entity::Direction Entity::getDirection() const {
    return direction;
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
float Entity::getAnimationTimer() const {
    return animationTimer;
}
void Entity::setAnimationTimer(float timer) {
    animationTimer = timer;
}
int Entity::getCurrentFrameIndex() const {
    return currentFrameIndex;
}
void Entity::setCurrentFrameIndex(int index) {
    currentFrameIndex = index;
}
SDL_Rect& Entity::getCurrentFrameRef() {
    return currentFrame;
}
bool Entity::isEntityMoving() const {
    return moving;
}
bool Entity::isEntityRunning() const {
    return running;
}
float Entity::getAnimationSpeed() const {
    return animationSpeed;
}
int Entity::getNumFrames() const {
    return numFrames;
}
void Entity::setNumFrames(int numFrames) {
    this->numFrames = numFrames;
}
void Entity::setCurrentFrame(const SDL_Rect& frame) {
    currentFrame = frame;
}
