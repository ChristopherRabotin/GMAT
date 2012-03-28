# $Id: OdEnv.mk,v 1.1.1.1 2010/01/14 23:37:24 djc Exp $
# Architecture flags
# Set to 0 for Windows, 1 for Linux or Mac - needed by Makefile
LINUX_MAC = 1
ifeq ($(BUILD_64BIT), 1)
GMAT_BIN_DIR = bin64
GMAT_LIB_DIR = lib64
GMAT_PLUGIN_DIR = plugins64
MAC_ARCHITECTURE_FLAGS = 
else
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
GMAT_PLUGIN_DIR = plugins
MAC_ARCHITECTURE_FLAGS = -arch i386
endif

# Environment settings for the estimation plugin
GMAT_Code_Location = ../../../../../trunk/src
GMAT_Bin_Location = ../../../../../trunk/application/$(GMAT_BIN_DIR)
GMAT_Plugin_Location = ../../../../../trunk/application/$(GMAT_PLUGIN_DIR)
GMAT_DataFile_Location = ./../../DataFilePlugin/src  # TBD #

# Toggle for using the older datafile code or the plugin code
USE_DATAFILE_PLUGIN = 0

# Toggle for including the ionosphere code

INCLUDE_IONOSPHERE = 0

DEBUG_BUILD = 0

# Ionosphere code needs f2c.h and libf2c
ifeq ($(INCLUDE_IONOSPHERE), 1)
IONOSHPERE_FLAGS = -DIONOSPHERE
F2C_INCLUDE = -IC:/f2c   # TBD for Mac
F2C_LIB = C:/MinGW/lib/libf2c.a     # TBD for Mac
else
IONOSHPERE_FLAGS =
F2C_INCLUDE = 
F2C_LIB =
endif


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

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing $(WX_28_DEFINES) -fno-rtti
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing $(WX_28_DEFINES) -fno-rtti
endif

ifeq ($(MAC_CPP_FLAGS), 1)
MAC_CPP_FLAGS=-current_version 0.1 -compatibility_version 0.1 -fvisibility=default
endif

# Do not edit below this line -- here we build up longer compile/link strings
CPP_BASE = $(OPTIMIZATIONS) $(IONOSPHERE_FLAGS) -Wall \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(F2C_INCLUDE) $(CPP_BASE) $(MAC_CPP_FLAGS) $(PLATFORM_FLAGS) $(MAC_ARCHITECTURE_FLAGS)
CFLAGS = $(F2C_INCLUDE) -Wall 
LINK_FLAGS = $(F2C_LIB) $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

