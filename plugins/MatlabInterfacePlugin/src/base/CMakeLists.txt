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
	SET(PLUGINTARGET  "libMatlabInterface")
else()
	SET(PLUGINTARGET "MatlabInterface")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DMATLAB_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES("./base/command")
INCLUDE_DIRECTORIES("./base/factory")
INCLUDE_DIRECTORIES("./base/function")
INCLUDE_DIRECTORIES("./base/include")
INCLUDE_DIRECTORIES("./base/interface")
INCLUDE_DIRECTORIES("./base/plugin")
INCLUDE_DIRECTORIES("./base/subscriber")
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIRS})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    base/command/CallMatlabFunction.cpp
    base/factory/CallMatlabFunctionFactory.cpp
    base/factory/MatlabFunctionFactory.cpp
    base/factory/MatlabInterfaceFactory.cpp
    base/factory/MatlabWorkspaceFactory.cpp
    base/function/MatlabFunction.cpp
    base/interface/MatlabInterface.cpp
    base/plugin/GmatPluginFunctions.cpp
    base/subscriber/MatlabWorkspace.cpp
)

# ====================================================================
# libraries
LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../application/bin")
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
else()
	LINK_LIBRARIES(GmatBase)
endif()

# ====================================================================
# add the install targets
ADD_LIBRARY(${TargetName} SHARED ${PLUGIN_SRCS})

INSTALL( TARGETS ${TargetName}
  DESTINATION plugins
  )
