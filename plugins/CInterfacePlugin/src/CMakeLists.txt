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
	SET(PLUGINTARGET  "libCInterface")
else()
	SET(PLUGINTARGET "CInterface")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DCINTERFACE_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES("./command")
INCLUDE_DIRECTORIES("./factory")
INCLUDE_DIRECTORIES("./include")
INCLUDE_DIRECTORIES("./plugin")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/PrepareMissionSequence.cpp
	factory/CCommandFactory.cpp
	plugin/CInterfacePluginFunctions.cpp
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
  DESTINATION bin
  )
