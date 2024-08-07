#include "MazeGenerator.h"
#include <random>

MazeGenerator::MazeGenerator(int width, int height) : width(width), height(height) {
    std::srand(std::time(0)); // Seed for random number generation
}

std::vector<std::vector<int>> MazeGenerator::generateMaze() {
    initializeMaze();
    carveMaze(1, 1);
    refineMaze();
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

void MazeGenerator::refineMaze() {
    std::vector<std::vector<int>> newMaze = maze;
    int directions[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

    // Ensure the margins of the maze are all -1
    for (int x = 0; x < width; ++x) {
        newMaze[0][x] = -1;
        newMaze[height - 1][x] = -1;
    }
    for (int y = 0; y < height; ++y) {
        newMaze[y][0] = -1;
        newMaze[y][width - 1] = -1;
    }

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            if (maze[y][x] == 0) {
                int wallCount = 0;
                for (const auto& dir : directions) {
                    int nx = x + dir[0];
                    int ny = y + dir[1];
                    if (maze[ny][nx] == -1) {
                        ++wallCount;
                    }
                }
                if (wallCount >= 3) {
                    newMaze[y][x] = -1;
                }
            }
        }
    }
    maze = newMaze;
}

bool MazeGenerator::isInBounds(int x, int y) {
    return x > 0 && x < width - 1 && y > 0 && y < height - 1;
}
