# $Id$
# Architecture flags
# Set to 0 for Windows, 1 for Linux or Mac - this is needed by the Makefile
LINUX_MAC = 1

CSPICE_VER = cspice
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
GMAT_PLUGIN_DIR = plugins
MAC_ARCHITECTURE_FLAGS = 

# Environment settings for the GMAT C Interface plugin
# location of GMAT base headers and libraries
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/$(GMAT_BIN_DIR)

DEBUG_BUILD = 0


USE_SPICE = 1

ifeq ($(USE_SPICE), 1)
# location of CSPICE headers and libraries
# *** EDIT THIS *** -this is where you installed the version of CSPICE that you're using ...
SPICE_DIR = /home/djc/TS_Code/Gmat3rdParty
SPICE_INCLUDE = -I$(SPICE_DIR)/$(CSPICE_VER)/include
SPICE_LIB_DIR = $(SPICE_DIR)/$(CSPICE_VER)/lib
SPICE_LIBRARIES = $(SPICE_LIB_DIR)/cspice.a
SPICE_DIRECTIVE = -D__USE_SPICE__
SPICE_STACKSIZE = ulimit -s 61440
else
SPICE_INCLUDE =
SPICE_LIB_DIR =
SPICE_LIBRARIES =
SPICE_DIRECTIVE = 
SPICE_STACKSIZE = echo 'SPICE not included in this build ...'
endif

# Select the base library
BASE_LIBRARY = GmatBase

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared -Wl

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
