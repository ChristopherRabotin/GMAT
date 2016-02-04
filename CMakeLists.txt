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

# Support OSX 10.8+
IF(APPLE)
  SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.8 CACHE STRING 
    "Minimum OSX version to target for deployment (at runtime). Set to empty string to target current MacOS version.")
ENDIF()

# Set Cmake project
PROJECT(GMAT C CXX)
SET(GMAT_RELEASE_NAME "R2015a" CACHE STRING "GMAT version name")
SET(GMAT_VERSION ${GMAT_RELEASE_NAME})

# Path to custom Find****.cmake files
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/build/cmake_modules")

# Detect 32/64 bit
if(CMAKE_CL_64 OR (CMAKE_SIZEOF_VOID_P EQUAL 8))
  SET(GMAT_64_BIT ON)
else()
  SET(GMAT_64_BIT OFF)
endif()

# Default to Release configuration on single-config generators
# e.g. make, but not VisualStudio or XCode
# Note that CMAKE_BUILD_TYPE is initialized to "" on the initial cmake-gui
# run, so we check for that and set the default appropriately
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build Configuration (Debug, Release, RelWithDebInfo, or MinSizeRel" FORCE)
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
OPTION(GMAT_GUI "Build the GUI, requires wxWidgets" ON)
if(APPLE)
  if(GMAT_GUI)
    if((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR 
      (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
      SET(GMAT_MAC_APPBUNDLE_PATH "bin/GMATd.app/Contents")
    else()
      SET(GMAT_MAC_APPBUNDLE_PATH "bin/GMAT_Beta.app/Contents")
    endif()
  else()
    SET(GMAT_MAC_APPBUNDLE_PATH "")
  endif()
endif()

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
    SET(wxWidgets_ROOT_DIR  "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.2/cocoa-install/bin" CACHE PATH "wxWidgets Root Directory")
  else()
    SET(wxWidgets_ROOT_DIR  "${wxWidgets_PARTIAL_DIR}/wxWidgets-3.0.2/gtk-install/bin" CACHE PATH "wxWidgets Root Directory")
  endif()

  # ... then append that to the PATH environment variable
  SET(ENV{PATH} "${SYS_PATH}:${wxWidgets_ROOT_DIR}")

  # Clear wxWidgets_CONFIG_EXECUTABLE in case user changed wxWidgets_ROOT_DIR
  UNSET(wxWidgets_CONFIG_EXECUTABLE CACHE) 
endif()

# Find wxWidgets
FIND_PACKAGE(wxWidgets COMPONENTS core base xml net richtext aui xrc qa html adv stc gl)

# User option for SPICE
OPTION(GMAT_USE_SPICE "Use CSPICE" ON)
if(GMAT_USE_SPICE)
  ADD_DEFINITIONS("-D__USE_SPICE__")
  INCLUDE_DIRECTORIES("${CSPICE_DIR}/include")
endif()

# Search for CSPICE even if it is not selected, because
# the CSPICE libs contain the F2C implementation

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
  if(NOT EXISTS ${CSPICE_LIB_DEBUG})
    SET(CSPICE_LIB_DEBUG ${CSPICE_LIB})
  endif()
else()
  MESSAGE(WARNING "CSPICE NOT FOUND. Set CSPICE_DIR to path to include/SpiceUsr.h and re-configure.")
endif()

# By default use the CSPICE version of F2C
SET(F2C_DEPENDS_DIR "${CSPICE_DIR}/include")

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

# Find Matlab
SET(Matlab_ROOT_DIR "" CACHE PATH "Path to user-specified MATLAB location. Leave empty to auto-find MATLAB.")
SET(MATLAB_ADDITIONAL_VERSIONS
  # Additional Matlab versions should be included here
  "R2015b=8.6"
  CACHE STRING "Additional searched MATLAB versions. Format as semicolon-separated Release=Version pairs, e.g. R20YYx=V.v"
  )
FIND_PACKAGE(Matlab COMPONENTS MAIN_PROGRAM MX_LIBRARY ENG_LIBRARY)

if(NOT Matlab_FOUND)
  MESSAGE(WARNING "Matlab NOT FOUND. Set Matlab_ROOT_DIR to the path to Matlab and ensure you have a working license.")
endif()

# Find Python
# todo: Python 2.7; let the user set library name?
MESSAGE(STATUS "Searching for Python")
SET(PYTHON_DIR "${PROJECT_SOURCE_DIR}/depends/Python")
SET(PYTHON_LIB_DIR "" CACHE PATH "Path to location of the Python library.")
SET(PYTHON_INCLUDE_DIR "" CACHE PATH "Path to location of python.h")
MESSAGE(STATUS "Please set the Python locations")
# FIND_PACKAGE(PythonLibs)

if(UNIX)
  if(APPLE)
    # Check this for Mac
    SET(PYTHON_LIBRARY "python3.4m")
  else()
    SET(PYTHON_LIBRARY "python3.4m")
  endif()
else()
  SET(PYTHON_LIBRARY "python34.lib")
endif()


MESSAGE(STATUS "Python Library: " ${PYTHON_LIBRARY})

if(PYTHON_LIB_DIR)
  MESSAGE(STATUS "Found Python: " ${PYTHON_LIB_DIR})
else()
  MESSAGE(WARNING "Python NOT FOUND. Set PYTHON_LIB_DIR to your Python34.lib library path, and PYTHON_INCLUDE_DIR to the path to Python.h and re-configure.")
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

# Mark debug libraries with a "d" suffix
SET(CMAKE_DEBUG_POSTFIX "d")

# Linker Flags
if(UNIX)
  if(NOT APPLE)
    LINK_LIBRARIES("dl")	
  endif()
endif()

# Warning suppression definitions
# Note that warning C4430 presents as an error in the MatlabInterface plugin
if(WIN32)
  ADD_DEFINITIONS("/wd4430 /wd4251 /wd4231 /wd4996 /nologo")
endif()

# Set 64 bit Definitions and directories
if(GMAT_64_BIT)
  MESSAGE(STATUS "Will compile 64-bit GMAT")
  ADD_DEFINITIONS("-DUSE_64_BIT_LONGS")
else()
  MESSAGE(STATUS "Will compile 32-bit GMAT")
endif()

# Additional *NIX-specific definitions
if(UNIX)
  ADD_DEFINITIONS("-DLINUX_MAC")

  if(APPLE)
    # GMAT GUI has many instances of text like "XX"GUI_ACCEL_KEY"YY"
    # This is incompatible with the C++11 standard, and should be fixed
    # For now, disable this compile error (-Wreserved-user-defined-literal)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-user-defined-literal")
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

# Declare global var for GmatBase header directories
# This will be defined in the GmatBase CMakeLists.txt file
SET(GMAT_BASE_INCLUDE_DIRECTORIES "" CACHE INTERNAL "base includes")

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

# ====================================================================
# Go to plugins directory and look for CMake instructions there
ADD_SUBDIRECTORY(plugins)

# ====================================================================
# Setup GMAT install process
ADD_SUBDIRECTORY(build/install)

