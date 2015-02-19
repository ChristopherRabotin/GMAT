# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT ProductionPropagators Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/ProductionPropagatorPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT ProductionPropagators Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libProductionPropagators")
else()
	SET(PLUGINTARGET "ProductionPropagators")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DPROPAGATOR_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin propagator)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/ProductionPropagatorFactory.cpp
    plugin/GmatPluginFunctions.cpp
    propagator/PrinceDormand853.cpp
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
