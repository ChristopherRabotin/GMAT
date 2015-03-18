# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT SaveCommand Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/SaveCommandPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT SaveCommand Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libSaveCommand")
else()
	SET(PLUGINTARGET "SaveCommand")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DSAVECOMMAND_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/Save.cpp
    factory/SaveCommandFactory.cpp
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
