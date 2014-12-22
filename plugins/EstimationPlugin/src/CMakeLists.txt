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

if(WIN32)
	SET(PLUGINTARGET  "libGmatEstimation")
else()
	SET(PLUGINTARGET "GmatEstimation")
endif()

SET(TargetName ${PLUGINTARGET})

# Add Preproccessor Definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DESTIMATION_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# specific compilation flags
if (UNIX)
	MESSAGE("-- Using a shared base library")
	IF(APPLE) # mac
		#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -dylib -dynamiclib -two_levelnamespace -undefined dynamic_lookup")
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -dylib -dynamiclib")

		# snow leopard
		#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -arch i386")
	endif()
endif (UNIX)

# Unix type systems need a change in the stack size
# Unix type systems include MAC, LINUX
MESSAGE("-- Executing... ulimit -s 61440")
execute_process(COMMAND ulimit -s 61440)

# ====================================================================
# include directories for header files
INCLUDE_DIRECTORIES(${GMAT_BASE_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES("./base/command")
INCLUDE_DIRECTORIES("./base/estimator")
INCLUDE_DIRECTORIES("./base/event")
INCLUDE_DIRECTORIES("./base/factory")
INCLUDE_DIRECTORIES("./base/hardware")
INCLUDE_DIRECTORIES("./base/include")
INCLUDE_DIRECTORIES("./base/measurement")
INCLUDE_DIRECTORIES("./base/measurement/Ionosphere")
INCLUDE_DIRECTORIES("./base/measurement/Troposphere")
INCLUDE_DIRECTORIES("./base/measurementfile")
INCLUDE_DIRECTORIES("./base/plugin")
INCLUDE_DIRECTORIES("./base/trackingsystem")

# ====================================================================
# source files
SET(PLUGIN_SRCS
    base/command/RunEstimator.cpp
    base/command/RunSimulator.cpp
    base/estimator/BatchEstimator.cpp
    base/estimator/BatchEstimatorInv.cpp
    base/estimator/BatchEstimatorSVD.cpp
    base/estimator/EstimationStateManager.cpp
    base/estimator/Estimator.cpp
    base/estimator/EstimatorException.cpp
    base/estimator/ExtendedKalmanInv.cpp
    base/estimator/SequentialEstimator.cpp
    base/estimator/Simulator.cpp
    base/event/EstimationRootFinder.cpp
    base/event/Event.cpp
    base/event/EventData.cpp
    base/event/EventException.cpp
    base/event/EventManager.cpp
    base/event/LightTimeCorrection.cpp
    base/factory/DataFileFactory.cpp
    base/factory/EstimationCommandFactory.cpp
    base/factory/EstimatorFactory.cpp
    base/factory/EstimatorHardwareFactory.cpp
    base/factory/EventFactory.cpp
    base/factory/MeasurementFactory.cpp
    base/factory/MeasurementModelFactory.cpp
    base/factory/ObTypeFactory.cpp
    base/factory/TrackingDataFactory.cpp
    base/factory/TrackingSystemFactory.cpp
    base/hardware/Antenna.cpp
    base/hardware/RFHardware.cpp
    base/hardware/Receiver.cpp
    base/hardware/Sensor.cpp
    base/hardware/Signal.cpp
    base/hardware/Transmitter.cpp
    base/hardware/Transponder.cpp
    base/measurement/AveragedDoppler.cpp
    base/measurement/CoreMeasurement.cpp
    base/measurement/DSNTwoWayDoppler.cpp
    base/measurement/DSNTwoWayRange.cpp
    base/measurement/MeasurementData.cpp
    base/measurement/MeasurementManager.cpp
    base/measurement/MeasurementModel.cpp
    base/measurement/MediaCorrection.cpp
    base/measurement/OpticalAngles.cpp
    base/measurement/OpticalAzEl.cpp
    base/measurement/PhysicalMeasurement.cpp
    base/measurement/SnTwoWayRange.cpp
    base/measurement/TDRSSTwoWayRange.cpp
    base/measurement/TrackingData.cpp
    base/measurement/TwoWayRange.cpp
    base/measurement/USNTwoWayRange.cpp
    base/measurement/Troposphere/Troposphere.cpp
    base/measurementfile/B3_obtype.cpp
    base/measurementfile/DataFile.cpp
    base/measurementfile/DataFileAdapter.cpp
    base/measurementfile/GmatObType.cpp
    base/measurementfile/ObType.cpp
    base/measurementfile/ObservationData.cpp
    base/plugin/GmatPluginFunctions.cpp
    base/trackingsystem/DSNTrackingSystem.cpp
    base/trackingsystem/OpticalTrackingSystem.cpp
    base/trackingsystem/TDRSSTrackingSystem.cpp
    base/trackingsystem/TrackingSystem.cpp
    base/trackingsystem/USNTrackingSystem.cpp
)

# Temporarily disable Ionosphere for Mac/Linux
if(WIN32)
  INCLUDE_DIRECTORIES("${F2C_DIR}")
  SET(PLUGIN_SRCS ${PLUGIN_SRCS}
    base/measurement/Ionosphere/Ionosphere.cpp
    base/measurement/Ionosphere/cira.c
    base/measurement/Ionosphere/igrf.c
    base/measurement/Ionosphere/iridreg.c
    base/measurement/Ionosphere/irifun.c
    base/measurement/Ionosphere/irisub.c
    base/measurement/Ionosphere/iritec.c
    )
endif()

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
