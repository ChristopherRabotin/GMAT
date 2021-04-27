# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Estimation SWIG interface
# This file must be installed in the plugins/EstimationPlugin/swig directory
#
# Author: Darrel Conway
# 
MESSAGE("==============================")
MESSAGE("ESTIMATION SWIG setup " ${VERSION}) 

INCLUDE(${PROJECT_SOURCE_DIR}/build/cmake_modules/GmatSwigConfig.cmake)

if (API_GENERATE_PYTHON)
  _SETUPSWIG("navigation" "Python" "navigation_py.i" "GmatEstimation;gmat${API_PYTHON_SUFFIX}")
  ADD_DEPENDENCIES(navigation${API_PYTHON_SUFFIX} gmat${API_PYTHON_SUFFIX} Station)
  SWIG_LINK_LIBRARIES(navigation${API_PYTHON_SUFFIX} GmatEstimation)
endif()

if (API_GENERATE_JAVA)
  # Get location of gmat.jar
  SET(OUTPUT_DIR $<$<CONFIG:Release>:${GMAT_BUILDOUTPUT_DIRECTORY}/bin>$<$<CONFIG:Debug>:${GMAT_BUILDOUTPUT_DEBUGDIR}>)
  SET(SWIG_EXTRA_JAR_FILES "${OUTPUT_DIR}/gmat.jar")

  _SETUPSWIG("navigation" "Java" "navigation.i" "GmatEstimation;gmat${API_JAVA_SUFFIX}")
  ADD_DEPENDENCIES(navigation${API_JAVA_SUFFIX} gmat${API_JAVA_SUFFIX} GmatEstimation)
  SWIG_LINK_LIBRARIES(navigation${API_JAVA_SUFFIX} GmatEstimation)
endif()
