#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SDL2/SDL.h>
#include "Entity.h"

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

class World {
public:
    World(SDL_Renderer* p_renderer);
    ~World();
    void update(float playerX, float playerY);
    void render(float playerX, float playerY, bool isPlayerInDungeon, SDL_Rect dungeonEntrance, const SDL_Rect& camera, SDL_Texture* dungeonEntranceTexture, SDL_Texture* tilesetTexture);
    
private:
    void generateChunk(int chunkX, int chunkY);
    std::string getChunkKey(int chunkX, int chunkY);

    // Threading related members
    void mapGenerationThread(); // Function for the background thread
    void requestChunkGeneration(int chunkX, int chunkY); // Add chunk to queue

    std::thread generationThread;
    std::mutex chunkMutex;  // Mutex to protect access to chunks
    std::condition_variable cv;  // For thread notification
    std::queue<std::pair<int, int>> chunkQueue;  // Queue of chunks to generate
    bool terminateThread;  // Flag to stop the background thread

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
