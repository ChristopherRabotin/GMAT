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

# Utility rule file for createapidocs.

# Include the progress variables for this target.
include doc/CMakeFiles/createapidocs.dir/progress.make

doc/CMakeFiles/createapidocs:
	cd /home/rb/xerces-c-3.2.3/bld/doc && /usr/bin/doxygen Doxyfile

createapidocs: doc/CMakeFiles/createapidocs
createapidocs: doc/CMakeFiles/createapidocs.dir/build.make

.PHONY : createapidocs

# Rule to build all files generated by this target.
doc/CMakeFiles/createapidocs.dir/build: createapidocs

.PHONY : doc/CMakeFiles/createapidocs.dir/build

doc/CMakeFiles/createapidocs.dir/clean:
	cd /home/rb/xerces-c-3.2.3/bld/doc && $(CMAKE_COMMAND) -P CMakeFiles/createapidocs.dir/cmake_clean.cmake
.PHONY : doc/CMakeFiles/createapidocs.dir/clean

doc/CMakeFiles/createapidocs.dir/depend:
	cd /home/rb/xerces-c-3.2.3/bld && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rb/xerces-c-3.2.3 /home/rb/xerces-c-3.2.3/doc /home/rb/xerces-c-3.2.3/bld /home/rb/xerces-c-3.2.3/bld/doc /home/rb/xerces-c-3.2.3/bld/doc/CMakeFiles/createapidocs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : doc/CMakeFiles/createapidocs.dir/depend
