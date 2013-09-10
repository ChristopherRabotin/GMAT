# $Id: BuildEnv.mk 6536 2009-04-20 16:24:12Z wendys-dev $
# 
# Build environment file for MAC OSX (Intel); currently supports only:
#    Snow Leopard (10.6)
#    Lion (10.7)
#
# NOTES to user - changes may be necessary where you see the "*** EDIT THIS ***" notes
# -------------------------------------------------------------------------------------------

# ------> FLAGS USED TO CONTROL THE BUILD
# *** EDIT THIS *** - Flags used to control the build
# 1 = 64-bit, 0 = 32-bit - BUT set this only if it is not set in the main project Makefile
#                          OR you are building the GmatConsole separately
ifndef BUILD_64BIT
BUILD_64BIT = 0
endif   
 
USE_SPICE = 1
# if this is 1, it will build the base library and the console app only
CONSOLE_APP = 0
USE_F2C_VERSION = 1
# STC_EDITOR not yet available on Mac
USE_STC_EDITOR = 0
DEBUG_BUILD = 0
PROFILE_BUILD = 0

# ------> TOP DIRECTORY
# *** EDIT THIS *** - put the top of the GMAT project directory structure here .... make sure not to have trailing blanks!!!
TOP_DIR = <path-to-your-GMAT-directory>

# ------> WXWIDGETS
# *** EDIT THIS *** -  this is where you installed wxWidgets ('sudo make install' of wxMac will put things in /usr/local directories)
#WX_INSTALLED = /usr/local/bin
#WX_LIB_LOC = /usr/local/lib
WX_INSTALLED = /Applications/wxMac-2.8.12/shared
WX_LIB_LOC = $(WX_INSTALLED)/lib
WX_LIB = wx_mac_gl-2.8
# WX_VERSION must be in this format
WX_VERSION = 2_8_12

# ------> CSPICE
# *** EDIT THIS *** - this is where you installed the version of CSPICE that you're using ...
ifeq ($(USE_SPICE), 1)
SPICE_DIR = /Applications/CSPICE_N0064
# 0 -> use cpspice.a, 1 -> use libcspice.dylib in bin directory
SPICE_USE_DYLIB = 0
else
SPICE_DIR =
endif

# ------> F2C
# *** EDIT THIS *** - If you are using F2C, say where it is located; if not, point to fortran libraries
ifeq ($(USE_F2C_VERSION), 1)
F2C_LOC = /Users/wshoan/Documents/workspace/GMAT_3rdParty
else
FORTRAN_LIB = -L/usr/local/gfortran/lib -lgfortran 
F2C_LOC = 
endif

# ------> PCRE
# *** EDIT THIS *** - where is PCRE installed
PCRE_LIB_LOC = /Applications/pcre-8.12/.libs


# *************************** DO NOT EDIT BELOW THIS LINE ********************************
LINUX_MAC = 1
MAC_SPECIFIC = 1

WX_28_SYNTAX = 1     # not currently modifiable

ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__
else
WX_28_DEFINES = 
endif

# this will point to the proper script for the Wx version
INSTALL_LIBS_INTO_BUNDLE = install_libs_into_bundle_$(WX_VERSION)

# flags that depend on whether you are building 32-bit or 64-bit
ifeq ($(BUILD_64BIT), 1)
CSPICE_VER = cspice64
GMAT_BIN_DIR = bin64
GMAT_LIB_DIR = lib64
MAC_ARCHITECTURE_FLAGS = 
WXCPPFLAGS = 
WXLINKFLAGS =
MAC_ARCHITECTURE_FLAGS = 
F2C_DIR = f2c64
PROCFLAGS = -DUSE_64_BIT_LONGS
else
CSPICE_VER = cspice
GMAT_BIN_DIR = bin
GMAT_LIB_DIR = lib
MAC_ARCHITECTURE_FLAGS = -arch i386
WXCPPFLAGS = `$(WX_INSTALLED)/wx-config --cppflags`  -D__WXMAC__ $(WX_28_DEFINES)
WXLINKFLAGS = `$(WX_INSTALLED)/wx-config --libs --universal=no --static=no`
F2C_DIR = f2c32
PROCFLAGS = 
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

SOME_OPTIMIZATIONS = -O3 -funroll-loops -fno-rtti  -ffriend-injection

ifeq ($(USE_PROFILING), 1)
OPTIMIZATIONS = $(SOME_OPTIMIZATIONS) $(PROFILE_FLAGS)
else
OPTIMIZATIONS = $(SOME_OPTIMIZATIONS) 
endif

# For MacOS application
MAC_CPP_FLAGS=-current_version 0.5 -compatibility_version 0.5 -fvisibility=default


# Define macros for linking the Carbon and wx resource files
REZ = /usr/bin/Rez -d __DARWIN__ -t APPL -d __WXMAC__ Carbon.r $(MAC_ARCHITECTURE_FLAGS)
SETFILE = /usr/bin/SetFile

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = 
else
DEBUG_FLAGS = 
endif

# Build the complete list of flags for the compilers - REMOVED $(CONSOLE_FLAGS)
CPPFLAGS = $(OPTIMIZATIONS) -Wall $(SHARED_BASE_FLAGS) $(PROCFLAGS) -D__MAC__\
           $(SPICE_INCLUDE) $(SPICE_DIRECTIVE) $(MAC_ARCHITECTURE_FLAGS)
F77_FLAGS = $(OPTIMIZATIONS) -Wall


# Link specific flags
LINK_FLAGS =  /usr/lib/libstdc++.6.dylib \
               -framework OpenGL -framework AGL  -headerpad_max_install_names \
               -L$(SPICE_LIB_DIR) $(SPICE_LIBRARIES) $(FORTRAN_LIB) -lm\
             -l$(WX_LIB) $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

GUI_CPP_FLAGS = $(WXCPPFLAGS)
GUI_LINK_FLAGS = $(WXLINKFLAGS)

# link flags for the Console version
CONSOLE_LINK_FLAGS = $(FORTRAN_LIB) -L$(SPICE_LIB_DIR) $(SPICE_LIBRARIES) -lm -ldl $(DEBUG_FLAGS) $(MAC_ARCHITECTURE_FLAGS)

