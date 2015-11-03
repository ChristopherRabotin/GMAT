# $Id: Vf13OptimizerEnv.mk 532 2010-08-26 16:58:53Z  $
# Environment settings for the Vf13Optimizer plugin; Linux 64-bit version

GMAT_CODE_LOCATION = ../../../../../GmatDevelopment/src
GMAT_BIN_LOCATION = ../../../../../GmatDevelopment/application/bin

# Set to 1 for debugging and profiling
DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# Requires f2c
THIRD_PARTY_DIR = /home/djc/gsfccode/Gmat3rdParty
#F2C_DIR = $(THIRD_PARTY_DIR)/f2c
F2C_DIR = $(THIRD_PARTY_DIR)/cspice
F2C_LIBDIR = $(F2C_DIR)/lib
F2C_HEADERDIR = $(F2C_DIR)/include
#F2C_LIB = -L$(F2C_LIBDIR) -lf2c -lm

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif


SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = -shared -Wl \
                   -L$(GMAT_CODE_LOCATION)/base/lib -l$(BASE_LIBRARY) \
                   $(F2C_LIB)
                   
# Removed for GCC 4.6:  --out-implib

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall -fPIC\
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) -I$(F2C_HEADERDIR)
CFLAGS = -Wall -fPIC -I$(F2C_HEADERDIR)