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

# Variable used to collect a list of all src targets
SET(SrcTargets "")

# ====================================================================
# GMAT base library
SET(SRCDIR "base")
ADD_SUBDIRECTORY(${SRCDIR})
GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
SET(SrcTargets ${SrcTargets} ${tmp})


# ====================================================================
# Console binary
SET(SRCDIR "console")
ADD_SUBDIRECTORY(${SRCDIR})
GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
SET(SrcTargets ${SrcTargets} ${tmp})

# ====================================================================
# GUI binary
if (GMAT_GUI)
  SET(SRCDIR "gui")
  ADD_SUBDIRECTORY(${SRCDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
  SET(SrcTargets ${SrcTargets} ${tmp})
endif()

# Enable RPATH on Mac/Linux so that the build path is not hardcoded into
# the library's internal name. This is necessary to use GMAT from
# a 3rd-party app such as Matlab.
if(UNIX)
  if(APPLE)
    SET_TARGET_PROPERTIES(${SrcTargets} PROPERTIES MACOSX_RPATH TRUE INSTALL_RPATH "@loader_path/")
  else()
    SET_TARGET_PROPERTIES(${SrcTargets} PROPERTIES INSTALL_RPATH "\$ORIGIN/")
  endif()
endif()
