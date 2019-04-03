# $Id$
#
# GMAT: General Mission Analysis Tool.
#
# CMAKE script file for GMAT ScriptDebug Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/ScriptDebugSwitchPlugin/src directory
#
# Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT ScriptTools Plugin setup " ${VERSION}) 

SET(TargetName ScriptTools)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DSCRIPTTOOLS_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CommandEcho.cpp
    factory/CommandEchoFactory.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
