# $Id$
# Architecture flags
# Set to 0 for Windows, 1 for Linux or Mac - this is needed by the Makefile
LINUX_MAC = 1

ifeq ($(BUILD_64BIT), 1)
#CSPICE_VER = cspice64
GMAT_BIN_DIR = bin64
GMAT_LIB_DIR = lib64
GMAT_PLUGIN_DIR = plugins64
MAC_ARCHITECTURE_FLAGS = 
else
#CSPICE_VER = cspice
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
GMAT_PLUGIN_DIR = plugins
MAC_ARCHITECTURE_FLAGS = -arch i386
endif

# Environment settings for the GMAT C Interface plugin
# location of GMAT base headers and libraries
GMAT_CODE_LOCATION = ../../../src
GMAT_BIN_LOCATION = ../../../application/$(GMAT_BIN_DIR)


DEBUG_BUILD = 0


#USE_SPICE = 1

#ifeq ($(USE_SPICE), 1)
## location of CSPICE headers and libraries
## *** EDIT THIS *** -this is where you installed the version of CSPICE that you're using ...
#SPICE_DIR = /Applications/CSPICE_N0064
#SPICE_INCLUDE = -I$(SPICE_DIR)/$(CSPICE_VER)/include
#SPICE_LIB_DIR = $(SPICE_DIR)/$(CSPICE_VER)/lib
#SPICE_LIBRARIES = $(SPICE_LIB_DIR)/cspice.a
#SPICE_DIRECTIVE = -D__USE_SPICE__
#SPICE_STACKSIZE = ulimit -s 61440
#else
#SPICE_INCLUDE =
#SPICE_LIB_DIR =
#SPICE_LIBRARIES =
#SPICE_DIRECTIVE = 
#SPICE_STACKSIZE = echo 'SPICE not included in this build ...'
#endif

# Select the base library
BASE_LIBRARY = GmatBase
# BASE_LIB_LOCATION is only used on Mac - must point to libGmatBase outside of bundle for Console GMAT
BASE_LIB_LOCATION = ../$(GMAT_BIN_DIR)/GMAT.app/Contents/Frameworks/

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -dylib -dynamiclib -two_levelnamespace -undefined dynamic_lookup -fPIC $(MAC_ARCHITECTURE_FLAGS)

# --out-implib

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -fno-rtti -fPIC
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -fno-rtti -fPIC
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

CPPFLAGS = $(CPP_BASE)
