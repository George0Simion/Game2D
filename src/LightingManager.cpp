#include "LightingManager.h"
#include <cmath>
#include <limits>
#include <algorithm>

const int TILE_SIZE = 96;
const int GRID_CELL_SIZE = 192; // Adjust grid cell size as needed
const float EPSILON = 1e-6f;

LightingManager::LightingManager(SDL_Renderer* renderer, int screenWidth,
                                 int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight),
      gridWidth(0), gridHeight(0) {

    // Create the light map texture
    lightMapTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET,
                                        screenWidth, screenHeight);
    if (!lightMapTexture) {
        SDL_Log("Failed to create light map texture: %s", SDL_GetError());
        return;
    }
    SDL_SetTextureBlendMode(lightMapTexture, SDL_BLENDMODE_MOD);

    // Create the dimming texture
    createDimmingTexture();
}

LightingManager::~LightingManager() {
    if (lightMapTexture) {
        SDL_DestroyTexture(lightMapTexture);
        lightMapTexture = nullptr;
    }
    if (dimmingTexture) {
        SDL_DestroyTexture(dimmingTexture);
        dimmingTexture = nullptr;
    }
}

void LightingManager::createDimmingTexture() {
    dimmingTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET,
                                       screenWidth, screenHeight);
    if (!dimmingTexture) {
        SDL_Log("Failed to create dimming texture: %s", SDL_GetError());
        return;
    }
    SDL_SetTextureBlendMode(dimmingTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, dimmingTexture);
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 180); // Adjust alpha for darkness
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr); // Reset to default render target
}

SDL_Texture* LightingManager::getDimmingTexture() const {
    return dimmingTexture;
}

void LightingManager::initializeGrid(int width, int height) {
    gridWidth = (width + GRID_CELL_SIZE - 1) / GRID_CELL_SIZE;
    gridHeight = (height + GRID_CELL_SIZE - 1) / GRID_CELL_SIZE;
    grid.resize(gridWidth * gridHeight);
}

void LightingManager::clearGrid() {
    for (auto& cell : grid) {
        cell.clear();
    }
}

void LightingManager::populateGrid(const std::vector<SDL_Rect>& obstacles) {
    clearGrid();
    for (const auto& obstacle : obstacles) {
        int minX = obstacle.x / GRID_CELL_SIZE;
        int minY = obstacle.y / GRID_CELL_SIZE;
        int maxX = (obstacle.x + obstacle.w) / GRID_CELL_SIZE;
        int maxY = (obstacle.y + obstacle.h) / GRID_CELL_SIZE;

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                    grid[y * gridWidth + x].push_back(obstacle);
                }
            }
        }
    }
}

std::vector<SDL_Rect> LightingManager::getPotentialObstacles(
    const Vector2& start, const Vector2& end) {
    std::vector<SDL_Rect> potentialObstacles;

    // Determine which grid cells the ray passes through
    int x0 = static_cast<int>(start.x) / GRID_CELL_SIZE;
    int y0 = static_cast<int>(start.y) / GRID_CELL_SIZE;
    int x1 = static_cast<int>(end.x) / GRID_CELL_SIZE;
    int y1 = static_cast<int>(end.y) / GRID_CELL_SIZE;

    x0 = std::max(0, std::min(x0, gridWidth - 1));
    y0 = std::max(0, std::min(y0, gridHeight - 1));
    x1 = std::max(0, std::min(x1, gridWidth - 1));
    y1 = std::max(0, std::min(y1, gridHeight - 1));

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int n = 1 + dx + dy;
    int x_inc = (x1 > x0) ? 1 : -1;
    int y_inc = (y1 > y0) ? 1 : -1;
    int error = dx - dy;

    dx *= 2;
    dy *= 2;

    int x = x0;
    int y = y0;

    for (; n > 0; --n) {
        const auto& cellObstacles = grid[y * gridWidth + x];
        potentialObstacles.insert(potentialObstacles.end(),
                                  cellObstacles.begin(), cellObstacles.end());

        if (error > 0) {
            x += x_inc;
            error -= dy;
        } else {
            y += y_inc;
            error += dx;
        }
    }

    return potentialObstacles;
}

std::vector<LightingManager::Ray> LightingManager::castLightRays(
    Vector2 lightPos, float radius, int numRays) {
    std::vector<Ray> rays;

    for (int i = 0; i < numRays; ++i) {
        double angle = (2.0 * M_PI) * i / numRays + 0.0001;
        double dirX = cos(angle);
        double dirY = sin(angle);

        Vector2 dir = { static_cast<float>(dirX), static_cast<float>(dirY) };
        Vector2 endPoint = { lightPos.x + dir.x * radius,
                             lightPos.y + dir.y * radius };

        // Get potential obstacles using spatial partitioning
        std::vector<SDL_Rect> potentialObstacles =
            getPotentialObstacles(lightPos, endPoint);

        // **Filter out obstacles that contain the light source**
        potentialObstacles.erase(
            std::remove_if(potentialObstacles.begin(), potentialObstacles.end(),
                [lightPos](const SDL_Rect& rect) {
                    SDL_Point lightPoint = { static_cast<int>(lightPos.x), static_cast<int>(lightPos.y) };
                    return SDL_PointInRect(&lightPoint, &rect);
                }),
            potentialObstacles.end());

        // Perform ray-wall intersection to find the closest obstacle
        float closestDist = radius;
        Vector2 closestPoint = endPoint;

        for (const auto& obstacle : potentialObstacles) {
            Vector2 intersectionPoint;
            if (rayIntersectsRect(lightPos, endPoint, obstacle,
                                  intersectionPoint)) {
                float dist = hypotf(intersectionPoint.x - lightPos.x,
                                    intersectionPoint.y - lightPos.y);
                if (dist < closestDist) {
                    closestDist = dist;
                    closestPoint = intersectionPoint;
                }
            }
        }

        rays.push_back({ lightPos, closestPoint });
    }

    return rays;
}

bool LightingManager::rayIntersectsRect(Vector2 rayStart, Vector2 rayEnd,
    const SDL_Rect& rect, Vector2& outIntersection) {
    // Define the rectangle edges as lines
    std::vector<std::pair<Vector2, Vector2>> rectEdges = {
        { { (float)rect.x, (float)rect.y },
          { (float)(rect.x + rect.w), (float)rect.y } }, // Top edge
        { { (float)(rect.x + rect.w), (float)rect.y },
          { (float)(rect.x + rect.w), (float)(rect.y + rect.h) } }, // Right edge
        { { (float)(rect.x + rect.w), (float)(rect.y + rect.h) },
          { (float)rect.x, (float)(rect.y + rect.h) } }, // Bottom edge
        { { (float)rect.x, (float)(rect.y + rect.h) },
          { (float)rect.x, (float)rect.y } } // Left edge
    };

    bool hit = false;
    float closestDist = std::numeric_limits<float>::max();

    for (const auto& edge : rectEdges) {
        Vector2 intersectionPoint;
        if (lineLineIntersection(rayStart, rayEnd, edge.first, edge.second,
                                 intersectionPoint)) {
            float dist = hypotf(intersectionPoint.x - rayStart.x,
                                intersectionPoint.y - rayStart.y);
            if (dist < closestDist) {
                closestDist = dist;
                outIntersection = intersectionPoint;
                hit = true;
            }
        }
    }

    return hit;
}

bool LightingManager::lineLineIntersection(Vector2 p1, Vector2 p2,
                                           Vector2 p3, Vector2 p4,
                                           Vector2& outIntersection) {
    // Line AB represented as p1 + r * (p2 - p1)
    // Line CD represented as p3 + s * (p4 - p3)
    Vector2 r = { p2.x - p1.x, p2.y - p1.y };
    Vector2 s = { p4.x - p3.x, p4.y - p3.y };

    float rxs = r.x * s.y - r.y * s.x;
    float qpxr = (p3.x - p1.x) * r.y - (p3.y - p1.y) * r.x;

    if (fabsf(rxs) < EPSILON && fabsf(qpxr) < EPSILON) {
        // Lines are colinear
        return false;
    }

    if (fabsf(rxs) < EPSILON && fabsf(qpxr) >= EPSILON) {
        // Lines are parallel and non-intersecting
        return false;
    }

    float t = ((p3.x - p1.x) * s.y - (p3.y - p1.y) * s.x) / rxs;
    float u = ((p3.x - p1.x) * r.y - (p3.y - p1.y) * r.x) / rxs;

    if (t >= -EPSILON && t <= 1 + EPSILON && u >= -EPSILON && u <= 1 + EPSILON) {
        // Intersection point
        outIntersection.x = p1.x + t * r.x;
        outIntersection.y = p1.y + t * r.y;
        return true;
    }

    return false;
}


bool LightingManager::onSegment(Vector2 p, Vector2 q, Vector2 r) {
    return (r.x <= std::max(p.x, q.x) + EPSILON && r.x >= std::min(p.x, q.x) - EPSILON &&
            r.y <= std::max(p.y, q.y) + EPSILON && r.y >= std::min(p.y, q.y) - EPSILON);
}

void LightingManager::drawLightArea(Vector2 lightPos, const std::vector<Ray>& rays,
                                    float lightRadius, const SDL_Rect& camera) {
    size_t numRays = rays.size();
    for (size_t i = 0; i < numRays; ++i) {
        Vector2 p1 = rays[i].end;
        Vector2 p2 = rays[(i + 1) % numRays].end;

        // Adjust positions relative to the camera
        Vector2 adjustedLightPos = { lightPos.x - camera.x, lightPos.y - camera.y };
        Vector2 adjustedP1 = { p1.x - camera.x, p1.y - camera.y };
        Vector2 adjustedP2 = { p2.x - camera.x, p2.y - camera.y };

        // Calculate distances for attenuation
        float dist1 = hypotf(p1.x - lightPos.x, p1.y - lightPos.y);
        float dist2 = hypotf(p2.x - lightPos.x, p2.y - lightPos.y);

        float maxDistance = std::max(dist1, dist2);

        // Determine alpha based on distance
        Uint8 alpha = static_cast<Uint8>(255 * (1.0f - (maxDistance / lightRadius)));
        alpha = std::clamp(alpha, static_cast<Uint8>(0), static_cast<Uint8>(255));

        // Draw triangle between adjustedLightPos, adjustedP1, and adjustedP2
        SDL_Vertex vertices[3];

        // Brighter light color
        vertices[0].position.x = adjustedLightPos.x;
        vertices[0].position.y = adjustedLightPos.y;
        vertices[0].color = { 255, 255, 240, 255 }; // Slightly brighter center
        vertices[0].tex_coord = { 0, 0 };

        vertices[1].position.x = adjustedP1.x;
        vertices[1].position.y = adjustedP1.y;
        vertices[1].color = { 60, 60, 60, alpha }; // Adjusted edge color
        vertices[1].tex_coord = { 0, 0 };

        vertices[2].position.x = adjustedP2.x;
        vertices[2].position.y = adjustedP2.y;
        vertices[2].color = { 60, 60, 60, alpha };
        vertices[2].tex_coord = { 0, 0 };

        SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);
    }
}

void LightingManager::renderLighting(
    const SDL_Rect& playerPosition,
    const std::vector<SDL_Rect>& enemyPositions,
    const std::vector<SDL_Rect>& spellPositions,
    const std::vector<std::vector<int>>& dungeonMaze,
    const SDL_Rect& camera) {

    // Set the render target to the light map texture
    SDL_SetRenderTarget(renderer, lightMapTexture);

    // Clear the light map (fully dark)
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Adjusted for brightness
    SDL_RenderClear(renderer);

    // Collect obstacles
    std::vector<SDL_Rect> obstacles;
    int cellSize = TILE_SIZE;
    int dungeonWidth = dungeonMaze[0].size() * cellSize;
    int dungeonHeight = dungeonMaze.size() * cellSize;

    // Initialize grid if necessary
    if (grid.empty()) {
        initializeGrid(dungeonWidth, dungeonHeight);
    }
    clearGrid();

    for (int y = 0; y < dungeonMaze.size(); ++y) {
        for (int x = 0; x < dungeonMaze[y].size(); ++x) {
            if (dungeonMaze[y][x] == -1) {
                SDL_Rect wallRect = {
                    x * cellSize,
                    y * cellSize,
                    cellSize,
                    cellSize
                };
                obstacles.push_back(wallRect);
            }
        }
    }

    // Populate the grid with obstacles
    populateGrid(obstacles);

    // Set blend mode for additive blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

    // Render lighting for the player
    Vector2 playerLightPos = {
        static_cast<float>(playerPosition.x + playerPosition.w / 2),
        static_cast<float>(playerPosition.y + playerPosition.h / 2)
    };

    float lightRadius = 400.0f; // Adjust as needed
    int numRays = 360; // Adjusted for performance
    auto rays = castLightRays(playerLightPos, lightRadius, numRays);
    drawLightArea(playerLightPos, rays, lightRadius, camera);

    // Render lighting for enemies
    for (const auto& enemyPosition : enemyPositions) {
        Vector2 enemyLightPos = {
            static_cast<float>(enemyPosition.x + enemyPosition.w / 2 + camera.x),
            static_cast<float>(enemyPosition.y + enemyPosition.h / 2 + camera.y)
        };
        float enemyLightRadius = 300.0f; // Adjust as needed
        int enemyNumRays = 240; // Fewer rays for enemies
        auto enemyRays = castLightRays(enemyLightPos, enemyLightRadius,
                                       enemyNumRays);
        drawLightArea(enemyLightPos, enemyRays, enemyLightRadius, camera);
    }

    // Render lighting for spells
    for (const auto& spellPosition : spellPositions) {
        Vector2 spellLightPos = {
            static_cast<float>(spellPosition.x + spellPosition.w / 2 + camera.x),
            static_cast<float>(spellPosition.y + spellPosition.h / 2 + camera.y)
        };
        float spellLightRadius = 200.0f; // Adjust as needed
        int spellNumRays = 240; // Fewer rays for spells
        auto spellRays = castLightRays(spellLightPos, spellLightRadius,
                                       spellNumRays);
        drawLightArea(spellLightPos, spellRays, spellLightRadius, camera);
    }

    // Reset the render target to the default
    SDL_SetRenderTarget(renderer, nullptr);

    // Render the dimming texture over the entire screen
    SDL_RenderCopy(renderer, dimmingTexture, nullptr, nullptr);

    // Render the light map over the scene
    SDL_SetTextureBlendMode(lightMapTexture, SDL_BLENDMODE_MOD);
    SDL_RenderCopy(renderer, lightMapTexture, nullptr, nullptr);

    // Reset blend mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}
