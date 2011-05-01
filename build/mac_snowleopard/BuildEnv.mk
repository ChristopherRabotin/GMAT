# $Id: BuildEnv.mk 6536 2009-04-20 16:24:12Z wendys-dev $
# Build environment file for Linux
# Modifed for Mac OS 10.3  W. Shoan - 2005.11.10
# Modifed for Mac OS 10.4  W. Shoan - 2006.01.19
# Modified a whole bunch after that  W. Shoan  - 2006 - 2007
# NOTES to user - changes are necessary where you see the "*** EDIT THIS ***" notes

# Flags used to control the build
USE_SPICE = 1
USE_DEVIL = 0
CONSOLE_APP = 0
DEBUG_BUILD = 0
PROFILE_BUILD = 0
WX_28_SYNTAX = 1
USE_STC_EDITOR = 0
USE_F2C_VERSION = 1

# Not currently used 
USE_SHARED = 1

# flag indicating whether or not to build as a shared library
SHARED_BASE = 1

# *** EDIT THIS *** - put the top of the GMAT project directory structure here .... make sure not to have trailing blanks
TOP_DIR = <your-top-level-directory-goes-here>
# *** EDIT THIS *** - this is where you installed the version of wxMac that you're using ...
WX_HOME = /Applications/wxmac-2.8.11/shared

# *** EDIT THIS *** - 'sudo make install' of wxMac will put things here ......
WX_INSTALLED = /usr/local/bin
# *** EDIT THIS *** - this should match the version of wxMac you are using
INSTALL_LIBS_INTO_BUNDLE = install_libs_into_bundle_2_8_11

# *** EDIT THIS *** - where is PCRE installed
PCRE_LIB_LOC = /Applications/pcre-8.00/.libs

ifeq ($(USE_F2C_VERSION), 1)
# *** EDIT THIS *** - If you are using F2C, say where it is located
F2C_INCLUDE_PATH = /Users/wshoan/Documents/workspace/GMAT_3rdParty/f2c32/include
F2C_LIB_PATH = /Users/wshoan/Documents/workspace/GMAT_3rdParty/f2c32/lib

F2C_INCLUDE = -I$(F2C_INCLUDE_PATH)
else
F2C_INCLUDE_PATH =
F2C_LIB_PATH =
endif

BUILD = release

ifeq ($(USE_SPICE), 1)
# location of CSPICE headers and libraries
# *** EDIT THIS *** -this is where you installed the version of CSPICE that you're using ...
SPICE_DIR = /Applications/CSPICE
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

ifeq ($(PROFILE_BUILD), 1)
USE_PROFILING = 1
else
USE_PROFILING = 0
endif

# currently cannot use MATLAB or shared base library with console version 
ifeq ($(CONSOLE_APP), 1)
SHARED_BASE = 0
endif


# DevIL data
ifeq ($(USE_DEVIL), 1)
IL_HEADERS = -I../DevIL/il -I../DevIL 
IL_LIBRARIES = -L../devIL -lilu -lilut -lDevIL
else
IL_HEADERS = -DSKIP_DEVIL 
IL_LIBRARIES = 
endif

# wxWidgets settings
ifeq ($(WX_28_SYNTAX), 1)
WX_28_DEFINES = -D__USE_WX280__
#WX_28_DEFINES = -D__USE_WX280__ -D__USE_WX280_GL__
else
WX_28_DEFINES = 
endif

#shared base settings
ifeq ($(SHARED_BASE), 1)
SHARED_BASE_FLAGS = -fno-common -dynamic
else
SHARED_BASE_FLAGS =
endif

# Snow Leopard Specific Build -- to use carbon based wx
#WX_CARBON_BUILD =  -m32 -arch i386
WX_CARBON_BUILD = -arch i386

WXCPPFLAGS = `$(WX_INSTALLED)/wx-config --cppflags`
WXLINKFLAGS = `$(WX_INSTALLED)/wx-config --libs --universal=no --static=no`

#
# Compiler options
CPP = g++
C = gcc
# *** EDIT THIS *** - You need a fortran compiler here ............
FORTRAN = /usr/local/bin/gfortran
#FORTRAN = /usr/bin/gfortran

PROFILE_FLAGS = -pg

SOME_OPTIMIZATIONS = -O3 -funroll-loops -fno-rtti  -ffriend-injection

ifeq ($(USE_PROFILING), 1)
OPTIMIZATIONS = $(SOME_OPTIMIZATIONS) $(PROFILE_FLAGS)
else
OPTIMIZATIONS = $(SOME_OPTIMIZATIONS) 
endif

LINUX_MAC = 1
MAC_SPECIFIC = 1

# For MacOS application
ifeq ($(MAC_SPECIFIC),1)

MAC_CPP_FLAGS=-current_version 0.5 -compatibility_version 0.5 -fvisibility=default


EXECUTABLE 	= $(TOP_DIR)/application/bin/GMAT

# *** EDIT THIS *** - put the version number of the wxMac that you're using here ...
WX_VERSION   = 2.8.11
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

#REZ =
# Define macros for linking the Carbon and wx resource files
REZ = /Developer/Tools/Rez -d __DARWIN__ -t APPL -d __WXMAC__ Carbon.r -arch i386
# *** EDIT THIS *** - Point to where the FORTRAN libraries are
ifeq ($(USE_F2C_VERSION), 0)
FORTRAN_LIB = -L/usr/local/gfortran/lib -lgfortran 
else
FORTRAN_LIB = -L$(F2C_LIB_PATH) -lf2c
endif
else 
REZ = 
FORTRAN_LIB =                
endif 

ifeq ($(CONSOLE_APP),1)
CONSOLE_FLAGS = -D__CONSOLE_APP__
else
CONSOLE_FLAGS = 
endif

# Set options for debugging and profiling
DEBUG_FLAGS = 

# Build the complete list of flags for the compilers
CPPFLAGS = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall $(SHARED_BASE_FLAGS) -D__SHOW_HELP_BUTTON__ -D__SMART_APPLY_BUTTON__\
           $(WXCPPFLAGS) $(SPICE_INCLUDE) $(SPICE_DIRECTIVE) $(IL_HEADERS) -D__WXMAC__ $(WX_28_DEFINES) $(WX_CARBON_BUILD)
F77_FLAGS = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) -Wall -m32 \
            $(WXCPPFLAGS) $(IL_HEADERS) -D__WXMAC__ $(WX_28_DEFINES)


# Link specific flags
# *** EDIT THIS *** - put the correct wx lib here (based on the version you're using, 
#                     i.e. 2.8, 2.6, etc.)
LINK_FLAGS =  $(WXLINKFLAGS) /usr/lib/libstdc++.6.dylib \
               -framework OpenGL -framework AGL  -headerpad_max_install_names \
               $(SPICE_LIBRARIES) $(FORTRAN_LIB) -lm\
             -lwx_mac_gl-2.8 $(DEBUG_FLAGS) $(IL_LIBRARIES) $(WX_CARBON_BUILD)

# currently cannot use MATLAB with console version
CONSOLE_LINK_FLAGS = -L../../base/lib $(FORTRAN_LIB) $(SPICE_LIBRARIES) -lm -ldl $(DEBUG_FLAGS) 

