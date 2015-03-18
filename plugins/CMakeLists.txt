# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the GMAT plugins directory
# That is, we should have the directory structure:
#   ./CInterfacePlugin
#   ./DataInterfacePlugin
#   etc... other plugins
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# Convenience macro used by plugins to enable common functionality
# Inputs:
#   TargetName:  Name of plugin target
#   PLUGIN_DIRS: List of directories containing source/header files
#   PLUGIN_SRCS: List of .c and .cpp source files
#   INSTALL_DIR: Directory to install plugin (plugins, bin, etc.)
# Note that all paths are relative to the plugin's CMakeLists.txt
MACRO(_SETUPPLUGIN TargetName PLUGIN_DIRS PLUGIN_SRCS INSTALL_DIR)
  # Add Preproccessor Definitions
  ADD_DEFINITIONS("-D_DYNAMICLINK")
  #ADD_DEFINITIONS("-DEXP_STL")

  # ====================================================================
  # prepend include directories for header files
  INCLUDE_DIRECTORIES(BEFORE ${GMAT_BASE_INCLUDE_DIRECTORIES})
  INCLUDE_DIRECTORIES(BEFORE ${PLUGIN_DIRS})

  # ====================================================================
  # Recursively find all include files in specified directories
  FOREACH(I_DIR ${PLUGIN_DIRS})
    FILE(GLOB_RECURSE IDIR_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${I_DIR}/*.hpp ${I_DIR}/*.h)
    SET(PLUGIN_HEADERS ${PLUGIN_HEADERS} ${IDIR_HEADERS})
  ENDFOREACH()

  # ====================================================================
  # add the install targets
  # headers are included here for IDE-based projects (VS, XCode, etc.)
  if(SETUPPLUGIN_VERBOSE)
    MESSAGE("HEADERS: ${PLUGIN_HEADERS}")
  endif()
  ADD_LIBRARY(${TargetName} SHARED ${PLUGIN_SRCS} ${PLUGIN_HEADERS})

  # ====================================================================
  # Add source/header files to IDE-based project source groups
  # Macro defined in top-level CMakeLists.txt
  _ADDSOURCEGROUPS("${PLUGIN_DIRS}")

  # ====================================================================
  # Specify where to install plugin
  #  - On Windows the dll is RUNTIME (lib is ARCHIVE, ignored here)
  #  - On Mac/Linux the dylib/so is LIBRARY
  INSTALL(TARGETS ${TargetName}
    RUNTIME DESTINATION ${INSTALL_DIR} # Windows
    LIBRARY DESTINATION ${INSTALL_DIR} # Mac/Linux
    )

ENDMACRO(_SETUPPLUGIN)

# Variable used to collect a list of all enabled plugins
SET(PluginTargets "")

# ====================================================================
# CInterface Plugin binary
if (PLUGIN_CINTERFACE)
  SET(PLUGINDIR "CInterfacePlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# DataInterface Plugin binary
if (PLUGIN_DATAINTERFACE)
  SET(PLUGINDIR "DataInterfacePlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# DataCallback Plugin binary
if (PLUGIN_DATACALLBACK)
  SET(PLUGINDIR "DataCallbackPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EphemPropagator Plugin binary
if (PLUGIN_EPHEMPROPAGATOR)
  SET(PLUGINDIR "EphemPropagatorPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Estimation Plugin binary
if (PLUGIN_ESTIMATION)
  SET(PLUGINDIR "EstimationPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EventLocator Plugin binary
if (PLUGIN_EVENTLOCATOR)
  SET(PLUGINDIR "EventLocatorPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ExtraPropagators Plugin binary
if (PLUGIN_EXTRAPROPAGATORS)
  SET(PLUGINDIR "ExtraPropagatorsPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# FminconOptimizer Plugin binary
if (PLUGIN_FMINCONOPTIMIZER)
  SET(PLUGINDIR "FminconOptimizerPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Formation Plugin binary
if (PLUGIN_FORMATION)
  SET(PLUGINDIR "FormationPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# GeometricMeasurement Plugin binary
if (PLUGIN_GEOMETRICMEASUREMENT)
  SET(PLUGINDIR "GeometricMeasurementPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# GmatFunction Plugin binary
if (PLUGIN_GMATFUNCTION)
  SET(PLUGINDIR "GmatFunctionPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# MatlabInterface Plugin binary
if (PLUGIN_MATLABINTERFACE)
  SET(PLUGINDIR "MatlabInterfacePlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ProductionPropagators Plugin binary
if (PLUGIN_PRODUCTIONPROPAGATORS)
  SET(PLUGINDIR "ProductionPropagatorPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# SaveCommand Plugin binary
if (PLUGIN_SAVECOMMAND)
  SET(PLUGINDIR "SaveCommandPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Station Plugin binary
if (PLUGIN_STATION)
  SET(PLUGINDIR "StationPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Proprietary plugins

# Search for proprietary plugin folder in default paths
# User can change this via CMake GUI or command line
FIND_PATH(GMAT_PROPRIETARYPLUGINS_PATH
  NAMES code/CMakeLists.txt
  PATHS
    ${PROJECT_SOURCE_DIR}/../gmatinternal-git
    ${PROJECT_SOURCE_DIR}/plugins/gmatinternal-git
  DOC "Path to GMAT proprietary plugins"
  NO_DEFAULT_PATH
  )

# Load CMakeLists.txt file from top-level proprietary plugins folder
if(GMAT_PROPRIETARYPLUGINS_PATH)
  SET(PLUGINDIR "${GMAT_PROPRIETARYPLUGINS_PATH}/code")
  ADD_SUBDIRECTORY(${PLUGINDIR} "${CMAKE_CURRENT_BINARY_DIR}/internal")
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION PluginTargets)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Set plugin properties all at once
if(PluginTargets)
  # Specify the library version
  SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES VERSION ${GMAT_VERSION})

  # Specify link flags for Win32 targets
  if(WIN32)
    SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\"")
  endif()
endif()
