# $Id$
# Build environment file for Linux

# Flags used to control the build
USE_MATLAB = 1
USE_DEVIL = 1
CONSOLE_APP = 0
DEBUG_BUILD = 0
PROFILE_BUILD = 0
WX_28_SYNTAX = 1
SHARED_BASE = 1

# For AMD64 native code, this variable should be set equal to 1
USE_64_BIT_LONGS = 0

# If the copy of wx-config you neeed is not in your path, enter the path to the
# file here, including a terminating slash.  (The commented version is an 
# example, showing what you would use if wx-config were in /usr/local/bin)

WX_CONFIG_PATH = 
# WX_CONFIG_PATH = /usr/local/bin/


# MATLAB specific data
# If you build with MATLAB support, you need to set the path infoirmation here.
# You may also need to edit this list, depending on your MATLAB version number;
# this set works with R2007b for Linux.
MATLAB_INCLUDE = -I/opt/matlab73/extern/include
MATLAB_LIB = -L/opt/matlab73/bin/glnx86
MATLAB_LIBRARIES = -leng -lmx -lut \
                   -lmat -lpthread -lstdc++ -lm \
                   -licudata -licuuc -licui18n -licuio -lz -lhdf5 -lxerces-c

# DevIL data
IL_HEADERS = -I/usr/local/include/IL
IL_LIBRARIES = -lIL -lILU -lILUT

# wxWidgets settings
ifeq ($(CONSOLE_APP), 0)
ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__ -D__USE_WX280_GL__ -DwxUSE_GLCANVAS
else
WX_28_DEFINES = 
endif
endif

# The Console app does not support MATLAB linkage for now
ifeq ($(CONSOLE_APP), 1)
USE_MATLAB = 0
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

# Build specific flags
MATLAB_FLAGS = -D__USE_MATLAB__=1

ifeq ($(CONSOLE_APP), 0)
WXCPPFLAGS = `$(WX_CONFIG_PATH)wx-config --cppflags`
WXLINKFLAGS = `$(WX_CONFIG_PATH)wx-config --libs --gl-libs --static=no`
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
ifeq ($(USE_MATLAB),1)
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall $(MATLAB_FLAGS) \
           $(WXCPPFLAGS) \
           $(MATLAB_INCLUDE) $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(PROCFLAGS)
else
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall \
           $(WXCPPFLAGS) $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(PROCFLAGS)
endif

ifeq ($(USE_DEVIL), 0)
CPPFLAGS = $(CPP_BASE) -DSKIP_DEVIL $(PROFILE_FLAGS) $(DEBUG_FLAGS)
else
CPPFLAGS = $(CPP_BASE) $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(IL_HEADERS)
endif


F77_FLAGS = $(CPPFLAGS)

F2C_FLAGS = -lgfortran -lm
# F2C_FLAGS = -lf2c

# Link specific flags
ifeq ($(USE_DEVIL),1)

ifeq ($(USE_MATLAB),1)
LINK_FLAGS = $(WXLINKFLAGS)\
             $(MATLAB_LIB) $(MATLAB_LIBRARIES) \
             $(DEBUG_FLAGS) \
             $(IL_LIBRARIES) $(PROFILE_FLAGS) $(F2C_FLAGS)
else
LINK_FLAGS = $(WXLINKFLAGS)\
             $(DEBUG_FLAGS) \
             $(IL_LIBRARIES) $(PROFILE_FLAGS) $(F2C_FLAGS)
endif

else

ifeq ($(USE_MATLAB),1)
LINK_FLAGS = $(WXLINKFLAGS)\
             $(MATLAB_LIB) $(MATLAB_LIBRARIES) \
             $(DEBUG_FLAGS) $(F2C_FLAGS)
else
LINK_FLAGS = $(WXLINKFLAGS) $(DEBUG_FLAGS) $(F2C_FLAGS)
endif

endif


ifeq ($(USE_MATLAB),1)
CONSOLE_LINK_FLAGS = $(MATLAB_LIB) $(MATLAB_LIBRARIES) -L../../base/lib \
                    -lgfortran $(DEBUG_FLAGS) $(PROFILE_FLAGS)
else
CONSOLE_LINK_FLAGS = -L../../base/lib -lgfortran $(DEBUG_FLAGS) $(PROFILE_FLAGS) 
endif
