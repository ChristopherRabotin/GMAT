# $Id$
# makefile for the GMAT extra propagator Plugin
# This is the src level makefile 
#
# This file builds the plug-in base code

include ../ExtraPropagatorEnv.mk

TARGET = ../../lib/libExtraPropagators$(SHARED_EXTENSION)

all: prep $(TARGET)

prep:
	rm -rf $(TARGET)
	
HEADERS = -Iinclude \
          -Iplugin \
          -Ifactory \
          -Ipropagator \
          -I$(GMAT_CODE_LOCATION)/base/include \
          -I$(GMAT_CODE_LOCATION)/base/foundation \
          -I$(GMAT_CODE_LOCATION)/base/factory \
          -I$(GMAT_CODE_LOCATION)/base/util \
          -I$(GMAT_CODE_LOCATION)/base/forcemodel \
          -I$(GMAT_CODE_LOCATION)/base/hardware \
          -I$(GMAT_CODE_LOCATION)/base/propagator \
          -I$(GMAT_CODE_LOCATION)/base/solarsys \
          -I$(GMAT_CODE_LOCATION)/base/spacecraft \
          -I$(GMAT_CODE_LOCATION)/base/coordsystem \
          -I$(GMAT_CODE_LOCATION)/base/attitude \
          -I$(GMAT_CODE_LOCATION)/base/plugin 

clean : archclean

archclean :
	rm -rf */*.o
	rm -rf */*/*.o
	rm -rf */*/*/*.o
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

OBJECTS = \
    plugin/GmatPluginFunctions.o \
    factory/ExtraPropagatorFactory.o \
    propagator/BulirschStoer.o
   
ifeq ($(MAC_SPECIFIC), 1)
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS) -arch i386 -fPIC
	install_name_tool -change lib/lib$(BASE_LIBRARY).dylib \
	$(BASE_LIB_LOCATION)lib$(BASE_LIBRARY).dylib $(TARGET)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
else
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)/../plugins
endif


$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) -fno-rtti $(HEADERS) -c -o $@ $<
