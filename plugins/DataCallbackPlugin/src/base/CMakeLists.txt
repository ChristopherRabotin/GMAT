# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT DataCallback Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/DataCallbackPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT DataCallback Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libDataCallback")
else()
	SET(PLUGINTARGET "DataCallback")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DDATACALLBACK_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin subscriber)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/DataCallbackFactory.cpp
    plugin/GmatPluginFunctions.cpp
    subscriber/DataCallback.cpp
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
