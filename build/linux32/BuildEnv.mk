# $Id: BuildEnv.mk 10160 2012-01-18 15:11:32Z djcinsb $
# Build environment file for Linux

# Flags used to control the build
USE_SPICE = 1
CONSOLE_APP = 0
DEBUG_BUILD = 0
PROFILE_BUILD = 0
WX_28_SYNTAX = 1
USE_STC_EDITOR = 0

SHARED_BASE = 1

# For 64-bit versions of Linux, this variable should be set equal to 1
USE_64_BIT_LONGS = 0

# If the copy of wx-config you need is not in your path, enter the path to the
# file here, including a terminating slash.  (The commented version is an 
# example, showing what you would use if wx-config were in /usr/local/bin)

WX_CONFIG_PATH = 
# WX_CONFIG_PATH = /usr/local/bin/

ifeq ($(USE_SPICE), 1)
# location of CSPICE headers and libraries
# *** EDIT THIS *** -this is where you installed the version of CSPICE that you're using ...
SPICE_DIR = /home/djc/TS_Code/Gmat3rdParty
SPICE_INCLUDE = -I$(SPICE_DIR)/cspice/include
SPICE_LIB_DIR = $(SPICE_DIR)/cspice/lib
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


# wxWidgets settings
ifeq ($(CONSOLE_APP), 0)
ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__ -D__USE_WX280_GL__ -DwxUSE_GLCANVAS
else
WX_28_DEFINES = 
endif
endif


ifeq ($(USE_STC_EDITOR), 1)
STC_CPP_FLAGS = -D__USE_STC_EDITOR__
STC_LIBRARIES = -lwx_gtk2_stc-2.8
else
STC_CPP_FLAGS =
STC_LIBRARIES =
endif


# The Console app does not support shared base libraries for now
ifeq ($(CONSOLE_APP), 1)
SHARED_BASE = 0
endif

# Compiler options
CPP = g++
C = gcc
FORTRAN = gfortran

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

DESIRED_OPTIMIZATIONS =  -DSTRICT -Wall -fno-pcc-struct-return -O3 \
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -march=pentium -malign-double -fexceptions \
                 -fexpensive-optimizations

ifeq ($(DEBUG_BUILD), 1)
OPTIMIZATIONS = -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
else
OPTIMIZATIONS = -O3 -fno-strict-aliasing -ffriend-injection $(WX_28_DEFINES)
endif

# Do not edit below this line -- here we build up longer compile/link strings
LINUX_MAC = 1

ifeq ($(CONSOLE_APP), 0)
WXCPPFLAGS = `$(WX_CONFIG_PATH)wx-config --cppflags`
WXLINKFLAGS = `$(WX_CONFIG_PATH)wx-config --libs --gl-libs --static=no` -lGL -lGLU
endif


ifeq ($(CONSOLE_APP),1)
CONSOLE_FLAGS = -D__CONSOLE_APP__
else
CONSOLE_FLAGS = 
endif

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

ifeq ($(USE_64_BIT_LONGS), 1)
PROCFLAGS = -DUSE_64_BIT_LONGS
endif

# Build the complete list of flags for the compilers
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall \
           $(WXCPPFLAGS) $(SPICE_INCLUDE) $(SPICE_DIRECTIVE) $(PROFILE_FLAGS) \
           $(DEBUG_FLAGS) $(PROCFLAGS)

CPPFLAGS = $(CPP_BASE) $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(STC_CPP_FLAGS) -fPIC

F77_FLAGS = $(CPPFLAGS)

F2C_FLAGS = -lgfortran -lm
# F2C_FLAGS = -lf2c

# Link specific flags
LINK_FLAGS = $(WXLINKFLAGS) $(SPICE_LIBRARIES) $(DEBUG_FLAGS) $(STC_LIBRARIES) \
             $(PROFILE_FLAGS) $(F2C_FLAGS)

CONSOLE_LINK_FLAGS = -L../../base/lib -lgfortran -ldl $(DEBUG_FLAGS) \
                     $(PROFILE_FLAGS)
