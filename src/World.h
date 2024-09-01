#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SDL2/SDL.h>
#include "Entity.h"

class World {
public:
    World(SDL_Renderer* p_renderer);
    ~World();
    void update(float playerX, float playerY);
    void render(float playerX, float playerY, bool isPlayerInDungeon, SDL_Rect dungeonEntrance, const SDL_Rect& camera, SDL_Texture* dungeonEntranceTexture, SDL_Texture* tilesetTexture);
    
private:
    void generateChunk(int chunkX, int chunkY);
    std::string getChunkKey(int chunkX, int chunkY);
    SDL_Rect getPathTileSourceRect(int x, int y);
    SDL_Rect getFenceTileSourceRect(int x, int y);
    int bushIndex;
    int crossIndex;
    int graveIndex;
    
    SDL_Renderer* renderer;
    int chunkSize;
    std::unordered_map<std::string, std::vector<std::vector<int>>> chunks;
    std::unordered_set<std::string> visibleChunks; // Keep track of visible chunks
};

#endif // WORLD_H
