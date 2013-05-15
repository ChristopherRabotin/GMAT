# $Id$
# Mac OS X environment settings for a GMAT plugin
GMAT_CODE_LOCATION = /Users/djc/gsfccode/GmatDevelopment/src
GMAT_BIN_LOCATION  = /Users/djc/gsfccode/GmatDevelopment/application/bin

DEBUG_BUILD = 0

# Select the base library
BASE_LIBRARY = GmatBase

SAMPLE_CPP_FLAGS = -fPIC -arch i386 -fno-rtti
SAMPLE_LINK_FLAGS = -fPIC -arch i386 -fno-rtti

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

# 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 1

# Shared library settings
SHARED_EXTENSION = .dylib
SHARED_LIB_FLAGS = -bundle -two_levelnamespace -undefined dynamic_lookup \
                   -L$(GMAT_CODE_LOCATION)/base/lib -l$(BASE_LIBRARY) \
                    $(SAMPLE_LINK_FLAGS)

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(SAMPLE_CPP_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
