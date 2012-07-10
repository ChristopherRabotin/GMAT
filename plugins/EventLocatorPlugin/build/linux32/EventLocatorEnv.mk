# $Id: EventLocatorEnv.mk 1706 2011-09-10 00:13:03Z djconway@NDC $
# Environment settings for the event locator plugin; Linux 64-bit version

GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION = ../../../../application/bin

# Set to 1 for debugging and profiling
DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

# Uncomment if we ever require f2c
##F2C_DIR = ../../../../Gmat3rdParty/f2c
#F2C_DIR = ../../../../Gmat3rdParty/cspice
#F2C_LIBDIR = $(F2C_DIR)/lib
#F2C_HEADERDIR = $(F2C_DIR)/include
#F2C_LIB = -L$(F2C_LIBDIR) -lf2c -lm

# Use this if no f2c files used (Will be replaced with a later fix)
F2C_HEADERDIR = .

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
