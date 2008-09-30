# $Id$
# Build environment file for Windows

# Flags used to control the build
USE_MATLAB = 1
USE_DEVIL = 0
CONSOLE_APP = 0
DEBUG_BUILD = 0
PROFILE_BUILD = 0
WX_28_SYNTAX = 1
SHARED_BASE = 1
#WX_SHARED = 1

# MATLAB specific data
MATLAB_INCLUDE = -IC:/Program\ Files/MATLAB/R2007B/extern/include
MATLAB_LIB = -LC:/Program\ Files/MATLAB/R2007B/bin/win32
MATLAB_LIBRARIES = -leng -lmx -lmat

# DevIL data
IL_HEADERS = -ID:/DevIL/include/il -ID:/DevIL/include
IL_LIBRARIES = -LD:/devIL/dlls -lilu -lilut -lDevIL

# wxWidgets settings
ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__ -D__USE_WX280_GL__ -DwxUSE_GLCANVAS
else
WX_28_DEFINES = 
endif

# The Console app does not support MATLAB linkage or shared base libraries for now
ifeq ($(CONSOLE_APP), 1)
USE_MATLAB = 0
SHARED_BASE = 0
endif

# GMAT application icon for Windows only
GMAT_ICON_RC = D:/Projects/GMAT/Icons/GmatIcon.rc
GMAT_ICON_O  = D:/Projects/GMAT/Icons/GmatIcon.o

# Compiler options
CPP = g++
C = gcc
FORTRAN = g77
FORTRAN_LIB = -LC:/MinGW/lib -lg2c

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

ifeq ($(SHARED_BASE), 1)

ifeq ($(USE_MATLAB), 1)
SHARED_LIB_FLAGS = $(FORTRAN_LIB) $(MATLAB_LIB) $(MATLAB_LIBRARIES) -shared -Wl --out-implib
else
SHARED_LIB_FLAGS = $(FORTRAN_LIB) -shared -Wl --out-implib
endif

else
SHARED_LIB_FLAGS = 
endif

OPTIMIZATIONS =  -DwxUSE_UNIX=0 -D_X86_=1 -DWIN32 -DWINVER=0x0400 -D__WIN95__ \
                 -D__GNUWIN32__ -D__WIN32__ -mthreads -DSTRICT  -D__WXMSW__ \
                 -D__WINDOWS__ -Wall -fno-pcc-struct-return -O3\
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -malign-double -fexceptions -D__USE_WX280_GL__\
                 -fexpensive-optimizations -march=pentium4
                 
#  -march=pentium4 -mfpmath=sse -fomit-frame-pointer -DNDEBUG

# Do not edit below this line -- here we build up longer compile/link strings
LINUX_MAC = 0

# Build specific flags
MATLAB_FLAGS = -D__USE_MATLAB__=1

WXCPPFLAGS = `/usr/local/bin/wx-config --cppflags`
WXLINKFLAGS = `/usr/local/bin/wx-config --libs --gl-libs --static=no` \
               -lopengl32 -lglu32


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

# Build the complete list of flags for the compilers
ifeq ($(USE_MATLAB),1)
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall $(MATLAB_FLAGS) \
           $(WXCPPFLAGS) \
           $(MATLAB_INCLUDE) $(PROFILE_FLAGS) $(DEBUG_FLAGS)
else
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall \
           $(WXCPPFLAGS) $(PROFILE_FLAGS) $(DEBUG_FLAGS)
endif

ifeq ($(USE_DEVIL), 0)
CPPFLAGS = $(CPP_BASE) -DSKIP_DEVIL $(PROFILE_FLAGS) $(DEBUG_FLAGS)
else
CPPFLAGS = $(CPP_BASE) $(PROFILE_FLAGS) $(DEBUG_FLAGS) $(IL_HEADERS)
endif


F77_FLAGS = $(CPPFLAGS)

# Link specific flags
ifeq ($(USE_DEVIL),1)

ifeq ($(USE_MATLAB),1)
LINK_FLAGS = $(WXLINKFLAGS)\
             $(MATLAB_LIB) $(MATLAB_LIBRARIES) \
             $(FORTRAN_LIB) $(DEBUG_FLAGS) $(IL_LIBRARIES)
else
LINK_FLAGS = $(WXLINKFLAGS)\
             $(FORTRAN_LIB) $(DEBUG_FLAGS) $(IL_LIBRARIES)
endif

else

ifeq ($(USE_MATLAB),1)
LINK_FLAGS = $(WXLINKFLAGS)\
             $(MATLAB_LIB) $(MATLAB_LIBRARIES) \
             $(FORTRAN_LIB) $(DEBUG_FLAGS)
else
LINK_FLAGS = $(WXLINKFLAGS)\
             $(FORTRAN_LIB) $(DEBUG_FLAGS)
endif

endif


ifeq ($(USE_MATLAB),1)
CONSOLE_LINK_FLAGS = $(MATLAB_LIB) $(MATLAB_LIBRARIES) -L../base/lib \
                    -lgfortran $(DEBUG_FLAGS) $(PROFILE_FLAGS)
else
CONSOLE_LINK_FLAGS = -L../base/lib $(FORTRAN_LIB) $(DEBUG_FLAGS) 
endif
