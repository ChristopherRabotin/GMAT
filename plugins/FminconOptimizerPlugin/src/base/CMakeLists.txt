# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT FminconOptimizer Plugin.
# (Adapted from the CInterface plugin CMake script)
# (NOTE FROM RAVI: This needs debugging due to need for Matlab Optimization Toolbox
# This file must be installed in the plugins/FminconOptimizerPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT FminconOptimizer Plugin setup " ${VERSION}) 

# ====================================================================
# Major error checks
if(NOT Matlab_FOUND)
  MESSAGE(WARNING "Skipping due to missing Matlab")
  RETURN()
endif()

if(NOT WIN32)
  MESSAGE(WARNING "Skipping on Mac/Linux")
  RETURN()
endif()

# ====================================================================
# Define plugin target
if(WIN32)
	SET(PLUGINTARGET  "libFminconOptimizer")
else()
	SET(PLUGINTARGET "FminconOptimizer")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DFMINCON_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin solver)

# ====================================================================
# extra include directories for header files
INCLUDE_DIRECTORIES("../../../MatlabInterfacePlugin/src/base/include")
INCLUDE_DIRECTORIES("../../../MatlabInterfacePlugin/src/base/interface")
INCLUDE_DIRECTORIES(${Matlab_INCLUDE_DIRS})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/FminconOptimizerFactory.cpp
    plugin/GmatPluginFunctions.cpp
    solver/FminconOptimizer.cpp
)

# ====================================================================
# libraries
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
	LINK_LIBRARIES(libMatlabInterface)
else()
	LINK_LIBRARIES(GmatBase)
	LINK_LIBRARIES(MatlabInterface)
endif()
LINK_LIBRARIES(${Matlab_LIBRARIES})

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
