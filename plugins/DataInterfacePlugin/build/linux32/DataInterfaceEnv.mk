# $Id$
# Linux environment settings for the DataInterface GMAT plugin
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION  = ../../../../application/bin

DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

SAMPLE_CPP_FLAGS = -fPIC
SAMPLE_LINK_FLAGS = -fPIC

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

# 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

# Shared library settings
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = $(SAMPLE_LINK_FLAGS) -shared -Wl

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(SAMPLE_CPP_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
