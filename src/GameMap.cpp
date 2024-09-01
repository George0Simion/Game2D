#include <GameMap.h>

std::vector<std::vector<int>> mapMatrix = {
    {0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {0, 6, 8, 0, 11, 0, 9, 8, 0, 0, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 7, 6},
    {0, 6, 0, 8, 0, 0, 0, 0, 8, 0, 12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 7, 6},
    {0, 6, 0, 0, 9, 0, 0, 0, 0, 8, 12, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 0, 0, 5, 0, 5, 6},
    {0, 6, 0, 8, 0, 8, 0, 0, 0, 0, 13, 1, 1, 6, 13, 0, 0, 0, 0, 0, 12, 6, 1, 1, 0, 0, 0, 0, 0, 6},
    {0, 6, 0, 0, 0, 0, 11, 0, 0, 8, 12, 1, 1, 6, 12, 0, 0, 0, 0, 0, 13, 6, 1, 1, 0, 5, 0, 0, 5, 6},
    {0, 6, 8, 0, 0, 0, 8, 0, 9, 0, 12, 1, 1, 6, 8, 0, 0, 0, 0, 0, 8, 6, 1, 1, 0, 0, 0, 5, 0, 6},
    {0, 6, 0, 0, 11, 0, 0, 0, 0, 0, 9, 1, 1, 6, 0, 0, 0, 4, 0, 0, 0, 6, 1, 1, 0, 0, 5, 0, 5, 6},
    {0, 6, 9, 8, 0, 0, 9, 0, 8, 0, 0, 1, 1, 6, 0, 0, 0, 0, 0, 0, 0, 6, 1, 1, 0, 0, 0, 0, 0, 6},
    {0, 6, 0, 0, 0, 0, 0, 0, 0, 13, 0, 1, 1, 6, 0, 0, 5, 0, 0, 0, 5, 6, 1, 1, 0, 5, 0, 5, 0, 6},
    {0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 6, 6, 6, 1, 1, 1, 6, 6, 6, 1, 1, 0, 0, 0, 0, 5, 6},
    {0, 6, 8, 0, 0, 0, 0, 10, 0, 8, 8, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 0, 0, 5, 0, 0, 6},
    {0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 0, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 0, 0, 0, 5, 0, 6},
    {0, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 0, 0, 5, 0, 5, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 6, 6, 6, 6, 6, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 7, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 7, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 7, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 7, 7, 3, 0, 0, 7, 7, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 7, 14, 0, 0, 0, 6},
    {0, 6, 1, 1, 7, 7, 0, 0, 0, 7, 7, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 1, 1, 1, 1, 1, 6},
    {0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 1, 1, 1, 1, 1, 6},
    {0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 6, 6, 6, 1, 1, 6},
    {0, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 6, 13, 6, 1, 1, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 6, 0, 6, 1, 1, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 6, 0, 6, 1, 1, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 0, 7, 1, 1, 1, 7, 0, 6, 1, 1, 6, 0, 6, 1, 1, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 6, 1, 1, 6, 8, 7, 1, 1, 1, 7, 8, 6, 1, 1, 1, 1, 1, 1, 1, 6},
    {0, 6, 1, 1, 7, 7, 2, 0, 0, 7, 7, 1, 1, 6, 6, 6, 1, 1, 1, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 6},
    {0, 6, 1, 1, 7, 7, 0, 0, 0, 7, 7, 1, 1, 0, 0, 6, 1, 1, 1, 6, 0, 0, 1, 1, 6, 6, 6, 6, 6, 6},
    {0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 6, 7, 7, 0, 0, 0, 7, 7, 0, 0, 0, 7, 7, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 5, 6},
    {0, 6, 1, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
    {0, 6, 1, 1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6}
};