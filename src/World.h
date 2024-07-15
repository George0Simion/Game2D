#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SDL2/SDL.h>
#include "Entity.h"
#include "FastNoiseLite.h"

class World {
public:
    World(SDL_Renderer* p_renderer, int seed);
    ~World();
    void update(float playerX, float playerY);
    void render(float playerX, float playerY);
    
private:
    void generateChunk(int chunkX, int chunkY);
    void renderChunk(int chunkX, int chunkY, float playerX, float playerY);
    void despawnChunks(float playerX, float playerY);
    std::string getChunkKey(int chunkX, int chunkY);
    
    SDL_Renderer* renderer;
    int chunkSize;
    FastNoiseLite noise;
    std::unordered_map<std::string, std::vector<std::vector<int>>> chunks;
    std::unordered_set<std::string> visibleChunks; // Keep track of visible chunks
};

#endif // WORLD_H
