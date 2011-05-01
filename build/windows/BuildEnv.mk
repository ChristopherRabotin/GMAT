# $Id$
# Build environment file for Windows

# Flags used to control the build
USE_SPICE = 1
USE_DEVIL = 0
CONSOLE_APP = 0
DEBUG_BUILD = 0
PROFILE_BUILD = 0
WX_28_SYNTAX = 1
WX_SHARED = 1
SHARED_BASE = 1
USE_STC_EDITOR = 1
USE_WX_EMAIL = 1
SHOW_HELP_BUTTON = 1
SMART_APPLY_BUTTON = 1

# GMAT application icon for Windows only
# location of GmatIcon
GMAT_ICON_DIR = C:/Projects/GmatDev/src/gui/resource
GMAT_ICON_RC = $(GMAT_ICON_DIR)/GmatIcon.rc
GMAT_ICON_O  = $(GMAT_ICON_DIR)/GmatIcon.o

# The Console app does not support MATLAB linkage or shared base libraries for now
ifeq ($(CONSOLE_APP), 1)
USE_STC_EDITOR = 0
CONSOLE_FLAGS = -D__CONSOLE_APP__
else
CONSOLE_FLAGS =
endif

# MATLAB data
# location of MATLAB headers and libraries
# Removed MATLAB dependency

# SPICE data
# location of CSPICE headers and libraries
ifeq ($(USE_SPICE), 1)
SPICE_DIR = C:/cspice
SPICE_INCLUDE = -I$(SPICE_DIR)/include
SPICE_CPP_FLAGS = -D__USE_SPICE__ $(SPICE_INCLUDE)
SPICE_LIB_DIR = $(SPICE_DIR)/lib
SPICE_LIBRARIES = $(SPICE_LIB_DIR)/cspice.a
else
SPICE_INCLUDE =
SPICE_CPP_FLAGS =
SPICE_LIB_DIR =
SPICE_LIBRARIES =
endif

# DevIL data
# location of DevIL headers and libraries
ifeq ($(USE_DEVIL), 1)
IL_CPP_FLAGS = -IC:/DevIL/include/il -IC:/DevIL/include
IL_LIBRARIES = -LC:/devIL/dlls -lilu -lilut -lDevIL
else
IL_CPP_FLAGS = -DSKIP_DEVIL
IL_LIBRARIES =
endif

# STC editor (wxStyledTextCtrl) data
# location of STC headers and libraries
ifeq ($(USE_STC_EDITOR), 1)
STC_CPP_FLAGS = -D__USE_STC_EDITOR__ -I/C:/wxWidgets-2.8.11/contrib/include
STC_LIBRARIES = -LC:/wxWidgets-2.8.11/lib -lwx_msw_stc-2.8 
else
STC_CPP_FLAGS =
STC_LIBRARIES =
endif

# wxEmail
# location of wxEmail headers and libraries
ifeq ($(USE_WX_EMAIL), 1)
EMAIL_CPP_FLAGS = -D__ENABLE_EMAIL__ -I/C:/wxWidgets-2.8.11/contrib/include
EMAIL_LIBRARIES = -LC:/wxWidgets-2.8.11/lib -lwx_msw_netutils-2.8 
else
EMAIL_CPP_FLAGS =
EMAIL_LIBRARIES =
endif

ifeq ($(SHOW_HELP_BUTTON), 1)
ifeq ($(SMART_APPLY_BUTTON), 1)
GUI_CPP_FLAGS = -D__SHOW_HELP_BUTTON__ -D__SMART_APPLY_BUTTON__
else
GUI_CPP_FLAGS = -D__SHOW_HELP_BUTTON__
endif
else
ifeq ($(SMART_APPLY_BUTTON), 1)
GUI_CPP_FLAGS = -D__SMART_APPLY_BUTTON__
else
GUI_CPP_FLAGS =
endif
endif

GMAT_CPP_FLAGS = $(SPICE_CPP_FLAGS) $(IL_CPP_FLAGS) $(STC_CPP_FLAGS) $(EMAIL_CPP_FLAGS) $(GUI_CPP_FLAGS) 
GMAT_LINK_FLAGS = $(SPICE_LIBRARIES) $(IL_LIBRARIES) $(STC_LIBRARIES) $(EMAIL_LIBRARIES) 

# wxWidgets settings
ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__ -D__USE_WX280_GL__ -DwxUSE_GLCANVAS
else
WX_28_DEFINES = 
endif

# Compiler options
CPP = g++
C = gcc
FORTRAN = g77
ifeq ($(USE_SPICE), 1)
FORTRAN_LIB =
else
FORTRAN_LIB = -LC:/MinGW/lib -lg2c
endif

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

ifeq ($(SHARED_BASE), 1)
SHARED_LIB_FLAGS = $(FORTRAN_LIB) $(GMAT_LINK_FLAGS) -shared -Wl --out-implib
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

WXCPPFLAGS = `/usr/local/bin/wx-config --cppflags`
WXLINKFLAGS = `/usr/local/bin/wx-config --libs --gl-libs --static=no` \
               -lopengl32 -lglu32

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Build the complete list of flags for the compilers
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) $(GMAT_CPP_FLAGS) -Wall \
           $(WXCPPFLAGS) $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) $(PROFILE_FLAGS) $(DEBUG_FLAGS)

F77_FLAGS = $(CPPFLAGS)

LINK_FLAGS = $(GMAT_LINK_FLAGS) $(WXLINKFLAGS) \
             $(FORTRAN_LIB) $(DEBUG_FLAGS)

CONSOLE_LINK_FLAGS = -L../base/lib $(FORTRAN_LIB) $(DEBUG_FLAGS) $(PROFILE_FLAGS)

