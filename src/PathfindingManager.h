#ifndef PATHFINDING_MANAGER_H
#define PATHFINDING_MANAGER_H

#include <vector>
#include <unordered_map>
#include <utility>

class Player; // Forward declaration
class Game; // Forward declaration
class Enemy; // Forward declaration

class PathfindingManager {
public:
    std::vector<std::pair<int, int>> getSharedPathToPlayer(Player& player, Game& game, Enemy& enemy);

private:
    std::unordered_map<int, std::vector<std::pair<int, int>>> sharedPaths;
    std::vector<std::pair<int, int>> calculateSharedPath(Player& player, Game& game, int gridKey);

    int calculateGridKey(int x, int y);
};

#endif
