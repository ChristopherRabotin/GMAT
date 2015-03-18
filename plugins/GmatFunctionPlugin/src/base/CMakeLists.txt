# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GmatFunction Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/GmatFunctionPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT GmatFunction Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libGmatFunction")
else()
	SET(PLUGINTARGET "GmatFunction")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DGMATFUNCTION_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CallGmatFunction.cpp
    command/Global.cpp
    factory/GmatFunctionCommandFactory.cpp
    factory/GmatFunctionFactory.cpp
    function/GmatFunction.cpp
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
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
