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
    src/LightingManager.cpp
    src/GameMap.cpp
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

# Manually specify the paths for SDL2_image and SDL2_ttf
# Replace these paths with the actual paths where the libraries are located

# SDL2_image
set(SDL2_IMAGE_INCLUDE_DIRS "/usr/include/SDL2")
set(SDL2_IMAGE_LIBRARIES "/usr/lib/x86_64-linux-gnu/libSDL2_image.so")

# SDL2_ttf
set(SDL2_TTF_INCLUDE_DIRS "/usr/include/SDL2")
set(SDL2_TTF_LIBRARIES "/usr/lib/x86_64-linux-gnu/libSDL2_ttf.so")

# Include directories for SDL2_image and SDL2_ttf
include_directories(${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# Link the SDL2_image and SDL2_ttf libraries
target_link_libraries(SimpleGame ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
