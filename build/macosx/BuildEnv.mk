# $Id: BuildEnv.mk 6536 2009-04-20 16:24:12Z wendys-dev $
# 
# Build environment file for MAC OSX (Intel); currently supports only:
#    Snow Leopard (10.6)
#    Lion (10.7)
#
# NOTES to user - changes may be necessary where you see the "*** EDIT THIS ***" or 
# "*** EDIT THESE ***" notes
# -------------------------------------------------------------------------------------------

# ------> FLAGS USED TO CONTROL THE BUILD

## *** EDIT THIS ***
# ------> TOP DIRECTORY
# put the top of the GMAT project directory structure here .... make sure not to have trailing blanks!!!
TOP_DIR = <path-to-your-GMAT-directory>

## *** EDIT THIS ***
# ------> WX version
# select the wx version (wx2.8.12 will be 32-bit; wx 3.0.2 will be 64-bit)
# WX_VERSION must be in this format
WX_VERSION = 3
#WX_VERSION = 2

## *** EDIT THESE ***
# ------> WX installed location
#WX3_INSTALLED = /Applications/wxWidgets-3.0.1/build-cocoa-debug
WX3_INSTALLED = /Applications/wxWidgets-3.0.2/build-cocoa
WX2_INSTALLED = /Applications/wxMac-2.8.12/shared

## *** EDIT THIS ***
# ------> Console app (0-> full GUI build; 1 -> console only)
# if this is 1, it will build the base library and the console app only
CONSOLE_APP = 0

## *** EDIT THESE ***
# ------> Spice, f2c, debug, profile (these should be left at default for most cases)
USE_SPICE = 1
USE_F2C_VERSION = 1
DEBUG_BUILD = 0
PROFILE_BUILD = 0

## *** EDIT THIS ***
# ------> CSPICE
# this is where you installed the version of CSPICE that you're using 
# NOTE: it is assumed that inside this folder, there is a 'cspice' folder for 32-bit and
# a 'cspice64' folder for 64-bit
ifeq ($(USE_SPICE), 1)
SPICE_DIR = /Applications/CSPICE_N0065
# 0 -> use cpspice.a, 1 -> use libcspice.dylib (NOT CURRENTLY SUPPORTED) in bin directory
SPICE_USE_DYLIB = 0
else
SPICE_DIR =
endif

## *** EDIT THIS ***
# ------> F2C
# If you are using F2C, say where it is located; if not, point to fortran libraries
# This assumes that, in the f2c location you specify, you have 'f2c32' and 'f2c64' 
# folders
ifeq ($(USE_F2C_VERSION), 1)
F2C_LOC = <path-to-your-F2C-directory>
else
FORTRAN_LIB = -L/usr/local/gfortran/lib -lgfortran 
F2C_LOC = 
endif

## *** EDIT THIS ***
# ------> PCRE
# where is PCRE installed (NOTE: pcre NOT CURRENTLY USED)
PCRE_LIB_LOC = /Applications/pcre-8.12/.libs


# ****************************************************************************************
# *************************** DO NOT EDIT BELOW THIS LINE ********************************
# ****************************************************************************************
LINUX_MAC = 1
MAC_SPECIFIC = 1

ifeq ($(WX_VERSION),3)
WX_INSTALLED = $(WX3_INSTALLED)
WX_LIB_LOC = $(WX_INSTALLED)/lib
WX_LIB = -lwx_osx_cocoau_gl-3.0 -lwx_osx_cocoau_stc-3.0
BUILD_64BIT = 1
# STC_EDITOR now available on Mac (wx 3.0.n) - set this to 0 for Console version
USE_STC_EDITOR = 1
INSTALL_LIBS_INTO_BUNDLE = install_libs_into_bundle
else
WX_INSTALLED = $(WX2_INSTALLED)
WX_LIB_LOC = $(WX_INSTALLED)/lib
WX_LIB = -lwx_mac_gl-2.8
#WX_LIB_2 =
BUILD_64BIT = 0
USE_STC_EDITOR = 0
INSTALL_LIBS_INTO_BUNDLE = install_libs_into_bundle_2_8_12
endif

WX_28_SYNTAX = 0     # not currently modifiable

ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__
else
WX_28_DEFINES = 
endif

## bin and lib directories
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib

# flags that depend on whether you are building 32-bit or 64-bit
ifeq ($(BUILD_64BIT), 1)
CSPICE_VER = cspice64
MAC_ARCHITECTURE_FLAGS =
F2C_DIR = f2c64
PROCFLAGS = -DUSE_64_BIT_LONGS
else
CSPICE_VER = cspice
MAC_ARCHITECTURE_FLAGS = -arch i386
F2C_DIR = f2c32
PROCFLAGS =
endif

WXLINKFLAGS = `$(WX_INSTALLED)/wx-config --libs --universal=no --static=no`

ifeq ($(USE_STC_EDITOR), 1)
WXCPPFLAGS = `$(WX_INSTALLED)/wx-config --cppflags`  -D__WXMAC__ $(WX_28_DEFINES) -D__USE_STC_EDITOR__
else
WXCPPFLAGS = `$(WX_INSTALLED)/wx-config --cppflags`  -D__WXMAC__ $(WX_28_DEFINES)
endif


# flag indicating whether or not to build as a shared library
SHARED_BASE = 1

ifeq ($(USE_SPICE), 1)
SPICE_INCLUDE = -I$(SPICE_DIR)/$(CSPICE_VER)/include
SPICE_LIB_DIR = $(SPICE_DIR)/$(CSPICE_VER)/lib
ifeq ($(SPICE_USE_DYLIB), 1)
SPICE_LIBRARIES = -lcspice
else
SPICE_LIBRARIES = $(SPICE_LIB_DIR)/cspice.a
endif
SPICE_DIRECTIVE = -D__USE_SPICE__
SPICE_STACKSIZE = ulimit -s 61440
else
SPICE_INCLUDE =
SPICE_LIB_DIR =
SPICE_LIBRARIES =
SPICE_DIRECTIVE = 
SPICE_STACKSIZE = echo 'SPICE not included in this build ...'
endif

# set up F2C flags
ifeq ($(USE_F2C_VERSION), 1)
F2C_INCLUDE_PATH = $(F2C_LOC)/$(F2C_DIR)/include
F2C_LIB_PATH = $(F2C_LOC)/$(F2C_DIR)/lib
F2C_INCLUDE = -I$(F2C_INCLUDE_PATH)
FORTRAN_LIB = -L$(F2C_LIB_PATH) -lf2c
else
F2C_INCLUDE_PATH =
F2C_LIB_PATH =
F2C_INCLUDE = 
endif

EXECUTABLE 	= $(TOP_DIR)/application/$(GMAT_BIN_DIR)/GMAT

GMAT_INFO    = $(TOP_DIR)/src/gui/Info_GMAT.plist
CONTENTS_DIR = $(EXECUTABLE).app/Contents
MACOS_DIR    = $(CONTENTS_DIR)/MacOS
RES_DIR      = $(CONTENTS_DIR)/Resources
LIBS_DIR     = $(CONTENTS_DIR)/Frameworks
MAC_APP      = $(MACOS_DIR)/GMAT
MAC_SCRIPT_DIR   = $(MACOS_DIR)/
MAC_PKG      = $(CONTENTS_DIR)/Info.plist
MAC_PKGINFO  = $(CONTENTS_DIR)/PkgInfo
GMAT_ICONS   = $(TOP_DIR)/src/gui/resource/GMATMac.icns
MAC_ICONS    = $(RES_DIR)/gmat.icns


ifeq ($(PROFILE_BUILD), 1)
USE_PROFILING = 1
else
USE_PROFILING = 0
endif

#shared base settings
ifeq ($(SHARED_BASE), 1)
SHARED_BASE_FLAGS = -fno-common -dynamic
else
SHARED_BASE_FLAGS =
endif

#
# Compiler options
CPP = g++
C = gcc
PROFILE_FLAGS = -pg

SOME_OPTIMIZATIONS = -O3 -funroll-loops
# No longer used:
# -ffriend-injection
# -fno-rtti

ifeq ($(USE_PROFILING), 1)
OPTIMIZATIONS = $(SOME_OPTIMIZATIONS) $(PROFILE_FLAGS)
else
OPTIMIZATIONS = $(SOME_OPTIMIZATIONS) 
endif

# For MacOS application
## MAC_CPP_FLAGS=-current_version 0.5 -compatibility_version 0.5 -fvisibility=default


# Define macros for linking the Carbon and wx resource files
#REZ = /usr/bin/Rez -d __DARWIN__ -t APPL -d __WXMAC__ Carbon.r $(MAC_ARCHITECTURE_FLAGS)
# Specify where the SetFIle tool is
SETFILE = /usr/bin/SetFile

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = 
else
DEBUG_FLAGS = 
endif

# Build the complete list of flags for the compilers - REMOVED $(CONSOLE_FLAGS)
CPPFLAGS = $(OPTIMIZATIONS) $(SHARED_BASE_FLAGS) $(PROCFLAGS) -D__MAC__\
           $(SPICE_INCLUDE) $(SPICE_DIRECTIVE) $(MAC_ARCHITECTURE_FLAGS)
F77_FLAGS = $(OPTIMIZATIONS) -Wall

# removed for now -Wall


# Link specific flags
LINK_FLAGS =  /usr/lib/libstdc++.6.dylib \
               -headerpad_max_install_names \
               -L$(SPICE_LIB_DIR) $(SPICE_LIBRARIES) $(FORTRAN_LIB) -lm\
             -L$(WX_LIB_LOC) $(WX_LIB) $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

# removed
# AGL is unneeded and OpenGL is included automatically
# -framework OpenGL  -framework AGL

GUI_CPP_FLAGS = $(WXCPPFLAGS)
GUI_LINK_FLAGS = $(WXLINKFLAGS)

# link flags for the Console version
CONSOLE_LINK_FLAGS = $(FORTRAN_LIB) -L$(SPICE_LIB_DIR) $(SPICE_LIBRARIES) -lm -ldl $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

