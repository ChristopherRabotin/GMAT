# $Id: Makefile 1343 2011-03-31 18:47:39Z wshoan@NDC $
# Makefile for GMAT's OD Plugin
# This is the base level makefile 
#
# This file builds the plug-in base code

include ../OdEnv.mk

# handle different bin and lib directories here; for now, set to bin and lib for non-Mac platforms
ifeq ($(PLATFORM), mac)
else
GMAT_LIB_DIR = lib
GMAT_BIN_DIR = bin
GMAT_PLUGIN_DIR = plugins
endif

ifeq ($(LINUX_MAC), 1)

ifeq ($(PLATFORM), mac)
SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -dylib -dynamiclib -two_levelnamespace -undefined dynamic_lookup \
                   -L$(GMAT_Code_Location)/base/$(GMAT_LIB_DIR) -l$(BASE_LIBRARY) \
                    $(MAC_CPP_FLAGS) $(ARCHITECTURE_FLAGS) $(LINK_FLAGS)
                   
# -reexport-lGmatBaseNoMatlab -dylib -dynamiclib -Wl -two_levelnamespace -undefined dynamic_lookup

else
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared
# No longer in gcc 4.6:  --out-implib
endif

else

SHARED_EXTENSION = .dll
SHARED_LIB_FLAGS = -shared --out-implib $(LINK_FLAGS) -L$(GMAT_Code_Location)/base/$(GMAT_LIB_DIR) -lGmatBase

endif

TARGET = ../../$(GMAT_LIB_DIR)/libGmatEstimation$(SHARED_EXTENSION)


ifeq ($(USE_DATAFILE_PLUGIN), 0)
DATAFILE_INCLUDES = -Imeasurementfile

DATAFILE_OBJECTS = measurementfile/B3_obtype.o \
                   measurementfile/DataFile.o \
                   measurementfile/ObservationData.o \
                   measurementfile/ObType.o \
                   measurementfile/GmatObType.o \
                   factory/DataFileFactory.o \
                   factory/ObTypeFactory.o
else
DATAFILE_INCLUDES = -I$(GMAT_DataFile_Location)/base/datafile \
          -I$(GMAT_DataFile_Location)/base/datafile/CCSDS \
          -I$(GMAT_DataFile_Location)/base/datafile/CCSDS/CCSDSData \
          -I$(GMAT_DataFile_Location)/base/datafile/CCSDS/CCSDSMetaData \
          -I$(GMAT_DataFile_Location)/base/datafile/CCSDS/CCSDSObTypes

DATAFILE_OBJECTS = 
endif


ifeq ($(INCLUDE_IONOSPHERE), 1)
IONOSPHERE_C_OBJECTS = \
	measurement/Ionosphere/cira.o \
	measurement/Ionosphere/igrf.o \
	measurement/Ionosphere/iridreg.o \
	measurement/Ionosphere/irifun.o \
	measurement/Ionosphere/iritec.o \
	measurement/Ionosphere/irisub.o
IONOSPHERE_CPP_OBJECTS = \
	measurement/Ionosphere/Ionosphere.o 
else
IONOSPHERE_C_OBJECTS = 
IONOSPHERE_CPP_OBJECTS = 
endif


all: prep $(TARGET)

prep:
	rm -rf $(TARGET)

HEADERS = -Iinclude \
          -Icommand \
          -Iestimator \
          -Ievent \
          -Ifactory \
          -Ihardware \
          -Iplugin \
          -Imeasurement \
          -Imeasurement/Ionosphere \
          -Imeasurement/Troposphere \
          -Itrackingsystem \
          $(DATAFILE_INCLUDES) \
          -I$(GMAT_Code_Location)/base/include \
          -I$(GMAT_Code_Location)/base/foundation \
          -I$(GMAT_Code_Location)/base/event \
          -I$(GMAT_Code_Location)/base/executive \
          -I$(GMAT_Code_Location)/base/attitude \
          -I$(GMAT_Code_Location)/base/hardware \
          -I$(GMAT_Code_Location)/base/spacecraft \
          -I$(GMAT_Code_Location)/base/asset \
          -I$(GMAT_Code_Location)/base/forcemodel \
          -I$(GMAT_Code_Location)/base/propagator \
          -I$(GMAT_Code_Location)/base/subscriber \
          -I$(GMAT_Code_Location)/base/command \
          -I$(GMAT_Code_Location)/base/solarsys \
          -I$(GMAT_Code_Location)/base/factory \
          -I$(GMAT_Code_Location)/base/stopcond \
          -I$(GMAT_Code_Location)/base/interpreter \
          -I$(GMAT_Code_Location)/base/util \
          -I$(GMAT_Code_Location)/base/interpolator \
          -I$(GMAT_Code_Location)/base/interface \
          -I$(GMAT_Code_Location)/base/parameter \
          -I$(GMAT_Code_Location)/base/configs \
          -I$(GMAT_Code_Location)/base/burn \
          -I$(GMAT_Code_Location)/base/solver \
          -I$(GMAT_Code_Location)/base/function \
          -I$(GMAT_Code_Location)/base/coordsystem \
          -I$(GMAT_Code_Location)/base/math \
          -I$(GMAT_Code_Location)/base/plugin \
          -I$(GMAT_Code_Location)/base/estimator

clean : archclean

archclean :
	rm -rf */*.o
	rm -rf measurement/Ionosphere/*.o
	rm -rf measurement/Troposphere/*.o
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

C_OBJECTS = \
    $(IONOSPHERE_C_OBJECTS) 

CPP_OBJECTS = \
    plugin/GmatPluginFunctions.o \
    command/RunEstimator.o \
    command/RunSimulator.o \
    estimator/Estimator.o \
    estimator/BatchEstimator.o \
    estimator/BatchEstimatorInv.o \
    estimator/BatchEstimatorSVD.o \
    estimator/SequentialEstimator.o \
    estimator/ExtendedKalmanInv.o \
    estimator/EstimationStateManager.o \
    estimator/EstimatorException.o \
    estimator/Simulator.o \
    event/Event.o \
    event/EventException.o \
    event/EventData.o \
    event/EventManager.o \
    event/LightTimeCorrection.o \
    event/EstimationRootFinder.o \
    factory/EstimationCommandFactory.o \
    factory/EstimatorFactory.o \
    factory/EventFactory.o \
    factory/EstimatorHardwareFactory.o \
    factory/MeasurementFactory.o \
    factory/MeasurementModelFactory.o \
    factory/TrackingDataFactory.o \
    factory/TrackingSystemFactory.o \
    hardware/Sensor.o \
    hardware/Signal.o \
    hardware/RFHardware.o \
    hardware/Transmitter.o \
    hardware/Antenna.o \
    hardware/Receiver.o \
    hardware/Transponder.o \
    measurementfile/DataFileAdapter.o \
    measurement/CoreMeasurement.o \
    measurement/MediaCorrection.o \
    measurement/GeometricMeasurement.o \
    measurement/MeasurementData.o \
    measurement/MeasurementManager.o \
    measurement/MeasurementModel.o \
    measurement/PhysicalMeasurement.o \
    measurement/GeometricAzEl.o \
    measurement/GeometricRADec.o \
    measurement/GeometricRange.o \
    measurement/GeometricRangeRate.o \
    measurement/SnTwoWayRange.o \
    measurement/TwoWayRange.o \
    measurement/AveragedDoppler.o \
    measurement/TrackingData.o \
    measurement/DSNTwoWayRange.o \
    measurement/DSNTwoWayDoppler.o \
    measurement/TDRSSTwoWayRange.o \
    measurement/USNTwoWayRange.o \
    measurement/OpticalAngles.o \
    measurement/OpticalAzEl.o \
    measurement/Troposphere/Troposphere.o \
    $(IONOSPHERE_CPP_OBJECTS) \
    $(DATAFILE_OBJECTS) \
    trackingsystem/TrackingSystem.o \
    trackingsystem/DSNTrackingSystem.o \
    trackingsystem/OpticalTrackingSystem.o \
    trackingsystem/TDRSSTrackingSystem.o \
    trackingsystem/USNTrackingSystem.o

ifeq ($(PLATFORM), mac)
$(TARGET) : $(C_OBJECTS) $(CPP_OBJECTS)
	mkdir -p ../../$(GMAT_LIB_DIR)
	$(CPP) $(C_OBJECTS) $(CPP_OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS) $(PLATFORM_FLAGS)
	install_name_tool -change $(GMAT_LIB_DIR)/lib$(BASE_LIBRARY).dylib $(BASE_LIB_LOCATION)lib$(BASE_LIBRARY).dylib $(TARGET)
	cp $(TARGET) $(GMAT_Plugin_Location)/../$(GMAT_PLUGIN_DIR)
else
$(TARGET) : $(C_OBJECTS) $(CPP_OBJECTS)
	mkdir -p ../../$(GMAT_LIB_DIR)
	$(CPP) $(C_OBJECTS)  $(CPP_OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS) $(PLATFORM_FLAGS)
	cp $(TARGET) $(GMAT_Bin_Location)/../$(GMAT_PLUGIN_DIR)
endif


$(C_OBJECTS): %.o: %.c %.h
	$(C) $(CFLAGS) -c -o $@ $<

$(CPP_OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<
