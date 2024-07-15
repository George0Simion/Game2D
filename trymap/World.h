#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SDL2/SDL.h>
#include "Entity.h"
#include "FastNoiseLite.h"

const int TILE_SIZE = 64;
const int CHUNK_SIZE = 12;

enum TileType {
    WATER,
    GRASS,
    MOUNTAIN,
    TILE_TYPE_COUNT
};

struct Chunk {
    std::vector<std::vector<TileType>> tiles;
    Chunk() : tiles(CHUNK_SIZE, std::vector<TileType>(CHUNK_SIZE, GRASS)) {}
};

class World {
public:
    World(SDL_Renderer* p_renderer, int seed);
    ~World();
    void update(float playerX, float playerY);
    void render(float playerX, float playerY);
    
private:
    void generateChunk(int chunkX, int chunkY);
    void renderChunk(int chunkX, int chunkY, float playerX, float playerY);
    std::string getChunkKey(int chunkX, int chunkY);
    
    SDL_Renderer* renderer;
    FastNoiseLite noise;
    std::unordered_map<std::string, Chunk> chunks;
    std::unordered_set<std::string> visibleChunks; // Keep track of visible chunks
};

#endif // WORLD_H
