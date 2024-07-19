#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class Player : public Entity {
public:
    Player(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed);

    void handleInput(const SDL_Event& event);
    void update(float deltaTime) override;
};

#endif
