# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Ground Station SWIG interface
# This file must be installed in the plugins/StationPlugin/swig directory
#
# Author: Darrel Conway
# 
MESSAGE("==============================")
MESSAGE("STATION SWIG setup " ${VERSION}) 

INCLUDE(${PROJECT_SOURCE_DIR}/build/cmake_modules/GmatSwigConfig.cmake)

if (API_GENERATE_PYTHON)
  _SETUPSWIG("station" "Python" "station_py.i" "Station;gmat${API_PYTHON_SUFFIX}")
  ADD_DEPENDENCIES(station${API_PYTHON_SUFFIX} gmat${API_PYTHON_SUFFIX} Station)
  SWIG_LINK_LIBRARIES(station${API_PYTHON_SUFFIX} Station)
endif()

if (API_GENERATE_JAVA)
  # Get location of gmat.jar
  SET(OUTPUT_DIR $<$<CONFIG:Release>:${GMAT_BUILDOUTPUT_DIRECTORY}/bin>$<$<CONFIG:Debug>:${GMAT_BUILDOUTPUT_DEBUGDIR}>)
  SET(SWIG_EXTRA_JAR_FILES "${OUTPUT_DIR}/gmat.jar")

  _SETUPSWIG("station" "Java" "station.i" "Station;gmat${API_JAVA_SUFFIX}")
  ADD_DEPENDENCIES(station${API_JAVA_SUFFIX} gmat${API_JAVA_SUFFIX} Station)
  SWIG_LINK_LIBRARIES(station${API_JAVA_SUFFIX} Station)
endif()
