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
PROJECT(GMAT)
SET(GMAT_RELEASE_NAME "R2014a" CACHE STRING "Build version")
SET(GMAT_VERSION ${GMAT_RELEASE_NAME})

# Set SPICE and wxWidgets directories
if(WIN32)
  SET(WXWIDGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/wxWidgets/wxMSW-2.8.12" CACHE PATH "wxWidgets Root Directory")
  SET(CSPICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/cspice/windows" CACHE PATH "CSpice Root Directory")

elseif(UNIX)
  SET(WXWIDGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/wxWidgets/wxWidgets-2.8.12" CACHE PATH "wxWidgets Root Directory")

  if(APPLE)
    SET(CSPICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/cspice/macosx" CACHE PATH "CSpice Root Directory")
  else()
    SET(CSPICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/cspice/linux" CACHE PATH "CSpice Root Directory")
  endif()

endif()

# Set general build options
OPTION(DEBUG_BUILD "DebugBuild" OFF)
OPTION(PROFILE_BUILD "ProfileBuild" OFF)
OPTION(GMAT_64_BIT "64-Bit Binaries" ON)
OPTION(GMAT_GUI "Build the GUI, requires wxWidgets" OFF)

# Optional GMAT plugins
OPTION(PLUGIN_CINTERFACE "CInterface Plugin" ON)
OPTION(PLUGIN_DATAINTERFACE "DataInterface Plugin" ON)
OPTION(PLUGIN_EPHEMPROPAGATOR "EphemPropagator Plugin" ON)
OPTION(PLUGIN_ESTIMATION "Estimation Plugins" ON)
OPTION(PLUGIN_EVENTLOCATOR "EventLocator Plugin" ON)
OPTION(PLUGIN_EXTRAPROPAGATORS "ExtraPropagators Plugin" ON)
OPTION(PLUGIN_FMINCONOPTIMIZER "FminconOptimizer Plugin" OFF)
OPTION(PLUGIN_FORMATION "Formation Plugin" ON)
OPTION(PLUGIN_GEOMETRICMEASUREMENT "GeometricMeasurement Plugin" ON)
OPTION(PLUGIN_GMATFUNCTION "GmatFunction Plugin" ON)
OPTION(PLUGIN_MATLABINTERFACE "MatlabInterface Plugin" OFF)
OPTION(PLUGIN_PRODUCTIONPROPAGATORS "ProductionPropagators Plugin" ON)
OPTION(PLUGIN_SAVECOMMAND "SaveCommand Plugin" ON)
OPTION(PLUGIN_STATION "Station Plugin" ON)

# Optional Jazz Plugin Inputs
# to-do

# The FMinConOptimizer and MatlabInterface require Matlab installed
if(PLUGIN_FMINCONOPTIMIZER OR PLUGIN_MATLABINTERFACE)
  FIND_PACKAGE(Matlab)
  if(NOT MATLAB_FOUND)
    MESSAGE(WARNING "MATLAB not found, FminconOptimizer and MatlabInterface plugins disabled.")
    SET(PLUGIN_FMINCONOPTIMIZER OFF CACHE BOOL "Disabled due to missing Matlab" FORCE)
    SET(PLUGIN_MATLABINTERFACE OFF CACHE BOOL "Disabled due to missing Matlab" FORCE)
  endif()
endif()

# Find appropriate 32/64-bit SPICE directories
if(GMAT_64_BIT)
  SET(CSPICE_DIR "${CSPICE_DIR}/cspice64")
else()
  SET(CSPICE_DIR "${CSPICE_DIR}/cspice32")
endif()

if( IS_DIRECTORY ${CSPICE_DIR} )
  MESSAGE(STATUS "SPICE FOUND in " ${CSPICE_DIR})
else()
  MESSAGE(FATAL_ERROR "SPICE NOT FOUND in directory " ${CSPICE_DIR} " , Stopping build.")
endif()

# If building GUI, check if wxWidgets directory exists
if( GMAT_GUI )
  if( IS_DIRECTORY ${WXWIDGET_DIR} )
    MESSAGE(STATUS "wxWidgets FOUND in" ${WXWIDGET_DIR})
  else()
    MESSAGE(WARNING "wxWidgets NOT FOUND in directory " ${WXWIDGET_DIR} " , GUI disabled.")
    SET(GMAT_GUI OFF CACHE BOOL "Disabled due to missing wxWidgets" FORCE)
  endif()
endif()

# Default to Release build
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
endif()

if (DEBUG_BUILD)
  SET(CMAKE_BUILD_TYPE Debug)
endif (DEBUG_BUILD)

# ====================================================================
# specific win32 ressources
if(WIN32)
  SET(GMAT_ICON_DIR "resource")  # relative to src/gui/
  if(WIN32)
    SET(GMAT_ICON_RC  "GmatIcon.rc")
  elseif(APPLE) 
    SET(GMAT_ICON_RC  "GMATMac.icns")       
  endif()
  SET(GMAT_ICON_O  "GmatIcon.o")
endif()

# ====================================================================
#need to include Find****.cmake
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ====================================================================
# Common build options
INCLUDE(CheckCXXCompilerFlag)

if(WIN32)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W2")
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

# Set DevIL Proprocessor defs for linux
if (WIN32)
  ADD_DEFINITIONS("-DSKIP_DEVIL")
endif()

# ====================================================================
# Mac specifics (not tested)
if(APPLE)
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} current_version 0.5 -/#compatibility_version 0.5 -fvisibility=default")
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenGL -framework AGL -headerpad_max_install_names")
  #LINK_DIRECTORIES("stdc++.6")
endif()

if(NOT WIN32)
  ADD_DEFINITIONS("-DLINUX_MAC")
endif()

# ====================================================================
# Output directories based on configuration type.
# (Make all output go to the /gmat/application/bin/ directory.

# First for the generic no-config case (e.g. with mingw)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/application/bin" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/application/bin" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/application/bin" )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
  string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/application/bin" )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/application/bin" )
  set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/application/bin" )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# ====================================================================
# Go to src directory and look for CMake instructions there
ADD_SUBDIRECTORY(src)

# ====================================================================
# Go to plugins directory and look for CMake instructions there
ADD_SUBDIRECTORY(plugins)
