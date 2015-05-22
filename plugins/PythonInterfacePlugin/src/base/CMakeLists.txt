# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMake script for GMAT Python Interface plugin
# (Adapted from the GmatFunctions plugin CMake script)
# This file must be installed in the plugins/PythonInterface/src/base directory
#
# Author: Darrel Conway (5/22/2015, based on Ravi Mathur's template)
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
# Replace <PluginName> with a simple name for your plugin
MESSAGE("==============================")
MESSAGE("GMAT PythonInterface Plugin setup " ${VERSION}) 

# ====================================================================
# Major error checks
if(NOT Python_FOUND)
  MESSAGE(WARNING "Skipping PythonInterface: Python missing")
  RETURN()
endif()

SET(TargetName PythonInterface)

# ====================================================================
# Preproccessor definitions for the plugin
ADD_DEFINITIONS("-DPYTHON_EXPORTS")

# ====================================================================
# List of directory containing source/header files
SET(PLUGIN_DIRS command factory function include interface plugin)

# ====================================================================
# Source files
SET(PLUGIN_SRCS
    plugin/GmatPluginFunctions.cpp
    command/CallPythonFunction.cpp
    factory/PythonCommandFactory.cpp
    function/PythonModule.cpp
    interface/PythonInterface.cpp
)

# ====================================================================
# This CMake macro (defined in <GMAT>/plugins/CMakeLists.txt) sets up
# definitions and variables common to all plugins.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
