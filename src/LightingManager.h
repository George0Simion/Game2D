#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include <SDL2/SDL.h>
#include <vector>

class LightingManager {
public:
    LightingManager(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~LightingManager();

    SDL_Texture* getDimmingTexture() const;

    // Ensure this prototype matches the function definition
    void renderLighting(
        const SDL_Rect& playerPosition,
        const std::vector<SDL_Rect>& enemyPositions,
        const std::vector<SDL_Rect>& spellPositions, // Include spell positions if used
        const std::vector<std::vector<int>>& dungeonMaze,
        const SDL_Rect& camera
    );

private:
    SDL_Renderer* renderer;
    int screenWidth;
    int screenHeight;
    SDL_Texture* lightingTexture;
    SDL_Texture* mediumLightingTexture;
    SDL_Texture* largeLightingTexture;
    SDL_Texture* dimmingTexture;

    void createLightTexture(int radius, float intensity, SDL_Texture*& texture);
};

#endif // LIGHTINGMANAGER_H
