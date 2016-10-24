# $Id$
# Architecture flags
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
GMAT_PLUGIN_DIR = plugins

ifeq ($(BUILD_64BIT), 1)
MAC_ARCHITECTURE_FLAGS =
F2C_DIR = f2c64
else
MAC_ARCHITECTURE_FLAGS = -arch i386
F2C_DIR = f2c32
endif

# ifeq ($(BUILD_64BIT), 1)
# CSPICE_VER = cspice64
# MAC_ARCHITECTURE_FLAGS =
# else
# CSPICE_VER = cspice
# MAC_ARCHITECTURE_FLAGS = -arch i386
# endif

# Environment settings for the EventLocator plugin
# location of GMAT base headers and libraries
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/$(GMAT_BIN_DIR)

DEBUG_BUILD = 0

USE_SPICE = 0

ifeq ($(USE_SPICE), 1)
# location of CSPICE headers and libraries
# *** EDIT THIS *** -this is where you installed the version of CSPICE that you're using ...
SPICE_DIR = /Applications/CSPICE_N0065
SPICE_INCLUDE = -I$(SPICE_DIR)/$(CSPICE_VER)/include
#SPICE_LIB_DIR = $(SPICE_DIR)/$(CSPICE_VER)/lib
#SPICE_LIBRARIES = $(SPICE_LIB_DIR)/cspice.a
#SPICE_DIRECTIVE = -D__USE_SPICE__
#SPICE_STACKSIZE = ulimit -s 61440
else
SPICE_INCLUDE =
SPICE_LIB_DIR =
SPICE_LIBRARIES =
SPICE_DIRECTIVE = 
SPICE_STACKSIZE = echo 'SPICE not included in this build ...'
endif

# Environment settings for the event locator plugin; Linux 64-bit version

GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/$(GMAT_BIN_DIR)

# Set to 1 for debugging and profiling
DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# BASE_LIB_LOCATION is only used on Mac - must point to libGmatBase outside of bundle for Console GMAT
BASE_LIB_LOCATION = ../$(GMAT_BIN_DIR)/GMAT.app/Contents/Frameworks/

# Uncomment if we ever require f2c
##F2C_DIR = ../../../../GMAT_3rdParty/f2c
#F2C_DIR = ../../../../GMAT_3rdParty/f2c
#F2C_LIBDIR = $(F2C_DIR)/lib
#F2C_HEADERDIR = $(F2C_DIR)/include
#F2C_LIB = -L$(F2C_LIBDIR) -lf2c -lm

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -funroll-loops -fno-rtti
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -funroll-loops -fno-rtti
endif

PLATFORM_FLAGS=-fPIC

SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -dylib -dynamiclib -undefined dynamic_lookup -headerpad_max_install_names\
                   -L$(GMAT_CODE_LOCATION)/base/$(GMAT_LIB_DIR) -l$(BASE_LIBRARY) $(PLATFORM_FLAGS)\
                    $(MAC_CPP_FLAGS) -lgcc_s.1 $(MAC_ARCHITECTURE_FLAGS) -bind_at_load
               
# $(WXLINKFLAGS) $(MATLAB_LIBRARIES    
# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall $(PLATFORM_FLAGS)\
           $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

CPPFLAGS = $(CPP_BASE)
CFLAGS = -Wall $(PLATFORM_FLAGS) $(MAC_ARCHITECTURE_FLAGS)
