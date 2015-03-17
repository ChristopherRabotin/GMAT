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
SET(CONSOLE_SRCS 
    driver.cpp 
    ConsoleAppException.cpp
    PrintUtility.cpp
    ConsoleMessageReceiver.cpp
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE CONSOLE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
	
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
ADD_EXECUTABLE(${TargetName} ${CONSOLE_SRCS} ${CONSOLE_HEADERS})

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("")

INSTALL( TARGETS ${TargetName}
  DESTINATION bin
  )
