#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include <vector>
#include <stack>
#include <utility> // for std::pair
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <tuple>

class MazeGenerator {
public:
    MazeGenerator(int width, int height);
    std::vector<std::vector<int>> generateMaze();

private:
    int width;
    int height;
    std::vector<std::vector<int>> maze;
    void initializeMaze();
    void carveMaze(int x, int y);
    bool isInBounds(int x, int y);
};

#endif // MAZEGENERATOR_H
