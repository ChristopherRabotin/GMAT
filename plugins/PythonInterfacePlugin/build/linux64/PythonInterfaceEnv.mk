# $Id$
# Linux environment settings for the SaveCommand GMAT plugin
GMAT_CODE_LOCATION = ../../../../src
GMAT_BIN_LOCATION  = ../../../../application/bin
PYTHON_LOCATION =  ../../../../../Gmat3rdParty/python
PYTHON_INCLUDE_LOCATION = $(PYTHON_LOCATION)/include/python3.4m
PYTHON_LIB_LOCATION = $(PYTHON_LOCATION)/lib
PYTHON_BIN_LOCATION = $(PYTHON_LOCATION)/bin
PYTHON_CONFIG=$(PYTHON_BIN_LOCATION)/python3.4-config

PYTHON_INCLUDE=`$(PYTHON_CONFIG) --cflags`
PYTHON_LINK=`$(PYTHON_CONFIG) --ldflags`


DEBUG_BUILD = 1

# Select the base library
BASE_LIBRARY = GmatBase

SAVECOMMAND_CPP_FLAGS = -fPIC
SAVECOMMAND_LINK_FLAGS = -fPIC

# Compiler options
CPP = g++
C = gcc

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# 0 for Linux, 1 for Mac 
MAC_SPECIFIC = 0

# Shared library settings
SHARED_EXTENSION = .so
SHARED_LIB_FLAGS = $(PYTHON_LINK) $(SAVECOMMAND_LINK_FLAGS) -shared -fPIC

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing $(WX_28_DEFINES)
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing $(WX_28_DEFINES)
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(SAVECOMMAND_CPP_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE)
