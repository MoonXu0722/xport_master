# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xpy/xport

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xpy/xport

# Include any dependencies generated for this target.
include example/CMakeFiles/simple_demo.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/simple_demo.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/simple_demo.dir/flags.make

example/CMakeFiles/simple_demo.dir/simple.cpp.o: example/CMakeFiles/simple_demo.dir/flags.make
example/CMakeFiles/simple_demo.dir/simple.cpp.o: example/simple.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xpy/xport/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/simple_demo.dir/simple.cpp.o"
	cd /home/xpy/xport/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/simple_demo.dir/simple.cpp.o -c /home/xpy/xport/example/simple.cpp

example/CMakeFiles/simple_demo.dir/simple.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simple_demo.dir/simple.cpp.i"
	cd /home/xpy/xport/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xpy/xport/example/simple.cpp > CMakeFiles/simple_demo.dir/simple.cpp.i

example/CMakeFiles/simple_demo.dir/simple.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simple_demo.dir/simple.cpp.s"
	cd /home/xpy/xport/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xpy/xport/example/simple.cpp -o CMakeFiles/simple_demo.dir/simple.cpp.s

# Object files for target simple_demo
simple_demo_OBJECTS = \
"CMakeFiles/simple_demo.dir/simple.cpp.o"

# External object files for target simple_demo
simple_demo_EXTERNAL_OBJECTS =

example/simple_demo: example/CMakeFiles/simple_demo.dir/simple.cpp.o
example/simple_demo: example/CMakeFiles/simple_demo.dir/build.make
example/simple_demo: src/libxport.a
example/simple_demo: example/CMakeFiles/simple_demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/xpy/xport/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable simple_demo"
	cd /home/xpy/xport/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/simple_demo.dir/build: example/simple_demo

.PHONY : example/CMakeFiles/simple_demo.dir/build

example/CMakeFiles/simple_demo.dir/clean:
	cd /home/xpy/xport/example && $(CMAKE_COMMAND) -P CMakeFiles/simple_demo.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/simple_demo.dir/clean

example/CMakeFiles/simple_demo.dir/depend:
	cd /home/xpy/xport && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xpy/xport /home/xpy/xport/example /home/xpy/xport /home/xpy/xport/example /home/xpy/xport/example/CMakeFiles/simple_demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/simple_demo.dir/depend

