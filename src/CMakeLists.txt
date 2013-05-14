# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the build directory of GMAT.
# That is, we should have the directory structure
#   ./src
#   ./src/base 
#   ./src/console
#   ./src/gui
#
# Original Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#  

# Set Cmake project
PROJECT(GMAT)

# Require cmake minimum version
cmake_minimum_required(VERSION 2.6)

# Set Input Options
SET(GMAT_VERSION "R2013a" CACHE STRING "The version of the build")
SET(CSPICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../depends/cspice" CACHE STRING "CSpice Root Directory")
if(WIN32)
    SET(WXWIDGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../depends/wxWidgets/wxMSW-2.8.12" CACHE STRING "wxWidget Root Directory")
endif()
if(UNIX)
    SET(WXWIDGET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../depends/wxWidgets/wxWidgets-2.8.12" CACHE STRING "wxWidget Root Directory")
endif()
OPTION(VERBOSE_MAKE "VerboseMake" OFF)
OPTION(DEBUG_BUILD "DebugBuild" OFF)
OPTION(PROFILE_BUILD "ProfileBuild" OFF)
OPTION(64_BIT "64Bit" OFF)

# Optional Plugin Inputs
OPTION(PLUGIN_CINTERFACE "CInterface Plugin" ON)
OPTION(PLUGIN_EPHEMPROPAGATOR "EphemPropagator Plugin" OFF)
OPTION(PLUGIN_ESTIMATION "Estimation Plugin" OFF)
OPTION(PLUGIN_EVENTLOCATOR "EventLocator Plugin" OFF)
OPTION(PLUGIN_EXTRAPROPAGATOR "ExtraPropagation Plugin" OFF)
OPTION(PLUGIN_GMATFUNCTION "GmatFunction Plugin" OFF)
OPTION(PLUGIN_MATLABINTERFACE "MatlabInterface Plguin" OFF)

# Optional Jazz Plugin Inputs


# Check if provided cspice directories exists
if( (IS_DIRECTORY ${CSPICE_DIR}) AND (EXISTS ${CSPICE_DIR}) )
	MESSAGE(STATUS "SPICE FOUND")
	MESSAGE("-- CSPICE_DIR=" ${CSPICE_DIR})
else()
	MESSAGE(STATUS "** SPICE NOT FOUND in directory " ${CSPICE_DIR})
	SET(DEPENDS_FAIL ON)
endif()

# Check if provided WxWidgets directories exists
if( (IS_DIRECTORY ${WXWIDGET_DIR}) AND (EXISTS ${WXWIDGET_DIR}) )
	MESSAGE(STATUS "WxWidgets FOUND")
	MESSAGE("-- WXWIDGET_DIR=" ${WXWIDGET_DIR})
else()
	MESSAGE(STATUS "** WxWidgets NOT FOUND in directory " ${WXWIDGET_DIR})
	SET(DEPENDS_FAIL ON)
endif()

# Fail Configuration if any dependencies are missing
if(DEPENDS_FAIL)
	MESSAGE(FATAL_ERROR "Missing Required Dependencies. Stoping Build.")
endif()


# build configurate
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

# check ffriend-injection flag is allowed
CHECK_CXX_COMPILER_FLAG("-ffriend-injection" GCC_FFRIEND_INJECTION)
if( GCC_FFRIEND_INJECTION )
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffriend-injection")
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
if (64_BIT)
    MESSAGE("64-bit compilation")
    ADD_DEFINITIONS("-DUSE_64_BIT_LONGS")
endif()

# Set DevIL Proprocessor defs for linux
if (WIN32)
    ADD_DEFINITIONS("-DSKIP_DEVIL")
endif()

# to print out what Make is doing
if (VERBOSE_MAKE)
    SET(CMAKE_VERBOSE_MAKEFILE ON)
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
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../application/bin" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../application/bin" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../application/bin" )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/../application/bin" )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/../application/bin" )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_CURRENT_SOURCE_DIR}/../application/bin" )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# ====================================================================
# dynamic base lib
SUBDIRS(../src/base)

# ====================================================================
# Console binary
SUBDIRS(../src/console)

# ====================================================================
# GUI binary
SUBDIRS(../src/gui)

# ====================================================================
# CInterface Plugin binary
if (PLUGIN_CINTERFACE)
	SUBDIRS(../plugins/CInterfacePlugin/src/)
endif()

# ====================================================================
# EphemPropagator Plugin binary
if (PLUGIN_EPHEMPROPAGATOR)
	SUBDIRS(../plugins/EphemPropagatorPlugin/src/)
endif()

# ====================================================================
# Estimation Plugin binary
if (PLUGIN_ESTIMATION)
endif()

# ====================================================================
# EventLocator Plugin binary
if (PLUGIN_EVENTLOCATOR)
endif()

# ====================================================================
# ExtraPropagation Plugin binary
if (PLUGIN_EXTRAPROPAGATOR)
endif()

# ====================================================================
# GmatFunction Plugin binary
if (PLUGIN_GMATFUNCTION)
endif()

# ====================================================================
# MatlabInterface Plugin binary
if (PLUGIN_MATLABINTERFACE)
endif()
