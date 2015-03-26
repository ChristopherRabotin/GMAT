# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Station Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/StationPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Station Plugin setup " ${VERSION}) 

SET(TargetName Station)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DSTATION_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin station)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/StationFactory.cpp
    plugin/GmatPluginFunctions.cpp
    station/GroundStation.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
