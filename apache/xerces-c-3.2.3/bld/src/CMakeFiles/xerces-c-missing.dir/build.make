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

# Utility rule file for xerces-c-missing.

# Include the progress variables for this target.
include src/CMakeFiles/xerces-c-missing.dir/progress.make

xerces-c-missing: src/CMakeFiles/xerces-c-missing.dir/build.make

.PHONY : xerces-c-missing

# Rule to build all files generated by this target.
src/CMakeFiles/xerces-c-missing.dir/build: xerces-c-missing

.PHONY : src/CMakeFiles/xerces-c-missing.dir/build

src/CMakeFiles/xerces-c-missing.dir/clean:
	cd /home/rb/xerces-c-3.2.3/bld/src && $(CMAKE_COMMAND) -P CMakeFiles/xerces-c-missing.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/xerces-c-missing.dir/clean

src/CMakeFiles/xerces-c-missing.dir/depend:
	cd /home/rb/xerces-c-3.2.3/bld && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rb/xerces-c-3.2.3 /home/rb/xerces-c-3.2.3/src /home/rb/xerces-c-3.2.3/bld /home/rb/xerces-c-3.2.3/bld/src /home/rb/xerces-c-3.2.3/bld/src/CMakeFiles/xerces-c-missing.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/xerces-c-missing.dir/depend

