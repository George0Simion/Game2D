#include "Entity.h"
#include "Enemy.h"

const int FRAME_WIDTH = 64;
const int FRAME_HEIGHT = 64;

Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex, int p_numFrames, float p_animationSpeed)
: x(p_x), y(p_y), tex(p_tex), numFrames(p_numFrames), animationSpeed(p_animationSpeed), arrowActive(false), spellActive(false), arrowSpeed(400.0f), spellSpeed(200.0f), spellCurveFactor(0.1f), arrowMaxDistance(800.0f), arrowTravelDistance(0.0f), moving(false), running(false), direction(Down), action(Walking), health(100) { // Initialize health
    currentFrame = {0, 0, FRAME_WIDTH, FRAME_HEIGHT};
}

bool Entity::isMarkedForRemoval() const {
    return markedForRemoval;
}

void Entity::markForRemoval() {
    markedForRemoval = true;
}

// Define the handleInput method in the base Entity class
void Entity::handleInput(const SDL_Event& event) {
    // Base class method is empty because it's meant to be overridden by derived classes
}

void Entity::setSpellTarget(float targetX, float targetY) {
    spellActive = true;
    spellX = x;
    spellY = y;
    spellTargetX = targetX;
    spellTargetY = targetY;
    spellDuration = 4000;
}

void Entity::updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    if (spellActive) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - spellStartTime > spellDuration) {
            deactivateSpell();
            return;
        }

        // Update target position more frequently to ensure smooth tracking
        Enemy* closestEnemy = nullptr;
        float minDistance = std::numeric_limits<float>::max();
        for (auto& entity : entities) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                float distance = std::hypot(spellX - enemy->getX(), spellY - enemy->getY());
                if (distance < minDistance) {
                    minDistance = distance;
                    closestEnemy = enemy;
                }
            }
        }
        if (closestEnemy) {
            spellTargetX = closestEnemy->getX();
            spellTargetY = closestEnemy->getY();
        }

        float dx = spellTargetX - spellX;
        float dy = spellTargetY - spellY;
        float distance = std::hypot(dx, dy);

        if (distance > 5.0f) {
            float angle = atan2(dy, dx);
            float curve = sin(SDL_GetTicks() * spellCurveFactor);

            spellX += spellSpeed * deltaTime * cos(angle + curve);
            spellY += spellSpeed * deltaTime * sin(angle + curve);
        } else {
            deactivateSpell();
        }

        // Check for collision with enemies
        SDL_Rect spellRect = { static_cast<int>(spellX), static_cast<int>(spellY), FRAME_WIDTH, FRAME_HEIGHT };
        for (auto& entity : entities) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                SDL_Rect enemyBoundingBox = enemy->getBoundingBox();
                if (SDL_HasIntersection(&spellRect, &enemyBoundingBox)) {
                    enemy->takeDamage(Player::SPELL_DAMAGE);
                    if (!enemy->isAlive()) {
                        // Mark the enemy for removal
                        enemy->markForRemoval();
                    }
                    deactivateSpell();
                    break;
                }
            }
        }
    }
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

SDL_Rect Entity::getSpellFrame() const {
    // Assuming the spell animation frames are in a specific row in the sprite sheet
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

int Entity::getThrustRange() const {
    return 30; // Default thrust range for generic entities
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

void Entity::takeDamage(int damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
    }
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

SDL_Rect Entity::getBoundingBox() const {
    SDL_Rect boundingBox = { static_cast<int>(x), static_cast<int>(y), FRAME_WIDTH, FRAME_HEIGHT };

    if (action == Slashing) {
        switch (direction) {
            case Up:
            case Down:
                boundingBox.y -= FRAME_HEIGHT / 4; // Adjust starting position
                boundingBox.h += FRAME_HEIGHT / 2; // Increase height
                break;
            case Left:
            case Right:
                boundingBox.x -= FRAME_WIDTH / 4; // Adjust starting position
                boundingBox.w += FRAME_WIDTH / 2; // Increase width
                break;
        }
    } else if (action == Thrusting) {
        switch (direction) {
            case Up:
            case Down:
                boundingBox.y -= FRAME_HEIGHT / 8; // Adjust starting position
                boundingBox.h += FRAME_HEIGHT / 4; // Increase height slightly
                break;
            case Left:
            case Right:
                boundingBox.x -= FRAME_WIDTH / 8; // Adjust starting position
                boundingBox.w += FRAME_WIDTH / 4; // Increase width slightly
                break;
        }
    }

    return boundingBox;
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
            currentFrame.y = 24 * FRAME_HEIGHT; // Assuming row 24 for dying animation

            if (currentFrameIndex == numFrames - 1) {
                // Animation finished
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
    //if (arrowActive) return;

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

// Protected getters and setters
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
