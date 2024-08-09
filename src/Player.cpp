#include "Player.h"
#include "Game.h"
#include <iostream>

const float Player::SPELL_COOLDOWN = 15.0f;
const float Player::SLASH_COOLDOWN = 10.0f;
const float Player::SHOOTING_COOLDOWN = 5.0f;
const float INITIAL_SPELL_SPEED = 200.0f;

Player::Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed),
      isDead(false),
      deathAnimationFinished(false),
      stamina(INITIAL_STAMINA),
      bounceCount(0),
      spellDirX(0.0f),  // Initialize direction vectors to zero
      spellDirY(0.0f),
      spellState(CURVED_TRAJECTORY),  // Start in the curved trajectory state
      lastBounceTime(0),
      bounceDistance(0.0f) {}

void Player::updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities, Game& game) {
    if (!spellActive) {
        return;
    }

    Uint32 currentTime = SDL_GetTicks();

    // Check if the spell duration has expired
    if (currentTime - spellStartTime > spellDuration) {
        deactivateSpell();
        return;
    }

    // Padding to transition to bouncing before impact
    const float wallPadding = 5.0f;

    // Switch to curved trajectory if enough time has passed since the last bounce
    if (spellState == BOUNCING && currentTime - lastBounceTime > 2000) { // 3 seconds
        spellState = CURVED_TRAJECTORY;
    }

    if (spellState == CURVED_TRAJECTORY) {
        // Update target position more frequently to ensure smooth tracking
        Enemy* closestEnemy = nullptr;
        float minDistance = std::numeric_limits<float>::max();
        for (const auto& entity : entities) {
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
            float curve = sin(SDL_GetTicks() * 0.001f);

            float controlPointX = (spellX + spellTargetX) / 2 + curve * 50;  // Adding a control point for curve
            float controlPointY = (spellY + spellTargetY) / 2 + curve * 50;

            // Interpolating positions for smooth, curved movement
            float t = spellSpeed * deltaTime / distance;  // Parametric time
            spellX = (1 - t) * (1 - t) * spellX + 2 * (1 - t) * t * controlPointX + t * t * spellTargetX;
            spellY = (1 - t) * (1 - t) * spellY + 2 * (1 - t) * t * controlPointY + t * t * spellTargetY;

            // Check for collision with walls while in curved trajectory state
            if (isSpellCollidingWithWall(spellX + spellDirX * wallPadding, spellY + spellDirY * wallPadding, 96, game.getDungeonMaze())) {
                std::cout << "Collision detected during curved trajectory. Switching to bouncing state." << std::endl;

                spellState = BOUNCING;
                lastBounceTime = currentTime;
                bounceDistance = 0.0f;

                // Set initial direction based on the current trajectory
                float directionDx = spellTargetX - spellX;
                float directionDy = spellTargetY - spellY;
                float directionDistance = std::hypot(directionDx, directionDy);

                if (directionDistance > 0) {
                    spellDirX = directionDx / directionDistance;
                    spellDirY = directionDy / directionDistance;
                }
            }
        } else {
            deactivateSpell();
            return;
        }
    } else if (spellState == BOUNCING) {
        // Calculate the new position during the bouncing state
        float newSpellX = spellX + spellDirX * spellSpeed * deltaTime;
        float newSpellY = spellY + spellDirY * spellSpeed * deltaTime;

        // Track the distance traveled during bouncing
        bounceDistance += std::hypot(newSpellX - spellX, newSpellY - spellY);

        // Check for collision with walls using the existing method
        if (isSpellCollidingWithWall(newSpellX + spellDirX * wallPadding, newSpellY + spellDirY * wallPadding, 96, game.getDungeonMaze())) {
            std::cout << "Collision detected. Bounce Count: " << bounceCount << std::endl;

            // Reflect the direction vector based on the collision
            if (isSpellCollidingWithWall(newSpellX + spellDirX * wallPadding, spellY, 96, game.getDungeonMaze())) {
                spellDirX = -spellDirX;  // Reverse horizontal direction
            }
            if (isSpellCollidingWithWall(spellX, newSpellY + spellDirY * wallPadding, 96, game.getDungeonMaze())) {
                spellDirY = -spellDirY;  // Reverse vertical direction
            }

            // Recalculate the new position after reflection
            newSpellX = spellX + spellDirX * spellSpeed * deltaTime;
            newSpellY = spellY + spellDirY * spellSpeed * deltaTime;

            bounceCount++;
            lastBounceTime = currentTime;  // Reset the bounce time
            bounceDistance = 0.0f;  // Reset the bounce distance

            // Deactivate the spell if the maximum number of bounces is reached
            if (bounceCount >= 10) {
                deactivateSpell();
                return;
            }
        }

        // Update spell position
        spellX = newSpellX;
        spellY = newSpellY;
    }

    // Check for collision with enemies
    SDL_Rect spellRect = { static_cast<int>(spellX), static_cast<int>(spellY), FRAME_WIDTH - 40, FRAME_HEIGHT - 40 };
    for (auto& entity : entities) {
        if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
            SDL_Rect enemyBoundingBox = enemy->getBoundingBox();
            if (SDL_HasIntersection(&spellRect, &enemyBoundingBox)) {
                enemy->takeDamage(Player::SPELL_DAMAGE);
                if (!enemy->isAlive()) {
                    enemy->markForRemoval();
                }

                deactivateSpell();
                return;
            }
        }
    }
}

void Player::deactivateSpell() {
    spellActive = false;
    bounceCount = 0;  // Reset bounce count when the spell is deactivated
    spellSpeed = INITIAL_SPELL_SPEED;  // Reset spell speed to its initial value
    spellState = CURVED_TRAJECTORY;  // Reset to curved trajectory state
}

void Player::handleInput(const SDL_Event& event) {
    if (isDead && deathAnimationFinished) return;

    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_e && !isCooldownActive("Slashing") && stamina >= 15) {
                setAction(Slashing);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(200);
                setDamageApplied(false);
                setCooldown("Slashing", SLASH_COOLDOWN);
                useStamina(15);

            } else if (event.key.keysym.sym == SDLK_q && !isMoving() && !isCooldownActive("Spellcasting") && stamina >= 20) {
                setAction(Spellcasting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(500);
                setDamageApplied(false);
                setSpellTarget(x + FRAME_WIDTH / 2, y - 40);
                spellStartTime = SDL_GetTicks();
                setCooldown("Spellcasting", SPELL_COOLDOWN);
                useStamina(20);

            } else if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                setRunning(true);
            }
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                setRunning(false);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && !isArrowActive() && !isCooldownActive("Shooting") && stamina >= 10) {
                setAction(Shooting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(400);
                setDamageApplied(false);
                setCooldown("Shooting", SHOOTING_COOLDOWN);
                useStamina(10);

            } else if (event.button.button == SDL_BUTTON_RIGHT && stamina >= 5) {
                setAction(Thrusting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(150);
                setDamageApplied(false);
                useStamina(5);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (getAction() == Shooting) {
                    stopAnimation();
                    shootArrow(getDirection());
                    if (isMoving()) {
                        setAction(Walking);
                    } else {
                        setAction(Walking);
                    }
                    startAnimation();
                }
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                if (getAction() == Thrusting) {
                    stopAnimation();
                    setAction(Walking);
                }
            }
            break;
        default:
            break;
    }
}

void Player::updateArrowPosition(float deltaTime, const std::vector<std::vector<int>>& dungeonMaze, int cellSize) {
    if (arrowActive) {
        float distance = arrowSpeed * deltaTime;
        arrowTravelDistance += distance;

        float newArrowX = arrowX;
        float newArrowY = arrowY;

        switch (arrowDirection) {
            case Up:    newArrowY -= distance; break;
            case Down:  newArrowY += distance; break;
            case Left:  newArrowX -= distance; break;
            case Right: newArrowX += distance; break;
        }

        if (arrowTravelDistance >= arrowMaxDistance || isArrowCollidingWithWall(newArrowX, newArrowY, cellSize, dungeonMaze)) {
            arrowActive = false;
        }
    }
}

void Player::handleCooldowns(float deltaTime) {
    updateCooldowns(deltaTime);

    // Example: When the player shoots, set the shooting cooldown
    if (isCooldownActive("Shooting")) {
        setCooldown("Shooting", SHOOTING_COOLDOWN);
    }
}

void Player::update(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities, Game& game) {
    if (isDead) {
        if (!deathAnimationFinished) {
            if (getAction() != Entity::Dying) {
                setAction(Entity::Dying);
                startAnimation();
                setCurrentFrameIndex(0);
                setNumFrames(6);
            }

            setAnimationTimer(getAnimationTimer() + deltaTime);
            if (getAnimationTimer() >= getAnimationSpeed()) {
                setAnimationTimer(0.0f);
                setCurrentFrameIndex((getCurrentFrameIndex() + 1));

                SDL_Rect frame = getCurrentFrame();
                frame.x = getCurrentFrameIndex() * FRAME_WIDTH;
                frame.y = 24 * FRAME_HEIGHT;
                setCurrentFrame(frame);

                if (getCurrentFrameIndex() == getNumFrames() - 1) {
                    stopAnimation();
                    setDeathAnimationFinished(true);
                }
            }
        }
    } else {
        Entity::update(deltaTime);
        updateCooldowns(deltaTime);

        if (isRunning()) {
            useStamina(3 * deltaTime);
        } else {
            regenerateStamina(deltaTime);
        }

        if (stamina <= 0) {
            setRunning(false);
        }

        // Check for attack damage application
        if (getAction() == Thrusting && !getDamageApplied() && (SDL_GetTicks() - getAttackStartTime() >= getAttackDelay())) {
            for (auto& entity : entities) {
                if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                    if (Entity::checkCollision(getAttackBoundingBox(), enemy->getBoundingBox())) {
                        game.applyDamage(*this, *enemy, Player::THRUST_DAMAGE);
                        setDamageApplied(true);
                    }
                }
            }
        } else if (getAction() == Slashing && !getDamageApplied() && (SDL_GetTicks() - getAttackStartTime() >= getAttackDelay())) {
            for (auto& entity : entities) {
                if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                    if (Entity::checkCollision(getAttackBoundingBox(), enemy->getBoundingBox())) {
                        game.applyDamage(*this, *enemy, Player::SLASH_DAMAGE);
                        setDamageApplied(true);
                    }
                }
            }
        }
    }
}

void Player::useStamina(float amount) {
    stamina -= amount;
    if (stamina < 0) {
        stamina = 0;
    }
}

void Player::regenerateStamina(float deltaTime) {
    if (!isRunning()) {
        stamina += 4 * deltaTime;
        if (stamina > INITIAL_STAMINA) {
            stamina = INITIAL_STAMINA;
        }
    }
}

SDL_Rect Player::getAttackBoundingBox() const {
    SDL_Rect boundingBox = getBoundingBox();
    int thrustRange = getThrustRange();

    if (getAction() == Thrusting) {
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
    } else if (getAction() == Slashing) {
        switch (getDirection()) {
            case Up:
                boundingBox.y -= 60; boundingBox.h += 60; break;
            case Down:
                boundingBox.h += 60; break;
            case Left:
                boundingBox.x -= 60; boundingBox.w += 60; break;
            case Right:
                boundingBox.w += 60; break;
        }
    }

    return boundingBox;
}

int Player::getThrustRange() const {
    return 30; // Thrust range for the player
}

bool Player::isDeathAnimationFinished() const {
    return getAction() == Entity::Dying && getCurrentFrameIndex() == getNumFrames() - 1;
}
