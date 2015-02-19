# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT MatlabInterface Plugin.
# (Adapted from the CInterface plugin CMake script)
# (NOTE FROM RAVI: This needs debugging due to need for Matlab Optimization Toolbox)
# This file must be installed in the plugins/MatlabInterfacePlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT MatlabInterface Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET "libMatlabInterface")
else()
	SET(PLUGINTARGET "MatlabInterface")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DMATLAB_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include interface plugin subscriber)

# ====================================================================
# extra include directories for header files
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIRS})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CallMatlabFunction.cpp
    factory/CallMatlabFunctionFactory.cpp
    factory/MatlabFunctionFactory.cpp
    factory/MatlabInterfaceFactory.cpp
    factory/MatlabWorkspaceFactory.cpp
    function/MatlabFunction.cpp
    interface/MatlabInterface.cpp
    plugin/GmatPluginFunctions.cpp
    subscriber/MatlabWorkspace.cpp
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
