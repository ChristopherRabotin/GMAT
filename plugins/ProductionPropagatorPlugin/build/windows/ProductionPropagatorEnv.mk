# $Id$
# Environment settings for the extra propagator plugin

GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/bin

# Set to 1 for debugging and profiling
F2C_REQUIRED = 0

# Set to 1 if f2c required
DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# Requires f2c
ifeq ($F2C_REQUIRED), 1)
F2C_LOC = ../../../../../Gmat3rdParty/f2c
F2C_INC = -I$(F2C_LOC)/include
F2C_LIB = $(F2C_LOC)/lib/libf2c.a
else
F2C_LOC = 
F2C_INC = 
F2C_LIB = 
endif

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif


SHARED_EXTENSION = .dll
SHARED_LIB_FLAGS = -shared -Wl --out-implib \
                   -L$(GMAT_CODE_LOCATION)/base/lib -l$(BASE_LIBRARY) \
                   $(F2C_LIB)

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) $(F2C_INC)
CFLAGS = -Wall $(F2C_INC)
