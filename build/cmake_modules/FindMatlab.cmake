#.rst:
# FindMatlab
# ----------
#
# Finds Matlab installations and provides Matlab tools and libraries to cmake.
#
# This package first intention is to find the libraries associated with Matlab
# in order to be able to build Matlab extensions (mex files). It can also be
# used:
#
# * run specific commands in Matlab
# * declare Matlab unit test
# * retrieve various information from Matlab (mex extensions, versions and
#   release queries, ...)
#
# The module supports the following components:
#
# * ``MX_LIBRARY`` and ``ENG_LIBRARY`` respectively the MX and ENG libraries of
#   Matlab
# * ``MAIN_PROGRAM`` the Matlab binary program.
#
# .. note::
#
#   The version given to the :command:`find_package` directive is the Matlab
#   **version**, which should not be confused with the Matlab *release* name
#   (eg. `R2014`).
#   The :command:`matlab_get_version_from_release_name` and
#   :command:`matlab_get_release_name_from_version` allow a mapping
#   from the release name to the version.
#
# The variable :variable:`MATLAB_USER_ROOT` may be specified in order to give
# the path of the desired Matlab version. Otherwise, the behaviour is platform
# specific:
#
# * Windows: The installed versions of Matlab are retrieved from the
#   Windows registry
# * OS X: The installed versions of Matlab are given by the MATLAB
#   paths in ``/Application``
# * Unix: The desired Matlab should be accessible from the PATH.
#
# Additional information is provided when :variable:`MATLAB_FIND_DEBUG` is set.
# When a Matlab binary is found automatically and the ``MATLAB_VERSION``
# is not given, the version is queried from Matlab directly.
# On Windows, it can make a window running Matlab appear.
#
# The mapping of the release names and the version of Matlab is performed by
# defining pairs (name, version).  The variable
# :variable:`MATLAB_ADDITIONAL_VERSIONS` may be provided before the call to
# the :command:`find_package` in order to handle additional versions.
#
# A Matlab scripts can be added to the set of tests using the
# :command:`matlab_add_unit_test`. By default, the Matlab unit test framework
# will be used (>= 2013a) to run this script, but regular ``.m`` files
# returning an exit code can be used as well (0 indicating a success).
#
# Module Input Variables
# ----------------------
#
# Users or projects may set the following variables to configure the module
# behaviour:
#
# :variable:`MATLAB_USER_ROOT`
#   the root of the Matlab installation.
# :variable:`MATLAB_FIND_DEBUG`
#   outputs debug information
# :variable:`MATLAB_ADDITIONAL_VERSIONS`
#   additional versions of Matlab for the automatic retrieval of the installed
#   versions.
#
# Variables defined by the module
# -------------------------------
#
# Result variables
# ^^^^^^^^^^^^^^^^
#
# ``Matlab_FOUND``
#   ``TRUE`` if the Matlab installation is found, ``FALSE``
#   otherwise. All variable below are defined if Matlab is found.
# ``Matlab_ROOT_DIR``
#   the root of the Matlab installation determined by the FindMatlab module.
# ``Matlab_MAIN_PROGRAM``
#   the Matlab binary program. Available only if the component ``MAIN_PROGRAM``
#   is given in the :command:`find_package` directive.
# ``Matlab_INCLUDE_DIRS``
#  the path of the Matlab libraries headers
# ``Matlab_MEX_LIBRARY``
#   library for mex
# ``Matlab_MX_LIBRARY``
#   mx library of Matlab (arrays). Available only if the component
#   ``MX_LIBRARY`` has been requested.
# ``Matlab_ENG_LIBRARY``
#   Matlab engine library. Available only if the component ``ENG_LIBRARY``
#   is requested.
# ``Matlab_LIBRARIES``
#   the whole set of libraries of Matlab
# ``Matlab_MEX_COMPILER``
#   the mex compiler of Matlab. Currently not used.
#   Available only if the component ``MEX_COMPILER`` is asked
#
# Cached variables
# ^^^^^^^^^^^^^^^^
#
# ``Matlab_MEX_EXTENSION``
#   the extension of the mex files for the current platform (given by Matlab).
#
# Provided macros
# ---------------
#
# :command:`matlab_get_version_from_release_name`
#   returns the version from the release name
# :command:`matlab_get_release_name_from_version`
#   returns the release name from the Matlab version
#
# Provided functions
# ------------------
#
# :command:`matlab_add_mex`
#   adds a target compiling a MEX file.
# :command:`matlab_add_unit_test`
#   adds a Matlab unit test file as a test to the project.
# :command:`matlab_extract_all_installed_versions_from_registry`
#   parses the registry for all Matlab versions. Available on Windows only.
#   The part of the registry parsed is dependent on the host processor
# :command:`matlab_get_all_valid_matlab_roots_from_registry`
#   returns all the possible Matlab paths, according to a previously
#   given list. Only the existing/accessible paths are kept. This is mainly
#   useful for the searching all possible Matlab installation.
# :command:`matlab_get_mex_suffix`
#   returns the suffix to be used for the mex files
#   (platform/architecture dependant)
# :command:`matlab_get_version_from_matlab_run`
#   returns the version of Matlab, given the full directory of the Matlab program.
#
#
# Known issues
# ------------
#
# **Symbol clash in a MEX target**
#   By default, every symbols inside a MEX
#   file defined with the command :command:`matlab_add_mex` have hidden
#   visibility, except for the entry point. This is the default behaviour of
#   the MEX compiler, which lowers the risk of symbol collision between the
#   libraries shipped with Matlab, and the libraries to which the MEX file is
#   linking to. This is also the default on Windows platforms.
#
#   However, this is not sufficient in certain case, where for instance your
#   MEX file is linking against libraries that are already loaded by Matlab,
#   even if those libraries have different SONAMES.
#   A possible solution is to hide the symbols of the libraries to which the
#   MEX target is linking to. This can be achieved in GNU GCC compilers with
#   the linker option ``-Wl,--exclude-libs,ALL``.
#
# **Tests using GPU resources**
#   in case your MEX file is using the GPU and
#   in order to be able to run unit tests on this MEX file, the GPU resources
#   should be properly released by Matlab. A possible solution is to make
#   Matlab aware of the use of the GPU resources in the session, which can be
#   performed by a command such as ``D = gpuDevice()`` at the beginning of
#   the test script (or via a fixture).
#
#
# Reference
# --------------
#
# .. variable:: MATLAB_USER_ROOT
#
#    The root folder of the Matlab installation. This is set before the call to
#    :command:`find_package`. If not set, then an automatic search of Matlab
#    will be performed. If set, it should point to a valid version of Matlab.
#
# .. variable:: MATLAB_FIND_DEBUG
#
#    If set, the lookup of Matlab and the intermediate configuration steps are
#    outputted to the console.
#
# .. variable:: MATLAB_ADDITIONAL_VERSIONS
#
#   If set, specifies additional versions of Matlab that may be looked for.
#   This variable will be used if :variable:`MATLAB_USER_ROOT` is not set.
#   The variable should be a list of strings, organised by pairs of release
#   name and versions, such as follows::
#
#     set(MATLAB_ADDITIONAL_VERSIONS
#         "release_name1=corresponding_version1"
#         "release_name2=corresponding_version2"
#         ...
#         )
#
#   Example::
#
#     set(MATLAB_ADDITIONAL_VERSIONS
#         "R2013b=8.2"
#         "R2013a=8.1"
#         "R2012b=8.0")

#=============================================================================
# Copyright 2014-2015 Raffi Enficiaud, Max Planck Society
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

set(_FindMatlab_SELF_DIR "${CMAKE_CURRENT_LIST_DIR}")

include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)


# The currently supported versions. Other version can be added by the user by
# providing MATLAB_ADDITIONAL_VERSIONS
if(NOT MATLAB_ADDITIONAL_VERSIONS)
  set(MATLAB_ADDITIONAL_VERSIONS)
endif()

set(MATLAB_VERSIONS_MAPPING
  ${MATLAB_ADDITIONAL_VERSIONS}
  "R2014a=8.3"
  "R2013b=8.2"
  "R2013a=8.1"
  "R2012b=8.0"
  "R2012a=7.14"

  "R2011b=7.13"
  "R2011a=7.12"
  "R2010b=7.11"
  )


# temporary folder for all Matlab runs
set(_matlab_temporary_folder ${CMAKE_BINARY_DIR}/Matlab)

if(NOT EXISTS "${_matlab_temporary_folder}")
  file(MAKE_DIRECTORY "${_matlab_temporary_folder}")
endif()

#.rst:
# .. command:: matlab_get_version_from_release_name
#
#   Returns the version of Matlab (17.58) from a release name (R2017k)
macro (matlab_get_version_from_release_name release_name version_name)

  string(REGEX MATCHALL "${release_name}=([0-9]+\\.?[0-9]*)" _matched ${MATLAB_VERSIONS_MAPPING})

  set(${version_name} "")
  if(NOT _matched STREQUAL "")
    set(${version_name} ${CMAKE_MATCH_1})
  else()
    message(WARNING "The release name ${release_name} is not registered")
  endif()

endmacro()





#.rst:
# .. command:: matlab_get_release_name_from_version
#
#   Returns the release name (R2017k) from the version of Matlab (17.58)
macro (matlab_get_release_name_from_version version release_name)

  set(${release_name} "")
  foreach(_var IN LISTS MATLAB_VERSIONS_MAPPING)
    string(REGEX MATCHALL "(.+)=${version}" _matched ${_var})
    if(NOT _matched STREQUAL "")
      set(${release_name} ${CMAKE_MATCH_1})
      break()
    endif()
  endforeach(_var)

  unset(_var)
  unset(_matched)
  if(${release_name} STREQUAL "")
    message(WARNING "The version ${version} is not registered")
  endif()

endmacro()





# extracts all the supported release names (R2017k...) of Matlab
# internal use
macro(matlab_get_supported_releases list_releases)
  set(${list_releases})
  foreach(_var IN LISTS MATLAB_VERSIONS_MAPPING)
    string(REGEX MATCHALL "(.+)=([0-9]+\\.?[0-9]*)" _matched ${_var})
    if(NOT _matched STREQUAL "")
      list(APPEND ${list_releases} ${CMAKE_MATCH_1})
    endif()
    unset(_matched)
    unset(CMAKE_MATCH_1)
  endforeach(_var)
  unset(_var)
endmacro()



# extracts all the supported versions of Matlab
# internal use
macro(matlab_get_supported_versions list_versions)
  set(${list_versions})
  foreach(_var IN LISTS MATLAB_VERSIONS_MAPPING)
    string(REGEX MATCHALL "(.+)=([0-9]+\\.?[0-9]*)" _matched ${_var})
    if(NOT _matched STREQUAL "")
      list(APPEND ${list_versions} ${CMAKE_MATCH_2})
    endif()
    unset(_matched)
    unset(CMAKE_MATCH_1)
  endforeach(_var)
  unset(_var)
endmacro()


#.rst:
# .. command:: matlab_extract_all_installed_versions_from_registry
#
#   This function parses the registry and founds the Matlab versions that are
#   installed. The found versions are returned in `matlab_versions`.
#   Set `win64` to `TRUE` if the 64 bit version of Matlab should be looked for
#   The returned list contains all versions under
#   ``HKLM\\SOFTWARE\\Mathworks\\MATLAB`` or an empty list in case an error
#   occurred (or nothing found).
#
#   .. note::
#
#     Only the versions are provided. No check is made over the existence of the
#     installation referenced in the registry,
#
function(matlab_extract_all_installed_versions_from_registry win64 matlab_versions)

  if(NOT CMAKE_HOST_WIN32)
    message(FATAL_ERROR "This macro can only be called by a windows host (call to reg.exe")
  endif()


  if(${win64} AND ${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "64")
    set(APPEND_REG "/reg:64")
  else()
    set(APPEND_REG "/reg:32")
  endif()

  # /reg:64 should be added on 64 bits capable OSs in order to enable the
  # redirection of 64 bits applications
  execute_process(
    COMMAND reg query HKEY_LOCAL_MACHINE\\SOFTWARE\\Mathworks\\MATLAB /f * /k ${APPEND_REG}
    RESULT_VARIABLE resultMatlab
    OUTPUT_VARIABLE varMatlab
    ERROR_VARIABLE errMatlab
    )
  #message("Matlabs = ${varMatlab} | ${resultMatlab}")


  set(matlabs_from_registry)
  if(${resultMatlab} EQUAL 0)

    string(
      REGEX MATCHALL "MATLAB\\\\([0-9]+(\\.[0-9]+)?)"
      matlab_versions_regex ${varMatlab})

    foreach(match IN LISTS matlab_versions_regex)
      string(
        REGEX MATCH "MATLAB\\\\(([0-9]+)(\\.([0-9]+))?)"
        current_match ${match})

      set(_matlab_current_version ${CMAKE_MATCH_1})
      set(current_matlab_version_major ${CMAKE_MATCH_2})
      set(current_matlab_version_minor ${CMAKE_MATCH_4})
      if(NOT current_matlab_version_minor)
        set(current_matlab_version_minor "0")
      endif()

      list(APPEND matlabs_from_registry ${_matlab_current_version})
      unset(_matlab_current_version)
    endforeach(match)

  endif()

  set(${matlab_versions} ${matlabs_from_registry} PARENT_SCOPE)

endfunction()



# (internal)
macro(extract_matlab_versions_from_registry_brute_force matlab_versions)
  # get the supported versions
  set(matlab_supported_versions)
  matlab_get_supported_versions(matlab_supported_versions)


  # this is a manual population of the versions we want to look for
  # this can be done as is, but preferably with the call to
  # matlab_get_supported_versions and variable

  # populating the versions we want to look for
  # set(matlab_supported_versions)

  # # Matlab 7
  # set(matlab_major 7)
  # foreach(current_matlab_minor RANGE 4 20)
    # list(APPEND matlab_supported_versions "${matlab_major}.${current_matlab_minor}")
  # endforeach(current_matlab_minor)

  # # Matlab 8
  # set(matlab_major 8)
  # foreach(current_matlab_minor RANGE 0 5)
    # list(APPEND matlab_supported_versions "${matlab_major}.${current_matlab_minor}")
  # endforeach(current_matlab_minor)

  # # taking into account the possible additional versions provided by the user
  # if(DEFINED MATLAB_ADDITIONAL_VERSIONS)
    # list(APPEND matlab_supported_versions MATLAB_ADDITIONAL_VERSIONS)
  # endif()


  # we order from more recent to older
  list(REMOVE_DUPLICATES matlab_supported_versions)
  list(SORT matlab_supported_versions)
  list(REVERSE matlab_supported_versions)


  set(${matlab_versions} ${matlab_supported_versions})


endmacro()




#.rst:
# .. command:: matlab_get_all_valid_matlab_roots_from_registry
#
#   Populates the Matlab root with valid versions of Matlab.
#   The returned matlab_roots is organized in pairs
#   ``(version_number,matlab_root_path)``.
#
#   ::
#
#     matlab_get_all_valid_matlab_roots_from_registry(
#         matlab_versions
#         matlab_roots)
#
#   ``matlab_versions``
#     the versions of each of the Matlab installations
#   ``matlab_roots``
#     the location of each of the Matlab installations
function(matlab_get_all_valid_matlab_roots_from_registry matlab_versions matlab_roots)

  # The matlab_versions comes either from
  # extract_matlab_versions_from_registry_brute_force or
  # matlab_extract_all_installed_versions_from_registry.


  set(_matlab_roots_list )
  foreach(_matlab_current_version ${matlab_versions})
    get_filename_component(
      current_MATLAB_ROOT
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MathWorks\\MATLAB\\${_matlab_current_version};MATLABROOT]"
      ABSOLUTE)

    if(EXISTS ${current_MATLAB_ROOT})
      list(APPEND _matlab_roots_list ${_matlab_current_version} ${current_MATLAB_ROOT})
    endif()

  endforeach(_matlab_current_version)
  unset(_matlab_current_version)
  set(${matlab_roots} ${_matlab_roots_list} PARENT_SCOPE)
  unset(_matlab_roots_list)
endfunction()

#.rst:
# .. command:: matlab_get_mex_suffix
#
#   Returns the extension of the mex files (the suffixes).
#   This function should not be called before the appropriate Matlab root has
#   been found.
#
#   ::
#
#     matlab_get_mex_suffix(
#         matlab_root
#         mex_suffix)
#
#   ``matlab_root``
#     the root of the Matlab installation
#   ``mex_suffix``
#     the variable name in which the suffix will be returned.
function(matlab_get_mex_suffix matlab_root mex_suffix)

  # todo setup the extension properly. Currently I do not know if this is
  # sufficient for all win32 distributions.
  # there is also CMAKE_EXECUTABLE_SUFFIX that could be tweaked
  set(mexext_suffix "")
  if(WIN32)
    list(APPEND mexext_suffix ".bat")
  endif()

  # we first try without suffix, since cmake does not understand a list with
  # one empty string element
  find_program(
    MATLAB_MEXEXTENSIONS_PROG
    NAMES mexext
    PATHS ${matlab_root}/bin
    DOC "Matlab MEX extension provider"
    NO_DEFAULT_PATH
  )

  foreach(current_mexext_suffix IN LISTS mexext_suffix)
    if(NOT DEFINED MATLAB_MEXEXTENSIONS_PROG OR NOT MATLAB_MEXEXTENSIONS_PROG)
      # this call should populate the cache automatically
      find_program(
        MATLAB_MEXEXTENSIONS_PROG
        "mexext${current_mexext_suffix}"
        PATHS ${matlab_root}/bin
        DOC "Matlab MEX extension provider"
        NO_DEFAULT_PATH
      )
    endif()
  endforeach(current_mexext_suffix)


  # the program has been found?
  if((NOT MATLAB_MEXEXTENSIONS_PROG) OR (NOT EXISTS ${MATLAB_MEXEXTENSIONS_PROG}))
    if(MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] Cannot found mexext program. Matlab root is ${matlab_root}")
    endif()
    return()
  endif()

  set(_matlab_mex_extension)


  execute_process(
    COMMAND ${MATLAB_MEXEXTENSIONS_PROG}
    OUTPUT_VARIABLE _matlab_mex_extension
    ERROR_VARIABLE _matlab_mex_extension_error)
  string(STRIP ${_matlab_mex_extension} _matlab_mex_extension)

  set(${mex_suffix} ${_matlab_mex_extension} PARENT_SCOPE)
endfunction()




#.rst:
# .. command:: matlab_get_version_from_matlab_run
#
#   This function runs Matlab program specified on arguments and extracts its
#   version.
#
#   ::
#
#     matlab_get_version_from_matlab_run(
#         matlab_binary_path
#         matlab_list_versions)
#
#   ``matlab_binary_path``
#     the location of the `matlab` binary executable
#   ``matlab_list_versions``
#     the version extracted from Matlab
function(matlab_get_version_from_matlab_run matlab_binary_program matlab_list_versions)

  set(${matlab_list_versions} "" PARENT_SCOPE)


  if(MATLAB_FIND_DEBUG)
    message(STATUS "[MATLAB] Determining the version of Matlab from ${matlab_binary_program}")
  endif()

  if(EXISTS "${_matlab_temporary_folder}/matlabVersionLog.cmaketmp")
    if(MATLAB_FIND_DEBUG)
      message(STATUS "[MATLAB] Removing previous ${_matlab_temporary_folder}/matlabVersionLog.cmaketmp file")
    endif()
    file(REMOVE "${_matlab_temporary_folder}/matlabVersionLog.cmaketmp")
  endif()


  # the log file is needed since on windows the command executes in a new
  # window and it is not possible to get back the answer of Matlab
  # the -wait command is needed on windows, otherwise the call returns
  # immediately after the program launches itself.
  if(WIN32)
    set(_matlab_additional_commands "-wait")
  endif()

  # timeout set to 30 seconds, in case it does not start
  # note as said before OUTPUT_VARIABLE cannot be used in a platform
  # independent manner however, not setting it would flush the output of Matlab
  # in the current console (unix variant)
  execute_process(
    COMMAND "${matlab_binary_program}" -nosplash -nojvm ${_matlab_additional_commands} -logfile "matlabVersionLog.cmaketmp" -nodesktop -nodisplay -r "version, exit"
    OUTPUT_VARIABLE _matlab_version_from_cmd_dummy
    RESULT_VARIABLE _matlab_result_version_call
    ERROR_VARIABLE _matlab_result_version_call_error
    TIMEOUT 30
    WORKING_DIRECTORY "${_matlab_temporary_folder}"
    )


  if(${_matlab_result_version_call})
    if(MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] Unable to determine the version of Matlab. Matlab call returned with error ${_matlab_result_version_call}.")
    endif()
    return()
  elseif(NOT EXISTS "${_matlab_temporary_folder}/matlabVersionLog.cmaketmp")
    if(MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] Unable to determine the version of Matlab. The log file does not exist.")
    endif()
    return()
  endif()

  # if successful, read back the log
  file(READ "${_matlab_temporary_folder}/matlabVersionLog.cmaketmp" _matlab_version_from_cmd)
  file(REMOVE "${_matlab_temporary_folder}/matlabVersionLog.cmaketmp")

  set(index -1)
  string(FIND ${_matlab_version_from_cmd} "ans" index)
  if(index EQUAL -1)

    if(MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] Cannot find the version of Matlab returned by the run.")
    endif()

  else()
    set(matlab_list_of_all_versions_tmp)

    string(SUBSTRING ${_matlab_version_from_cmd} ${index} -1 substring_ans)
    string(
      REGEX MATCHALL "ans[\r\n\t ]*=[\r\n\t ]*([0-9]+(\\.[0-9]+)?)"
      matlab_versions_regex
      ${substring_ans})
    foreach(match IN LISTS matlab_versions_regex)
      string(
        REGEX MATCH "ans[\r\n\t ]*=[\r\n\t ]*(([0-9]+)(\\.([0-9]+))?)"
        current_match ${match})

      list(APPEND matlab_list_of_all_versions_tmp ${CMAKE_MATCH_1})
    endforeach()
    list(REMOVE_DUPLICATES matlab_list_of_all_versions_tmp)
    set(${matlab_list_versions} ${matlab_list_of_all_versions_tmp} PARENT_SCOPE)

  endif()

endfunction()


#.rst:
# .. command:: matlab_add_unit_test
#
#   Adds a Matlab unit test to the test set of cmake/ctest.
#   This command requires the component ``MAIN_PROGRAM``.
#   The unit test uses the Matlab unittest framework (default, available
#   starting Matlab 2013b+) except if the option ``NO_UNITTEST_FRAMEWORK``
#   is given.
#
#   The function expects one Matlab test script file to be given.
#   In the case ``NO_UNITTEST_FRAMEWORK`` is given, the unittest script file
#   should contain the script to be run, plus an exit command with the exit
#   value. This exit value will be passed to the ctest framework (0 success,
#   non 0 failure). Additional arguments accepted by :command:`add_test` can be
#   passed through ``TEST_ARGS`` (eg. ``CONFIGURATION <config> ...``).
#
#   ::
#
#     matlab_add_unit_test(
#         NAME <name>
#         UNITTEST_FILE matlab_file_containing_unittest.m
#         [UNITTEST_PRECOMMAND matlab_command_to_run]
#         [TIMEOUT timeout]
#         [ADDITIONAL_PATH path1 [path2 ...]]
#         [MATLAB_ADDITIONAL_STARTUP_OPTIONS option1 [option2 ...]]
#         [TEST_ARGS arg1 [arg2 ...]]
#         [NO_UNITTEST_FRAMEWORK]
#         )
#
#   The function arguments are:
#
#   ``NAME``
#     name of the unittest in ctest.
#   ``UNITTEST_FILE``
#     the matlab unittest file. Its path will be automatically
#     added to the Matlab path.
#   ``UNITTEST_PRECOMMAND``
#     Matlab script command to be ran before the file
#     containing the test (eg. GPU device initialisation based on CMake
#     variables).
#   ``TIMEOUT``
#     the test timeout in seconds. Defaults to 180 seconds as the
#     Matlab unit test may hang.
#   ``ADDITIONAL_PATH``
#     a list of paths to add to the Matlab path prior to
#     running the unit test.
#   ``MATLAB_ADDITIONAL_STARTUP_OPTIONS``
#     a list of additional option in order
#     to run Matlab from the command line.
#   ``TEST_ARGS``
#     Additional options provided to the add_test command. These
#     options are added to the default options (eg. "CONFIGURATIONS Release")
#   ``NO_UNITTEST_FRAMEWORK``
#     when set, indicates that the test should not
#     use the unittest framework of Matlab (available for versions >= R2013a).
#
function(matlab_add_unit_test)

  if(NOT Matlab_MAIN_PROGRAM)
    message(FATAL_ERROR "[MATLAB] This functionality needs the MAIN_PROGRAM component (not default)")
  endif()

  set(options NO_UNITTEST_FRAMEWORK)
  set(oneValueArgs NAME UNITTEST_PRECOMMAND UNITTEST_FILE TIMEOUT)
  set(multiValueArgs ADDITIONAL_PATH MATLAB_ADDITIONAL_STARTUP_OPTIONS TEST_ARGS)

  set(prefix _matlab_unittest_prefix)
  cmake_parse_arguments(${prefix} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(NOT ${prefix}_NAME)
    message(FATAL_ERROR "[MATLAB] The Matlab test name cannot be empty")
  endif()

  add_test(NAME ${${prefix}_NAME}
           COMMAND ${CMAKE_COMMAND}
            -Dtest_name=${${prefix}_NAME}
            -Dadditional_paths=${${prefix}_ADDITIONAL_PATH}
            -Dtest_timeout=${${prefix}_TIMEOUT}
            -Doutput_directory=${_matlab_temporary_folder}
            -DMatlab_PROGRAM=${Matlab_MAIN_PROGRAM}
            -Dno_unittest_framework=${${prefix}_NO_UNITTEST_FRAMEWORK}
            -DMatlab_ADDITIONNAL_STARTUP_OPTIONS=${${prefix}_MATLAB_ADDITIONAL_STARTUP_OPTIONS}
            -Dunittest_file_to_run=${${prefix}_UNITTEST_FILE}
            -Dcmd_to_run_before_test=${${prefix}_UNITTEST_PRECOMMAND}
            -P ${_FindMatlab_SELF_DIR}/MatlabTestsRedirect.cmake
           ${${prefix}_TEST_ARGS}
           ${${prefix}_UNPARSED_ARGUMENTS}
           )
endfunction()


#.rst:
# .. command:: matlab_add_mex
#
#   Adds a Matlab MEX target.
#   This commands compiles the given sources with the current tool-chain in
#   order to produce a MEX file. The final name of the produced output may be
#   specified, as well as additional link libraries, and a documentation entry
#   for the MEX file. Remaining arguments of the call are passed to the
#   :command:`add_library` command.
#
#   ::
#
#      matlab_add_mex(
#          NAME <name>
#          SRC src1 [src2 ...]
#          [OUTPUT_NAME output_name]
#          [DOCUMENTATION file.txt]
#          [LINK_TO target1 target2 ...]
#          [...]
#      )
#
#   ``NAME``
#     name of the target.
#   ``SRC``
#     list of tje source files.
#   ``LINK_TO``
#     a list of additional link dependencies.  The target links to ``libmex``
#     by default. If ``Matlab_MX_LIBRARY`` is defined, it also
#     links to ``libmx``.
#   ``OUTPUT_NAME``
#     if given, overrides the default name. The default name is
#     the name of the target without any prefix and
#     with ``Matlab_MEX_EXTENSION`` suffix.
#   ``DOCUMENTATION``
#     if given, the file ``file.txt`` will be considered as
#     being the documentation file for the MEX file. This file is copied into
#     the same folder without any processing, with the same name as the final
#     mex file, and with extension `.m`. In that case, typing ``help <name>``
#     in Matlab prints the documentation contained in this file.
#
#   The documentation file is not processed and should be in the following
#   format:
#
#   ::
#
#     % This is the documentation
#     function ret = mex_target_output_name(input1)
#
function(matlab_add_mex )

  if(NOT WIN32)
    # we do not need all this on Windows
    # pthread options
    check_cxx_compiler_flag(-pthread                    HAS_MINUS_PTHREAD)
    # we should use try_compile instead, the link flags are discarded from
    # this compiler_flag function.
    #check_cxx_compiler_flag(-Wl,--exclude-libs,ALL HAS_SYMBOL_HIDING_CAPABILITY)

  endif()

  set(oneValueArgs NAME DOCUMENTATION OUTPUT_NAME)
  set(multiValueArgs LINK_TO SRC)

  set(prefix _matlab_addmex_prefix)
  cmake_parse_arguments(${prefix} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(NOT ${prefix}_NAME)
    message(FATAL_ERROR "[MATLAB] The MEX target name cannot be empty")
  endif()

  if(NOT ${prefix}_OUTPUT_NAME)
    set(${prefix}_OUTPUT_NAME ${${prefix}_NAME})
  endif()

  add_library(${${prefix}_NAME}
    SHARED
      ${${prefix}_SRC}
      ${${prefix}_DOCUMENTATION}
      ${${prefix}_UNPARSED_ARGUMENTS})
  target_include_directories(${${prefix}_NAME} PRIVATE ${Matlab_INCLUDE_DIRS})

  if(DEFINED Matlab_MX_LIBRARY)
    target_link_libraries(${${prefix}_NAME} ${Matlab_MX_LIBRARY})
  endif()

  target_link_libraries(${${prefix}_NAME} ${Matlab_MEX_LIBRARY} ${${prefix}_LINK_TO})
  set_target_properties(${${prefix}_NAME}
      PROPERTIES
        PREFIX ""
        OUTPUT_NAME ${${prefix}_OUTPUT_NAME}
        SUFFIX ".${Matlab_MEX_EXTENSION}")


  # documentation
  if(NOT ${${prefix}_DOCUMENTATION} STREQUAL "")
    get_target_property(output_name ${${prefix}_NAME} OUTPUT_NAME)
    add_custom_command(
      TARGET ${${prefix}_NAME}
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${prefix}_DOCUMENTATION} $<TARGET_FILE_DIR:${${prefix}_NAME}>/${output_name}.m
      COMMENT "Copy ${${prefix}_NAME} documentation file into the output folder"
    )
  endif() # documentation

  # entry point in the mex file + taking care of visibility and symbol clashes.
  if(WIN32)
    set_target_properties(${${prefix}_NAME}
      PROPERTIES
        DEFINE_SYMBOL "DLL_EXPORT_SYM=__declspec(dllexport)")
  else()

    if(HAS_MINUS_PTHREAD AND NOT APPLE)
      # Apparently, compiling with -pthread generated the proper link flags
      # and some defines at compilation
      target_compile_options(${${prefix}_NAME} PRIVATE "-pthread")
    endif()


    # if we do not do that, the symbols linked from eg. boost remain weak and
    # then clash with the ones defined in the matlab process. So by default
    # the symbols are hidden.
    # This also means that for shared libraries (like MEX), the entry point
    # should be explicitly declared with default visibility, otherwise Matlab
    # cannot find the entry point.
    # Note that this is particularly meaningful if the MEX wrapper itself
    # contains symbols that are clashing with Matlab (that are compiled in the
    # MEX file). In order to propagate the visibility options to the libraries
    # to which the MEX file is linked against, the -Wl,--exclude-libs,ALL
    # option should also be specified.

    set_target_properties(${${prefix}_NAME}
      PROPERTIES
        CXX_VISIBILITY_PRESET "hidden"
        C_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN "hidden"
    )

    #  get_target_property(
    #    _previous_link_flags
    #    ${${prefix}_NAME}
    #    LINK_FLAGS)
    #  if(NOT _previous_link_flags)
    #    set(_previous_link_flags)
    #  endif()

    #  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    #    set_target_properties(${${prefix}_NAME}
    #      PROPERTIES
    #        LINK_FLAGS "${_previous_link_flags} -Wl,--exclude-libs,ALL"
    #        # -Wl,--version-script=${_FindMatlab_SELF_DIR}/MatlabLinuxVisibility.map"
    #    )
    #  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    #    # in this case, all other symbols become hidden.
    #    set_target_properties(${${prefix}_NAME}
    #      PROPERTIES
    #        LINK_FLAGS "${_previous_link_flags} -Wl,-exported_symbol,_mexFunction"
    #        #-Wl,-exported_symbols_list,${_FindMatlab_SELF_DIR}/MatlabOSXVisilibity.map"
    #    )
    #  endif()



    set_target_properties(${${prefix}_NAME}
      PROPERTIES
        DEFINE_SYMBOL "DLL_EXPORT_SYM=__attribute__ ((visibility (\"default\")))"
    )


  endif()

endfunction()








# this variable will get all Matlab installations found in the current system.
set(_matlab_possible_roots)


# listing the Matlab versions installed on the WIN machine if MATLAB_USER_ROOT
# is not set
if(WIN32)

  # for windows


  if(NOT MATLAB_USER_ROOT)
    # if MATLAB_USER_ROOT not specified, we look for Matlab installation in
    # the registry
    # if unsuccessful, we look for all known revision and filter the existing
    # ones.

    # testing if we are able to extract the needed information from the registry
    set(matlab_versions_from_registry)
    matlab_extract_all_installed_versions_from_registry(CMAKE_CL_64 matlab_versions_from_registry)

    # the returned list is empty, doing the search on all known versions
    if(NOT matlab_versions_from_registry)

      if(MATLAB_FIND_DEBUG)
        message(STATUS "[MATLAB] Search for Matlab from the registry unsuccessful, testing all supported versions")
      endif()

      extract_matlab_versions_from_registry_brute_force(matlab_versions_from_registry)
    endif()

    # filtering the results with the registry keys
    matlab_get_all_valid_matlab_roots_from_registry("${matlab_versions_from_registry}" _matlab_possible_roots)



  elseif(NOT EXISTS ${MATLAB_USER_ROOT})

    # if MATLAB_USER_ROOT specified but erroneous
    if(MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] the specified path for MATLAB_USER_ROOT does not exist (${MATLAB_USER_ROOT})")
    endif()
  else()
    list(APPEND _matlab_possible_roots "NOTFOUND" ${MATLAB_USER_ROOT}) # empty version
  endif()


else()
  # for linux/osx

  if(NOT MATLAB_USER_ROOT)

    # if MATLAB_USER_ROOT not specified, we look for Matlab from the command
    # line PATH
    # maybe using CMAKE_PROGRAM_PATH to add some more hints

    set(_matlab_main_tmp "")
    if(Matlab_MAIN_PROGRAM)
      set(_matlab_main_tmp ${Matlab_MAIN_PROGRAM})
    endif()

    if(NOT _matlab_main_tmp)

      find_program(
        _matlab_main_tmp
        NAMES matlab)

      if(NOT _matlab_main_tmp)
        execute_process(
          COMMAND "which" matlab
          OUTPUT_VARIABLE _matlab_main_tmp
          ERROR_VARIABLE _matlab_main_tmp_err)
        # the output should be cleaned up
        string(STRIP "${_matlab_main_tmp}" _matlab_main_tmp)
      endif()

      if(EXISTS ${_matlab_main_tmp})
        if(MATLAB_FIND_DEBUG)
          message(STATUS "[MATLAB] matlab found from PATH: ${_matlab_main_tmp}")
        endif()
      else()
        # found candidate is not good, we remove it
        unset(_matlab_main_tmp)
        unset(_matlab_main_tmp CACHE)
      endif()

    endif() # if(NOT _matlab_main_tmp)

    if(_matlab_main_tmp AND EXISTS ${_matlab_main_tmp})
      # resolve symlinks
      get_filename_component(_matlab_current_location "${_matlab_main_tmp}" REALPATH)
      set(_directory_alias DIRECTORY)

      # get the directory (the command below has to be run twice)
      get_filename_component(_matlab_current_location "${_matlab_current_location}" ${_directory_alias})
      get_filename_component(_matlab_current_location "${_matlab_current_location}" ${_directory_alias}) # Matlab should be in bin
      list(APPEND _matlab_possible_roots "NOTFOUND" ${_matlab_current_location}) # empty version
    endif()


    # on mac, we look for the /Application paths
    # this corresponds to the behaviour on Windows. On Linux, we do not have
    # any other guess.
    if((NOT _matlab_possible_roots) AND APPLE)

      matlab_get_supported_releases(_matlab_releases)
      if(MATLAB_FIND_DEBUG)
        message(STATUS "[MATLAB] Matlab supported versions ${_matlab_releases}. If more version should be supported "
                     "the variable MATLAB_ADDITIONAL_VERSIONS can be set according to the documentation")
      endif()

      foreach(_matlab_current_release IN LISTS _matlab_releases)
        set(_matlab_full_string "/Applications/MATLAB_${_matlab_current_release}.app")
        if(EXISTS ${_matlab_full_string})
          set(_matlab_current_version)
          matlab_get_version_from_release_name("${_matlab_current_release}" _matlab_current_version)
          if(MATLAB_FIND_DEBUG)
            message(STATUS "[MATLAB] Found version ${_matlab_current_release} (${_matlab_current_version}) in ${_matlab_full_string}")
          endif()
          list(APPEND _matlab_possible_roots ${_matlab_current_version})
          list(APPEND _matlab_possible_roots ${_matlab_full_string})
          unset(_matlab_current_version)
        endif()

        unset(_matlab_full_string)
      endforeach(_matlab_current_release)
      unset(_matlab_current_release)
      unset(_matlab_releases)
    endif()

    # we need to clear _matlab_main_tmp here
    unset(_matlab_main_tmp CACHE)
    unset(_matlab_main_tmp)

  elseif(NOT EXISTS ${MATLAB_USER_ROOT})
    # if MATLAB_USER_ROOT specified but erroneous
    if(MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] the specified path for MATLAB_USER_ROOT does not exist (${MATLAB_USER_ROOT})")
    endif()
  else()
    list(APPEND _matlab_possible_roots "NOTFOUND" ${MATLAB_USER_ROOT}) # empty version
  endif()
endif()


if(MATLAB_FIND_DEBUG)
  message(STATUS "[MATLAB] Matlab root folders are ${_matlab_possible_roots}")
endif()



if(DEFINED Matlab_ROOT_DIR)
  unset(Matlab_ROOT_DIR)
endif()

# take the first possible Matlab root
if(_matlab_possible_roots)
  list(GET _matlab_possible_roots 0 Matlab_VERSION_STRING)
  list(GET _matlab_possible_roots 1 Matlab_ROOT_DIR)
  list(LENGTH _matlab_possible_roots numbers_of_matlab_roots)

  # adding a warning in case of ambiguity
  if(numbers_of_matlab_roots GREATER 2 AND MATLAB_FIND_DEBUG)
    message(WARNING "[MATLAB] Found several distributions of Matlab. Setting the current version to ${Matlab_VERSION_STRING} (located ${Matlab_ROOT_DIR})."
                    " If this is not the desired behaviour, provide the -DMATLAB_USER_ROOT on the command line")
  endif()
endif()


if((NOT (DEFINED Matlab_VERSION_STRING)) OR (NOT Matlab_VERSION_STRING))
  set(Matlab_VERSION_STRING "NOTFOUND")
endif()



# if the version is not trivial, we query matlab for that
# we keep track of the location of matlab that induced this version
set(Matlab_VERSION_STRING_AUTO_DETECT "" CACHE INTERNAL "internal matlab location for the discovered version")

set(_matlab_version_should_be_recomputed FALSE)
if(${Matlab_VERSION_STRING} STREQUAL "NOTFOUND")
  set(_matlab_version_should_be_recomputed TRUE)
elseif((NOT Matlab_MAIN_PROGRAM) OR (NOT EXISTS ${Matlab_MAIN_PROGRAM}))
  set(_matlab_version_should_be_recomputed TRUE)
else()
  # just in case the variable is modified to a symlink
  get_filename_component(_matlab_main_tmp "${Matlab_MAIN_PROGRAM}" REALPATH)
  if(NOT (_matlab_main_tmp STREQUAL Matlab_VERSION_STRING_AUTO_DETECT))
    set(_matlab_version_should_be_recomputed TRUE)
  endif()
endif()

if(_matlab_version_should_be_recomputed)
  if(MATLAB_FIND_DEBUG)
    message(STATUS "[MATLAB] querying for version from Matlab root = ${Matlab_ROOT_DIR}")
  endif()

  if(Matlab_MAIN_PROGRAM)
    set(_matlab_main_tmp ${Matlab_MAIN_PROGRAM})
  else()
    find_program(
      _matlab_main_tmp
      matlab
      PATHS ${Matlab_ROOT_DIR} ${Matlab_ROOT_DIR}/bin
      DOC "Matlab main program"
      NO_DEFAULT_PATH
    )
  endif()

  if(_matlab_main_tmp)
    set(matlab_list_of_all_versions)
    matlab_get_version_from_matlab_run("${_matlab_main_tmp}" matlab_list_of_all_versions)

    list(GET matlab_list_of_all_versions 0 MATLAB_VERSION_tmp)

    # set the version into the cache
    set(Matlab_VERSION_STRING ${MATLAB_VERSION_tmp} CACHE INTERNAL "Matlab version (automatically determined)" FORCE)
    list(LENGTH list_of_all_versions list_of_all_versions_length)
    if((${list_of_all_versions_length} GREATER 1) AND MATLAB_FIND_DEBUG)
      message(WARNING "[MATLAB] Found several versions, taking the first one (versions found ${list_of_all_versions})")
    endif()

    # update the location of Matlab that produced this value
    # we store the real path for comparison
    get_filename_component(_matlab_main_tmp1 "${_matlab_main_tmp}" REALPATH)
    set(Matlab_VERSION_STRING_AUTO_DETECT ${_matlab_main_tmp1} CACHE INTERNAL "internal matlab location for the discovered version" FORCE)

    # these are internal variables
    unset(_matlab_main_tmp)
    unset(_matlab_main_tmp CACHE)
    unset(_matlab_main_tmp1)
  endif()
endif()


if(MATLAB_FIND_DEBUG)
  message(STATUS "[MATLAB] Current version is ${Matlab_VERSION_STRING} located ${Matlab_ROOT_DIR}")
endif()



if(Matlab_ROOT_DIR)
  file(TO_CMAKE_PATH ${Matlab_ROOT_DIR} Matlab_ROOT_DIR)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(_matlab_64Build FALSE)
else()
  set(_matlab_64Build TRUE)
endif()

if(APPLE)
  set(_matlab_bin_prefix "mac") # i should be for intel
  set(_matlab_bin_suffix_32bits "i")
  set(_matlab_bin_suffix_64bits "i64")
elseif(UNIX)
  set(_matlab_bin_prefix "gln")
  set(_matlab_bin_suffix_32bits "x86")
  set(_matlab_bin_suffix_64bits "xa64")
else()
  set(_matlab_bin_prefix "win")
  set(_matlab_bin_suffix_32bits "32")
  set(_matlab_bin_suffix_64bits "64")
endif()



set(MATLAB_INCLUDE_DIR_TO_LOOK ${Matlab_ROOT_DIR}/extern/include)
if(_matlab_64Build)
  set(_matlab_current_suffix ${_matlab_bin_suffix_64bits})
else()
  set(_matlab_current_suffix ${_matlab_bin_suffix_32bits})
endif()

set(Matlab_BINARIES_DIR
    ${Matlab_ROOT_DIR}/bin/${_matlab_bin_prefix}${_matlab_current_suffix})
set(Matlab_EXTERN_LIBRARY_DIR
    ${Matlab_ROOT_DIR}/extern/lib/${_matlab_bin_prefix}${_matlab_current_suffix})

if(WIN32)
  set(_matlab_lib_dir_for_search ${Matlab_EXTERN_LIBRARY_DIR}/microsoft)
  set(_matlab_lib_prefix_for_search "lib")
else()
  set(_matlab_lib_dir_for_search ${Matlab_BINARIES_DIR})
  set(_matlab_lib_prefix_for_search "lib")
endif()

unset(_matlab_64Build)


if(NOT DEFINED Matlab_MEX_EXTENSION)
  set(_matlab_mex_extension "")
  matlab_get_mex_suffix("${Matlab_ROOT_DIR}" _matlab_mex_extension)

  # This variable goes to the cache.
  set(Matlab_MEX_EXTENSION ${_matlab_mex_extension} CACHE STRING "Extensions for the mex targets (automatically given by Matlab)")
  unset(_matlab_mex_extension)
endif()


if(MATLAB_FIND_DEBUG)
  message(STATUS "[MATLAB] [DEBUG]_matlab_lib_prefix_for_search = ${_matlab_lib_prefix_for_search} | _matlab_lib_dir_for_search = ${_matlab_lib_dir_for_search}")
endif()



# internal
# This small stub around find_library is to prevent any pollution of CMAKE_FIND_LIBRARY_PREFIXES in the global scope.
# This is the function to be used below instead of the find_library directives.
function(_Matlab_find_library _matlab_library_prefix)
  set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} ${_matlab_library_prefix})
  find_library(${ARGN})
endfunction()


set(_matlab_required_variables)


# the MEX library/header are required
find_path(
  Matlab_INCLUDE_DIRS
  mex.h
  PATHS ${MATLAB_INCLUDE_DIR_TO_LOOK}
  NO_DEFAULT_PATH
  )
list(APPEND _matlab_required_variables Matlab_INCLUDE_DIRS)

_Matlab_find_library(
  ${_matlab_lib_prefix_for_search}
  Matlab_MEX_LIBRARY
  mex
  PATHS ${_matlab_lib_dir_for_search}
  NO_DEFAULT_PATH
)


list(APPEND _matlab_required_variables Matlab_MEX_LIBRARY)

# the MEX extension is required
list(APPEND _matlab_required_variables Matlab_MEX_EXTENSION)

# the matlab root is required
list(APPEND _matlab_required_variables Matlab_ROOT_DIR)


# component Mex Compiler
list(FIND Matlab_FIND_COMPONENTS MEX_COMPILER _matlab_find_mex_compiler)
if(_matlab_find_mex_compiler GREATER -1)
  find_program(
    Matlab_MEX_COMPILER
    "mex"
    PATHS ${Matlab_BINARIES_DIR}
    DOC "Matlab MEX compiler"
    NO_DEFAULT_PATH
  )

  if(Matlab_MEX_COMPILER)
    set(Matlab_MEX_COMPILER_FOUND TRUE)
  endif()
endif()
unset(_matlab_find_mex_compiler)

# component Matlab program
list(FIND Matlab_FIND_COMPONENTS MAIN_PROGRAM _matlab_find_matlab_program)
if(_matlab_find_matlab_program GREATER -1)
  # todo cleanup with code above
  if(NOT DEFINED Matlab_MAIN_PROGRAM)
    find_program(
      Matlab_MAIN_PROGRAM
      matlab
      PATHS ${Matlab_ROOT_DIR} ${Matlab_ROOT_DIR}/bin
      DOC "Matlab main program"
      NO_DEFAULT_PATH
    )
  endif()
  if(Matlab_MAIN_PROGRAM)
    set(Matlab_MAIN_PROGRAM_FOUND TRUE)
  endif()

endif()
unset(_matlab_find_matlab_program)

# Component MX library
list(FIND Matlab_FIND_COMPONENTS MX_LIBRARY _matlab_find_mx)
if(_matlab_find_mx GREATER -1)
  _Matlab_find_library(
    ${_matlab_lib_prefix_for_search}
    Matlab_MX_LIBRARY
    mx
    PATHS ${_matlab_lib_dir_for_search}
    NO_DEFAULT_PATH
  )

  if(Matlab_MX_LIBRARY)
    set(Matlab_MX_LIBRARY_FOUND TRUE)
  endif()
endif()
unset(_matlab_find_mx)


# Component ENG library
list(FIND Matlab_FIND_COMPONENTS ENG_LIBRARY _matlab_find_eng)
if(_matlab_find_eng GREATER -1)
  _Matlab_find_library(
    ${_matlab_lib_prefix_for_search}
    Matlab_ENG_LIBRARY
    eng
    PATHS ${_matlab_lib_dir_for_search}
    NO_DEFAULT_PATH
  )
  if(Matlab_ENG_LIBRARY)
    set(Matlab_ENG_LIBRARY_FOUND TRUE)
  endif()
endif()
unset(_matlab_find_eng)





unset(_matlab_lib_dir_for_search)


set(Matlab_LIBRARIES ${Matlab_MEX_LIBRARY} ${Matlab_MX_LIBRARY} ${Matlab_ENG_LIBRARY})

if(CMAKE_VERSION VERSION_LESS "2.8.11")
  find_package_handle_standard_args(
    Matlab
    REQUIRED_VARS ${_matlab_required_variables}
    VERSION_VAR Matlab_VERSION_STRING)
else()
  find_package_handle_standard_args(
    Matlab
    FOUND_VAR Matlab_FOUND
    REQUIRED_VARS ${_matlab_required_variables} #MATLAB_REQUIRED_PROGRAMS MATLAB_REQUIRED_LIBRARIES MATLAB_REQUIRED_INCLUDE_DIRS
    VERSION_VAR Matlab_VERSION_STRING
    HANDLE_COMPONENTS)
endif()

unset(_matlab_required_variables)
unset(_matlab_bin_prefix)
unset(_matlab_bin_suffix_32bits)
unset(_matlab_bin_suffix_64bits)
unset(_matlab_current_suffix)
unset(_matlab_lib_dir_for_search)
unset(_matlab_lib_prefix_for_search)

if(Matlab_INCLUDE_DIRS AND Matlab_LIBRARIES)
  mark_as_advanced(
    Matlab_LIBRARIES
    Matlab_MEX_LIBRARY
    Matlab_MX_LIBRARY
    Matlab_ENG_LIBRARY
    Matlab_INCLUDE_DIRS
    Matlab_FOUND
    MATLAB_USER_ROOT
    Matlab_ROOT_DIR
    Matlab_VERSION_STRING
    Matlab_MAIN_PROGRAM
    Matlab_MEX_EXTENSION
    Matlab_BINARIES_DIR
  )
endif()
