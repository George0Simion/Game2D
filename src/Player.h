#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <vector>
#include <memory>

// Forward declaration of Game class
class Game;

class Player : public Entity {
public:
    Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    void handleInput(const SDL_Event& event) override;
    void update(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities, Game& game);

    int getThrustRange() const override;
    SDL_Rect getAttackBoundingBox() const override;

    static const int INITIAL_HEALTH = 100;
    static const int INITIAL_STAMINA = 100;
    static const int THRUST_DAMAGE = 15;
    static const int SPELL_DAMAGE = 35;
    static const int SLASH_DAMAGE = 25;
    static const int ARROW_DAMAGE = 20;

    int getMaxHealth() const override { return INITIAL_HEALTH; }
    float getMaxStamina() const { return INITIAL_STAMINA; }

    bool getIsDead() const { return isDead; }
    void setIsDead(bool dead) { isDead = dead; }

    void updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) override;
    void updateArrowPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities); // Add this line

    void setDeathAnimationFinished(bool finished) { deathAnimationFinished = finished; }
    bool isDeathAnimationFinished() const;

    float getStamina() const { return stamina; }
    void useStamina(float amount);
    void regenerateStamina(float deltaTime);

private:
    bool isDead;
    bool deathAnimationFinished;
    float stamina;

    void handleCooldowns(float deltaTime);

    static const float SPELL_COOLDOWN;
    static const float SLASH_COOLDOWN;
    static const float SHOOTING_COOLDOWN;
};

#endif