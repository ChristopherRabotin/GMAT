# $Id: Makefile 35 2012-10-08 16:30:01Z djcinsb $
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

CPP = g++

GMAT_SRC_LOCATION = /Users/djc/TS_Code/GmatDevelopment/src/base
GMAT_LIB_LOCATION = /Users/djc/TS_Code/GmatDevelopment/application/bin

PLATFORM_FLAGS=-fPIC -DBOOST_NO_RTTI -DBOOST_NO_TYPEID
MAC_ARCHITECTURE_FLAGS = -arch i386

SHARED_LIB_FLAGS = -dylib -dynamiclib -two_levelnamespace \
                   -undefined dynamic_lookup \
                   -L$(GMAT_LIB_LOCATION) -lGmatBase $(PLATFORM_FLAGS)\
                   $(MAC_ARCHITECTURE_FLAGS)
                   
#AVRO_FLAGS = -D_THREAD_SAFE -I$(AVRO_LOCATION)/include -D_THREAD_SAFE \
#                       -L$(AVRO_LOCATION)/lib -lz
                   
CPPFLAGS = -fno-strict-aliasing -funroll-loops -fno-rtti \
           -ffriend-injection $(MAC_ARCHITECTURE_FLAGS)

TARGET = ../lib/libConsolePrint.dylib

all: $(TARGET)

clean:
	rm -rf */*.o
	rm -rf $(TARGET)

OBJECTS = \
          command/ConsolePrint.o \
          factory/PrintFactory.o \
          plugin/GmatPluginFunctions.o

INCLUDES = \
          -Iinclude \
          -Icommand \
          -Ifactory \
          -Iplugin \
          -I$(GMAT_SRC_LOCATION)/include \
          -I$(GMAT_SRC_LOCATION)/util \
          -I$(GMAT_SRC_LOCATION)/foundation \
          -I$(GMAT_SRC_LOCATION)/coordsystem \
          -I$(GMAT_SRC_LOCATION)/solarsys \
          -I$(GMAT_SRC_LOCATION)/command \
          -I$(GMAT_SRC_LOCATION)/executive \
          -I$(GMAT_SRC_LOCATION)/factory \
          -I$(GMAT_SRC_LOCATION)/subscriber \
          -I$(GMAT_SRC_LOCATION)/asset \
          -I$(GMAT_SRC_LOCATION)/attitude \
          -I$(GMAT_SRC_LOCATION)/hardware \
          -I$(GMAT_SRC_LOCATION)/parameter \
          -I$(GMAT_SRC_LOCATION)/spacecraft

ifeq ($(MAC_SPECIFIC), 1)
$(TARGET): $(OBJECTS)
	mkdir -p ../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	install_name_tool -change lib/libGmatBase.dylib ../bin/libGmatBase.dylib $(TARGET)
	mv $(TARGET) $(GMAT_LIB_LOCATION)/../plugins
else
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
endif

$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<
