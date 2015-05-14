# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Geometric Measurement Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/GeometricMeasurementPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Geometric Measurement Plugin setup " ${VERSION}) 

SET(TargetName GeometricMeasurements)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DGEOMETRICMEAS_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include measurement plugin)

# ====================================================================
# extra include directories for header files
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/command")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/estimator")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/event")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/factory")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/hardware")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/include")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/measurement")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/measurementfile")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/plugin")
INCLUDE_DIRECTORIES("../../../EstimationPlugin/src/base/trackingsystem")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/MeasurementFactory.cpp
    measurement/GeometricAzEl.cpp
    measurement/GeometricRADec.cpp
    measurement/GeometricRange.cpp
    measurement/GeometricRangeRate.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} GmatEstimation)
