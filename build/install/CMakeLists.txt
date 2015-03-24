# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE install script for GMAT Project
# This file must be installed in the GMAT build/install directory
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
# Set overall install instructions
# executed when running "make install" or building the VS "INSTALL" target

MESSAGE("==============================")
MESSAGE("Setting up steps to install GMAT " ${VERSION})

# ====================================================================
# Install the default application/ directory structure
MESSAGE(STATUS "Will install GMAT to " ${CMAKE_INSTALL_PREFIX})
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/application/"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  USE_SOURCE_PERMISSIONS
  PATTERN ".*" EXCLUDE  # e.g. .gitignore, .ds_store, etc...
  PATTERN "debug" EXCLUDE # don't copy debug directory
  PATTERN "bin" EXCLUDE # don't copy bin directory
  PATTERN "plugins" EXCLUDE # don't copy plugins directory
  )
INSTALL( FILES
  ${PROJECT_SOURCE_DIR}/application/bin/gmat_startup_file.txt
  ${PROJECT_SOURCE_DIR}/application/bin/GMAT.ini
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  )

# ====================================================================
# Install the CInterface Matlab scripts (opengmat, closegmat, etc...)
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/plugins/CInterfacePlugin/matlab/"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/matlab/libCInterface
  USE_SOURCE_PERMISSIONS
  PATTERN ".*" EXCLUDE  # e.g. .gitignore, .ds_store, etc...
  )

# Create the CInterface Matlab thunk files if Matlab exists
if(PLUGIN_CINTERFACE)
  if(Matlab_FOUND)
    # Create the command to be executed by Matlab
    SET(MSTART "\"${Matlab_MAIN_PROGRAM}\"")
    if(WIN32)
      # Need -wait on Windows so matlab call doesn't immediately return
      SET(MSTART "${MSTART} -wait")
    endif()
    SET(MSTART "${MSTART} -nosplash -nodesktop -nodisplay -r")
    SET(MPATH1 "addpath('${CMAKE_INSTALL_PREFIX}/bin');")
    SET(MPATH2 "addpath('${CMAKE_INSTALL_PREFIX}/matlab/libCInterface');")
    SET(MCD "cd('${PROJECT_SOURCE_DIR}/plugins/CInterfacePlugin/build/matlab');")
    SET(MDEBUG "debugsuffix='${CMAKE_DEBUG_POSTFIX}';")
    SET(MPREP "prepareInterface_cmake;")
    SET(MEXIT "exit;")
    SET(MATLAB_COMMAND "${MSTART} \"${MPATH1} ${MPATH2} ${MCD} ${MPREP} ${MEXIT}\"")
    SET(MATLAB_COMMAND_DEBUG "${MSTART} \"${MPATH1} ${MPATH2} ${MCD} ${MDEBUG} ${MPREP} ${MEXIT}\"")

    # Running Matlab can take a while, so notify the user
    if(WIN32)
      INSTALL(CODE "MESSAGE(STATUS \"Launching Matlab in a separate window twice to build CInterface thunk files...patience is a virtue...\")")
    else()
      INSTALL(CODE "MESSAGE(STATUS \"Launching Matlab to build CInterface thunk files...patience is a virtue...\")")
    endif()

    # Set up install commands for debug/release configurations.
    # On single-config generators (e.g. make), simply check the current config.
    # On multi-config generators (e.g. VS, XCode), there is a CMake deficiency
    # that does not allow per-generator INSTALL(CODE ...) commands. The
    # workaround for this is to try creating both release and debug versions
    # of the CInterface Matlab thunk files. The Matlab prepareInterface script
    # will gently exit if it couldn't build either one (won't error out).

    # Matlab command for multi-config or release builds
    if( CMAKE_CONFIGURATION_TYPES OR (CMAKE_BUILD_TYPE STREQUAL Release) OR (CMAKE_BUILD_TYPE STREQUAL MinSizeRel))
      # Matlab command for release configurations
      MESSAGE(STATUS "Will install CInterface (release) Matlab thunk files with command:\n${MATLAB_COMMAND}")
      INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${MATLAB_COMMAND} WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/bin\")")
    endif()

    # Matlab command for multi-config or debug builds
    if( CMAKE_CONFIGURATION_TYPES OR (CMAKE_BUILD_TYPE STREQUAL Debug) OR (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo))
      # Matlab command for debug configurations
      MESSAGE(STATUS "Will install CInterface (debug) Matlab thunk files with command:\n${MATLAB_COMMAND_DEBUG}")
      INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${MATLAB_COMMAND_DEBUG} WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/bin\")")
    endif()

  # Warn user if Matlab was not found
  else()
    MESSAGE(WARNING "Matlab not found, will not build Matlab prototype files for the GMAT CInterface Plugin.")
  endif()
endif()

# ====================================================================
# Install wxWidgets libraries
if(wxWidgets_FOUND)
  # For Windows, dlls must be next to executables in bin
  if(WIN32)
    INSTALL(DIRECTORY ${wxWidgets_LIB_DIR}/
      DESTINATION bin
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Release MinSizeRel
      FILES_MATCHING PATTERN "*u_*.dll" # Only release ("u" suffix)
      PATTERN "msw*" EXCLUDE
      )
    INSTALL(DIRECTORY ${wxWidgets_LIB_DIR}/
      DESTINATION bin
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Debug RelWithDebInfo
      FILES_MATCHING PATTERN "*ud_*.dll" # Only debug ("ud" suffix)
      PATTERN "msw*" EXCLUDE
      )
  # For Mac/Linux, dylib/so can be in subdirectory of bin
  else()
    INSTALL(DIRECTORY ${wxWidgets_LIBRARY_DIRS}/
      DESTINATION bin/libwx
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Release MinSizeRel
      FILES_MATCHING PATTERN "libwx_*u[_-]*" # Only release ("u" suffix)
      PATTERN "wx" EXCLUDE
      )
    INSTALL(DIRECTORY ${wxWidgets_LIBRARY_DIRS}/
      DESTINATION bin/libwx
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Debug RelWithDebInfo
      FILES_MATCHING PATTERN "libwx_*ud[_-]*" # Only debug ("ud" suffix)
      PATTERN "wx" EXCLUDE
      )

    # On Mac, change dylib install names to use RPATH
    # This is done using the wxmac_change_install_names.sh script
    if(APPLE)
      # Create command for wxmac_change_install_names script
      SET(CHNM "./wxmac_change_install_names.sh")
      SET(ARG1 "\"${CMAKE_INSTALL_PREFIX}/bin/libwx\"")
      SET(ARG2 "\"${wxWidgets_LIBRARY_DIRS}\"")
      SET(ARG3 "\"@rpath\"")

      # ARGN contains all GMAT components that use wxWidgets
      # Append new components here
      SET(ARGN "\"${CMAKE_INSTALL_PREFIX}/bin/Gmat\"
      \"${CMAKE_INSTALL_PREFIX}/plugins/libMatlabInterface.dylib\"")

      SET(CHNM_COMMAND "${CHNM} ${ARG1} ${ARG2} ${ARG3} ${ARGN}")
      INSTALL(CODE "MESSAGE(STATUS \"Changing wxWidgets library install names\")")
      INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${CHNM_COMMAND} WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\")")
    endif()
  endif()
endif()
