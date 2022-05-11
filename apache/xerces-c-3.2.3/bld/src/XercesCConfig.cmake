
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was XercesCConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(${CMAKE_CURRENT_LIST_DIR}/XercesCConfigInternal.cmake)

add_library(XercesC::XercesC INTERFACE IMPORTED)
set_target_properties(XercesC::XercesC PROPERTIES INTERFACE_LINK_LIBRARIES xerces_xerces-c)

# For compatibility with FindXercesC.
set(XercesC_VERSION "3.2.3")
get_target_property(XercesC_INCLUDE_DIRS xerces_xerces-c INTERFACE_INCLUDE_DIRECTORIES)
set(XercesC_LIBRARIES XercesC::XercesC)
