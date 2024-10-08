#include "World.h"
#include <iostream>
#include "GameMap.h"  // Include your map header to use mapMatrix

const int TILE_SIZE = 96;
const int TILE_SOURCE_SIZE = 32;

World::World(SDL_Renderer* p_renderer) : renderer(p_renderer), chunkSize(12), terminateThread(false) {
    generationThread = std::thread(&World::mapGenerationThread, this);
}

World::~World() {
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        terminateThread = true;
    }
    cv.notify_one();  // Notify the thread to stop
    if (generationThread.joinable()) {
        generationThread.join();
    }
}

void World::mapGenerationThread() {
    while (true) {
        std::pair<int, int> chunkCoords;
        {
            std::unique_lock<std::mutex> lock(chunkMutex);
            cv.wait(lock, [this] { return !chunkQueue.empty() || terminateThread; });
            if (terminateThread) break;
            chunkCoords = chunkQueue.front();
            chunkQueue.pop();
        }
        // Generate the chunk outside of the critical section
        generateChunk(chunkCoords.first, chunkCoords.second);
    }
}

void World::requestChunkGeneration(int chunkX, int chunkY) {
    std::lock_guard<std::mutex> lock(chunkMutex);
    chunkQueue.push({chunkX, chunkY});
    cv.notify_one();  // Notify the thread that there's a new chunk to generate
}

void World::generateChunk(int chunkX, int chunkY) {
    std::string key = getChunkKey(chunkX, chunkY);
    
    // Lock the mutex while modifying the chunks map
    {
        std::lock_guard<std::mutex> lock(chunkMutex);
        if (chunks.find(key) != chunks.end()) return;  // Chunk already exists
        chunks[key] = mapMatrix;  // For simplicity, assume mapMatrix is static for this example
    }
}

void World::update(float playerX, float playerY) {
    int centerX = static_cast<int>(playerX) / (chunkSize * TILE_SIZE);
    int centerY = static_cast<int>(playerY) / (chunkSize * TILE_SIZE);

    std::unordered_set<std::string> newVisibleChunks;

    for (int x = centerX - 2; x <= centerX + 2; ++x) {
        for (int y = centerY - 2; y <= centerY + 2; ++y) {
            std::string key = getChunkKey(x, y);
            if (chunks.find(key) == chunks.end()) {
                requestChunkGeneration(x, y);  // Request chunk generation in background
            }
            newVisibleChunks.insert(key);
        }
    }

    // Remove chunks that are no longer visible
    for (const auto& chunkKey : visibleChunks) {
        if (newVisibleChunks.find(chunkKey) == newVisibleChunks.end()) {
            chunks.erase(chunkKey);  // Despawn the chunk
        }
    }

    visibleChunks = newVisibleChunks;
}

void World::render(float playerX, float playerY, bool isPlayerInDungeon, SDL_Rect dungeonEntrance, const SDL_Rect& camera, SDL_Texture* dungeonEntranceTexture, SDL_Texture* tilesetTexture) {
    std::lock_guard<std::mutex> lock(chunkMutex);  // Protect access to chunks

    // Define shadow parameters
    const int shadowOffset = 15; // Offset for the shadow
    const Uint8 shadowAlpha = 128; // Shadow transparency

    // Calculate the world rendering area (shadow area)
    int worldWidth = mapMatrix[0].size() * TILE_SIZE;
    int worldHeight = mapMatrix.size() * TILE_SIZE;

    SDL_Rect shadowRect = {
        -camera.x + shadowOffset,
        -camera.y + shadowOffset,
        worldWidth,
        worldHeight
    };

    // Set the renderer color for the shadow (semi-transparent black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, shadowAlpha);

    // Render the shadow
    SDL_RenderFillRect(renderer, &shadowRect);

    // Render visible chunks
    for (const auto& [chunkKey, chunk] : chunks) {
        for (int y = 0; y < chunk.size(); ++y) {
            for (int x = 0; x < chunk[y].size(); ++x) {
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

                // Switch case handling different tile types in the chunk
                switch (chunk[y][x]) {
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
                    case TILE_GATE:
                        srcRect = {0, TILE_SOURCE_SIZE * 14 + 9, TILE_SOURCE_SIZE * 4, TILE_SOURCE_SIZE * 2};
                        destRect.x = (x - 3) * TILE_SIZE - camera.x; // Adjust x-position to include the left tile
                        destRect.y = (y - 1) * TILE_SIZE - camera.y;
                        destRect.w = TILE_SIZE * 4;
                        destRect.h = TILE_SIZE * 2;
                        break;
                    case TILE_CROSS:
                        srcRect = {crossIndex * TILE_SOURCE_SIZE, TILE_SOURCE_SIZE * 12 - 10, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE + 3};
                        crossIndex = (crossIndex + 1) % 4;
                        break;
                    case TILE_GRAVE:
                        srcRect = {graveIndex * TILE_SOURCE_SIZE + 6, TILE_SOURCE_SIZE * 13, TILE_SOURCE_SIZE + 4, TILE_SOURCE_SIZE + 3};
                        graveIndex = (graveIndex + 1) % 2;
                        break;
                    case TILE_COFFIN:
                        srcRect = {6, TILE_SOURCE_SIZE * 17 - 15, TILE_SOURCE_SIZE * 2, TILE_SOURCE_SIZE};
                        destRect.w = TILE_SIZE * 2;
                        break;
                    case TILE_BONE:
                        srcRect = {6, TILE_SOURCE_SIZE * 18 - 13, TILE_SOURCE_SIZE + 4, TILE_SOURCE_SIZE};
                        break;
                    case TILE_SKULL:
                        srcRect = {6, TILE_SOURCE_SIZE * 19 - 10, TILE_SOURCE_SIZE + 4, TILE_SOURCE_SIZE + 3};
                        break;
                    case TILE_HOUSE_GREEN:
                        srcRect = {TILE_SOURCE_SIZE * 6, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE * 4, TILE_SOURCE_SIZE * 2};
                        destRect.y = (y - 3) * TILE_SIZE - camera.y;
                        destRect.w = TILE_SIZE * 5;
                        destRect.h = TILE_SIZE * 3;
                        break;
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
