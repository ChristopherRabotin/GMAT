# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT CInterface Plugin.
# This file must be installed in the plugins/CInterfacePlugin/src directory
#
# Author: Josh Fisher
# Modified by Ravi Mathur: Extensive CMake build system overhaul
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT CInterface Plugin setup " ${VERSION}) 

SET(TargetName CInterface)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DCINTERFACE_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory include plugin)

# ====================================================================
# specify source files to be compiled
SET(PLUGIN_SRCS
    command/PrepareMissionSequence.cpp
    factory/CCommandFactory.cpp
    plugin/CInterfacePluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" bin)
