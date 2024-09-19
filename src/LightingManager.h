#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include <SDL2/SDL.h>
#include <vector>

class LightingManager {
public:
    LightingManager(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~LightingManager();

    // Method to retrieve the dimming texture for menu use
    SDL_Texture* getDimmingTexture() const;

    void renderLighting(
        const SDL_Rect& playerPosition,
        const std::vector<SDL_Rect>& enemyPositions,
        const std::vector<SDL_Rect>& spellPositions,
        const std::vector<std::vector<int>>& dungeonMaze,
        const SDL_Rect& camera
    );

private:
    SDL_Renderer* renderer;
    int screenWidth;
    int screenHeight;
    SDL_Texture* lightMapTexture; // Texture for the light map
    SDL_Texture* dimmingTexture;  // Dimming texture for menu

    // Spatial partitioning grid
    int gridWidth;
    int gridHeight;
    std::vector<std::vector<SDL_Rect>> grid;

    // Helper structures for ray casting
    struct Vector2 {
        float x, y;
    };

    struct Ray {
        Vector2 start;
        Vector2 end;
    };

    // Methods for dynamic lighting
    void initializeGrid(int width, int height);
    void clearGrid();
    void populateGrid(const std::vector<SDL_Rect>& obstacles);
    std::vector<SDL_Rect> getPotentialObstacles(const Vector2& start, const Vector2& end);

    std::vector<Ray> castLightRays(Vector2 lightPos, float radius, int numRays);
    bool rayIntersectsRect(Vector2 rayStart, Vector2 rayEnd,
                           const SDL_Rect& rect, Vector2& outIntersection);
    bool lineLineIntersection(Vector2 p1, Vector2 p2,
                              Vector2 p3, Vector2 p4, Vector2& outIntersection);
    bool onSegment(Vector2 p, Vector2 q, Vector2 r);
    void drawLightArea(Vector2 lightPos, const std::vector<Ray>& rays,
                       float lightRadius, const SDL_Rect& camera);

    void createDimmingTexture();
};

#endif // LIGHTINGMANAGER_H
