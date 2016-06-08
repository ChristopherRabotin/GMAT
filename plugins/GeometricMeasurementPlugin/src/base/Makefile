# Makefile for GMAT's OD Plugin
# This is the base level makefile 
#
# This file builds the plug-in base code

include ../GeoMeasEnv.mk

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
SHARED_LIB_FLAGS = -dylib -dynamiclib -undefined dynamic_lookup \
                   -L$(GMAT_Code_Location)/base/$(GMAT_LIB_DIR) -l$(BASE_LIBRARY) \
                    $(MAC_CPP_FLAGS) $(ARCHITECTURE_FLAGS) $(LINK_FLAGS)
                   
# -reexport-lGmatBaseNoMatlab -dylib -dynamiclib -Wl -undefined dynamic_lookup

else
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared -L$(GMAT_Code_Location)/base/$(GMAT_LIB_DIR) \
                   -lGmatBase -L$(GMAT_Estimation_Location)/../../lib \
                   -lGmatEstimation
endif

else

SHARED_EXTENSION = .dll
SHARED_LIB_FLAGS = -shared --out-implib $(LINK_FLAGS) -L$(GMAT_Code_Location)/base/$(GMAT_LIB_DIR) -lGmatBase

endif

TARGET = ../../$(GMAT_LIB_DIR)/libGeometricMeasurements$(SHARED_EXTENSION)


all: prep $(TARGET)

prep:
	rm -rf $(TARGET)

HEADERS = -Iinclude \
          -Ifactory \
          -Iplugin \
          -Imeasurement \
          -I$(GMAT_Estimation_Location)/include \
          -I$(GMAT_Estimation_Location)/measurement \
          -I$(GMAT_Estimation_Location)/hardware \
          -I$(GMAT_Estimation_Location)/event \
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
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

C_OBJECTS = \
    $(IONOSPHERE_C_OBJECTS) 

CPP_OBJECTS = \
    plugin/GmatPluginFunctions.o \
    factory/MeasurementFactory.o \
    measurement/GeometricAzEl.o \
    measurement/GeometricRADec.o \
    measurement/GeometricRange.o \
    measurement/GeometricRangeRate.o

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
