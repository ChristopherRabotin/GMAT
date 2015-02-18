# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT ExtraPropagators Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/ExtraPropagatorsPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT ExtraPropagators Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libExtraPropagators")
else()
	SET(PLUGINTARGET "ExtraPropagators")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DPROPAGATOR_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES("./base/factory")
INCLUDE_DIRECTORIES("./base/include")
INCLUDE_DIRECTORIES("./base/plugin")
INCLUDE_DIRECTORIES("./base/propagator")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    base/factory/ExtraPropagatorFactory.cpp
    base/plugin/GmatPluginFunctions.cpp
    base/propagator/BulirschStoer.cpp
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
