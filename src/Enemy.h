#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Player.h"
#include <cstdlib>
#include <cmath>

class Enemy : public Entity {
public:
    Enemy(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    void updateBehavior(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities);
    void updateEnemy(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities);

    SDL_Rect getAttackBoundingBox() const override;
    int getThrustRange() const override;

    static const int INITIAL_HEALTH = 150;
    static const int THRUST_DAMAGE = 35;
    static const int SPELL_DAMAGE = 40;
    static const int SPELL_DURATION = 6000;
    static const float SPELL_COOLDOWN;

    int getMaxHealth() const override { return INITIAL_HEALTH; }

    void setSpellTarget(float targetX, float targetY) override;
    void updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) override;

protected:
    int getActionOffset() const override;

private:
    float attackCooldown;
    float spellRange;
    float thrustRange;
    float moveSpeed;
    float spellCooldownRemaining;
    void decideAction(Player& player, float distance);
};

#endif