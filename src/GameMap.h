#include <vector>

const int TILE_GRASS = 0;            // Grass tile
const int TILE_PATH = 1;             // Path tile
const int TILE_HOUSE_BLUE = 2;       // Blue house tile
const int TILE_HOUSE_RED = 3;        // Red house tile
const int TILE_DUNGEON_ENTRANCE = 4; // Dungeon entrance tile
const int TILE_TREE = 5;             // Tree tile
const int TILE_FENCE = 6;            // Fence tile
const int TILE_BUSH = 7;             // Bush tile
const int TILE_CROSS = 8;            // Cross tile
const int TILE_GRAVE = 9;            // Grave tile
const int TILE_GATE = 10;            // Gate tile
const int TILE_COFFIN = 11;          // Coffin tile
const int TILE_BONE = 12;
const int TILE_SKULL = 13;
const int TILE_HOUSE_GREEN = 14;

// Example map matrix (can be loaded from a file for larger maps)
extern std::vector<std::vector<int>> mapMatrix;