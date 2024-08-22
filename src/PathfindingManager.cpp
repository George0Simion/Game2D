#include "PathfindingManager.h"
#include "Player.h"  // Include full definition of Player
#include "Game.h"    // Include full definition of Game
#include "Enemy.h"   // Include full definition of Enemy
#include <queue>
#include <cmath>

const int CELL_SIZE = 96;
const int ENEMY_PADDING_X = 32;
const int ENEMY_PADDING_Y = 56;

std::vector<std::pair<int, int>> PathfindingManager::getSharedPathToPlayer(Player& player, Game& game, Enemy& enemy) {
    int gridKey = calculateGridKey(static_cast<int>(enemy.getX()), static_cast<int>(enemy.getY()));

    if (sharedPaths.find(gridKey) == sharedPaths.end()) {
        sharedPaths[gridKey] = calculateSharedPath(player, game, gridKey);
    }

    return sharedPaths[gridKey];
}

std::vector<std::pair<int, int>> PathfindingManager::calculateSharedPath(Player& player, Game& game, int gridKey) {
    auto& dungeonMaze = game.getDungeonMaze();
    int startX = gridKey % 100;
    int startY = gridKey / 100;
    int goalX = static_cast<int>(player.getX()) / CELL_SIZE;
    int goalY = static_cast<int>(player.getY()) / CELL_SIZE;

    std::vector<std::pair<int, int>> path;
    if (startX == goalX && startY == goalY) return path;

    std::priority_queue<std::tuple<int, int, int, int, std::vector<std::pair<int, int>>>> openSet;
    std::unordered_map<int, std::unordered_map<int, int>> costSoFar;
    openSet.emplace(0, startX, startY, 0, path);
    costSoFar[startX][startY] = 0;

    while (!openSet.empty()) {
        auto [priority, x, y, cost, path] = openSet.top();
        openSet.pop();

        if (x == goalX && y == goalY) return path;

        static const std::vector<std::pair<int, int>> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        for (const auto& [dx, dy] : directions) {
            int nextX = x + dx;
            int nextY = y + dy;
            int newCost = cost + 1;

            if (nextX >= 0 && nextX < dungeonMaze[0].size() && nextY >= 0 && nextY < dungeonMaze.size() && dungeonMaze[nextY][nextX] != -1) {
                if (!costSoFar.count(nextX) || !costSoFar[nextX].count(nextY) || newCost < costSoFar[nextX][nextY]) {
                    costSoFar[nextX][nextY] = newCost;
                    int priority = newCost + abs(nextX - goalX) + abs(nextY - goalY);
                    auto newPath = path;
                    newPath.emplace_back(nextX, nextY);
                    openSet.emplace(priority, nextX, nextY, newCost, newPath);
                }
            }
        }
    }

    return {};
}

int PathfindingManager::calculateGridKey(int x, int y) {
    // Adjusting the position with padding before calculating the grid key
    int adjustedX = (x + ENEMY_PADDING_X) / CELL_SIZE;
    int adjustedY = (y + ENEMY_PADDING_Y) / CELL_SIZE;
    return adjustedX + adjustedY * 100;
}

