# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT PythonInterface Plugin.
# (Adapted from the MatlabInterface plugin CMake script)
# This file must be installed in the plugins/PythonInterfacePlugin/src/base directory
#
# Author: Darrel Conway
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT PythonInterface Plugin setup " ${VERSION}) 

# ====================================================================
# Major error checks
if(NOT PYTHON_LIB_DIR)
  MESSAGE(WARNING "Skipping PythonInterface: Python missing")
  RETURN()
endif()

SET(TargetName PythonInterface)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DPYTHON_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include interface plugin)

# ====================================================================
# extra include directory for header file
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CallPythonFunction.cpp
    factory/PythonCommandFactory.cpp
    function/PythonModule.cpp
    interface/PythonInterface.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# ====================================================================
# Additional link libraries
if(UNIX)
  if(APPLE)
    # Check this for Mac
    FIND_LIBRARY(PYTHON_LIB ${PYTHON_LIBRARY} ${PYTHON_LIB_DIR})
  else()
    FIND_LIBRARY(PYTHON_LIB ${PYTHON_LIBRARY} ${PYTHON_LIB_DIR})
  endif()
else()
  set(PYTHON_LIB ${PYTHON_LIB_DIR}/${PYTHON_LIBRARY}) 
endif()

TARGET_LINK_LIBRARIES(${TargetName} ${PYTHON_LIB})

MESSAGE(STATUS "Python Library: " ${PYTHON_LIB})
