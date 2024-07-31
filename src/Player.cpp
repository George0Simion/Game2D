#include "Player.h"
#include "Game.h"

const float Player::SPELL_COOLDOWN = 15.0f;
const float Player::SLASH_COOLDOWN = 10.0f;
const float Player::SHOOTING_COOLDOWN = 5.0f;

Player::Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed), isDead(false), deathAnimationFinished(false), stamina(INITIAL_STAMINA) {}

void Player::updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    if (spellActive) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - spellStartTime > spellDuration) {
            deactivateSpell();
            return;
        }

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
            float angle = atan2(dy, dx);
            float curve = sin(SDL_GetTicks() * spellCurveFactor);

            float controlPointX = (spellX + spellTargetX) / 2 + curve * 50; // Adding a control point for curve
            float controlPointY = (spellY + spellTargetY) / 2 + curve * 50;

            // Interpolating positions for smooth, curved movement
            float t = spellSpeed * deltaTime / distance; // Parametric time
            spellX = (1 - t) * (1 - t) * spellX + 2 * (1 - t) * t * controlPointX + t * t * spellTargetX;
            spellY = (1 - t) * (1 - t) * spellY + 2 * (1 - t) * t * controlPointY + t * t * spellTargetY;
        } else {
            deactivateSpell();
        }

        // Check for collision with enemies
        SDL_Rect spellRect = { static_cast<int>(spellX), static_cast<int>(spellY), FRAME_WIDTH - 20, FRAME_HEIGHT - 20 }; // Reduced collision box size
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

void Player::updateArrowPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
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
            return;
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
