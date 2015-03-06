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

if(WIN32)
	SET(PLUGINTARGET  "libStation")
else()
	SET(PLUGINTARGET "Station")
endif()

SET(TargetName ${PLUGINTARGET})

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
# libraries
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
else()
	LINK_LIBRARIES(GmatBase)
endif()

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}")

INSTALL( TARGETS ${TargetName}
  DESTINATION plugins
  )
