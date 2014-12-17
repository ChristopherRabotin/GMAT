# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Geometric Measurement Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/GeometricMeasurementPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Geometric Measurement Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libGeometricMeasurements")
else()
	SET(PLUGINTARGET "GeometricMeasurements")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DGEOMETRICMEAS_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# specific compilation flags
if (UNIX)
	MESSAGE("-- Using a shared base library")
	IF(APPLE) # mac
		#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -dylib -dynamiclib -two_levelnamespace -undefined dynamic_lookup")
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -dylib -dynamiclib")

		# snow leopard
		#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -arch i386")
	endif()
endif (UNIX)

# Unix type systems need a change in the stack size
# Unix type systems include MAC, LINUX
MESSAGE("-- Executing... ulimit -s 61440")
execute_process(COMMAND ulimit -s 61440)

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES("./base/factory")
INCLUDE_DIRECTORIES("./base/include")
INCLUDE_DIRECTORIES("./base/measurement")
INCLUDE_DIRECTORIES("./base/plugin")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/command")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/estimator")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/event")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/factory")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/hardware")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/include")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/measurement")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/measurementfile")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/plugin")
INCLUDE_DIRECTORIES("../../EstimationPlugin/src/base/trackingsystem")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    base/factory/MeasurementFactory.cpp
    base/measurement/GeometricAzEl.cpp
    base/measurement/GeometricRADec.cpp
    base/measurement/GeometricRange.cpp
    base/measurement/GeometricRangeRate.cpp
    base/plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# libraries
LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../application/bin")
if(WIN32)
	LINK_LIBRARIES(libGmatBase libGmatEstimation)
else()
	LINK_LIBRARIES(GmatBase GmatEstimation)
endif()

# ====================================================================
# add the install targets
ADD_LIBRARY(${TargetName} SHARED ${PLUGIN_SRCS})

INSTALL( TARGETS ${TargetName}
  DESTINATION plugins
  )
