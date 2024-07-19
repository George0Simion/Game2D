#include "Player.h"

Player::Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed) {}

void Player::handleInput(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_e) {
                setAction(Slashing);
                startAnimation();
            } else if (event.key.keysym.sym == SDLK_q) {
                setAction(Spellcasting);
                startAnimation();
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
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                setAction(Thrusting);
                startAnimation();
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (getAction() == Shooting) {
                    stopAnimation();
                    shootArrow(getDirection());

                    switch (getDirection()) {
                        case Up:
                            setAction(ArrowFlyingUp);
                            break;
                        case Down:
                            setAction(ArrowFlyingDown);
                            break;
                        case Left:
                            setAction(ArrowFlyingLeft);
                            break;
                        case Right:
                            setAction(ArrowFlyingRight);
                            break;
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

void Player::update(float deltaTime) {
    Entity::update(deltaTime);
    // Additional player-specific updates can be added here
}
