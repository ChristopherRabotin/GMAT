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
CMAKE_SOURCE_DIR = /home/rb/ninja-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rb/ninja-master/bld

# Include any dependencies generated for this target.
include CMakeFiles/hash_collision_bench.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/hash_collision_bench.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hash_collision_bench.dir/flags.make

CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.o: CMakeFiles/hash_collision_bench.dir/flags.make
CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.o: ../src/hash_collision_bench.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.o -c /home/rb/ninja-master/src/hash_collision_bench.cc

CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/hash_collision_bench.cc > CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.i

CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/hash_collision_bench.cc -o CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.s

# Object files for target hash_collision_bench
hash_collision_bench_OBJECTS = \
"CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.o"

# External object files for target hash_collision_bench
hash_collision_bench_EXTERNAL_OBJECTS = \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/build_log.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/build.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/clean.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/clparser.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/dyndep.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/dyndep_parser.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/debug_flags.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/deps_log.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/disk_interface.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/edit_distance.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/eval_env.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/graph.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/graphviz.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/json.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/line_printer.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/manifest_parser.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/metrics.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/missing_deps.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/parser.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/state.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/status.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/string_piece_util.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/util.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/version.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja.dir/src/subprocess-posix.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja-re2c.dir/src/depfile_parser.cc.o" \
"/home/rb/ninja-master/bld/CMakeFiles/libninja-re2c.dir/src/lexer.cc.o"

hash_collision_bench: CMakeFiles/hash_collision_bench.dir/src/hash_collision_bench.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/build_log.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/build.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/clean.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/clparser.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/dyndep.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/dyndep_parser.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/debug_flags.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/deps_log.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/disk_interface.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/edit_distance.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/eval_env.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/graph.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/graphviz.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/json.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/line_printer.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/manifest_parser.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/metrics.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/missing_deps.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/parser.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/state.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/status.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/string_piece_util.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/util.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/version.cc.o
hash_collision_bench: CMakeFiles/libninja.dir/src/subprocess-posix.cc.o
hash_collision_bench: CMakeFiles/libninja-re2c.dir/src/depfile_parser.cc.o
hash_collision_bench: CMakeFiles/libninja-re2c.dir/src/lexer.cc.o
hash_collision_bench: CMakeFiles/hash_collision_bench.dir/build.make
hash_collision_bench: CMakeFiles/hash_collision_bench.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable hash_collision_bench"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hash_collision_bench.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hash_collision_bench.dir/build: hash_collision_bench

.PHONY : CMakeFiles/hash_collision_bench.dir/build

CMakeFiles/hash_collision_bench.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hash_collision_bench.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hash_collision_bench.dir/clean

CMakeFiles/hash_collision_bench.dir/depend:
	cd /home/rb/ninja-master/bld && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rb/ninja-master /home/rb/ninja-master /home/rb/ninja-master/bld /home/rb/ninja-master/bld /home/rb/ninja-master/bld/CMakeFiles/hash_collision_bench.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hash_collision_bench.dir/depend

