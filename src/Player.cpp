#include "Player.h"

Player::Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed), isDead(false), deathAnimationFinished(false) {}

void Player::handleInput(const SDL_Event& event) {
    if (isDead && deathAnimationFinished) return;

    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_e) {
                setAction(Slashing);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(200);
                setDamageApplied(false);
            } else if (event.key.keysym.sym == SDLK_q && !isMoving()) { // Ensure the player is idle
                setAction(Spellcasting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(500);
                setDamageApplied(false);
                // Set spell position
                spellActive = true;
                spellX = x;
                spellY = y;
                spellStartTime = SDL_GetTicks();
                spellDuration = 4000;
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
            if (event.button.button == SDL_BUTTON_LEFT && !isArrowActive()) {
                setAction(Shooting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(400); // Set delay to 300ms
                setDamageApplied(false);
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                setAction(Thrusting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(150); // Set delay to 150ms
                setDamageApplied(false);
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

void Player::update(float deltaTime) {
    if (isDead) {
        if (!deathAnimationFinished) {
            if (getAction() != Entity::Dying) {
                setAction(Entity::Dying);
                startAnimation();
                setCurrentFrameIndex(0); // Start the dying animation from the first frame
                setNumFrames(6); // Set the number of frames for the dying animation
            }

            setAnimationTimer(getAnimationTimer() + deltaTime);
            if (getAnimationTimer() >= getAnimationSpeed()) {
                setAnimationTimer(0.0f);
                setCurrentFrameIndex((getCurrentFrameIndex() + 1));

                SDL_Rect frame = getCurrentFrame();
                frame.x = getCurrentFrameIndex() * FRAME_WIDTH;
                frame.y = 24 * FRAME_HEIGHT; // Assuming row 24 for dying animation
                setCurrentFrame(frame);

                if (getCurrentFrameIndex() == getNumFrames() - 1) {
                    // Stop the animation after the last frame
                    stopAnimation();
                    setDeathAnimationFinished(true); // Mark the death animation as finished
                }
            }
        }
    } else {
        Entity::update(deltaTime);
    }
}

bool Player::isDeathAnimationFinished() const {
    return getAction() == Entity::Dying && getCurrentFrameIndex() == getNumFrames() - 1;
}
