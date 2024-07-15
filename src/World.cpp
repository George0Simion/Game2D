#include "World.h"
#include <iostream>

World::World(SDL_Renderer* p_renderer, int seed) : renderer(p_renderer), chunkSize(12), noise() {
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

World::~World() {}

void World::generateChunk(int chunkX, int chunkY) {
    std::string key = getChunkKey(chunkX, chunkY);
    if (chunks.find(key) != chunks.end()) return; // Chunk already generated

    std::vector<std::vector<int>> chunk(chunkSize, std::vector<int>(chunkSize, 0));

    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            float nx = (chunkX * chunkSize + x) / 100.0;
            float ny = (chunkY * chunkSize + y) / 100.0;
            float noiseValue = noise.GetNoise(nx, ny);
            chunk[y][x] = static_cast<int>((noiseValue + 1) * 5); // Example terrain height
        }
    }

    chunks[key] = chunk;
}

void World::update(float playerX, float playerY) {
    int centerX = static_cast<int>(playerX) / (chunkSize * 64);
    int centerY = static_cast<int>(playerY) / (chunkSize * 64);
    
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

    std::vector<std::vector<int>>& chunk = chunks[key];
    SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255); // Pink color for the chunk
    SDL_Rect dstRect = {
        chunkX * chunkSize * 64 - static_cast<int>(playerX) + 840, // Center the player on the screen (assuming screen width is 1920)
        chunkY * chunkSize * 64 - static_cast<int>(playerY) + 450, // Center the player on the screen (assuming screen height is 1080)
        chunkSize * 64,
        chunkSize * 64
    };

    SDL_RenderFillRect(renderer, &dstRect);

    // Draw a border around the chunk
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for the border
    SDL_RenderDrawRect(renderer, &dstRect);
}

std::string World::getChunkKey(int chunkX, int chunkY) {
    return std::to_string(chunkX) + "," + std::to_string(chunkY);
}
