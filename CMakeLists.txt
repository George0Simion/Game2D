cmake_minimum_required(VERSION 3.10)
project(SimpleGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(SimpleGame
    src/main.cpp
    src/Game.cpp
    src/Entity.cpp
    src/Menu.cpp
    src/World.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/MazeGenerator.cpp
    src/PathfindingManager.cpp
)

# Include directories
target_include_directories(SimpleGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find SDL2
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(SimpleGame ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()

# Manually specify SDL2_image and SDL2_ttf paths
include_directories(/usr/include/SDL2)
target_link_libraries(SimpleGame /usr/lib/x86_64-linux-gnu/libSDL2_image.so)
target_link_libraries(SimpleGame /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so)
