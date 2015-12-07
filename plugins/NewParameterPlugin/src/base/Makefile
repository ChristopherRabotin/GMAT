# $Id: Makefile 2228 2012-03-28 17:36:30Z  $
# 
# GMAT: General Mission Analysis Tool.
# 
# Copyright (c) 2002-2014 United States Government as represented by the
# Administrator of The National Aeronautics and Space Administration.
# All Other Rights Reserved.
#
# makefile for GMAT's Vf13Optimizer Plugin
# This is the base level makefile 
#
# This file builds the plug-in base code

include ../NewParameterEnv.mk

# handle different bin and lib directories here; for now, set to bin and lib for non-Mac platforms
ifeq ($(PLATFORM), mac)
else
GMAT_LIB_DIR = lib
GMAT_BIN_DIR = bin
GMAT_PLUGIN_DIR = plugins
endif

TARGET = ../../$(GMAT_LIB_DIR)/libNewParameters$(SHARED_EXTENSION)


all: prep $(TARGET)

prep:
	rm -rf $(TARGET)

HEADERS = -Iinclude \
          -Iplugin \
          -Ifactory \
          -Iparameter \
          -I$(GMAT_CODE_LOCATION)/base/include \
          -I$(GMAT_CODE_LOCATION)/base/asset \
          -I$(GMAT_CODE_LOCATION)/base/attitude \
          -I$(GMAT_CODE_LOCATION)/base/burn \
          -I$(GMAT_CODE_LOCATION)/base/command \
          -I$(GMAT_CODE_LOCATION)/base/coordsystem \
          -I$(GMAT_CODE_LOCATION)/base/event \
          -I$(GMAT_CODE_LOCATION)/base/factory \
          -I$(GMAT_CODE_LOCATION)/base/forcemodel \
          -I$(GMAT_CODE_LOCATION)/base/foundation \
          -I$(GMAT_CODE_LOCATION)/base/hardware \
          -I$(GMAT_CODE_LOCATION)/base/interpolator \
          -I$(GMAT_CODE_LOCATION)/base/parameter \
          -I$(GMAT_CODE_LOCATION)/base/propagator \
          -I$(GMAT_CODE_LOCATION)/base/solarsys \
          -I$(GMAT_CODE_LOCATION)/base/solver \
          -I$(GMAT_CODE_LOCATION)/base/spacecraft \
          -I$(GMAT_CODE_LOCATION)/base/subscriber \
          -I$(GMAT_CODE_LOCATION)/base/util


clean : archclean

archclean :
	rm -rf */*.o
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

CPP_OBJECTS = \
    plugin/GmatPluginFunctions.o \
    factory/NewParameterFactory.o \
    parameter/OdeData.o \
    parameter/OdeReal.o \
    parameter/FMAcceleration.o \
    parameter/FMDensity.o \
    parameter/SolverData.o \
    parameter/SolverReal.o \
    parameter/SolverString.o \
    parameter/SolverState.o \
    parameter/SolverStatus.o

ifeq ($(PLATFORM), mac)
$(TARGET) : $(CPP_OBJECTS)
	mkdir -p ../../$(GMAT_LIB_DIR)
	$(CPP) -o $(TARGET) $(CPP_OBJECTS) $(SHARED_LIB_FLAGS)
	install_name_tool -change $(GMAT_LIB_DIR)/lib$(BASE_LIBRARY).dylib \
	$(BASE_LIB_LOCATION)/lib$(BASE_LIBRARY).dylib ../../$(GMAT_LIB_DIR)/libNewParameters.dylib
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../$(GMAT_PLUGIN_DIR)
else
$(TARGET) : $(CPP_OBJECTS)
	mkdir -p ../../$(GMAT_LIB_DIR)
	$(CPP) -o $(TARGET) $(CPP_OBJECTS) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../$(GMAT_PLUGIN_DIR)
endif

$(C_OBJECTS): %.o: %.c %.h
	$(C) $(CFLAGS) $(HEADERS) -c -o $@ $<
    
$(CPP_OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<
    
