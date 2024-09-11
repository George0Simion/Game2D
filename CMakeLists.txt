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

# Find SDL2_image and SDL2_ttf libraries
find_library(SDL2_IMAGE_LIBRARIES NAMES SDL2_image sdl2_image)
find_library(SDL2_TTF_LIBRARIES NAMES SDL2_ttf sdl2_ttf)

if (SDL2_IMAGE_LIBRARIES)
    message(STATUS "SDL2_image found: ${SDL2_IMAGE_LIBRARIES}")
    target_link_libraries(SimpleGame ${SDL2_IMAGE_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2_image not found!")
endif()

if (SDL2_TTF_LIBRARIES)
    message(STATUS "SDL2_ttf found: ${SDL2_TTF_LIBRARIES}")
    target_link_libraries(SimpleGame ${SDL2_TTF_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2_ttf not found!")
endif()

# Find include directories for SDL2_image and SDL2_ttf
find_path(SDL2_IMAGE_INCLUDE_DIRS SDL_image.h HINTS /usr/include/SDL2)
find_path(SDL2_TTF_INCLUDE_DIRS SDL_ttf.h HINTS /usr/include/SDL2)

if (SDL2_IMAGE_INCLUDE_DIRS AND SDL2_TTF_INCLUDE_DIRS)
    message(STATUS "SDL2_image includes found: ${SDL2_IMAGE_INCLUDE_DIRS}")
    message(STATUS "SDL2_ttf includes found: ${SDL2_TTF_INCLUDE_DIRS}")
    include_directories(${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2_image or SDL2_ttf include directories not found!")
endif()
