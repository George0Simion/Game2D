#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include "Entity.h"
#include "json.hpp" // Include the JSON library

using json = nlohmann::json;

class World {
public:
    World(SDL_Renderer* renderer);
    ~World();

    void loadMap(const std::string& mapFile);
    void render(float playerX, float playerY);
    void update();

    void addEntity(Entity* entity);
    void removeEntity(Entity* entity);

private:
    SDL_Renderer* renderer;
    std::vector<Entity*> entities;

    // Chunk-related variables
    int chunkSize; // Size of each chunk in tiles
    std::unordered_map<std::string, std::vector<std::vector<int>>> chunks;

    // Tileset information
    void loadTileset(const std::string& tilesetFile);
    SDL_Texture* tileset;
    int tileWidth;
    int tileHeight;

    // Helper functions
    std::string getChunkKey(int chunkX, int chunkY);
    void loadChunk(int chunkX, int chunkY);
    void unloadChunk(int chunkX, int chunkY);
    void renderChunk(int chunkX, int chunkY, float playerX, float playerY);
};

#endif
