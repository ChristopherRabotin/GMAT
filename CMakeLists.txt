# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the main GMAT directory.
# That is, we should have the directory structure:
#   ./src
#   ./plugins
#   ./depends
#   ./application
#   etc...
#
# Original Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
# Modified for CMake standardization by: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# Require CMake minimum version
cmake_minimum_required(VERSION 2.8)

# Set Cmake project
PROJECT(GMAT C CXX)
SET(GMAT_RELEASE_NAME "R2015a" CACHE STRING "Build version")
SET(GMAT_VERSION ${GMAT_RELEASE_NAME})

# Set default GMAT installation location
# Can be overridden by using "-DCMAKE_INSTALL_PREFIX=/foo/bar" at command line
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set (CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/GMAT-${GMAT_RELEASE_NAME}-${CMAKE_SYSTEM_NAME}" CACHE PATH "GMAT top-directory installation location" FORCE )
endif()

# Set general build options
OPTION(PROFILE_BUILD "ProfileBuild" OFF)
OPTION(GMAT_64_BIT "64-Bit Binaries" ON)
OPTION(GMAT_GUI "Build the GUI, requires wxWidgets" ON)

# Optional GMAT plugins
OPTION(PLUGIN_CINTERFACE "CInterface Plugin" ON)
OPTION(PLUGIN_DATAINTERFACE "DataInterface Plugin" ON)
OPTION(PLUGIN_DATACALLBACK "DataCallback Plugin" ON)
OPTION(PLUGIN_EPHEMPROPAGATOR "EphemPropagator Plugin" ON)
OPTION(PLUGIN_ESTIMATION "Estimation Plugins" ON)
OPTION(PLUGIN_EVENTLOCATOR "EventLocator Plugin" ON)
OPTION(PLUGIN_EXTRAPROPAGATORS "ExtraPropagators Plugin" ON)
OPTION(PLUGIN_FMINCONOPTIMIZER "FminconOptimizer Plugin" ON)
OPTION(PLUGIN_FORMATION "Formation Plugin" ON)
OPTION(PLUGIN_GEOMETRICMEASUREMENT "GeometricMeasurement Plugin" ON)
OPTION(PLUGIN_GMATFUNCTION "GmatFunction Plugin" ON)
OPTION(PLUGIN_MATLABINTERFACE "MatlabInterface Plugin" ON)
OPTION(PLUGIN_PRODUCTIONPROPAGATORS "ProductionPropagators Plugin" ON)
OPTION(PLUGIN_SAVECOMMAND "SaveCommand Plugin" ON)
OPTION(PLUGIN_STATION "Station Plugin" ON)

# Path to custom Find****.cmake files
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/build/cmake_modules")

# Optional Jazz Plugin Inputs
# to-do

# Set default wxWidgets directory based on OS
SET(wxWidgets_PARTIAL_DIR "${PROJECT_SOURCE_DIR}/depends/wxWidgets")
if(WIN32)
  # On Windows, FindwxWidgets searches the wxWidgets_ROOT_DIR path
  SET(wxWidgets_ROOT_DIR "${wxWidgets_PARTIAL_DIR}/wxMSW-3.0.2" CACHE PATH "wxWidgets Root Directory")

elseif(UNIX)
  # On Mac/Linux, FindwxWidgets uses the wx-config utility which must be
  # accessible from the system PATH environment variable. 
  # So first save PATH (just the first time to avoid recursive setting)...
  if(NOT wxWidgets_ROOT_DIR)
    SET(SYS_PATH "$ENV{PATH}" CACHE INTERNAL "System PATH environment variable")
  endif()

  # ... then get the expected path to the wx-config utility ...
  if(APPLE)
    SET(wxWidgets_ROOT_DIR  "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.2/cocoa-build" CACHE PATH "wxWidgets Root Directory")
  else()
    SET(wxWidgets_ROOT_DIR  "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.2/gtk-build" CACHE PATH "wxWidgets Root Directory")
  endif()

  # ... then append that to the PATH environment variable
  SET(ENV{PATH} "${SYS_PATH}:${wxWidgets_ROOT_DIR}")
endif()

# Find wxWidgets if needed
if(GMAT_GUI OR PLUGIN_MATLABINTERFACE)
  FIND_PACKAGE(wxWidgets COMPONENTS core base xml net richtext aui xrc qa html adv stc gl)
endif()

# Set default CSPICE directory based on OS
if(WIN32)
  SET(CSPICE_DIR_TEMP "${PROJECT_SOURCE_DIR}/depends/cspice/windows")
elseif(APPLE)
  SET(CSPICE_DIR_TEMP "${PROJECT_SOURCE_DIR}/depends/cspice/macosx")
else()
  SET(CSPICE_DIR_TEMP "${PROJECT_SOURCE_DIR}/depends/cspice/linux")
endif()

# Use appropriate 32/64-bit CSPICE directories
if(GMAT_64_BIT)
  SET(CSPICE_DIR "${CSPICE_DIR_TEMP}/cspice64" CACHE PATH "CSPICE Root Directory")
else()
  SET(CSPICE_DIR "${CSPICE_DIR_TEMP}/cspice32" CACHE PATH "CSPICE Root Directory")
endif()

# Make sure CSPICE exists
if( IS_DIRECTORY ${CSPICE_DIR} )
  MESSAGE(STATUS "CSPICE FOUND in " ${CSPICE_DIR})
else()
  MESSAGE(FATAL_ERROR "CSPICE NOT FOUND in " ${CSPICE_DIR} " , Stopping build.")
endif()

# Set default F2C directory based on OS
if(WIN32)
  # On Windows, use F2C version from CSPICE
  SET(F2C_DIR "${CSPICE_DIR}/include" CACHE PATH "f2c Root Directory")
elseif(UNIX)
  SET(F2C_DIR "${PROJECT_SOURCE_DIR}/depends/f2c" CACHE PATH "f2c Root Directory")
endif()

# Make sure F2C exists
if( IS_DIRECTORY ${F2C_DIR} )
  MESSAGE(STATUS "F2C FOUND in " ${F2C_DIR})
else()
  MESSAGE(FATAL ERROR "F2C NOT FOUND in " ${F2C_DIR} " , Stopping build.")
endif()

# Find Matlab if needed
if(PLUGIN_FMINCONOPTIMIZER OR PLUGIN_MATLABINTERFACE OR PLUGIN_CINTERFACE)
  # Additional (future) Matlab versions should be included here
  SET(MATLAB_ADDITIONAL_VERSIONS
    "R2015a=8.5"
    "R2014b=8.4"
    )
  FIND_PACKAGE(Matlab COMPONENTS MAIN_PROGRAM MX_LIBRARY ENG_LIBRARY)
endif()


# Default to Release configuration on single-config generators
# e.g. make, but not VisualStudio
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build Configuration (<empty>, Debug, Release, RelWithDebInfo, or MinSizeRel" FORCE)
endif()

# ====================================================================
# Common build options
INCLUDE(CheckCXXCompilerFlag)

if(WIN32)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W2")
endif()

# Enable parallel builds on VisualStudio
if(MSVC)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if(UNIX)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif()

# check fPIC flag is allowed
CHECK_CXX_COMPILER_FLAG("-fPIC" GCC_PIC)
if( GCC_PIC )
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# flags to append to CMAKE_CXX_FLAGS according to CMAKE_BUILD_TYPE 
if(UNIX)
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
  SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
  SET(CMAKE_CXX_FLAGS_MINSIZEREL "-O3")
endif()

# some definitions
ADD_DEFINITIONS("-DNO_GCC_PRAGMA")
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

# Linker Flags
if(UNIX)
  if(NOT APPLE)
    LINK_LIBRARIES("dl")	
  endif()
endif()

# Profile Build Compile Flags
if (PROFILE_BUILD)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()

# Warning Supression Definitions
if(WIN32)
  ADD_DEFINITIONS("/wd4005 /wd4251 /wd4231 /wd4996 /nologo")
endif()

# Set 64 bit Definitions and directories
if (GMAT_64_BIT)
  MESSAGE(STATUS "Will compile 64-bit GMAT")
  ADD_DEFINITIONS("-DUSE_64_BIT_LONGS")
else()
  MESSAGE(STATUS "Will compile 32-bit GMAT")
endif()

# Set DevIL Preprocessor defs
if (WIN32)
  ADD_DEFINITIONS("-DSKIP_DEVIL")
endif()

if(UNIX)
  ADD_DEFINITIONS("-DLINUX_MAC")
endif()

# Enable RPATH on Mac/Linux so that GMAT shared libraries can be found
# relative to each other. This is necessary to distribute GMAT and use it
# from 3rd-party apps such as Matlab.
# Assumes that GMAT libraries are stored in bin/, plugins/, or proprietary/
if(UNIX)
  if(APPLE)
    # On Mac, @loader_path points to directory of current shared library
    SET(CMAKE_MACOSX_RPATH TRUE)
    SET(CMAKE_INSTALL_RPATH "@loader_path/../bin/;@loader_path/../plugins/;@loader_path/../plugins/proprietary/")
  else()
    # On Linux, $ORIGIN points to directory of current shared library
    # Linux searches the RPATH before the system LD_LIBRARY_PATH, so enable
    # the use of RUNPATH which is searched after LD_LIBRARY_PATH. This makes
    # the Linux search order similar to Mac/Windows.
    SET(CMAKE_INSTALL_RPATH "\$ORIGIN/../bin/;\$ORIGIN/../plugins/;\$ORIGIN/../plugins/proprietary/")
    SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags")
    SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags")
  endif()
endif()

# Declare global var for GmatBase header directories
# This will be defined in the GmatBase CMakeLists.txt file
SET(GMAT_BASE_INCLUDE_DIRECTORIES "" CACHE INTERNAL "base includes")

# ====================================================================
# Define macro that adds source/header files to appropriate groups
# for IDE-based projects (e.g. VisualStudio, XCode, ...).
MACRO(_ADDSOURCEGROUPS ALL_DIRS)
  # Loop over each directory containing source/header files
  FOREACH(currdir ${ALL_DIRS})
    # Create the source group name from the current directory name
    STRING(REPLACE "/" "\\\\" groupname ${currdir})

    # Add source files (.c, .cpp) to IDE group
    SOURCE_GROUP("Source Files\\${groupname}" REGULAR_EXPRESSION "${currdir}/.*\\.c(pp)?")

    # Add header files (.hpp) to IDE group
    SOURCE_GROUP("Header Files\\${groupname}" REGULAR_EXPRESSION "${currdir}/.*\\.h(pp)?")
  ENDFOREACH(currdir)
ENDMACRO(_ADDSOURCEGROUPS)

# ====================================================================
# Go to src directory and look for CMake instructions there
ADD_SUBDIRECTORY(src)

# ====================================================================
# Go to plugins directory and look for CMake instructions there
ADD_SUBDIRECTORY(plugins)

# ====================================================================
# Setup GMAT install process
ADD_SUBDIRECTORY(build/install)

