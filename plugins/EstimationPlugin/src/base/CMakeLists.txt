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

if(NOT XERCES_LIB_DIR)
  MESSAGE(WARNING "Skipping Estimation: Xerces missing")
  RETURN()
endif()

SET(TargetName GmatEstimation)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DESTIMATION_EXPORTS")

# ====================================================================
# extra include directory for header file
INCLUDE_DIRECTORIES(${XERCES_INCLUDE_DIR})

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS
  adapter
  command
  datafilter
  errormodel
  estimator
  event
  factory
  hardware
  include
  measurement
  measurement/Ionosphere
  measurement/Troposphere
  measurementfile
  measurementmodel
  plugin
  propagator
  reporter
  signal
  tdmReader
  trackingfile
  trackingsystem
  )

# ====================================================================
# Include F2C
INCLUDE_DIRECTORIES(${F2C_DIR})

# ====================================================================
# source files
SET(PLUGIN_SRCS
    adapter/DopplerAdapter.cpp
    adapter/DSNRangeAdapter.cpp
    adapter/GNDopplerAdapter.cpp
    adapter/PointRangeRateAdapterKps.cpp
    adapter/RangeAdapterKm.cpp
    adapter/RangeRateAdapterKps.cpp
    adapter/TDRSDopplerAdapter.cpp
    adapter/TrackingDataAdapter.cpp
    command/RunEstimator.cpp
    command/RunSimulator.cpp
    datafilter/DataFilter.cpp
    datafilter/StatisticAcceptFilter.cpp
    datafilter/StatisticRejectFilter.cpp
    errormodel/ErrorModel.cpp
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
    factory/AdapterFactory.cpp
    factory/DataFileFactory.cpp
    factory/ErrorModelFactory.cpp
    factory/EstimationCommandFactory.cpp
    factory/EstimationDataFilterFactory.cpp
    factory/EstimatorFactory.cpp
    factory/EstimatorHardwareFactory.cpp
    factory/EventFactory.cpp
    factory/MeasurementFactory.cpp
    factory/MeasurementModelFactory.cpp
    factory/NavPropagatorFactory.cpp
    factory/ObTypeFactory.cpp
    factory/SignalModelFactory.cpp
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
    measurement/MeasurementModelBase.cpp
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
    measurementfile/GmatData.cpp
    measurementfile/GmatObType.cpp
    measurementfile/GmatODDopplerType.cpp
    measurementfile/GmatODType.cpp
    measurementfile/ObservationData.cpp
    measurementfile/ObType.cpp
    measurementfile/RampTableData.cpp
    measurementfile/RampTableType.cpp
    measurementmodel/MeasureModel.cpp
    plugin/GmatPluginFunctions.cpp
    propagator/RungeKutta4.cpp
    reporter/ProgressReporter.cpp
    signal/PhysicalSignal.cpp
    signal/SignalBase.cpp
    signal/SignalData.cpp
    signal/SinglePointSignal.cpp
    tdmReader/TdmErrorHandler.cpp
    tdmReader/TdmObType.cpp
    tdmReader/TdmReadWriter.cpp
    trackingfile/TFSMagicNumbers.cpp
    trackingfile/TrackingFileSet.cpp
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
    measurement/Ionosphere/iri2007.c
    )
endif()

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# ====================================================================
# Additional link libraries
if(UNIX)
  if(APPLE)
    # Check this for Mac
    FIND_LIBRARY(XERCES_LIB ${XERCES_LIBRARY} ${XERCES_LIB_DIR})
  else()
    FIND_LIBRARY(XERCES_LIB ${XERCES_LIBRARY} ${XERCES_LIB_DIR})
  endif()
else()
  set(XERCES_LIB ${XERCES_LIB_DIR}/${XERCES_LIBRARY}) 
  set(WS2_32_LIB Ws2_32)
endif()

TARGET_LINK_LIBRARIES(${TargetName} ${XERCES_LIB})
TARGET_LINK_LIBRARIES(${TargetName} ${WS2_32_LIB})