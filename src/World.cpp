#include "World.h"
#include <fstream>
#include <iostream>

World::World(SDL_Renderer* p_renderer) : renderer(p_renderer), chunkSize(4), tileset(nullptr), tileWidth(32), tileHeight(32) {}

World::~World() {
    if (tileset) {
        SDL_DestroyTexture(tileset);
    }
}

void World::loadMap(const std::string& mapFile) {
    std::ifstream file(mapFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << mapFile << std::endl;
        return;
    }

    json mapJson;
    file >> mapJson;
    file.close();

    chunkSize = mapJson["chunkSize"];
    std::string tilesetFile = mapJson["tileset"];
    loadTileset(tilesetFile);

    for (auto& chunk : mapJson["chunks"].items()) {
        std::string key = chunk.key();
        std::vector<std::vector<int>> chunkData = chunk.value();
        chunks[key] = chunkData;
    }
}

void World::loadTileset(const std::string& tilesetFile) {
    std::cout << "Loading tileset from: " << tilesetFile << std::endl;
    tileset = IMG_LoadTexture(renderer, tilesetFile.c_str());
    if (!tileset) {
        std::cerr << "Failed to load tileset: " << tilesetFile << " Error: " << IMG_GetError() << std::endl;
    }
}

void World::render(float cameraX, float cameraY) {
    int chunkX = static_cast<int>(cameraX) / (chunkSize * tileWidth);
    int chunkY = static_cast<int>(cameraY) / (chunkSize * tileHeight);

    for (int x = chunkX - 1; x <= chunkX + 1; ++x) {
        for (int y = chunkY - 1; y <= chunkY + 1; ++y) {
            renderChunk(x, y, cameraX, cameraY);
        }
    }
}

void World::renderChunk(int chunkX, int chunkY, float cameraX, float cameraY) {
    std::string key = getChunkKey(chunkX, chunkY);
    if (chunks.find(key) == chunks.end()) {
        return;
    }

    std::vector<std::vector<int>>& chunk = chunks[key];
    for (int i = 0; i < chunkSize; ++i) {
        for (int j = 0; j < chunkSize; ++j) {
            int tileType = chunk[i][j];
            if (tileType < 0) continue;  // Skip invalid tile types
            
            // Calculate the source rectangle for the tile in the tileset
            int tilesetCols = 8;  // Assuming the tileset image has 8 columns of tiles
            SDL_Rect srcRect = {
                (tileType % tilesetCols) * tileWidth,
                (tileType / tilesetCols) * tileHeight,
                tileWidth,
                tileHeight
            };

            SDL_Rect dstRect = {
                (chunkX * chunkSize + i) * tileWidth - static_cast<int>(cameraX),
                (chunkY * chunkSize + j) * tileHeight - static_cast<int>(cameraY),
                tileWidth,
                tileHeight
            };

            SDL_RenderCopy(renderer, tileset, &srcRect, &dstRect);
        }
    }
}


std::string World::getChunkKey(int chunkX, int chunkY) {
    return std::to_string(chunkX) + "," + std::to_string(chunkY);
}
