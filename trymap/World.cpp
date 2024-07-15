#include "World.h"
#include <iostream>
#include "FastNoiseLite.h"  // Ensure the correct inclusion of FastNoiseLite

World::World(SDL_Renderer* p_renderer, int seed) : renderer(p_renderer), noise() {
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02); // Lower frequency to generate larger features
}

World::~World() {}

std::string World::getChunkKey(int chunkX, int chunkY) {
    return std::to_string(chunkX) + "," + std::to_string(chunkY);
}

void World::generateChunk(int chunkX, int chunkY) {
    std::string key = getChunkKey(chunkX, chunkY);
    if (chunks.find(key) != chunks.end()) return; // Chunk already generated

    Chunk chunk;
    for (int y = 0; y < CHUNK_SIZE; ++y) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            float nx = (chunkX * CHUNK_SIZE + x) / 100.0f;
            float ny = (chunkY * CHUNK_SIZE + y) / 100.0f;
            float noiseValue = noise.GetNoise(nx, ny);

            // Remap noiseValue from [min, max] to [-1, 1]
            float remappedNoiseValue = 2 * (noiseValue - 0.005) / (0.016 - 0.005) - 1;

            std::cout << "Remapped Noise Value: " << remappedNoiseValue << std::endl;

            if (remappedNoiseValue < -0.5f) {
                chunk.tiles[y][x] = WATER;
            } else if (remappedNoiseValue < 0.5f) {
                chunk.tiles[y][x] = GRASS;
            } else {
                chunk.tiles[y][x] = MOUNTAIN;
            }
        }
    }
    chunks[key] = chunk;
}

void World::update(float playerX, float playerY) {
    int centerX = static_cast<int>(playerX) / (CHUNK_SIZE * TILE_SIZE);
    int centerY = static_cast<int>(playerY) / (CHUNK_SIZE * TILE_SIZE);
    
    std::unordered_set<std::string> newVisibleChunks;
    
    for (int x = centerX - 2; x <= centerX + 2; ++x) {
        for (int y = centerY - 2; y <= centerY + 2; ++y) {
            std::string key = getChunkKey(x, y);
            generateChunk(x, y); // Ensure chunk is generated
            newVisibleChunks.insert(key);
        }
    }
    
    // Despawn chunks that are no longer visible
    for (const auto& chunkKey : visibleChunks) {
        if (newVisibleChunks.find(chunkKey) == newVisibleChunks.end()) {
            chunks.erase(chunkKey); // Despawn the chunk
        }
    }
    
    visibleChunks = newVisibleChunks;
}

void World::render(float playerX, float playerY) {
    for (const auto& chunkKey : visibleChunks) {
        int delimiterPos = chunkKey.find(",");
        int chunkX = std::stoi(chunkKey.substr(0, delimiterPos));
        int chunkY = std::stoi(chunkKey.substr(delimiterPos + 1));
        renderChunk(chunkX, chunkY, playerX, playerY);
    }
}

void World::renderChunk(int chunkX, int chunkY, float playerX, float playerY) {
    std::string key = getChunkKey(chunkX, chunkY);
    if (chunks.find(key) == chunks.end()) {
        return;
    }

    Chunk& chunk = chunks[key];
    for (int y = 0; y < CHUNK_SIZE; ++y) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            TileType type = chunk.tiles[y][x];
            SDL_Rect destRect = {
                chunkX * CHUNK_SIZE * TILE_SIZE + x * TILE_SIZE - static_cast<int>(playerX) + 840,
                chunkY * CHUNK_SIZE * TILE_SIZE + y * TILE_SIZE - static_cast<int>(playerY) + 450,
                TILE_SIZE,
                TILE_SIZE
            };

            switch (type) {
                case WATER:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for water
                    break;
                case GRASS:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for grass
                    break;
                case MOUNTAIN:
                    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brown for mountains
                    break;
            }
            SDL_RenderFillRect(renderer, &destRect);
        }
    }
}
