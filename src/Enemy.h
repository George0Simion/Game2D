#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Player.h"
#include <cstdlib>
#include <cmath>

class Enemy : public Entity {
public:
    Enemy(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    void updateBehavior(float deltaTime, Player& player);
    void update(float deltaTime) override;

protected:
    int getActionOffset() const override; // Override the action offset method

private:
    float attackCooldown;
    float spellRange;
    float thrustRange;
    float moveSpeed;
    void decideAction(Player& player, float distance);
};

#endif
