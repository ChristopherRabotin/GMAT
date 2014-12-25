# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT EventLocator Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/EventLocatorPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT EventLocator Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET  "libEventLocator")
else()
	SET(PLUGINTARGET "EventLocator")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DLOCATOR_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES("./base/event")
INCLUDE_DIRECTORIES("./base/factory")
INCLUDE_DIRECTORIES("./base/include")
INCLUDE_DIRECTORIES("./base/locator")
INCLUDE_DIRECTORIES("./base/plugin")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    base/event/Antumbra.cpp
    base/event/Contact.cpp
    base/event/Eclipse.cpp
    base/event/Elevation.cpp
    base/event/LineOfSight.cpp
    base/event/Penumbra.cpp
    base/event/Umbra.cpp
    base/factory/EventLocatorFactory.cpp
    base/locator/ContactLocator.cpp
    base/locator/EclipseLocator.cpp
    base/plugin/GmatPluginFunctions.cpp
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
