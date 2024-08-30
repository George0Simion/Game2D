#include "World.h"
#include <iostream>
#include "GameMap.h"  // Include your map header to use mapMatrix

const int TILE_SIZE = 96;
const int TILE_SOURCE_SIZE = 32;

World::World(SDL_Renderer* p_renderer) : renderer(p_renderer), chunkSize(12), bushIndex(0) {
    // No need to initialize noise or similar elements
}

World::~World() {
    chunks.clear();
    visibleChunks.clear();
}

// No need to generate chunks based on noise; we use mapMatrix directly
void World::generateChunk(int chunkX, int chunkY) {
    std::string key = getChunkKey(chunkX, chunkY);
    if (chunks.find(key) != chunks.end()) return; // Chunk already generated

    // For a static world, you might predefine chunks or use mapMatrix directly
    // Here you could dynamically adjust if needed
    chunks[key] = mapMatrix;  // Assuming you adjust chunks by predefined rules
}

// Update visible chunks based on player's position
void World::update(float playerX, float playerY) {
    // Use the mapMatrix directly for simplicity in a static or semi-static world
    int centerX = static_cast<int>(playerX) / (chunkSize * TILE_SIZE);
    int centerY = static_cast<int>(playerY) / (chunkSize * TILE_SIZE);
    
    std::unordered_set<std::string> newVisibleChunks;
    
    for (int x = centerX - 2; x <= centerX + 2; ++x) {
        for (int y = centerY - 2; y <= centerY + 2; ++y) {
            std::string key = getChunkKey(x, y);
            generateChunk(x, y); // Ensure chunk is generated
            newVisibleChunks.insert(key);
        }
    }
    
    // Remove chunks that are no longer visible
    for (const auto& chunkKey : visibleChunks) {
        if (newVisibleChunks.find(chunkKey) == newVisibleChunks.end()) {
            chunks.erase(chunkKey); // Despawn the chunk
        }
    }
    
    visibleChunks = newVisibleChunks;
}

void World::render(float playerX, float playerY, bool isPlayerInDungeon, SDL_Rect dungeonEntrance, const SDL_Rect& camera, SDL_Texture* dungeonEntranceTexture, SDL_Texture* tilesetTexture) {
    for (int y = 0; y < mapMatrix.size(); ++y) {
        for (int x = 0; x < mapMatrix[y].size(); ++x) {
            SDL_Rect destRect = {
                x * TILE_SIZE - camera.x,
                y * TILE_SIZE - camera.y,
                TILE_SIZE,
                TILE_SIZE
            };

            // Always render grass first as the base layer
            SDL_Rect grassSrcRect = {TILE_SOURCE_SIZE * 9, TILE_SOURCE_SIZE * 4, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE};
            SDL_RenderCopy(renderer, tilesetTexture, &grassSrcRect, &destRect);

            // Now render other elements on top of the grass
            SDL_Rect srcRect;
            bool renderTile = true;

            switch (mapMatrix[y][x]) {
                case TILE_PATH:
                    srcRect = getPathTileSourceRect(x, y); // Determine which path tile to use
                    break;
                case TILE_FENCE:
                    srcRect = getFenceTileSourceRect(x, y);
                    break;
                case TILE_BUSH:
                    srcRect = {bushIndex * TILE_SOURCE_SIZE, TILE_SOURCE_SIZE * 6, TILE_SOURCE_SIZE - 12, TILE_SOURCE_SIZE - 12};
                    bushIndex = (bushIndex + 1) % 4;
                    break;
                case TILE_HOUSE_BLUE:
                    srcRect = {16, 0, TILE_SOURCE_SIZE * 3, TILE_SOURCE_SIZE * 3}; // Blue house
                    destRect.x = (x - 1) * TILE_SIZE - camera.x; // Adjust x-position to include the left tile
                    destRect.y = (y - 5) * TILE_SIZE - camera.y;
                    destRect.w = TILE_SIZE * 5;
                    destRect.h = TILE_SIZE * 5;
                    break;
                case TILE_HOUSE_RED:
                    srcRect = {TILE_SOURCE_SIZE * 4, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE * 2, TILE_SOURCE_SIZE * 2};
                    destRect.x = (x - 1) * TILE_SIZE - camera.x;
                    destRect.y = (y - 4) * TILE_SIZE - camera.y;
                    destRect.w = TILE_SIZE * 5;
                    destRect.h = TILE_SIZE * 4;
                    break;
                case TILE_TREE:
                    srcRect = {TILE_SOURCE_SIZE * 10, 0, TILE_SIZE - 25, TILE_SIZE}; // Tree
                    destRect.x = (x - 2) * TILE_SIZE - camera.x; // Adjust x-position to include the left tile
                    destRect.y = (y - 3) * TILE_SIZE - camera.y;
                    destRect.w = TILE_SIZE * 3;
                    destRect.h = TILE_SIZE * 4;
                    break;
                case TILE_DUNGEON_ENTRANCE:
                    destRect.x = (x - 2) * TILE_SIZE - camera.x; // Adjust x-position to include the left tile
                    destRect.y = (y - 3) * TILE_SIZE - camera.y;
                    destRect.w = TILE_SIZE * 5;
                    destRect.h = TILE_SIZE * 3;
                    if (!isPlayerInDungeon) {
                        SDL_RenderCopy(renderer, dungeonEntranceTexture, nullptr, &destRect);
                    }
                    renderTile = false;
                default:
                    renderTile = false;
                    break;
            }

            if (renderTile) {
                SDL_RenderCopy(renderer, tilesetTexture, &srcRect, &destRect);
            }
        }
    }
}

SDL_Rect World::getFenceTileSourceRect(int x, int y) {
    bool up = y > 0 && mapMatrix[y - 1][x] == TILE_FENCE;
    bool down = y < mapMatrix.size() - 1 && mapMatrix[y + 1][x] == TILE_FENCE;
    bool left = x > 0 && mapMatrix[y][x - 1] == TILE_FENCE;
    bool right = x < mapMatrix[y].size() - 1 && mapMatrix[y][x + 1] == TILE_FENCE;

    // Check for corners first
    if (left && up) {
        return {TILE_SOURCE_SIZE + 3, (TILE_SOURCE_SIZE - 3) * 12, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Bottom-right corner
    } else if (right && up) {
        return {TILE_SOURCE_SIZE + 3, (TILE_SOURCE_SIZE - 3) * 11, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Bottom-left corner
    } else if (left && down) {
        return {TILE_SOURCE_SIZE + 3, (TILE_SOURCE_SIZE - 3) * 10, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Top-right corner
    } else if (right && down) {
        return {TILE_SOURCE_SIZE + 3, (TILE_SOURCE_SIZE - 3) * 9, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Top-left corner
    }

    // Horizontal fence (left, right, or both)
    if (left || right) {
        return {TILE_SOURCE_SIZE + 3, (TILE_SOURCE_SIZE - 2) * 7, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Horizontal fence
    }

    // Vertical fence (up, down, or both)
    if (up || down) {
        return {TILE_SOURCE_SIZE + 3, (TILE_SOURCE_SIZE - 3) * 8, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 5}; // Vertical fence
    }

    // Default to a single fence post or a simple vertical/horizontal fence piece
    // Adjust coordinates as per your tileset if you have a single piece tile.
    return {TILE_SOURCE_SIZE * 7, TILE_SOURCE_SIZE * 4, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE}; // Single piece or default fence
}

// Determine which path tile to use based on surrounding tiles
SDL_Rect World::getPathTileSourceRect(int x, int y) {
    bool up = y > 0 && mapMatrix[y - 1][x] == TILE_PATH;
    bool down = y < mapMatrix.size() - 1 && mapMatrix[y + 1][x] == TILE_PATH;
    bool left = x > 0 && mapMatrix[y][x - 1] == TILE_PATH;
    bool right = x < mapMatrix[y].size() - 1 && mapMatrix[y][x + 1] == TILE_PATH;

    if (!left && right) {
        // Path tile with grass on the left side
        return {TILE_SOURCE_SIZE * 10, TILE_SOURCE_SIZE * 4, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Adjust coordinates as per tileset
    } else if (left && !right) {
        // Path tile with grass on the right side
        return {(TILE_SOURCE_SIZE + 1) * 8, (TILE_SOURCE_SIZE - 1) * 4, TILE_SOURCE_SIZE - 10, TILE_SOURCE_SIZE - 10}; // Adjust coordinates as per tileset
    } else if (!up && down) {
        // Path tile with grass on the top side
        return {(TILE_SOURCE_SIZE - 3) * 10, (TILE_SOURCE_SIZE + 1) * 5, TILE_SOURCE_SIZE - 11, TILE_SOURCE_SIZE - 13};
    } else if (up && !down) {
        // Path tile with grass on the bottom side
        return {TILE_SOURCE_SIZE * 9, (TILE_SOURCE_SIZE + 4) * 3, TILE_SOURCE_SIZE - 12, TILE_SOURCE_SIZE - 14}; // Adjust coordinates as per tileset
    } else {
        // Default path tile (horizontal or vertical based on common case)
        return {(TILE_SOURCE_SIZE - 3) * 13, (TILE_SOURCE_SIZE - 2) * 5, TILE_SOURCE_SIZE - 18, TILE_SOURCE_SIZE - 20}; // Horizontal or vertical, adjust as needed
    }
}

// Helper to convert chunk coordinates to a string key
std::string World::getChunkKey(int chunkX, int chunkY) {
    return std::to_string(chunkX) + "," + std::to_string(chunkY);
}
