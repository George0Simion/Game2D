#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include <SDL2/SDL.h>
#include <vector>

class LightingManager {
public:
    LightingManager(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~LightingManager();

    SDL_Texture* getDimmingTexture() const;

    void renderLighting(
        const SDL_Rect& playerPosition,
        const std::vector<SDL_Rect>& enemyPositions,
        const std::vector<SDL_Rect>& spellPositions,
        const std::vector<std::vector<int>>& dungeonMaze,
        const SDL_Rect& camera
    );

    // New function to render shadows
    void renderShadows(
        const SDL_Rect& playerPosition,
        const std::vector<SDL_Rect>& enemyPositions,
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

    // New helper function for shadow rendering
    void createShadowTexture(int width, int height, SDL_Texture*& texture);
};

#endif // LIGHTINGMANAGER_H
