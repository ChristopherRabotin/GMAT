# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT CInterface Plugin.
# This file must be installed in the plugins/CInterfacePlugin/src directory
#
# Author: Josh Fisher
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT CInterface Plugin setup " ${VERSION}) 

if(WIN32)
	SET(PLUGINTARGET "libCInterface")
else()
	SET(PLUGINTARGET "CInterface")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DCINTERFACE_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory include plugin)

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES(${PLUGIN_DIRS})

# ====================================================================
# specify source files to be compiled
SET(PLUGIN_SRCS
    command/PrepareMissionSequence.cpp
    factory/CCommandFactory.cpp
    plugin/CInterfacePluginFunctions.cpp
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE PLUGIN_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# libraries
if(WIN32)
	LINK_LIBRARIES(libGmatBase)
else()
	LINK_LIBRARIES(GmatBase)
endif()

# ====================================================================
# add the install targets
# headers are included here for IDE-based projects (VS, XCode, etc.)
ADD_LIBRARY(${TargetName} SHARED ${PLUGIN_SRCS} ${PLUGIN_HEADERS})

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("${PLUGIN_DIRS}")

INSTALL( TARGETS ${TargetName}
  DESTINATION bin
  )
