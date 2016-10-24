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
# Set overall GMAT directory structure
# executed when running "make install" or building the VS "INSTALL" target

MESSAGE("==============================")
MESSAGE("Setting up install directory structure for GMAT " ${VERSION})

# ====================================================================
# Create the overall GMAT directory structure
MESSAGE(STATUS "Will install GMAT to " ${CMAKE_INSTALL_PREFIX})

# Install common items from application/ directory
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/application/"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  USE_SOURCE_PERMISSIONS
  PATTERN ".*" EXCLUDE  # e.g. .gitignore, .ds_store, etc...
  PATTERN "debug" EXCLUDE # don't copy debug directory
  PATTERN "bin" EXCLUDE # don't copy bin directory
  PATTERN "plugins" EXCLUDE # don't copy plugins directory
  )

# Install gmat_startup_file based on OS and build configuration
IF(UNIX)
  INSTALL( FILES
    ${PROJECT_SOURCE_DIR}/application/bin/gmat_startup_file_mac_linux.txt
    DESTINATION bin
    CONFIGURATIONS Release MinSizeRel
    RENAME gmat_startup_file.txt
    )
ELSE()
  INSTALL( FILES
    ${PROJECT_SOURCE_DIR}/application/bin/gmat_startup_file.txt
    DESTINATION bin
    CONFIGURATIONS Release MinSizeRel
    )
ENDIF()

INSTALL( FILES
  ${PROJECT_SOURCE_DIR}/application/debug/gmat_startup_file_debuginstall.txt
  DESTINATION bin
  CONFIGURATIONS Debug RelWithDebInfo
  RENAME gmat_startup_file.txt
  )

# Install GMAT.ini based on build configuration
INSTALL( FILES
  ${PROJECT_SOURCE_DIR}/application/bin/GMAT.ini
  DESTINATION bin
  CONFIGURATIONS Release MinSizeRel
  )

INSTALL( FILES
  ${PROJECT_SOURCE_DIR}/application/debug/GMAT.ini
  DESTINATION bin
  CONFIGURATIONS Debug RelWithDebInfo
  )
