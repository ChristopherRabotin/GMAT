# $Id$
# Build environment file for Windows

# Flags used to control the build
CONSOLE_APP = 0
USE_SPICE = 1
DEBUG_BUILD = 0
PROFILE_BUILD = 0
WX_28_SYNTAX = 1
WX_SHARED = 1
SHARED_BASE = 1
USE_STC_EDITOR = 1
USE_WX_EMAIL = 1
USE_F2C_VERSION = 1

# if USE_F2C_VERSION = 0, provide FORTRAN_LIB below

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
SPICE_DIR = C:/projects/cspice
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

# STC editor (wxStyledTextCtrl) data
# location of STC headers and libraries
ifeq ($(USE_STC_EDITOR), 1)
STC_CPP_FLAGS = -D__USE_STC_EDITOR__ -I/C:/wxWidgets-2.8.12/contrib/include
STC_LIBRARIES = -LC:/wxWidgets-2.8.12/lib -lwx_msw_stc-2.8 
else
STC_CPP_FLAGS =
STC_LIBRARIES =
endif

# wxEmail
# location of wxEmail headers and libraries
ifeq ($(USE_WX_EMAIL), 1)
EMAIL_CPP_FLAGS = -D__ENABLE_EMAIL__ -I/C:/wxWidgets-2.8.12/contrib/include
EMAIL_LIBRARIES = -LC:/wxWidgets-2.8.12/lib -lwx_msw_netutils-2.8 
else
EMAIL_CPP_FLAGS =
EMAIL_LIBRARIES =
endif

#BASE_CPP_FLAGS = -DDEBUG_MEMORY $(SPICE_CPP_FLAGS)
BASE_CPP_FLAGS = $(SPICE_CPP_FLAGS)
BASE_LINK_FLAGS = $(SPICE_LIBRARIES)

# wxWidgets settings
ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__ -D__USE_WX280_GL__ -DwxUSE_GLCANVAS
else
WX_28_DEFINES = 
endif

# Compiler options
CPP = g++
C = gcc
FORTRAN = gfortran
ifeq ($(USE_F2C_VERSION), 1)
FORTRAN_LIB =
else
FORTRAN_LIB = C:/f2c/lib/libf2c.a
endif

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif

ifeq ($(SHARED_BASE), 1)
SHARED_LIB_FLAGS = $(FORTRAN_LIB) $(BASE_LINK_FLAGS) -shared -Wl --out-implib
else
SHARED_LIB_FLAGS = 
endif

OPTIMIZATIONS =  -DwxUSE_UNIX=0 -D_X86_=1 -DWIN32 -DWINVER=0x0400 -D__WIN95__ \
                 -D__GNUWIN32__ -D__WIN32__ -mthreads -DSTRICT  -D__WXMSW__ \
                 -D__WINDOWS__ -fno-pcc-struct-return -O2\
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -malign-double -fexceptions -D__USE_WX280_GL__\
                 -fexpensive-optimizations -march=pentium4
                 
#  -march=pentium4 -mfpmath=sse -fomit-frame-pointer -DNDEBUG

# Do not edit below this line -- here we build up longer compile/link strings
LINUX_MAC = 0

WX_CPP_FLAGS = $(STC_CPP_FLAGS) $(EMAIL_CPP_FLAGS)
WX_LINK_FLAGS = $(STC_LIBRARIES) $(EMAIL_LIBRARIES)

GUI_CPP_FLAGS = `/usr/local/bin/wx-config --cppflags` $(WX_CPP_FLAGS) $(IL_CPP_FLAGS) 
GUI_LINK_FLAGS = `/usr/local/bin/wx-config --libs --gl-libs --static=no` \
               $(WX_LINK_FLAGS) $(IL_LIBRARIES) -lopengl32 -lglu32

# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Build the complete list of flags for the compilers
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) $(BASE_CPP_FLAGS) -Wall -ffriend-injection \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) $(PROFILE_FLAGS) $(DEBUG_FLAGS)
CFLAGS = -Wall

F77_FLAGS = $(CPPFLAGS)

LINK_FLAGS = $(BASE_LINK_FLAGS) $(FORTRAN_LIB) $(DEBUG_FLAGS)

CONSOLE_LINK_FLAGS = -L../base/lib $(FORTRAN_LIB) $(DEBUG_FLAGS) $(PROFILE_FLAGS)

