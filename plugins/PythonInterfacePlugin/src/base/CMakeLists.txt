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
if(NOT PYTHONLIBS_FOUND)
  MESSAGE(WARNING "Skipping PythonInterface: Python missing")
  RETURN()
endif()

SET(TargetName PythonInterface)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include interface plugin)

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

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "PYTHON_EXPORTS")

# ====================================================================
# Additional includes
TARGET_INCLUDE_DIRECTORIES(${TargetName} PRIVATE ${PYTHON_INCLUDE_DIRS})

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${PYTHON_LIBRARIES})
