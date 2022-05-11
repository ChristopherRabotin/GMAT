#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "xerces_xerces-c" for configuration ""
set_property(TARGET xerces_xerces-c APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(xerces_xerces-c PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libxerces-c-3.2.so"
  IMPORTED_SONAME_NOCONFIG "libxerces-c-3.2.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS xerces_xerces-c )
list(APPEND _IMPORT_CHECK_FILES_FOR_xerces_xerces-c "${_IMPORT_PREFIX}/lib/libxerces-c-3.2.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
