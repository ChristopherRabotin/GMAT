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
include CMakeFiles/ninja_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ninja_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ninja_test.dir/flags.make

CMakeFiles/ninja_test.dir/src/build_log_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/build_log_test.cc.o: ../src/build_log_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ninja_test.dir/src/build_log_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/build_log_test.cc.o -c /home/rb/ninja-master/src/build_log_test.cc

CMakeFiles/ninja_test.dir/src/build_log_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/build_log_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/build_log_test.cc > CMakeFiles/ninja_test.dir/src/build_log_test.cc.i

CMakeFiles/ninja_test.dir/src/build_log_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/build_log_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/build_log_test.cc -o CMakeFiles/ninja_test.dir/src/build_log_test.cc.s

CMakeFiles/ninja_test.dir/src/build_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/build_test.cc.o: ../src/build_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ninja_test.dir/src/build_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/build_test.cc.o -c /home/rb/ninja-master/src/build_test.cc

CMakeFiles/ninja_test.dir/src/build_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/build_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/build_test.cc > CMakeFiles/ninja_test.dir/src/build_test.cc.i

CMakeFiles/ninja_test.dir/src/build_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/build_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/build_test.cc -o CMakeFiles/ninja_test.dir/src/build_test.cc.s

CMakeFiles/ninja_test.dir/src/clean_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/clean_test.cc.o: ../src/clean_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/ninja_test.dir/src/clean_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/clean_test.cc.o -c /home/rb/ninja-master/src/clean_test.cc

CMakeFiles/ninja_test.dir/src/clean_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/clean_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/clean_test.cc > CMakeFiles/ninja_test.dir/src/clean_test.cc.i

CMakeFiles/ninja_test.dir/src/clean_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/clean_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/clean_test.cc -o CMakeFiles/ninja_test.dir/src/clean_test.cc.s

CMakeFiles/ninja_test.dir/src/clparser_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/clparser_test.cc.o: ../src/clparser_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/ninja_test.dir/src/clparser_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/clparser_test.cc.o -c /home/rb/ninja-master/src/clparser_test.cc

CMakeFiles/ninja_test.dir/src/clparser_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/clparser_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/clparser_test.cc > CMakeFiles/ninja_test.dir/src/clparser_test.cc.i

CMakeFiles/ninja_test.dir/src/clparser_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/clparser_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/clparser_test.cc -o CMakeFiles/ninja_test.dir/src/clparser_test.cc.s

CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.o: ../src/depfile_parser_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.o -c /home/rb/ninja-master/src/depfile_parser_test.cc

CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/depfile_parser_test.cc > CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.i

CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/depfile_parser_test.cc -o CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.s

CMakeFiles/ninja_test.dir/src/deps_log_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/deps_log_test.cc.o: ../src/deps_log_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/ninja_test.dir/src/deps_log_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/deps_log_test.cc.o -c /home/rb/ninja-master/src/deps_log_test.cc

CMakeFiles/ninja_test.dir/src/deps_log_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/deps_log_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/deps_log_test.cc > CMakeFiles/ninja_test.dir/src/deps_log_test.cc.i

CMakeFiles/ninja_test.dir/src/deps_log_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/deps_log_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/deps_log_test.cc -o CMakeFiles/ninja_test.dir/src/deps_log_test.cc.s

CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.o: ../src/disk_interface_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.o -c /home/rb/ninja-master/src/disk_interface_test.cc

CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/disk_interface_test.cc > CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.i

CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/disk_interface_test.cc -o CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.s

CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.o: ../src/dyndep_parser_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.o -c /home/rb/ninja-master/src/dyndep_parser_test.cc

CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/dyndep_parser_test.cc > CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.i

CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/dyndep_parser_test.cc -o CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.s

CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.o: ../src/edit_distance_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.o -c /home/rb/ninja-master/src/edit_distance_test.cc

CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/edit_distance_test.cc > CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.i

CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/edit_distance_test.cc -o CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.s

CMakeFiles/ninja_test.dir/src/graph_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/graph_test.cc.o: ../src/graph_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/ninja_test.dir/src/graph_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/graph_test.cc.o -c /home/rb/ninja-master/src/graph_test.cc

CMakeFiles/ninja_test.dir/src/graph_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/graph_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/graph_test.cc > CMakeFiles/ninja_test.dir/src/graph_test.cc.i

CMakeFiles/ninja_test.dir/src/graph_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/graph_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/graph_test.cc -o CMakeFiles/ninja_test.dir/src/graph_test.cc.s

CMakeFiles/ninja_test.dir/src/json_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/json_test.cc.o: ../src/json_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/ninja_test.dir/src/json_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/json_test.cc.o -c /home/rb/ninja-master/src/json_test.cc

CMakeFiles/ninja_test.dir/src/json_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/json_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/json_test.cc > CMakeFiles/ninja_test.dir/src/json_test.cc.i

CMakeFiles/ninja_test.dir/src/json_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/json_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/json_test.cc -o CMakeFiles/ninja_test.dir/src/json_test.cc.s

CMakeFiles/ninja_test.dir/src/lexer_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/lexer_test.cc.o: ../src/lexer_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/ninja_test.dir/src/lexer_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/lexer_test.cc.o -c /home/rb/ninja-master/src/lexer_test.cc

CMakeFiles/ninja_test.dir/src/lexer_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/lexer_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/lexer_test.cc > CMakeFiles/ninja_test.dir/src/lexer_test.cc.i

CMakeFiles/ninja_test.dir/src/lexer_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/lexer_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/lexer_test.cc -o CMakeFiles/ninja_test.dir/src/lexer_test.cc.s

CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.o: ../src/manifest_parser_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.o -c /home/rb/ninja-master/src/manifest_parser_test.cc

CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/manifest_parser_test.cc > CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.i

CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/manifest_parser_test.cc -o CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.s

CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.o: ../src/missing_deps_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.o -c /home/rb/ninja-master/src/missing_deps_test.cc

CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/missing_deps_test.cc > CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.i

CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/missing_deps_test.cc -o CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.s

CMakeFiles/ninja_test.dir/src/ninja_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/ninja_test.cc.o: ../src/ninja_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/ninja_test.dir/src/ninja_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/ninja_test.cc.o -c /home/rb/ninja-master/src/ninja_test.cc

CMakeFiles/ninja_test.dir/src/ninja_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/ninja_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/ninja_test.cc > CMakeFiles/ninja_test.dir/src/ninja_test.cc.i

CMakeFiles/ninja_test.dir/src/ninja_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/ninja_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/ninja_test.cc -o CMakeFiles/ninja_test.dir/src/ninja_test.cc.s

CMakeFiles/ninja_test.dir/src/state_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/state_test.cc.o: ../src/state_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/ninja_test.dir/src/state_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/state_test.cc.o -c /home/rb/ninja-master/src/state_test.cc

CMakeFiles/ninja_test.dir/src/state_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/state_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/state_test.cc > CMakeFiles/ninja_test.dir/src/state_test.cc.i

CMakeFiles/ninja_test.dir/src/state_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/state_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/state_test.cc -o CMakeFiles/ninja_test.dir/src/state_test.cc.s

CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.o: ../src/string_piece_util_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.o -c /home/rb/ninja-master/src/string_piece_util_test.cc

CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/string_piece_util_test.cc > CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.i

CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/string_piece_util_test.cc -o CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.s

CMakeFiles/ninja_test.dir/src/subprocess_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/subprocess_test.cc.o: ../src/subprocess_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Building CXX object CMakeFiles/ninja_test.dir/src/subprocess_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/subprocess_test.cc.o -c /home/rb/ninja-master/src/subprocess_test.cc

CMakeFiles/ninja_test.dir/src/subprocess_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/subprocess_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/subprocess_test.cc > CMakeFiles/ninja_test.dir/src/subprocess_test.cc.i

CMakeFiles/ninja_test.dir/src/subprocess_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/subprocess_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/subprocess_test.cc -o CMakeFiles/ninja_test.dir/src/subprocess_test.cc.s

CMakeFiles/ninja_test.dir/src/test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/test.cc.o: ../src/test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_19) "Building CXX object CMakeFiles/ninja_test.dir/src/test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/test.cc.o -c /home/rb/ninja-master/src/test.cc

CMakeFiles/ninja_test.dir/src/test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/test.cc > CMakeFiles/ninja_test.dir/src/test.cc.i

CMakeFiles/ninja_test.dir/src/test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/test.cc -o CMakeFiles/ninja_test.dir/src/test.cc.s

CMakeFiles/ninja_test.dir/src/util_test.cc.o: CMakeFiles/ninja_test.dir/flags.make
CMakeFiles/ninja_test.dir/src/util_test.cc.o: ../src/util_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_20) "Building CXX object CMakeFiles/ninja_test.dir/src/util_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ninja_test.dir/src/util_test.cc.o -c /home/rb/ninja-master/src/util_test.cc

CMakeFiles/ninja_test.dir/src/util_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ninja_test.dir/src/util_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rb/ninja-master/src/util_test.cc > CMakeFiles/ninja_test.dir/src/util_test.cc.i

CMakeFiles/ninja_test.dir/src/util_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ninja_test.dir/src/util_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rb/ninja-master/src/util_test.cc -o CMakeFiles/ninja_test.dir/src/util_test.cc.s

# Object files for target ninja_test
ninja_test_OBJECTS = \
"CMakeFiles/ninja_test.dir/src/build_log_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/build_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/clean_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/clparser_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/deps_log_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/graph_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/json_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/lexer_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/ninja_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/state_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/subprocess_test.cc.o" \
"CMakeFiles/ninja_test.dir/src/test.cc.o" \
"CMakeFiles/ninja_test.dir/src/util_test.cc.o"

# External object files for target ninja_test
ninja_test_EXTERNAL_OBJECTS = \
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

ninja_test: CMakeFiles/ninja_test.dir/src/build_log_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/build_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/clean_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/clparser_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/depfile_parser_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/deps_log_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/disk_interface_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/dyndep_parser_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/edit_distance_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/graph_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/json_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/lexer_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/manifest_parser_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/missing_deps_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/ninja_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/state_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/string_piece_util_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/subprocess_test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/test.cc.o
ninja_test: CMakeFiles/ninja_test.dir/src/util_test.cc.o
ninja_test: CMakeFiles/libninja.dir/src/build_log.cc.o
ninja_test: CMakeFiles/libninja.dir/src/build.cc.o
ninja_test: CMakeFiles/libninja.dir/src/clean.cc.o
ninja_test: CMakeFiles/libninja.dir/src/clparser.cc.o
ninja_test: CMakeFiles/libninja.dir/src/dyndep.cc.o
ninja_test: CMakeFiles/libninja.dir/src/dyndep_parser.cc.o
ninja_test: CMakeFiles/libninja.dir/src/debug_flags.cc.o
ninja_test: CMakeFiles/libninja.dir/src/deps_log.cc.o
ninja_test: CMakeFiles/libninja.dir/src/disk_interface.cc.o
ninja_test: CMakeFiles/libninja.dir/src/edit_distance.cc.o
ninja_test: CMakeFiles/libninja.dir/src/eval_env.cc.o
ninja_test: CMakeFiles/libninja.dir/src/graph.cc.o
ninja_test: CMakeFiles/libninja.dir/src/graphviz.cc.o
ninja_test: CMakeFiles/libninja.dir/src/json.cc.o
ninja_test: CMakeFiles/libninja.dir/src/line_printer.cc.o
ninja_test: CMakeFiles/libninja.dir/src/manifest_parser.cc.o
ninja_test: CMakeFiles/libninja.dir/src/metrics.cc.o
ninja_test: CMakeFiles/libninja.dir/src/missing_deps.cc.o
ninja_test: CMakeFiles/libninja.dir/src/parser.cc.o
ninja_test: CMakeFiles/libninja.dir/src/state.cc.o
ninja_test: CMakeFiles/libninja.dir/src/status.cc.o
ninja_test: CMakeFiles/libninja.dir/src/string_piece_util.cc.o
ninja_test: CMakeFiles/libninja.dir/src/util.cc.o
ninja_test: CMakeFiles/libninja.dir/src/version.cc.o
ninja_test: CMakeFiles/libninja.dir/src/subprocess-posix.cc.o
ninja_test: CMakeFiles/libninja-re2c.dir/src/depfile_parser.cc.o
ninja_test: CMakeFiles/libninja-re2c.dir/src/lexer.cc.o
ninja_test: CMakeFiles/ninja_test.dir/build.make
ninja_test: CMakeFiles/ninja_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rb/ninja-master/bld/CMakeFiles --progress-num=$(CMAKE_PROGRESS_21) "Linking CXX executable ninja_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ninja_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ninja_test.dir/build: ninja_test

.PHONY : CMakeFiles/ninja_test.dir/build

CMakeFiles/ninja_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ninja_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ninja_test.dir/clean

CMakeFiles/ninja_test.dir/depend:
	cd /home/rb/ninja-master/bld && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rb/ninja-master /home/rb/ninja-master /home/rb/ninja-master/bld /home/rb/ninja-master/bld /home/rb/ninja-master/bld/CMakeFiles/ninja_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ninja_test.dir/depend

