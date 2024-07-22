#include "Player.h"

Player::Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed) {}

void Player::handleInput(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_e) {
                setAction(Slashing);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(200); // Set delay to 200ms
                setDamageApplied(false);
            } else if (event.key.keysym.sym == SDLK_q) {
                setAction(Spellcasting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(500); // Set delay to 500ms
                setDamageApplied(false);
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
            if (event.button.button == SDL_BUTTON_LEFT) {
                setAction(Shooting);
                startAnimation();
                setAttackStartTime(SDL_GetTicks());
                setAttackDelay(300); // Set delay to 300ms
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

                    // Return to idle or walking state after shooting
                    if (isMoving()) {
                        setAction(Walking);
                    } else {
                        setAction(Walking); // Change to Idle if you have an Idle action
                    }
                    startAnimation();
                }
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                if (getAction() == Thrusting) {
                    stopAnimation();
                    setAction(Walking); // Change to Idle if you have an Idle action
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
    Entity::update(deltaTime);
    // Additional player-specific updates can be added here
}