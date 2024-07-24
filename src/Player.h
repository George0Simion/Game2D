#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class Player : public Entity {
public:
    Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    void handleInput(const SDL_Event& event);
    void update(float deltaTime) override;

    int getThrustRange() const override;
    SDL_Rect getAttackBoundingBox() const override;

    static const int INITIAL_HEALTH = 100;
    static const int THRUST_DAMAGE = 15;
    static const int SPELL_DAMAGE = 35;
    static const int SLASH_DAMAGE = 25;
    static const int ARROW_DAMAGE = 20;

    int getMaxHealth() const override { return INITIAL_HEALTH; }

    bool getIsDead() const { return isDead; }
    void setIsDead(bool dead) { isDead = dead; }

    void setDeathAnimationFinished(bool finished) { deathAnimationFinished = finished; }
    bool isDeathAnimationFinished() const;

private:
    bool isDead;
    bool deathAnimationFinished;
};

#endif
