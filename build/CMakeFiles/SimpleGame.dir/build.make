# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/simion/Desktop/5/Game2D

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/simion/Desktop/5/Game2D/build

# Include any dependencies generated for this target.
include CMakeFiles/SimpleGame.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/SimpleGame.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/SimpleGame.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SimpleGame.dir/flags.make

CMakeFiles/SimpleGame.dir/src/main.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/SimpleGame.dir/src/main.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SimpleGame.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/main.cpp.o -MF CMakeFiles/SimpleGame.dir/src/main.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/main.cpp.o -c /home/simion/Desktop/5/Game2D/src/main.cpp

CMakeFiles/SimpleGame.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/main.cpp > CMakeFiles/SimpleGame.dir/src/main.cpp.i

CMakeFiles/SimpleGame.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/main.cpp -o CMakeFiles/SimpleGame.dir/src/main.cpp.s

CMakeFiles/SimpleGame.dir/src/Game.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/Game.cpp.o: ../src/Game.cpp
CMakeFiles/SimpleGame.dir/src/Game.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/SimpleGame.dir/src/Game.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/Game.cpp.o -MF CMakeFiles/SimpleGame.dir/src/Game.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/Game.cpp.o -c /home/simion/Desktop/5/Game2D/src/Game.cpp

CMakeFiles/SimpleGame.dir/src/Game.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/Game.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/Game.cpp > CMakeFiles/SimpleGame.dir/src/Game.cpp.i

CMakeFiles/SimpleGame.dir/src/Game.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/Game.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/Game.cpp -o CMakeFiles/SimpleGame.dir/src/Game.cpp.s

CMakeFiles/SimpleGame.dir/src/Entity.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/Entity.cpp.o: ../src/Entity.cpp
CMakeFiles/SimpleGame.dir/src/Entity.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/SimpleGame.dir/src/Entity.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/Entity.cpp.o -MF CMakeFiles/SimpleGame.dir/src/Entity.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/Entity.cpp.o -c /home/simion/Desktop/5/Game2D/src/Entity.cpp

CMakeFiles/SimpleGame.dir/src/Entity.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/Entity.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/Entity.cpp > CMakeFiles/SimpleGame.dir/src/Entity.cpp.i

CMakeFiles/SimpleGame.dir/src/Entity.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/Entity.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/Entity.cpp -o CMakeFiles/SimpleGame.dir/src/Entity.cpp.s

CMakeFiles/SimpleGame.dir/src/Menu.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/Menu.cpp.o: ../src/Menu.cpp
CMakeFiles/SimpleGame.dir/src/Menu.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/SimpleGame.dir/src/Menu.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/Menu.cpp.o -MF CMakeFiles/SimpleGame.dir/src/Menu.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/Menu.cpp.o -c /home/simion/Desktop/5/Game2D/src/Menu.cpp

CMakeFiles/SimpleGame.dir/src/Menu.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/Menu.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/Menu.cpp > CMakeFiles/SimpleGame.dir/src/Menu.cpp.i

CMakeFiles/SimpleGame.dir/src/Menu.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/Menu.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/Menu.cpp -o CMakeFiles/SimpleGame.dir/src/Menu.cpp.s

CMakeFiles/SimpleGame.dir/src/World.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/World.cpp.o: ../src/World.cpp
CMakeFiles/SimpleGame.dir/src/World.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/SimpleGame.dir/src/World.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/World.cpp.o -MF CMakeFiles/SimpleGame.dir/src/World.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/World.cpp.o -c /home/simion/Desktop/5/Game2D/src/World.cpp

CMakeFiles/SimpleGame.dir/src/World.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/World.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/World.cpp > CMakeFiles/SimpleGame.dir/src/World.cpp.i

CMakeFiles/SimpleGame.dir/src/World.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/World.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/World.cpp -o CMakeFiles/SimpleGame.dir/src/World.cpp.s

CMakeFiles/SimpleGame.dir/src/Player.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/Player.cpp.o: ../src/Player.cpp
CMakeFiles/SimpleGame.dir/src/Player.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/SimpleGame.dir/src/Player.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/Player.cpp.o -MF CMakeFiles/SimpleGame.dir/src/Player.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/Player.cpp.o -c /home/simion/Desktop/5/Game2D/src/Player.cpp

CMakeFiles/SimpleGame.dir/src/Player.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/Player.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/Player.cpp > CMakeFiles/SimpleGame.dir/src/Player.cpp.i

CMakeFiles/SimpleGame.dir/src/Player.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/Player.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/Player.cpp -o CMakeFiles/SimpleGame.dir/src/Player.cpp.s

CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o: ../src/Enemy.cpp
CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o -MF CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o -c /home/simion/Desktop/5/Game2D/src/Enemy.cpp

CMakeFiles/SimpleGame.dir/src/Enemy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/Enemy.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/Enemy.cpp > CMakeFiles/SimpleGame.dir/src/Enemy.cpp.i

CMakeFiles/SimpleGame.dir/src/Enemy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/Enemy.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/Enemy.cpp -o CMakeFiles/SimpleGame.dir/src/Enemy.cpp.s

CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o: ../src/MazeGenerator.cpp
CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o -MF CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o -c /home/simion/Desktop/5/Game2D/src/MazeGenerator.cpp

CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/MazeGenerator.cpp > CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.i

CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/MazeGenerator.cpp -o CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.s

CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o: ../src/PathfindingManager.cpp
CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o -MF CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o -c /home/simion/Desktop/5/Game2D/src/PathfindingManager.cpp

CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/PathfindingManager.cpp > CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.i

CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/PathfindingManager.cpp -o CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.s

CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o: CMakeFiles/SimpleGame.dir/flags.make
CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o: ../src/LightingManager.cpp
CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o: CMakeFiles/SimpleGame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o -MF CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o.d -o CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o -c /home/simion/Desktop/5/Game2D/src/LightingManager.cpp

CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/simion/Desktop/5/Game2D/src/LightingManager.cpp > CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.i

CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/simion/Desktop/5/Game2D/src/LightingManager.cpp -o CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.s

# Object files for target SimpleGame
SimpleGame_OBJECTS = \
"CMakeFiles/SimpleGame.dir/src/main.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/Game.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/Entity.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/Menu.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/World.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/Player.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o" \
"CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o"

# External object files for target SimpleGame
SimpleGame_EXTERNAL_OBJECTS =

SimpleGame: CMakeFiles/SimpleGame.dir/src/main.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/Game.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/Entity.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/Menu.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/World.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/Player.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/Enemy.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/MazeGenerator.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/PathfindingManager.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/src/LightingManager.cpp.o
SimpleGame: CMakeFiles/SimpleGame.dir/build.make
SimpleGame: /usr/lib/x86_64-linux-gnu/libSDL2_image.so
SimpleGame: /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so
SimpleGame: CMakeFiles/SimpleGame.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/simion/Desktop/5/Game2D/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable SimpleGame"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SimpleGame.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SimpleGame.dir/build: SimpleGame
.PHONY : CMakeFiles/SimpleGame.dir/build

CMakeFiles/SimpleGame.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SimpleGame.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SimpleGame.dir/clean

CMakeFiles/SimpleGame.dir/depend:
	cd /home/simion/Desktop/5/Game2D/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/simion/Desktop/5/Game2D /home/simion/Desktop/5/Game2D /home/simion/Desktop/5/Game2D/build /home/simion/Desktop/5/Game2D/build /home/simion/Desktop/5/Game2D/build/CMakeFiles/SimpleGame.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SimpleGame.dir/depend

