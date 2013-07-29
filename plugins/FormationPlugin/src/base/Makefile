# $Id$
# Makefile for plugin command to write to the GMAT console or message window
#
# This is not production code, and must not be treated as if it were!
#
# Written by:     Darrel J. Conway
#                 Thinking Systems, Inc.
#
# Contract:       Custom work performed at Thinking Systems, Inc.
#                 Copyright (c) 2012, Thinking Systems, Inc.
#                 Usage under Academic Free Licence permitted

include ../FormationEnv.mk

CPP = g++

ifeq ($(MAC_SPECIFIC), 1)
PLATFORM_FLAGS=-fPIC -DBOOST_NO_RTTI -DBOOST_NO_TYPEID
MAC_ARCHITECTURE_FLAGS = -arch i386

SHARED_LIB_FLAGS = -dylib -dynamiclib -two_levelnamespace \
                   -undefined dynamic_lookup \
                   -L$(GMAT_BIN_LOCATION) -lGmatBase $(PLATFORM_FLAGS)\
                   $(MAC_ARCHITECTURE_FLAGS)
TARGET = ../lib/libFormation.dylib
else
TARGET = ../lib/libFormation$(SHARED_EXTENSION)
endif

                   
#AVRO_FLAGS = -D_THREAD_SAFE -I$(AVRO_LOCATION)/include -D_THREAD_SAFE \
#                       -L$(AVRO_LOCATION)/lib -lz
                   
CPPFLAGS = -fno-strict-aliasing -funroll-loops -fno-rtti \
           -ffriend-injection $(MAC_ARCHITECTURE_FLAGS)


all: $(TARGET)

clean:
	rm -rf */*.o
	rm -rf $(TARGET)

OBJECTS = \
          formation/Formation.o \
          factory/FormationFactory.o \
          plugin/GmatPluginFunctions.o

INCLUDES = \
          -Iinclude \
          -Iformation \
          -Ifactory \
          -Iplugin \
          -I$(GMAT_CODE_LOCATION)/base/include \
          -I$(GMAT_CODE_LOCATION)/base/util \
          -I$(GMAT_CODE_LOCATION)/base/foundation \
          -I$(GMAT_CODE_LOCATION)/base/coordsystem \
          -I$(GMAT_CODE_LOCATION)/base/solarsys \
          -I$(GMAT_CODE_LOCATION)/base/command \
          -I$(GMAT_CODE_LOCATION)/base/executive \
          -I$(GMAT_CODE_LOCATION)/base/factory \
          -I$(GMAT_CODE_LOCATION)/base/subscriber \
          -I$(GMAT_CODE_LOCATION)/base/asset \
          -I$(GMAT_CODE_LOCATION)/base/attitude \
          -I$(GMAT_CODE_LOCATION)/base/hardware \
          -I$(GMAT_CODE_LOCATION)/base/parameter \
          -I$(GMAT_CODE_LOCATION)/base/spacecraft

ifeq ($(MAC_SPECIFIC), 1)
$(TARGET): $(OBJECTS)
	mkdir -p ../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	install_name_tool -change lib/libGmatBase.dylib $(BASE_LIB_LOCATION)lib$(BASE_LIBRARY).dylib $(TARGET)
	mv $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
else
$(TARGET) : $(OBJECTS)
	mkdir -p ../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
endif

$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<
