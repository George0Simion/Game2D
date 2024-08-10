#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Player.h"
#include "Game.h"
#include <cstdlib>
#include <cmath>
#include <queue>
#include <map>
#include <utility>
#include <unordered_map>

class Enemy : public Entity {
public:
    Enemy(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    void updateBehavior(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities, Game& game);
    void updateEnemy(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities, Game& game);
    SDL_Rect getAttackBoundingBox() const override;
    int getThrustRange() const override;

    static const int INITIAL_HEALTH = 150;
    static const int THRUST_DAMAGE = 35;
    static const int SPELL_DAMAGE = 45;
    static const int SPELL_DURATION = 6000;
    static const float SPELL_COOLDOWN;

    int getMaxHealth() const override { return INITIAL_HEALTH; }

    void setSpellTarget(float targetX, float targetY) override;
    void updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities, Game& game);

protected:
    int getActionOffset() const override;

private:
    float attackCooldown;
    float spellRange;
    float thrustRange;
    float moveSpeed;
    float spellCooldownRemaining;
    std::vector<std::pair<float, float>> path; // Path for the enemy to follow
    size_t currentPathIndex; // Current index in the path

    float directionChangeCooldown;
    float timeSinceLastDirectionChange;
    float targetX, targetY;
    bool hasTarget;

    void decideAction(Player& player, float distance);
    void followPlayer(float deltaTime, Player& player, Game& game);
    void randomMove(float deltaTime, Game& game);

    std::vector<std::pair<int, int>> pathToPlayer;
    bool hasPath;
    
    std::vector<std::pair<int, int>> findPathToPlayer(Player& player, Game& game);
    void moveDirectlyToPlayer(float deltaTime, Player& player, Game& game);

    enum SpellState {
        CURVED_TRAJECTORY,
        BOUNCING
    };

    SpellState spellState;
    Uint32 lastBounceTime;
    float bounceDistance;

    const Uint32 curveInterval = 2000;
    const float maxBounceDistance = 500.0f;
    int bounceCount;
    const int maxBounces = 10;
    float spellDirX;
    float spellDirY;
};

#endif
