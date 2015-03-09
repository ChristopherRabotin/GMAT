# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Formation Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/FormationPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Formation Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libFormation")
else()
	SET(PLUGINTARGET "Formation")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DFORMATION_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory formation include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/FormationFactory.cpp
    formation/Formation.cpp
    plugin/GmatPluginFunctions.cpp
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
