# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT DataCallback Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/DataCallbackPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT DataCallback Plugin setup " ${VERSION}) 

SET(TargetName DataCallback)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DDATACALLBACK_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin subscriber)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/DataCallbackFactory.cpp
    plugin/GmatPluginFunctions.cpp
    subscriber/DataCallback.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
if(APPLE)
  # Override appbundle path so plugin is installed directly into bin
  set(MAC_APPBUNDLE_PATH ${GMAT_MAC_APPBUNDLE_PATH}) # Save for later
  set(GMAT_MAC_APPBUNDLE_PATH "")
endif()

_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" bin)

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    set(GMAT_MAC_APPBUNDLE_PATH ${MAC_APPBUNDLE_PATH}) # Restore
    if(GMAT_MAC_APPBUNDLE_PATH)
      SET(MAC_BASEPATH "../${GMAT_MAC_APPBUNDLE_PATH}/Frameworks/")
    endif()           
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/"                             
      )
  endif()
endif()

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )
