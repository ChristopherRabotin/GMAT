# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the GMAT source directory.
# That is, we should have the directory structure:
#   ./base 
#   ./console
#   ./gui
#
# Original Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
# Modified for CMake standardization by: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
# Output directories based on configuration type.
# (Make all output go to the /gmat/application/bin/ directory.

# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application/bin" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application/bin" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application/bin" )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/application/bin" )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/application/bin" )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/application/bin" )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# ====================================================================
# GMAT base library
ADD_SUBDIRECTORY(base)

# Enable RPATH on Mac so that the build path is not hardcoded into
# the library's internal name. This is necessary to use GMAT from
# a 3rd-party app such as Matlab. Note that this will no longer be
# necessary once the project moves to CMake 3.0.
if(APPLE)
  GET_DIRECTORY_PROPERTY(BaseTarget DIRECTORY base DEFINITION TargetName)
  SET_TARGET_PROPERTIES(${BaseTarget} PROPERTIES MACOSX_RPATH TRUE)
endif()

# ====================================================================
# Console binary
ADD_SUBDIRECTORY(console)

# ====================================================================
# GUI binary
if (GMAT_GUI)
  ADD_SUBDIRECTORY(gui)
endif()

