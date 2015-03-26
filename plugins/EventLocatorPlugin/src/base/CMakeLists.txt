# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT EventLocator Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/EventLocatorPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT EventLocator Plugin setup " ${VERSION}) 

SET(TargetName EventLocator)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DLOCATOR_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS event factory include locator plugin)

# ====================================================================
# extra include directories for header files
INCLUDE_DIRECTORIES("${CSPICE_DIR}/include")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    event/Antumbra.cpp
    event/Contact.cpp
    event/ContactEvent.cpp
    event/Eclipse.cpp
    event/EclipseEvent.cpp
    event/EclipseTotalEvent.cpp
    event/Elevation.cpp
    event/LineOfSight.cpp
    event/Penumbra.cpp
    event/Umbra.cpp
    factory/EventLocatorFactory.cpp
    locator/ContactLocator.cpp
    locator/EclipseLocator.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
