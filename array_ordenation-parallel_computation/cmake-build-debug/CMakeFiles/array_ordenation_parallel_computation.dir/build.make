# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /snap/clion/107/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/107/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pedro/CLionProjects/array_ordenation-parallel_computation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/array_ordenation_parallel_computation.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/array_ordenation_parallel_computation.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/array_ordenation_parallel_computation.dir/flags.make

CMakeFiles/array_ordenation_parallel_computation.dir/main.c.o: CMakeFiles/array_ordenation_parallel_computation.dir/flags.make
CMakeFiles/array_ordenation_parallel_computation.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/array_ordenation_parallel_computation.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/array_ordenation_parallel_computation.dir/main.c.o   -c /home/pedro/CLionProjects/array_ordenation-parallel_computation/main.c

CMakeFiles/array_ordenation_parallel_computation.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/array_ordenation_parallel_computation.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pedro/CLionProjects/array_ordenation-parallel_computation/main.c > CMakeFiles/array_ordenation_parallel_computation.dir/main.c.i

CMakeFiles/array_ordenation_parallel_computation.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/array_ordenation_parallel_computation.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pedro/CLionProjects/array_ordenation-parallel_computation/main.c -o CMakeFiles/array_ordenation_parallel_computation.dir/main.c.s

# Object files for target array_ordenation_parallel_computation
array_ordenation_parallel_computation_OBJECTS = \
"CMakeFiles/array_ordenation_parallel_computation.dir/main.c.o"

# External object files for target array_ordenation_parallel_computation
array_ordenation_parallel_computation_EXTERNAL_OBJECTS =

array_ordenation_parallel_computation: CMakeFiles/array_ordenation_parallel_computation.dir/main.c.o
array_ordenation_parallel_computation: CMakeFiles/array_ordenation_parallel_computation.dir/build.make
array_ordenation_parallel_computation: CMakeFiles/array_ordenation_parallel_computation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable array_ordenation_parallel_computation"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/array_ordenation_parallel_computation.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/array_ordenation_parallel_computation.dir/build: array_ordenation_parallel_computation

.PHONY : CMakeFiles/array_ordenation_parallel_computation.dir/build

CMakeFiles/array_ordenation_parallel_computation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/array_ordenation_parallel_computation.dir/cmake_clean.cmake
.PHONY : CMakeFiles/array_ordenation_parallel_computation.dir/clean

CMakeFiles/array_ordenation_parallel_computation.dir/depend:
	cd /home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pedro/CLionProjects/array_ordenation-parallel_computation /home/pedro/CLionProjects/array_ordenation-parallel_computation /home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug /home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug /home/pedro/CLionProjects/array_ordenation-parallel_computation/cmake-build-debug/CMakeFiles/array_ordenation_parallel_computation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/array_ordenation_parallel_computation.dir/depend

