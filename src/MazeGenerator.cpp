#include "MazeGenerator.h"
#include <random>

MazeGenerator::MazeGenerator(int width, int height) : width(width), height(height) {
    std::srand(std::time(0)); // Seed for random number generation
}

std::vector<std::vector<int>> MazeGenerator::generateMaze() {
    initializeMaze();
    carveMaze(1, 1);
    return maze;
}

void MazeGenerator::initializeMaze() {
    maze = std::vector<std::vector<int>>(height, std::vector<int>(width, -1));
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            maze[y][x] = 0;
        }
    }
}

void MazeGenerator::carveMaze(int x, int y) {
    static const int directions[4][2] = {
        { 0, -2 }, { 2, 0 }, { 0, 2 }, { -2, 0 }
    };

    std::stack<std::pair<int, int>> stack;
    stack.push({ x, y });

    while (!stack.empty()) {
        int cx, cy;
        std::tie(cx, cy) = stack.top();
        stack.pop();

        std::vector<int> dir_indices = { 0, 1, 2, 3 };
        std::shuffle(dir_indices.begin(), dir_indices.end(), std::default_random_engine(std::rand()));

        for (int i : dir_indices) {
            int nx = cx + directions[i][0];
            int ny = cy + directions[i][1];
            if (isInBounds(nx, ny) && maze[ny][nx] == -1) {
                maze[cy + directions[i][1] / 2][cx + directions[i][0] / 2] = 0;
                maze[ny][nx] = 0;
                stack.push({ nx, ny });
            }
        }
    }
}

bool MazeGenerator::isInBounds(int x, int y) {
    return x > 0 && x < width - 1 && y > 0 && y < height - 1;
}
