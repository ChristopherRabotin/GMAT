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
include samples/CMakeFiles/StdInParse.dir/depend.make

# Include the progress variables for this target.
include samples/CMakeFiles/StdInParse.dir/progress.make

# Include the compile flags for this target's objects.
include samples/CMakeFiles/StdInParse.dir/flags.make

samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.o: samples/CMakeFiles/StdInParse.dir/flags.make
samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.o: ../samples/src/StdInParse/StdInParse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.o"
	cd /home/rb/xerces-c-3.2.3/bld/samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.o -c /home/rb/xerces-c-3.2.3/samples/src/StdInParse/StdInParse.cpp

samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.i"
	cd /home/rb/xerces-c-3.2.3/bld/samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/xerces-c-3.2.3/samples/src/StdInParse/StdInParse.cpp > CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.i

samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.s"
	cd /home/rb/xerces-c-3.2.3/bld/samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/xerces-c-3.2.3/samples/src/StdInParse/StdInParse.cpp -o CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.s

samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.o: samples/CMakeFiles/StdInParse.dir/flags.make
samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.o: ../samples/src/StdInParse/StdInParseHandlers.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.o"
	cd /home/rb/xerces-c-3.2.3/bld/samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.o -c /home/rb/xerces-c-3.2.3/samples/src/StdInParse/StdInParseHandlers.cpp

samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.i"
	cd /home/rb/xerces-c-3.2.3/bld/samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/xerces-c-3.2.3/samples/src/StdInParse/StdInParseHandlers.cpp > CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.i

samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.s"
	cd /home/rb/xerces-c-3.2.3/bld/samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/xerces-c-3.2.3/samples/src/StdInParse/StdInParseHandlers.cpp -o CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.s

# Object files for target StdInParse
StdInParse_OBJECTS = \
"CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.o" \
"CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.o"

# External object files for target StdInParse
StdInParse_EXTERNAL_OBJECTS =

samples/StdInParse: samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParse.cpp.o
samples/StdInParse: samples/CMakeFiles/StdInParse.dir/src/StdInParse/StdInParseHandlers.cpp.o
samples/StdInParse: samples/CMakeFiles/StdInParse.dir/build.make
samples/StdInParse: src/libxerces-c-3.2.so
samples/StdInParse: /usr/lib/aarch64-linux-gnu/libnsl.so
samples/StdInParse: samples/CMakeFiles/StdInParse.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rb/xerces-c-3.2.3/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable StdInParse"
	cd /home/rb/xerces-c-3.2.3/bld/samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/StdInParse.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/CMakeFiles/StdInParse.dir/build: samples/StdInParse

.PHONY : samples/CMakeFiles/StdInParse.dir/build

samples/CMakeFiles/StdInParse.dir/clean:
	cd /home/rb/xerces-c-3.2.3/bld/samples && $(CMAKE_COMMAND) -P CMakeFiles/StdInParse.dir/cmake_clean.cmake
.PHONY : samples/CMakeFiles/StdInParse.dir/clean

samples/CMakeFiles/StdInParse.dir/depend:
	cd /home/rb/xerces-c-3.2.3/bld && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rb/xerces-c-3.2.3 /home/rb/xerces-c-3.2.3/samples /home/rb/xerces-c-3.2.3/bld /home/rb/xerces-c-3.2.3/bld/samples /home/rb/xerces-c-3.2.3/bld/samples/CMakeFiles/StdInParse.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/CMakeFiles/StdInParse.dir/depend

