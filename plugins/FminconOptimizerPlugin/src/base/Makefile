# $Id: Makefile 9461 2011-04-21 22:10:15Z lindajun $
# 
# GMAT: General Mission Analysis Tool.
# 
#  Copyright (c) 2002-2011 United States Government as represented by the
#  Administrator of The National Aeronautics and Space Administration.
#  All Other Rights Reserved.
#
# makefile for GMAT's FminconOptimizer Plugin
# This is the base level makefile 
#
# This file builds the plug-in base code

include ../FminconOptimizerEnv.mk


TARGET = ../../lib/libFminconOptimizer$(SHARED_EXTENSION)


all: prep $(TARGET)

prep:
	rm -rf $(TARGET)

HEADERS = -Iinclude \
          -Iplugin \
          -Ifactory \
          -Isolver \
          -I$(GMAT_CODE_LOCATION)/base/include \
          -I$(GMAT_CODE_LOCATION)/base/attitude \
          -I$(GMAT_CODE_LOCATION)/base/coordsystem \
          -I$(GMAT_CODE_LOCATION)/base/util \
          -I$(GMAT_CODE_LOCATION)/base/solarsys \
          -I$(GMAT_CODE_LOCATION)/base/hardware \
          -I$(GMAT_CODE_LOCATION)/base/spacecraft \
          -I$(GMAT_CODE_LOCATION)/base/foundation \
          -I$(GMAT_CODE_LOCATION)/base/factory \
          -I$(GMAT_CODE_LOCATION)/base/function \
          -I$(GMAT_CODE_LOCATION)/base/parameter \
          -I$(GMAT_CODE_LOCATION)/base/solver \
          -I$(GMAT_CODE_LOCATION)/base/subscriber \
          -I$(GMAT_CODE_LOCATION)/base/interface \
          -I$(GMAT_CODE_LOCATION)/base/command \
          -I$(GMAT_CODE_LOCATION)/base/executive \
          -I$(GMAT_CODE_LOCATION)/base/interpreter \
          -I$(GMAT_CODE_LOCATION)/base/forcemodel \
          -I$(GMAT_CODE_LOCATION)/gui/include \
          -I$(GMAT_CODE_LOCATION)/gui/app \
          -I$(MATLAB_IF_CODE_LOCATION)/base/include \
          -I$(MATLAB_IF_CODE_LOCATION)/base/interface


clean : archclean

archclean :
	rm -rf */*.o
	rm -rf $(TARGET)
	rm -rf */*~
	rm -rf core
	rm -rf */core

OBJECTS = \
    plugin/GmatPluginFunctions.o \
    factory/FminconOptimizerFactory.o \
    solver/FminconOptimizer.o 

ifeq ($(MAC_SPECIFIC), 1)
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	install_name_tool -change lib/lib$(BASE_LIBRARY).dylib $(BASE_LIB_LOCATION)lib$(BASE_LIBRARY).dylib $(TARGET)
	cp $(TARGET) $(GMAT_Bin_Location)
else
$(TARGET) : $(OBJECTS)
	mkdir -p ../../lib
	$(CPP) $(OBJECTS) -o $(TARGET) $(SHARED_LIB_FLAGS)
	cp $(TARGET) $(GMAT_BIN_LOCATION)
endif


$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<
    
