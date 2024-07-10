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
)

# Include directories
target_include_directories(SimpleGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Find SDL2
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(SimpleGame ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()

# Find X11
find_package(X11 REQUIRED)
if (X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(SimpleGame ${X11_LIBRARIES})
else()
    message(FATAL_ERROR "X11 not found!")
endif()
