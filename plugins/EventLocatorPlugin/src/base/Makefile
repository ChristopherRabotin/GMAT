# $Id: Makefile 2504 2012-05-17 15:24:19Z wshoan@NDC $
# 
# GMAT: General Mission Analysis Tool.
# 
# Copyright (c) 2002-2011 United States Government as represented by the
# Administrator of The National Aeronautics and Space Administration.
# All Other Rights Reserved.
#
# makefile for GMAT's Vf13Optimizer Plugin
# This is the base level makefile 
#
# This file builds the plug-in base code

include ../EventLocatorEnv.mk

# handle different bin and lib directories here; for now, set to bin and lib for non-Mac platforms
ifeq ($(PLATFORM), mac)
else
GMAT_LIB_DIR = lib
GMAT_BIN_DIR = bin
GMAT_PLUGIN_DIR = plugins
endif

TARGET = ../../$(GMAT_LIB_DIR)/libEventLocator$(SHARED_EXTENSION)


all: prep $(TARGET)

prep:
	rm -rf $(TARGET)

HEADERS = -Ievent \
          -Ilocator \
          -Iinclude \
          -Iplugin \
          -Ifactory \
          -I$(GMAT_CODE_LOCATION)/base/include \
          -I$(GMAT_CODE_LOCATION)/base/asset \
          -I$(GMAT_CODE_LOCATION)/base/attitude \
          -I$(GMAT_CODE_LOCATION)/base/event \
          -I$(GMAT_CODE_LOCATION)/base/interpolator \
          -I$(GMAT_CODE_LOCATION)/base/coordsystem \
          -I$(GMAT_CODE_LOCATION)/base/util \
          -I$(GMAT_CODE_LOCATION)/base/solarsys \
          -I$(GMAT_CODE_LOCATION)/base/hardware \
          -I$(GMAT_CODE_LOCATION)/base/spacecraft \
          -I$(GMAT_CODE_LOCATION)/base/foundation \
          -I$(GMAT_CODE_LOCATION)/base/factory \
          -I$(GMAT_CODE_LOCATION)/base/subscriber 


clean : archclean

archclean :
	rm -rf */*.o
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

CPP_OBJECTS = \
    plugin/GmatPluginFunctions.o \
    event/Elevation.o \
    event/Contact.o \
    event/Eclipse.o \
    event/Antumbra.o \
    event/Penumbra.o \
    event/Umbra.o \
    event/LineOfSight.o \
    locator/ContactLocator.o \
    locator/EclipseLocator.o \
    factory/EventLocatorFactory.o 

ifeq ($(PLATFORM), mac)
$(TARGET) : $(C_OBJECTS) $(CPP_OBJECTS)
	mkdir -p ../../$(GMAT_LIB_DIR)
	$(CPP) -o $(TARGET) $(C_OBJECTS) $(CPP_OBJECTS) $(SHARED_LIB_FLAGS)
	install_name_tool -change $(GMAT_LIB_DIR)/lib$(BASE_LIBRARY).dylib $(BASE_LIB_LOCATION)lib$(BASE_LIBRARY).dylib $(TARGET)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../$(GMAT_PLUGIN_DIR)
else
$(TARGET) : $(C_OBJECTS) $(CPP_OBJECTS)
	mkdir -p ../../$(GMAT_LIB_DIR)
	$(CPP) -o $(TARGET) $(C_OBJECTS) $(CPP_OBJECTS) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../$(GMAT_PLUGIN_DIR)
endif

$(C_OBJECTS): %.o: %.c %.h
	$(C) $(CFLAGS) $(HEADERS) -c -o $@ $<
    
$(CPP_OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<
