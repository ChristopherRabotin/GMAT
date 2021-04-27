# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT SWIG interface
# This file must be installed in the swig directory
#
# Author: Matthew Handy
# 
MESSAGE("==============================")
MESSAGE("GMAT SWIG setup " ${VERSION}) 

OPTION(API_GENERATE_PYTHON "Python API" ON)
OPTION(API_GENERATE_JAVA "Java API" ON)

INCLUDE(${PROJECT_SOURCE_DIR}/build/cmake_modules/GmatSwigConfig.cmake)
  
# Enable big objects for SWIG generated code
if(MSVC)
  SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
  SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
  SET (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /bigobj")
endif()

SET(SWIG_IGNORED_WARNINGS "516,509" CACHE STRING "SWIG warnings to ignore when building the API")
string(LENGTH SWIG_IGNORED_WARNINGS WARNSIZE)

if (WARNSIZE GREATER 1)
   SET(CMAKE_SWIG_FLAGS "-w${SWIG_IGNORED_WARNINGS}")
endif()

if(GMAT_USE_BOOST_VARIANT)
   SET(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -DGMAT_USE_BOOST_VARIANT)
endif()

if(API_GENERATE_PYTHON)
   # define Python bindings target
   SET(API_PYTHON_SUFFIX "_py" CACHE STRING "Suffix for the Python API bindings")

   _SETUPSWIG("gmat" "Python" "gmat_py.i" "GmatUtil;GmatBase")
   ADD_DEPENDENCIES(${TargetNameFull} GmatBase)
endif()

if (API_GENERATE_JAVA)
   # define Java bindings target
   SET(API_JAVA_SUFFIX "_java" CACHE STRING "Suffix for the Java API bindings")

   SET(SWIG_EXTRA_JAVA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/gmat_matlab_loadlibrary.java")
   _SETUPSWIG("gmat" "Java" "gmat.i" "GmatUtil;GmatBase")
   ADD_DEPENDENCIES(${TargetNameFull} GmatBase)
endif()

if(WIN32)
  if (API_GENERATE_JAVA)
    INSTALL(FILES GMATAPI.m DESTINATION bin)
  endif()
else()
  if(APPLE AND GMAT_MAC_APPBUNDLE_PATH)
    SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
  else()
    # dump at bin, python modules need to execute from GmatBase for now
    SET(INSTALL_DIR bin)
  endif()

   if (API_GENERATE_JAVA)
     INSTALL(FILES GMATAPI.m DESTINATION ${INSTALL_DIR})
   endif()
endif()

