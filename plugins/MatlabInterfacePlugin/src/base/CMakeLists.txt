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

# ====================================================================
# Major error checks
if(NOT Matlab_FOUND)
  MESSAGE(WARNING "Skipping due to missing Matlab")
  RETURN()
endif()

if(NOT wxWidgets_FOUND)
  MESSAGE(WARNING "Skipping due to missing wxWidgets")
  RETURN()
endif()

# ====================================================================
# Define plugin target
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
if(UNIX)
  SET(PLUGIN_DIRS ${PLUGIN_DIRS} "../matlab/gmat_mex/src")
endif()

# ====================================================================
# extra include directories for header files
INCLUDE_DIRECTORIES(${Matlab_INCLUDE_DIRS})
if(UNIX)
  INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/gui/include")
  INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/gui/app")
  INCLUDE(${wxWidgets_USE_FILE})
endif()

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
if(UNIX)
  SET(PLUGIN_SRCS
    ${PLUGIN_SRCS}
    ../matlab/gmat_mex/src/MatlabClient.cpp
    ../matlab/gmat_mex/src/MatlabConnection.cpp
    )
endif()

# ====================================================================
# libraries
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
else()
	LINK_LIBRARIES(GmatBase)
endif()
LINK_LIBRARIES(${Matlab_LIBRARIES})
if(UNIX)
  LINK_LIBRARIES(${wxWidgets_LIBRARIES})
endif()

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
