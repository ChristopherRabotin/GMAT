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

# Install the default application/ directory structure
MESSAGE(STATUS "Will install GMAT to " ${CMAKE_INSTALL_PREFIX})
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/application/"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  USE_SOURCE_PERMISSIONS
  PATTERN ".*" EXCLUDE  # e.g. .gitignore, .ds_store, etc...
  PATTERN "debug" EXCLUDE # don't copy debug directory
  )

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
    SET(MSTART "\"${Matlab_MAIN_PROGRAM}\" -nodisplay -r")
    SET(MPATH1 "addpath('${CMAKE_INSTALL_PREFIX}/bin');")
    SET(MPATH2 "addpath('${CMAKE_INSTALL_PREFIX}/matlab/libCInterface');")
    SET(MCD "cd('${PROJECT_SOURCE_DIR}/plugins/CInterfacePlugin/build/matlab');")
    SET(MPREP "prepareInterface_cmake;")
    SET(MEXIT "exit;")
    SET(MATLAB_COMMAND "${MSTART} \"${MPATH1} ${MPATH2} ${MCD} ${MPREP} ${MEXIT}\"")
    MESSAGE(STATUS "Will install CInterface Matlab thunk files with command:\n${MATLAB_COMMAND}")

    # Running Matlab can take a while, so notify the user
    if(WIN32)
      INSTALL(CODE "MESSAGE(STATUS \"Launching Matlab in a separate window...patience is a virtue...\")")
    else()
      INSTALL(CODE "MESSAGE(STATUS \"Launching Matlab...patience is a virtue...\")")
    endif()

    # Git 'er done!
    INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${MATLAB_COMMAND} WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/bin\")")

  # Warn user if Matlab was not found
  else()
    MESSAGE(WARNING "Matlab not found, will not build Matlab prototype files for the GMAT CInterface Plugin.")
  endif()
endif()
