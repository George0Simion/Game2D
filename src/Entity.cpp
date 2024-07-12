#include "Entity.h"

/* Entity constructor with the intial pozitions */
Entity::Entity(float p_x, float p_y, SDL_Texture* p_tex): x(p_x), y(p_y), tex(p_tex) {
    currentFrame.x = 0;
    currentFrame.y = 0;
    currentFrame.w = 128;
    currentFrame.h = 128;
}

/* Methods for the entity */
float Entity::getX() {
    return x;
}
float Entity::getY() {
    return y;
}
void Entity::setX(float p_x) {
    x = p_x;
}
void Entity::setY(float p_y) {
    y = p_y;
}
SDL_Texture* Entity::getTex() {
    return tex;
}
SDL_Rect Entity::getCurrentFrame() {
    return currentFrame; 
}
