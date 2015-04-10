# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Estimation Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/EstimationPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Estimation Plugin setup " ${VERSION}) 

# ====================================================================
# Major error checks
if(NOT EXISTS ${F2C_DIR})
  MESSAGE(WARNING "Skipping due to missing F2C")
  RETURN()
endif()

SET(TargetName GmatEstimation)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DESTIMATION_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command estimator event factory hardware include measurement measurement/Ionosphere measurement/Troposphere measurementfile plugin trackingsystem)

# ====================================================================
# Include F2C
INCLUDE_DIRECTORIES(${F2C_DIR})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/RunEstimator.cpp
    command/RunSimulator.cpp
    estimator/BatchEstimator.cpp
    estimator/BatchEstimatorInv.cpp
    estimator/BatchEstimatorSVD.cpp
    estimator/EstimationStateManager.cpp
    estimator/Estimator.cpp
    estimator/EstimatorException.cpp
    estimator/ExtendedKalmanInv.cpp
    estimator/SequentialEstimator.cpp
    estimator/Simulator.cpp
    event/EstimationRootFinder.cpp
    event/Event.cpp
    event/EventData.cpp
    event/EventException.cpp
    event/EventManager.cpp
    event/LightTimeCorrection.cpp
    factory/DataFileFactory.cpp
    factory/EstimationCommandFactory.cpp
    factory/EstimatorFactory.cpp
    factory/EstimatorHardwareFactory.cpp
    factory/EventFactory.cpp
    factory/MeasurementFactory.cpp
    factory/MeasurementModelFactory.cpp
    factory/ObTypeFactory.cpp
    factory/TrackingDataFactory.cpp
    factory/TrackingSystemFactory.cpp
    hardware/Antenna.cpp
    hardware/RFHardware.cpp
    hardware/Receiver.cpp
    hardware/Sensor.cpp
    hardware/Signal.cpp
    hardware/Transmitter.cpp
    hardware/Transponder.cpp
    measurement/AveragedDoppler.cpp
    measurement/CoreMeasurement.cpp
    measurement/DSNTwoWayDoppler.cpp
    measurement/DSNTwoWayRange.cpp
    measurement/MeasurementData.cpp
    measurement/MeasurementManager.cpp
    measurement/MeasurementModel.cpp
    measurement/MediaCorrection.cpp
    measurement/OpticalAngles.cpp
    measurement/OpticalAzEl.cpp
    measurement/PhysicalMeasurement.cpp
    measurement/SnTwoWayRange.cpp
    measurement/TDRSSTwoWayRange.cpp
    measurement/TrackingData.cpp
    measurement/TwoWayRange.cpp
    measurement/USNTwoWayRange.cpp
    measurement/Troposphere/Troposphere.cpp
    measurementfile/B3_obtype.cpp
    measurementfile/DataFile.cpp
    measurementfile/DataFileAdapter.cpp
    measurementfile/GmatObType.cpp
    measurementfile/ObType.cpp
    measurementfile/ObservationData.cpp
    plugin/GmatPluginFunctions.cpp
    trackingsystem/DSNTrackingSystem.cpp
    trackingsystem/OpticalTrackingSystem.cpp
    trackingsystem/TDRSSTrackingSystem.cpp
    trackingsystem/TrackingSystem.cpp
    trackingsystem/USNTrackingSystem.cpp
)

# Temporarily disable Ionosphere for Mac/Linux
if(WIN32)
  ADD_DEFINITIONS("-DIONOSPHERE")
  SET(PLUGIN_SRCS ${PLUGIN_SRCS}
    measurement/Ionosphere/Ionosphere.cpp
    measurement/Ionosphere/cira.c
    measurement/Ionosphere/igrf.c
    measurement/Ionosphere/iridreg.c
    measurement/Ionosphere/irifun.c
    measurement/Ionosphere/irisub.c
    measurement/Ionosphere/iritec.c
    )
endif()

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
