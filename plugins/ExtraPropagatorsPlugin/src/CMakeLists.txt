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
INCLUDE_DIRECTORIES("../../../src/base/include")
INCLUDE_DIRECTORIES("../../../src/base/command")
INCLUDE_DIRECTORIES("../../../src/base/foundation")
INCLUDE_DIRECTORIES("../../../src/base/util")
INCLUDE_DIRECTORIES("../../../src/base/asset")
INCLUDE_DIRECTORIES("../../../src/base/solarsys")
INCLUDE_DIRECTORIES("../../../src/base/executive")
INCLUDE_DIRECTORIES("../../../src/base/subscriber")
INCLUDE_DIRECTORIES("../../../src/base/coordsystem")
INCLUDE_DIRECTORIES("../../../src/base/spacecraft")
INCLUDE_DIRECTORIES("../../../src/base/propagator")
INCLUDE_DIRECTORIES("../../../src/base/forcemodel")
INCLUDE_DIRECTORIES("../../../src/base/hardware")
INCLUDE_DIRECTORIES("../../../src/base/attitude")
INCLUDE_DIRECTORIES("../../../src/base/event")
INCLUDE_DIRECTORIES("../../../src/base/stopcond")
INCLUDE_DIRECTORIES("../../../src/base/parameter")
INCLUDE_DIRECTORIES("../../../src/base/interpolator")
INCLUDE_DIRECTORIES("../../../src/base/factory")
INCLUDE_DIRECTORIES("../../../src/base/burn")
INCLUDE_DIRECTORIES("../../../src/base/function")
INCLUDE_DIRECTORIES("../../../src/base/interface")
INCLUDE_DIRECTORIES("../../../src/base/interpreter")
INCLUDE_DIRECTORIES("../../../src/base/solver")
INCLUDE_DIRECTORIES("../../../src/base/math")
INCLUDE_DIRECTORIES("../../../src/base/configs")
INCLUDE_DIRECTORIES("../../../src/base/plugin")
INCLUDE_DIRECTORIES("../../../src/gui/app")
INCLUDE_DIRECTORIES("../../../src/gui/include")
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
