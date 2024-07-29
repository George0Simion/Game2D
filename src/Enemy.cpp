#include "Enemy.h"
#include <iostream>

Enemy::Enemy(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed), attackCooldown(0.0f), spellRange(300.0f), thrustRange(100.0f), moveSpeed(75.0f) {}

void Enemy::setSpellTarget(float targetX, float targetY) {
    spellActive = true;
    spellX = x + FRAME_WIDTH / 2;
    spellY = y - 10;
    spellTargetX = targetX;
    spellTargetY = targetY;
    spellStartTime = SDL_GetTicks();
    spellDuration = SPELL_DURATION;
    setAction(Spellcasting);
}

void Enemy::updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    if (spellActive) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - spellStartTime > spellDuration) {
            deactivateSpell();
            return;
        }

        Player* closestPlayer = nullptr;
        float minDistance = std::numeric_limits<float>::max();
        for (const auto& entity : entities) {
            if (Player* player = dynamic_cast<Player*>(entity.get())) {
                if (!player->getIsDead()) {
                    float distance = std::hypot(spellX - player->getX(), spellY - player->getY());
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestPlayer = player;
                    }
                }
            }
        }
        if (closestPlayer) {
            spellTargetX = closestPlayer->getX();
            spellTargetY = closestPlayer->getY();
        } else {
            deactivateSpell();
            return;
        }

        float dx = spellTargetX - spellX;
        float dy = spellTargetY - spellY;
        float distance = std::hypot(dx, dy);

        if (distance > 5.0f) {
            float angle = atan2(dy, dx);
            float curve = sin(SDL_GetTicks() * spellCurveFactor);

            float controlPointX = (spellX + spellTargetX) / 2 + curve * 50;
            float controlPointY = (spellY + spellTargetY) / 2 + curve * 50;

            float t = (spellSpeed / 2) * deltaTime / distance;
            spellX = (1 - t) * (1 - t) * spellX + 2 * (1 - t) * t * controlPointX + t * t * spellTargetX;
            spellY = (1 - t) * (1 - t) * spellY + 2 * (1 - t) * t * controlPointY + t * t * spellTargetY;
        } else {
            deactivateSpell();
        }

        SDL_Rect spellRect = { static_cast<int>(spellX), static_cast<int>(spellY), FRAME_WIDTH - 20, FRAME_HEIGHT - 20 };
        for (auto& entity : entities) {
            if (Player* player = dynamic_cast<Player*>(entity.get())) {
                if (!player->getIsDead()) {
                    SDL_Rect playerBoundingBox = player->getBoundingBox();
                    if (SDL_HasIntersection(&spellRect, &playerBoundingBox)) {
                        player->takeDamage(SPELL_DAMAGE);
                        deactivateSpell();
                        break;
                    }
                }
            }
        }
    }
}

void Enemy::updateBehavior(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities) {
    if (isMarkedForRemoval()) return;

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

    updateEnemy(deltaTime, player, entities);
}

void Enemy::updateEnemy(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities) {
    if (isMarkedForRemoval()) return;

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
                setAction(Walking);
            }
        }
    }

    if (isSpellActive()) {
        updateSpellPosition(deltaTime, entities);
    }
}

int Enemy::getActionOffset() const {
    int actionOffset = 0;
    switch (getAction()) {
        case Walking: actionOffset = 2; break;
        case Thrusting: actionOffset = 1; break;
        case Spellcasting: actionOffset = 0; break;
        default: actionOffset = 2; break;
    }
    return actionOffset;
}

void Enemy::decideAction(Player& player, float distance) {
    int randomFactor = rand() % 100;

    if (distance > spellRange && randomFactor < 20) {
        setAction(Spellcasting);
        attackCooldown = 5.0f;
        setAttackStartTime(SDL_GetTicks());
        setAttackDelay(500);
        setDamageApplied(false);
    } else if (distance <= thrustRange * 1.2 && randomFactor < 60) {
        setAction(Thrusting);
        attackCooldown = 1.0f;
        setAttackStartTime(SDL_GetTicks());
        setAttackDelay(150);
        setDamageApplied(false);
    } else {
        setAction(Walking);
    }

    startAnimation();
}

SDL_Rect Enemy::getAttackBoundingBox() const {
    SDL_Rect boundingBox = Entity::getAttackBoundingBox();

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
    return 60;
}