# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/neo/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/191.7479.33/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/neo/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/191.7479.33/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/neo/CLionProjects/epl421_as4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/neo/CLionProjects/epl421_as4/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/epl421_as4.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/epl421_as4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/epl421_as4.dir/flags.make

CMakeFiles/epl421_as4.dir/server.c.o: CMakeFiles/epl421_as4.dir/flags.make
CMakeFiles/epl421_as4.dir/server.c.o: ../server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/neo/CLionProjects/epl421_as4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/epl421_as4.dir/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/epl421_as4.dir/server.c.o   -c /home/neo/CLionProjects/epl421_as4/server.c

CMakeFiles/epl421_as4.dir/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/epl421_as4.dir/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/neo/CLionProjects/epl421_as4/server.c > CMakeFiles/epl421_as4.dir/server.c.i

CMakeFiles/epl421_as4.dir/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/epl421_as4.dir/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/neo/CLionProjects/epl421_as4/server.c -o CMakeFiles/epl421_as4.dir/server.c.s

# Object files for target epl421_as4
epl421_as4_OBJECTS = \
"CMakeFiles/epl421_as4.dir/server.c.o"

# External object files for target epl421_as4
epl421_as4_EXTERNAL_OBJECTS =

epl421_as4: CMakeFiles/epl421_as4.dir/server.c.o
epl421_as4: CMakeFiles/epl421_as4.dir/build.make
epl421_as4: CMakeFiles/epl421_as4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/neo/CLionProjects/epl421_as4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable epl421_as4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/epl421_as4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/epl421_as4.dir/build: epl421_as4

.PHONY : CMakeFiles/epl421_as4.dir/build

CMakeFiles/epl421_as4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/epl421_as4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/epl421_as4.dir/clean

CMakeFiles/epl421_as4.dir/depend:
	cd /home/neo/CLionProjects/epl421_as4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/neo/CLionProjects/epl421_as4 /home/neo/CLionProjects/epl421_as4 /home/neo/CLionProjects/epl421_as4/cmake-build-debug /home/neo/CLionProjects/epl421_as4/cmake-build-debug /home/neo/CLionProjects/epl421_as4/cmake-build-debug/CMakeFiles/epl421_as4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/epl421_as4.dir/depend

