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
# GMAT base library
ADD_SUBDIRECTORY(base)

# Enable RPATH on Mac so that the build path is not hardcoded into
# the library's internal name. This is necessary to use GMAT from
# a 3rd-party app such as Matlab.
if(APPLE)
  GET_DIRECTORY_PROPERTY(BaseTarget DIRECTORY base DEFINITION TargetName)
  SET_TARGET_PROPERTIES(${BaseTarget} PROPERTIES MACOSX_RPATH TRUE INSTALL_RPATH "@loader_path/")
endif()

# ====================================================================
# Console binary
ADD_SUBDIRECTORY(console)

# ====================================================================
# GUI binary
if (GMAT_GUI)
  ADD_SUBDIRECTORY(gui)
endif()

