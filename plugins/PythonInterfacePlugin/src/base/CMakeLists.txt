# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT PythonInterface Plugin.
# (Adapted from the MatlabInterface plugin CMake script)
# This file must be installed in the plugins/PythonInterfacePlugin/src/base directory
#
# Author: Darrel Conway
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT PythonInterface Plugin setup " ${VERSION}) 

# ====================================================================
# Major error checks
if(NOT Python_LIB_DIR)
  MESSAGE(WARNING "Skipping PythonInterface: Python missing")
  RETURN()
endif()

SET(TargetName PythonInterface)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DPYTHON_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include interface plugin)

# ====================================================================
# extra include directory for header file
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CallPythonFunction.cpp
    factory/PythonCommandFactory.cpp
    function/PythonModule.cpp
    interface/PythonInterface.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} ${PYTHON_LIBRARY})

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    if(GMAT_MAC_APPBUNDLE_PATH)
      SET(MAC_BASEPATH "../Frameworks/") # GmatBase and wxLibs in Frameworks
    else()
      SET(MAC_BASEPATH ";@loader_path/libwx/") # Separate base/wx dirs
    endif()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/libwx/"
      )
  endif()
endif()
