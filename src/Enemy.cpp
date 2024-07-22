#include "Enemy.h"

Enemy::Enemy(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed), attackCooldown(0.0f), spellRange(300.0f), thrustRange(100.0f), moveSpeed(75.0f) {}

void Enemy::update(float deltaTime) {
    float actualSpeed = isRunning() ? 1.5f * moveSpeed : moveSpeed;
    float actualAnimationSpeed = isRunning() ? getAnimationSpeed() / 1.5f : getAnimationSpeed();

    if (isMoving() || getAction() != Walking) {
        setAnimationTimer(getAnimationTimer() + deltaTime);

        if (getAnimationTimer() >= actualAnimationSpeed) {
            setAnimationTimer(0.0f);
            setCurrentFrameIndex((getCurrentFrameIndex() + 1) % getNumFrames());

            SDL_Rect& currentFrame = getCurrentFrameRef();
            currentFrame.x = getCurrentFrameIndex() * FRAME_WIDTH;
            currentFrame.y = (getDirection() + getActionOffset() * 4) * FRAME_HEIGHT;

            if ((getAction() == Slashing && getCurrentFrameIndex() == getNumFrames() - 1) ||
                (getAction() == Spellcasting && getCurrentFrameIndex() == getNumFrames() - 1) ||
                (getAction() == Thrusting && getCurrentFrameIndex() == getNumFrames() - 1)) {
                stopAnimation();
                setAction(Walking); // Ensure the enemy returns to walking after an action
            }
        }
    }
}

void Enemy::updateBehavior(float deltaTime, Player& player) {
    float distanceX = player.getX() - getX();
    float distanceY = player.getY() - getY();
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
    } else {
        decideAction(player, distance);
    }

    if (getAction() == Walking) {
        float moveSpeed = 100.0f;
        if (fabs(distanceX) > fabs(distanceY)) {
            if (distanceX > 0) {
                setX(getX() + moveSpeed * deltaTime);
                setDirection(Right);
            } else {
                setX(getX() - moveSpeed * deltaTime);
                setDirection(Left);
            }
        } else {
            if (distanceY > 0) {
                setY(getY() + moveSpeed * deltaTime);
                setDirection(Down);
            } else {
                setY(getY() - moveSpeed * deltaTime);
                setDirection(Up);
            }
        }
        startAnimation();
    }
}

int Enemy::getActionOffset() const {
    int actionOffset = 0;
    switch (getAction()) {
        case Walking: actionOffset = 2; break; // Adjusted based on sprite sheet
        case Thrusting: actionOffset = 1; break; // Adjusted based on sprite sheet
        case Spellcasting: actionOffset = 0; break; // Adjusted based on sprite sheet
        default: actionOffset = 2; break; // Walking, default to 1
    }
    return actionOffset;
}

void Enemy::decideAction(Player& player, float distance) {
    int randomFactor = rand() % 100;

    if (distance > spellRange && randomFactor < 20) { // 20% chance to spellcast at far range
        setAction(Spellcasting);
        attackCooldown = 5.0f; // Increased cooldown for spellcasting
        setAttackStartTime(SDL_GetTicks());
        setAttackDelay(500); // Set delay to 500ms
        setDamageApplied(false);
    } else if (distance <= thrustRange * 1.2 && randomFactor < 60) { // 60% chance to thrust at very close range
        setAction(Thrusting);
        attackCooldown = 1.0f;
        setAttackStartTime(SDL_GetTicks());
        setAttackDelay(150); // Set delay to 150ms
        setDamageApplied(false);
    } else {
        setAction(Walking);
    }

    startAnimation();
}

SDL_Rect Enemy::getAttackBoundingBox() const {
    SDL_Rect boundingBox = Entity::getAttackBoundingBox(); // Call base class method for slashing

    if (getAction() == Thrusting) {
        int thrustRange = getThrustRange();
        switch (getDirection()) {
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

int Enemy::getThrustRange() const {
    return 60; // Thrust range for the enemy
}