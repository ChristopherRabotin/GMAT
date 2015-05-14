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

MESSAGE("==============================")
MESSAGE("Setting up wxWidgets install instructions for GMAT " ${VERSION})

# ====================================================================
# Install wxWidgets libraries
# This should be investigated further, since it's possible to build
# release/debug GMAT using either release/debug wxWidgets. Therefore there
# should be a nice cross-platform way of checking for the version of
# wxWidgets that is actually being used.
if(wxWidgets_FOUND)
  if(WIN32)
    # For Windows, dlls must be next to executables in bin
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
  else()
    # For Mac/Linux, wxlibs are in bin/libwx or Mac app bundle
    if(APPLE AND GMAT_MAC_APPBUNDLE_PATH)
      SET(WX_INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
    else()
      SET(WX_INSTALL_DIR bin/libwx)
    endif()

    # Set wxlib type based on debug/release
    # This is needed because it's possible (on Mac/Linux) to build debug
    # GMAT using release wxWidgets
    if(wxWidgets_USE_DEBUG)
      SET(WX_DEBUG_PATTERN "libwx_*ud[_-]*")
    else()
      SET(WX_DEBUG_PATTERN "libwx_*u[_-]*")
    endif()

    INSTALL(DIRECTORY ${wxWidgets_LIBRARY_DIRS}/
      DESTINATION ${WX_INSTALL_DIR}
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Release MinSizeRel
      FILES_MATCHING PATTERN "libwx_*u[_-]*"
      PATTERN "wx" EXCLUDE
      )
    INSTALL(DIRECTORY ${wxWidgets_LIBRARY_DIRS}/
      DESTINATION ${WX_INSTALL_DIR}
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Debug RelWithDebInfo
      FILES_MATCHING PATTERN "${WX_DEBUG_PATTERN}"
      PATTERN "wx" EXCLUDE
      )

    # On Mac, change dylib install names to use RPATH
    # This is done using the wxmac_change_install_names.sh script
    if(APPLE)
      # Create command for wxmac_change_install_names script
      SET(CHNM "./wxmac_change_install_names.sh")
      SET(ARG1 "\"${CMAKE_INSTALL_PREFIX}/${WX_INSTALL_DIR}\"")
      SET(ARG2 "\"${wxWidgets_LIBRARY_DIRS}\"")
      SET(ARG3 "\"@rpath\"")

      # Collect list of all GMAT components that use wxWidgets
      if(GMAT_MAC_APPBUNDLE_PATH)
	SET(MAC_GUIDIR ${GMAT_MAC_APPBUNDLE_PATH}/MacOS)
	SET(MAC_PLUGINDIR ${GMAT_MAC_APPBUNDLE_PATH}/plugins)
      else()
	SET(MAC_GUIDIR bin)
	SET(MAC_PLUGINDIR plugins)
      endif()

      if(GMAT_GUI)
	SET(ARGN "\"${CMAKE_INSTALL_PREFIX}/${MAC_GUIDIR}/GMAT*\"")
      endif()

      if(PLUGIN_MATLABINTERFACE)
	SET(ARGN "${ARGN} \"${CMAKE_INSTALL_PREFIX}/${MAC_PLUGINDIR}/libMatlabInterface*.dylib\"")
      endif()

      # Append full pathname of new components here using above pattern

      # Finally create install command to change rpaths and install names
      SET(CHNM_COMMAND "${CHNM} ${ARG1} ${ARG2} ${ARG3} ${ARGN}")
      INSTALL(CODE "MESSAGE(STATUS \"Changing wxWidgets library install names\")")
      INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${CHNM_COMMAND} WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\")")
    endif()
  endif()
endif()
