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
cmake_minimum_required(VERSION 3.14.0)
CMAKE_POLICY(VERSION 3.14)

# Require OSX minimum version
IF(APPLE)
  SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.14 CACHE STRING
    "Minimum OSX version to target for deployment (at runtime). Set to empty string to target current MacOS version.")
ENDIF()

# Set Cmake project
PROJECT(GMAT C CXX)
SET(GMAT_RELEASE_NAME "R2020a" CACHE STRING "GMAT version name")
SET(GMAT_VERSION ${GMAT_RELEASE_NAME})
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Enforce C++ Standard. C++17 is preferred.
SET(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ Standard used for the build. C++17 is preferred")
SET_PROPERTY(CACHE CMAKE_CXX_STANDARD PROPERTY STRINGS 17 14 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# Path to custom Find****.cmake files
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/build/cmake_modules")

# Detect 32/64 bit compiler
if(CMAKE_CL_64 OR (CMAKE_SIZEOF_VOID_P EQUAL 8))
  SET(GMAT_64_BIT ON)
  MESSAGE(STATUS "*** Creating 64-bit GMAT Build System. Please ensure that 64-bit versions of all dependencies are available. ***")
else()
  SET(GMAT_64_BIT OFF)
  MESSAGE(STATUS "*** Creating 32-bit GMAT Build System. Please ensure that 32-bit versions of all dependencies are available. ***")
endif()

# Default to Release configuration on single-config generators
# e.g. make, but not VisualStudio or XCode
# Note that CMAKE_BUILD_TYPE is initialized to "" on the initial cmake-gui
# run, so we check for that and set the default appropriately
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build Configuration (Release, Debug, RelWithDebInfo, or MinSizeRel" FORCE)
endif()

# Set default GMAT installation location
# Can be overridden by using "-DCMAKE_INSTALL_PREFIX=/foo/bar" at command line
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(GMAT_64_BIT)
    SET(GMAT_RELEASE_TYPE "x64")
  else()
    SET(GMAT_RELEASE_TYPE "x86")
  endif()

  SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/GMAT-${GMAT_RELEASE_NAME}-${CMAKE_SYSTEM_NAME}-${GMAT_RELEASE_TYPE}" CACHE PATH "GMAT top-directory installation location" FORCE )
endif()

# Set path to app bundle on Mac, relative to CMAKE_INSTALL_PREFIX
# This may need to be a user-selectable cache variable in the future
if(APPLE)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    SET(GMAT_MAC_APPBUNDLE_PATH "bin/GMATd-${GMAT_RELEASE_NAME}_Beta.app/Contents")
  else()
    SET(GMAT_MAC_APPBUNDLE_PATH "bin/GMAT-${GMAT_RELEASE_NAME}_Beta.app/Contents")
  endif()
endif()

# Additional Linux compile/link flags
IF(UNIX AND NOT APPLE)
  SET(GMAT_RUN_ADDRESS_SANITIZER OFF CACHE BOOL "Check Memory using Address Sanitizer")

  # Tell CMake Find* commands to search for lib64 paths on Linux
  SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)

  # Generate link errors for undefined symbols 
  # This is enabled by default on OSX but not Linux
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  IF (GMAT_RUN_ADDRESS_SANITIZER)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -lasan -fno-omit-frame-pointer -fsanitize=address")
    SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -lasan -fno-omit-frame-pointer -fsanitize=address")
  ENDIF()
ENDIF()

# ====================================================================
# Allow user to enable GMAT components

OPTION(GMAT_INCLUDE_GUI "Build the GMAT GUI" ON)
OPTION(GMAT_INCLUDE_CSALT "Build CSALT with GMAT" OFF)
OPTION(GMAT_INCLUDE_CSALT_TESTPROGRAM "Build CSALT test program" OFF)
OPTION(GMAT_INCLUDE_API "Build the GMAT API" OFF)

# ====================================================================
# Enable boost::variant as needed

# Use if on OSX before 10.14
if(APPLE AND (CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS 10.14))
   SET(GMAT_USE_BOOST_VARIANT ON)
else()
   SET(GMAT_USE_BOOST_VARIANT OFF)
endif()

# Use if C++17 not available
if(NOT ${CMAKE_CXX_STANDARD} MATCHES "17")
   SET(GMAT_USE_BOOST_VARIANT ON)
endif()

# ====================================================================
# Mark Boost as a dependency if needed

if (GMAT_USE_BOOST_VARIANT OR GMAT_INCLUDE_CSALT)
   SET(GMAT_USE_BOOST ON)
else()
   SET(GMAT_USE_BOOST OFF)
endif()

# ====================================================================
# Setup CMake-managed dependencies
ADD_SUBDIRECTORY(depends)

# ====================================================================
# Find wxWidgets

if(GMAT_INCLUDE_GUI)
   # Set default wxWidgets directory based on OS
   SET(wxWidgets_PARTIAL_DIR "${PROJECT_SOURCE_DIR}/depends/wxWidgets")
   if(WIN32)
     # On Windows, FindwxWidgets searches the wxWidgets_ROOT_DIR path
     SET(wxWidgets_ROOT_DIR "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.4" CACHE PATH "wxWidgets Root Directory")

   elseif(UNIX)
     # On Mac/Linux, FindwxWidgets uses the wx-config utility which must be
     # accessible from the system PATH environment variable. 
     # So first save PATH (just the first time to avoid recursive setting)...
     if(NOT wxWidgets_ROOT_DIR)
       SET(SYS_PATH "$ENV{PATH}" CACHE INTERNAL "System PATH environment variable")
     endif()

     # ... then get the expected path to the wx-config utility ...
     if(APPLE)
       SET(wxWidgets_ROOT_DIR  "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.4/cocoa-install/bin" CACHE PATH "wxWidgets Root Directory")
     else()
       SET(wxWidgets_ROOT_DIR  "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.4/gtk-install/bin" CACHE PATH "wxWidgets Root Directory")
     endif()

     # ... then prepend that to the PATH environment variable
     SET(ENV{PATH} "${wxWidgets_ROOT_DIR}:${SYS_PATH}")

     # Clear wxWidgets_CONFIG_EXECUTABLE in case user changed wxWidgets_ROOT_DIR
     UNSET(wxWidgets_CONFIG_EXECUTABLE CACHE) 
   endif()

   # Find wxWidgets
   FIND_PACKAGE(wxWidgets COMPONENTS core base xml net richtext aui xrc qa html adv stc gl)
endif()

# ====================================================================
# Find CSPICE

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
  SET(CSPICE_DEPENDS_DIR "${CSPICE_DIR_TEMP}/cspice64")
else()
  SET(CSPICE_DEPENDS_DIR "${CSPICE_DIR_TEMP}/cspice32")
endif()

# Search for CSPICE folder in default paths
# User can change this via CMake GUI or command line
FIND_PATH(CSPICE_DIR
  NAMES include/SpiceUsr.h
  PATHS
    ${CSPICE_DEPENDS_DIR}
  DOC "Path to CSpice root directory (containing include/, lib/, etc.)"
  NO_DEFAULT_PATH
  )

# Make sure CSPICE exists
if(CSPICE_DIR)
  MESSAGE(STATUS "Found CSPICE: " ${CSPICE_DIR})

  # Set CSPICE library files
  if(WIN32)
    SET(CSPICE_EXT "lib")
  else()
    SET(CSPICE_EXT "a")
  endif()
  SET(CSPICE_LIB "${CSPICE_DIR}/lib/cspice.${CSPICE_EXT}")
  SET(CSPICE_LIB_DEBUG "${CSPICE_DIR}/lib/cspiced.${CSPICE_EXT}")
  SET(CSPICE_INCLUDE_DIR "${CSPICE_DIR}/include")
  if(NOT EXISTS ${CSPICE_LIB_DEBUG})
    SET(CSPICE_LIB_DEBUG ${CSPICE_LIB})
  endif()
else()
  MESSAGE(ERROR "CSPICE NOT FOUND. Set CSPICE_DIR to path to include/SpiceUsr.h and re-configure.")
endif()

# Add SPICE preprocessor definition to all projects
ADD_DEFINITIONS("-D__USE_SPICE__")

# ====================================================================
# Find F2C

# By default use the CSPICE version of F2C
SET(F2C_DEPENDS_DIR ${CSPICE_INCLUDE_DIR})

# Search for F2C folder in default paths
# User can change this via CMake GUI or command line
FIND_PATH(F2C_DIR
  NAMES f2c.h
  PATHS 
    ${F2C_DEPENDS_DIR}
  DOC "Path to F2C directory (containing f2c.h)"
  NO_DEFAULT_PATH
  )

# Make sure F2C exists
if(F2C_DIR)
  MESSAGE(STATUS "Found F2C: " ${F2C_DIR})
else()
  MESSAGE(WARNING "F2C NOT FOUND. Set F2C_DIR to path to f2c.h and re-configure.")
endif()

# ====================================================================
# Find Matlab
SET(Matlab_ROOT_DIR "" CACHE PATH "Path to user-specified MATLAB location. Leave empty to auto-find MATLAB.")
SET(MATLAB_ADDITIONAL_VERSIONS
  # Additional Matlab versions should be included here
  "R2016a=9.0"
  "R2016b=9.1"
  "R2017a=9.2"
  "R2017b=9.3"
  CACHE STRING "Additional searched MATLAB versions. Format as semicolon-separated Release=Version pairs, e.g. R20YYx=V.v"
  )
FIND_PACKAGE(Matlab COMPONENTS MAIN_PROGRAM MX_LIBRARY ENG_LIBRARY MAT_LIBRARY)

if(NOT Matlab_FOUND)
  IF(GMAT_64_BIT)
    SET(MATLAB_BIT 64-bit)
  ELSE()
    SET(MATLAB_BIT 32-bit)
  ENDIF()
  MESSAGE(WARNING "${MATLAB_BIT} Matlab NOT FOUND. Set Matlab_ROOT_DIR to the path to Matlab and ensure you have a working license.")
endif()

# ====================================================================
# Find Python
# User can set PYTHON_LIBRARY and PYTHON_INCLUDE_DIR if they have a custom installation
FIND_PACKAGE(PythonLibs)

# Let users know where to download Python
if(NOT PYTHONLIBS_FOUND)
  if(WIN32)
    SET(PYTHONURL "http://www.python.org/downloads/windows/")
  else()
    if(APPLE)
      SET(PYTHONURL "http://www.python.org/downloads/mac-osx/")
    else()
      SET(PYTHONURL "the Linux package manager")
    endif()
  endif()
  MESSAGE(WARNING "Python NOT FOUND. Please set PYTHON_LIBRARY (advanced variable) to the full path of pythonXX.lib. Python can be downloaded from ${PYTHONURL}.")
endif()

# ====================================================================
# Find Xerces

# Initialize default GMAT dependency path for Xerces
if(WIN32)
  SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/depends/xerces/windows-install/include")
  SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/depends/xerces/windows-install/lib")
elseif(APPLE)
  SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROJECT_SOURCE_DIR}/depends/xerces/cocoa-install")
else()
  SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROJECT_SOURCE_DIR}/depends/xerces/linux-install")
endif()

FIND_PACKAGE(XercesC REQUIRED)

# Additional libraries needed to use Xerces on Mac/Linux
if(UNIX)
  if(APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices)
  else()
    FIND_PACKAGE(Threads)
  endif()
endif()

# ====================================================================
# Find SWIG

# Handle SWIG policy warning
cmake_policy(SET CMP0086 "NEW")

# Initialize default GMAT dependency path for SWIG
if(WIN32)
  SET(SWIG_ROOT "${PROJECT_SOURCE_DIR}/depends/swig/swigwin")
elseif(APPLE)
  SET(SWIG_ROOT "${PROJECT_SOURCE_DIR}/depends/swig/swig/cocoa-install")
else()
  SET(SWIG_ROOT "${PROJECT_SOURCE_DIR}/depends/swig/swig/linux-install")
  SET(SWIG_EXECUTABLE_ROOT "${SWIG_ROOT}/bin")

  # Need to append SWIG to path to search for "swig" before "swig*.0" from package repositories
  SET(ENV{PATH} "${SWIG_EXECUTABLE_ROOT}:${SYS_PATH}")
  FIND_PROGRAM(SWIG_EXECUTABLE NAMES swig)
endif()

FIND_PACKAGE(SWIG)
# ====================================================================

# ====================================================================
# ------------------------------------------------------------------ #
# CSALT Dependencies: Only required if building the CSALT subsystem  #
# ------------------------------------------------------------------ #
if (GMAT_INCLUDE_CSALT)
   SET(CSALT_DEPENDS_FAIL FALSE)

   # Find SNOPT in depends folder
   SET(SNOPT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/snopt7" CACHE PATH "SNOPT Root Directory")
   SET(SNOPT_LIB_DIR "${SNOPT_ROOT_DIR}/lib" CACHE PATH "SNOPT Library Directory")
   SET(SNOPT_INCLUDE_DIR ${SNOPT_ROOT_DIR}/interfaces/include)
   FIND_LIBRARY(SNOPT_CPPLIBRARY snopt7_cpp PATHS ${SNOPT_LIB_DIR} NO_DEFAULT_PATH)
   FIND_LIBRARY(SNOPT_LIBRARY snopt7 PATHS ${SNOPT_LIB_DIR} NO_DEFAULT_PATH)
   if(NOT SNOPT_CPPLIBRARY)
      MESSAGE(WARNING "SNOPT CPP library not found. Check that SNOPT_ROOT_DIR contains a valid SNOPT installation.")
      SET(CSALT_DEPENDS_FAIL TRUE)
   endif()
   if(NOT SNOPT_LIBRARY)
      MESSAGE(WARNING "SNOPT library not found. Check that SNOPT_ROOT_DIR contains a valid SNOPT installation.")
      SET(CSALT_DEPENDS_FAIL TRUE)
   endif()

   # Fail Configuration if any dependencies are missing
   if(CSALT_DEPENDS_FAIL)
      MESSAGE(FATAL_ERROR "Missing required CSALT dependencies: aborting build.")
   else()
      MESSAGE(STATUS "Found SNOPT: " ${SNOPT_ROOT_DIR})
   endif()
endif()

# ====================================================================
# Common build options

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

# check fPIC flag on GCC compilers
if(CMAKE_COMPILER_IS_GNUCC)
  CHECK_CXX_COMPILER_FLAG("-fPIC" GCC_PIC)
  if(GCC_PIC)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  endif()
endif()

# flags to append to CMAKE_CXX_FLAGS according to CMAKE_BUILD_TYPE 
if(UNIX)
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
  SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
  SET(CMAKE_CXX_FLAGS_MINSIZEREL "-O3")
endif()

# Common definitions
ADD_DEFINITIONS(-DNO_GCC_PRAGMA)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

if(GMAT_USE_BOOST_VARIANT)
   ADD_DEFINITIONS(-DGMAT_USE_BOOST_VARIANT)
endif()

# Mark debug libraries with a "d" suffix
SET(CMAKE_DEBUG_POSTFIX "d")

# Warning suppression definitions
# Note that warning C4430 presents as an error in the MatlabInterface plugin
if(WIN32)
  ADD_DEFINITIONS("/wd4430 /wd4251 /wd4231 /wd4996 /nologo")
endif()

# Set 32/64-bit specific definitions and directories
if(GMAT_64_BIT)
  ADD_DEFINITIONS("-DUSE_64_BIT_LONGS")
else()
endif()

# Additional *NIX-specific definitions
if(UNIX)
  ADD_DEFINITIONS("-DLINUX_MAC")

  if(APPLE)
    # GMAT GUI has many instances of text like "XX"GUI_ACCEL_KEY"YY"
    # This is incompatible with the C++11 standard, and should be fixed
    # For now, disable this compile error (-Wreserved-user-defined-literal)
    # [UPDATE] Removed since this no longer seems to create build errors.
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-user-defined-literal")
    
    # Silence OpenGL deprecation warnings
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
  endif()
endif()

# Enable RPATH on Mac/Linux so that GMAT shared libraries can be found
# relative to each other. This is necessary to distribute GMAT and use it
# from 3rd-party apps such as Matlab.
if(UNIX)
  if(APPLE)
    SET(CMAKE_MACOSX_RPATH TRUE) # Enable Mac RPATH
  else()
    # Linux searches the RPATH before the system LD_LIBRARY_PATH, so enable
    # the use of RUNPATH which is searched after LD_LIBRARY_PATH. This makes
    # the Linux search order similar to Mac/Windows.
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-new-dtags")
  endif()
endif()

# ====================================================================
# Define macro that sets the output directory for build products,
# e.g. executables or shared libraries. By default these are placed under
# the <GMAT>/application/ directory so that devs can easily test without
# having to install the build (via make install or VS INSTALL).
# This default location can be changed by the user.
SET(GMAT_BUILDOUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/application" CACHE PATH "Base directory for GMAT binaries. Should contain bin, plugins, etc.")
SET(GMAT_BUILDOUTPUT_DEBUGDIR "${GMAT_BUILDOUTPUT_DIRECTORY}/debug")
MACRO(_SETOUTPUTDIRECTORY TargetName OUTPUT_DIR)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}/${OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}/${OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${GMAT_BUILDOUTPUT_DIRECTORY}/${OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${GMAT_BUILDOUTPUT_DIRECTORY}/${OUTPUT_DIR}
    )
ENDMACRO()

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
# Setup GMAT directory structure
INCLUDE(build/install/CMake_INSTALL_Setup.cmake)

# ====================================================================
# Go to src directory and look for CMake instructions there
ADD_SUBDIRECTORY(src)

if (GMAT_INCLUDE_API)
  # ====================================================================
  # Go to swig directory and look for CMake instructions there
  ADD_SUBDIRECTORY(swig)
endif()

# ====================================================================
# Go to plugins directory and look for CMake instructions there
ADD_SUBDIRECTORY(plugins)

# ====================================================================
# Setup GMAT install process
ADD_SUBDIRECTORY(build/install)
