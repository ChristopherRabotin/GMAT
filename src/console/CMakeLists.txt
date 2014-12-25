# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GUI
# This file must be installed in the src/console directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT console setup " ${VERSION})

if (PROFILE_BUILD)
	SET(CONSOLETARGET "gmatConsoleP")
else()
	SET(CONSOLETARGET "gmatConsole")
endif()

SET(TargetName ${CONSOLETARGET})

# ====================================================================
# SPICE specifics
INCLUDE_DIRECTORIES("${CSPICE_DIR}/include")
SET(SPICE_LIB_DIR  "${CSPICE_DIR}/lib")
LINK_DIRECTORIES(${SPICE_LIB_DIR})

if(WIN32)
	LINK_LIBRARIES("${CSPICE_DIR}/lib/cspice.lib")
else()
	LINK_LIBRARIES("${CSPICE_DIR}/lib/cspice.a")
endif()
        
ADD_DEFINITIONS("-D__USE_SPICE__")

SET(SPICE_SRCS
	 "attitude/SpiceAttitude.cpp"
	 "util/SpiceAttitudeKernelReader.cpp"
	 "util/SpiceInterface.cpp"
	 "util/SpiceOrbitKernelReader.cpp"
	 "util/SpiceKernelReader.cpp"
	 "util/SpiceKernelWriter.cpp"
	 "util/SpiceOrbitKernelWriter.cpp")


# ====================================================================
# source files
SET(SRCS 
    driver.cpp 
    ConsoleAppException.cpp
    PrintUtility.cpp
    ConsoleMessageReceiver.cpp
)

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES("../base/include")
INCLUDE_DIRECTORIES("../base/foundation")
INCLUDE_DIRECTORIES("../base/executive")
INCLUDE_DIRECTORIES("../base/factory")
INCLUDE_DIRECTORIES("../base/forcemodel") 
INCLUDE_DIRECTORIES("../base/subscriber")
INCLUDE_DIRECTORIES("../base/command")
INCLUDE_DIRECTORIES("../base/spacecraft")
INCLUDE_DIRECTORIES("../base/solarsys")
INCLUDE_DIRECTORIES("../base/propagator")
INCLUDE_DIRECTORIES("../base/interpreter")
INCLUDE_DIRECTORIES("../base/parameter")
INCLUDE_DIRECTORIES("../base/interpolator")
INCLUDE_DIRECTORIES("../base/util")
INCLUDE_DIRECTORIES("../base/stopcond") 
INCLUDE_DIRECTORIES("../base/hardware")
INCLUDE_DIRECTORIES("../base/configs")
INCLUDE_DIRECTORIES("../base/burn")
INCLUDE_DIRECTORIES("../base/solver")
INCLUDE_DIRECTORIES("../base/function")
INCLUDE_DIRECTORIES("../base/coordsystem")
INCLUDE_DIRECTORIES("../base/attitude")
INCLUDE_DIRECTORIES("../base/math")
INCLUDE_DIRECTORIES("../base/plugin")
INCLUDE_DIRECTORIES("../base/event")
	
# ====================================================================
# libraries
LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../application/bin")
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
else()
	LINK_LIBRARIES(GmatBase)
endif()


# ====================================================================
# compilation

# add the install targets
ADD_EXECUTABLE(${TargetName} ${SRCS})

SET_TARGET_PROPERTIES(${TargetName} PROPERTIES TARGET_VERSION_MAJOR "${VERSION}")

INSTALL( TARGETS ${TargetName}
  DESTINATION bin
  )
