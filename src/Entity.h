// Entity.h

#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <memory>
#include <limits>

class Enemy; // Forward declaration

class Entity {
public:
    enum Direction { Up, Left, Down, Right };
    enum Action { Walking, Slashing, Thrusting, Spellcasting, Shooting, ArrowFlyingUp, ArrowFlyingDown, ArrowFlyingLeft, ArrowFlyingRight, Dying };

    Entity(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    float getX();
    float getY();
    void setX(float p_x);
    void setY(float p_y);

    SDL_Texture* getTex();
    SDL_Rect getCurrentFrame();
    void setCurrentFrame(const SDL_Rect& frame);

    virtual void update(float deltaTime);
    virtual void handleInput(const SDL_Event& event); // Make handleInput virtual
    void startAnimation();
    void stopAnimation();
    void setDirection(Direction dir);
    void setAction(Action act);
    bool isMoving() const;
    Action getAction() const;
    Direction getDirection() const;
    void shootArrow(Direction dir);

    void setRunning(bool running);
    bool isRunning() const;

    bool isArrowActive() const;
    float getArrowX() const;
    float getArrowY() const;
    SDL_Rect getArrowFrame() const;

    virtual SDL_Rect getBoundingBox() const;
    virtual SDL_Rect getAttackBoundingBox() const;
    virtual int getThrustRange() const;
    virtual SDL_Rect getCollisionBoundingBox() const;

    static bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
        return SDL_HasIntersection(&a, &b);
    }

    static int getFrameWidth() { return FRAME_WIDTH; }
    static int getFrameHeight() { return FRAME_HEIGHT; }

    int getHealth() const;
    void setHealth(int health);
    bool isAlive() const;
    void takeDamage(int damage);
    virtual int getMaxHealth() const = 0;

    bool getDamageApplied() const; // New getter for damageApplied
    void setDamageApplied(bool value); // New setter for damageApplied
    Uint32 getAttackStartTime() const; // New getter for attackStartTime
    void setAttackStartTime(Uint32 time); // New setter for attackStartTime
    Uint32 getAttackDelay() const; // New getter for attackDelay
    void setAttackDelay(Uint32 delay); // New setter for attackDelay

    void setNumFrames(int numFrames);

    // New methods for spell handling
    void setSpellTarget(float targetX, float targetY);
    void updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities);
    bool isSpellActive() const;
    SDL_Rect getSpellFrame() const;
    float getSpellX() const;
    float getSpellY() const;
    void deactivateSpell();

protected:
    virtual int getActionOffset() const; // New virtual method for action offset
    float getAnimationTimer() const;
    void setAnimationTimer(float timer);
    int getCurrentFrameIndex() const;
    void setCurrentFrameIndex(int index);
    SDL_Rect& getCurrentFrameRef();
    bool isEntityMoving() const;
    bool isEntityRunning() const;
    float getAnimationSpeed() const;
    int getNumFrames() const;

    static const int FRAME_WIDTH = 64;
    static const int FRAME_HEIGHT = 64;

    bool damageApplied;
    Uint32 attackStartTime; // New member to track the start time of the attack
    Uint32 attackDelay;

    Direction direction;
    Action action;

    bool spellActive;
    float spellX, spellY;
    float spellTargetX, spellTargetY;
    float spellSpeed;
    float spellCurveFactor;

    float x, y;
    bool moving;
    bool running;

    SDL_Texture* tex;
    SDL_Rect currentFrame;

    int numFrames;
    int currentFrameIndex;
    float animationSpeed;
    float animationTimer;

    bool arrowActive;
    float arrowX, arrowY;
    float arrowSpeed;
    float arrowMaxDistance;
    float arrowTravelDistance;
    Direction arrowDirection;

    int health;

    Uint32 spellStartTime;
    Uint32 spellDuration;
};

#endif