# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/rb/xerces-c-3.2.3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rb/xerces-c-3.2.3/bld

# Include any dependencies generated for this target.
include tests/CMakeFiles/XSTSHarness.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/XSTSHarness.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/XSTSHarness.dir/flags.make

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.o: tests/CMakeFiles/XSTSHarness.dir/flags.make
tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.o: ../tests/src/XSTSHarness/XSTSHarness.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.o"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.o -c /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XSTSHarness.cpp

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.i"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XSTSHarness.cpp > CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.i

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.s"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XSTSHarness.cpp -o CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.s

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.o: tests/CMakeFiles/XSTSHarness.dir/flags.make
tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.o: ../tests/src/XSTSHarness/XSTSHarnessHandlers.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.o"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.o -c /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XSTSHarnessHandlers.cpp

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.i"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XSTSHarnessHandlers.cpp > CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.i

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.s"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XSTSHarnessHandlers.cpp -o CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.s

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.o: tests/CMakeFiles/XSTSHarness.dir/flags.make
tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.o: ../tests/src/XSTSHarness/XMLHarnessHandlers.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.o"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.o -c /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XMLHarnessHandlers.cpp

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.i"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XMLHarnessHandlers.cpp > CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.i

tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.s"
	cd /home/rb/xerces-c-3.2.3/bld/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/xerces-c-3.2.3/tests/src/XSTSHarness/XMLHarnessHandlers.cpp -o CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.s

# Object files for target XSTSHarness
XSTSHarness_OBJECTS = \
"CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.o" \
"CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.o" \
"CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.o"

# External object files for target XSTSHarness
XSTSHarness_EXTERNAL_OBJECTS =

tests/XSTSHarness: tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarness.cpp.o
tests/XSTSHarness: tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XSTSHarnessHandlers.cpp.o
tests/XSTSHarness: tests/CMakeFiles/XSTSHarness.dir/src/XSTSHarness/XMLHarnessHandlers.cpp.o
tests/XSTSHarness: tests/CMakeFiles/XSTSHarness.dir/build.make
tests/XSTSHarness: src/libxerces-c-3.2.so
tests/XSTSHarness: /usr/lib/aarch64-linux-gnu/libnsl.so
tests/XSTSHarness: tests/CMakeFiles/XSTSHarness.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable XSTSHarness"
	cd /home/rb/xerces-c-3.2.3/bld/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/XSTSHarness.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/XSTSHarness.dir/build: tests/XSTSHarness

.PHONY : tests/CMakeFiles/XSTSHarness.dir/build

tests/CMakeFiles/XSTSHarness.dir/clean:
	cd /home/rb/xerces-c-3.2.3/bld/tests && $(CMAKE_COMMAND) -P CMakeFiles/XSTSHarness.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/XSTSHarness.dir/clean

tests/CMakeFiles/XSTSHarness.dir/depend:
	cd /home/rb/xerces-c-3.2.3/bld && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rb/xerces-c-3.2.3 /home/rb/xerces-c-3.2.3/tests /home/rb/xerces-c-3.2.3/bld /home/rb/xerces-c-3.2.3/bld/tests /home/rb/xerces-c-3.2.3/bld/tests/CMakeFiles/XSTSHarness.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/XSTSHarness.dir/depend

