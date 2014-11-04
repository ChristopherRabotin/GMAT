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


# ====================================================================
# Output directories based on configuration type.
# (Make all output go to the /gmat/application/bin/ directory.

# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application/plugins" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application/plugins" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application/plugins" )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/application/plugins" )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/application/plugins" )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${PROJECT_SOURCE_DIR}/application/plugins" )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

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
# CInterface Plugin binary
if (PLUGIN_DATAINTERFACE)
  SET(PLUGINDIR "DataInterfacePlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EphemPropagator Plugin binary
if (PLUGIN_EPHEMPROPAGATOR)
  SET(PLUGINDIR "EphemPropagatorPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Estimation Plugin binary
if (PLUGIN_ESTIMATION)
  SET(PLUGINDIR "EstimationPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EventLocator Plugin binary
if (PLUGIN_EVENTLOCATOR)
  SET(PLUGINDIR "EventLocatorPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ExtraPropagators Plugin binary
if (PLUGIN_EXTRAPROPAGATORS)
  SET(PLUGINDIR "ExtraPropagatorsPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# FminconOptimizer Plugin binary
if (PLUGIN_FMINCONOPTIMIZER)
  SET(PLUGINDIR "FminconOptimizerPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Formation Plugin binary
if (PLUGIN_FORMATION)
  SET(PLUGINDIR "FormationPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# GeometricMeasurement Plugin binary
if (PLUGIN_GEOMETRICMEASUREMENT)
  SET(PLUGINDIR "GeometricMeasurementPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# GmatFunction Plugin binary
if (PLUGIN_GMATFUNCTION)
  SET(PLUGINDIR "GmatFunctionPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# MatlabInterface Plugin binary
if (PLUGIN_MATLABINTERFACE)
  SET(PLUGINDIR "MatlabInterfacePlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ProductionPropagators Plugin binary
if (PLUGIN_PRODUCTIONPROPAGATORS)
  SET(PLUGINDIR "ProductionPropagatorPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# SaveCommand Plugin binary
if (PLUGIN_SAVECOMMAND)
  SET(PLUGINDIR "SaveCommandPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Station Plugin binary
if (PLUGIN_STATION)
  SET(PLUGINDIR "StationPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# Set plugin properties all at once
if(PluginTargets)
  # Specify the library version
  SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES VERSION ${GMAT_VERSION})

  # Specify link flags for Win32 targets
  if(WIN32)
    SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\"")
  endif()

  # Enable RPATH on Mac so that the build path is not hardcoded into
  # the library's internal name. This is necessary to use GMAT from
  # a 3rd-party app such as Matlab. Note that this will no longer be
  # necessary once the project moves to CMake 3.0.
  if(APPLE)
    SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES MACOSX_RPATH TRUE)
  endif()
endif()
