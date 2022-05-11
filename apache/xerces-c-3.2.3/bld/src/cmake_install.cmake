# Install script for directory: /home/rb/xerces-c-3.2.3/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/rb/xerces-c-3.2.3/bld/src/InstallLibrarySymlink.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxerces-c-3.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxerces-c-3.2.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxerces-c-3.2.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/rb/xerces-c-3.2.3/bld/src/libxerces-c-3.2.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxerces-c-3.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxerces-c-3.2.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libxerces-c-3.2.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC/XercesCConfigInternal.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC/XercesCConfigInternal.cmake"
         "/home/rb/xerces-c-3.2.3/bld/src/CMakeFiles/Export/lib/cmake/XercesC/XercesCConfigInternal.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC/XercesCConfigInternal-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC/XercesCConfigInternal.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/bld/src/CMakeFiles/Export/lib/cmake/XercesC/XercesCConfigInternal.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/bld/src/CMakeFiles/Export/lib/cmake/XercesC/XercesCConfigInternal-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/XercesC" TYPE FILE FILES
    "/home/rb/xerces-c-3.2.3/bld/src/XercesCConfig.cmake"
    "/home/rb/xerces-c-3.2.3/bld/src/XercesCConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ArrayIndexOutOfBoundsException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Base64.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BaseRefVectorOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BaseRefVectorOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BinFileInputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BinInputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BinMemInputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BitOps.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/BitSet.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/CountedPointer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/CountedPointer.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/DefaultPanicHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/EmptyStackException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/EncodingValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/FlagJanitor.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/FlagJanitor.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Hashers.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Hash2KeysSetOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Hash2KeysSetOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/HexBin.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/IllegalArgumentException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/InvalidCastException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/IOException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Janitor.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Janitor.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/KeyRefPair.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/KeyRefPair.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/KeyValuePair.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/KeyValuePair.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/KVStringPair.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Mutexes.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/LogicalPath.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NameIdPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NameIdPool.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NoSuchElementException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NullPointerException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NumberFormatException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/OutOfMemoryException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/PanicHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ParseException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/PlatformUtils.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/PSVIUni.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/QName.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefArrayOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefArrayOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefArrayVectorOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefArrayVectorOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefHash2KeysTableOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefHash2KeysTableOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefHash3KeysIdPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefHash3KeysIdPool.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefHashTableOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefHashTableOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefStackOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefStackOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefVectorOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RefVectorOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/ASCIIRangeFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/BlockRangeFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/BMPattern.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/CharToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/ClosureToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/ConcatToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/Match.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/Op.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/OpFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/ParenToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/ParserForXMLSchema.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RangeFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RangeToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RangeTokenMap.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RegularExpression.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RegxDefs.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RegxParser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/RegxUtil.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/StringToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/Token.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/TokenFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/TokenInc.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/UniCharTable.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/UnicodeRangeFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/UnionToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/XMLRangeFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/regx" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/regx/XMLUniCharacter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/RuntimeException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/SchemaDateTimeException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/SecurityManager.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/StringPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/SynchronizedStringPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/TranscodingException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/TransENameMap.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/TransENameMap.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/TransService.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/UnexpectedEOFException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/UnsupportedEncodingException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/UTFDataFormatException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueArrayOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueArrayOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueHashTableOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueHashTableOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueStackOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueStackOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueVectorOf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/ValueVectorOf.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XercesDefs.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMemory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XML256TableTranscoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XML88591Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLAbstractDoubleFloat.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLASCIITranscoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLBigDecimal.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLBigInteger.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLChar.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLChTranscoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLDateTime.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLDOMMsg.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLDouble.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLEBCDICTranscoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLEntityResolver.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLEnumerator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLExceptMsgs.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLFileMgr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLFloat.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLIBM1047Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLIBM1140Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLInitializer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLInteger.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLMutexMgr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLMsgLoader.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLNetAccessor.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLNumber.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLResourceIdentifier.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLString.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLStringTokenizer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLUCS4Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLUni.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLUniDefs.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLUri.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLURL.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLUTF16Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLUTF8Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/XMLWin1252Transcoder.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOM.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMAttr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMCDATASection.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMCharacterData.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMComment.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMConfiguration.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMDocument.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMDocumentFragment.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMDocumentRange.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMDocumentTraversal.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMDocumentType.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMElement.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMEntity.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMEntityReference.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMError.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMErrorHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMImplementation.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMImplementationList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMImplementationLS.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMImplementationRegistry.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMImplementationSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLocator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSInput.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSOutput.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSParser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSParserFilter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSResourceResolver.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSSerializer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMLSSerializerFilter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMMemoryManager.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMNamedNodeMap.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMNode.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMNodeFilter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMNodeIterator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMNodeList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMNotation.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMProcessingInstruction.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMPSVITypeInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMRange.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMRangeException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMStringList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMText.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMTreeWalker.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMTypeInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMUserDataHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMXPathEvaluator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMXPathException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMXPathExpression.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMXPathNamespace.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMXPathNSResolver.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/DOMXPathResult.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/StDOMNode.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMAttrImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMAttrMapImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMAttrNSImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMCasts.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMCDATASectionImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMCharacterDataImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMChildNode.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMCommentImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMConfigurationImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMDeepNodeListImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMDeepNodeListPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMDeepNodeListPool.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMDocumentFragmentImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMDocumentImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMDocumentTypeImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMElementImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMElementNSImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMEntityImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMEntityReferenceImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMErrorImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMImplementationImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMImplementationListImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMLocatorImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNamedNodeMapImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNodeBase.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNodeIDMap.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNodeImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNodeIteratorImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNodeListImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNodeVector.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNormalizer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMNotationImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMParentNode.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMProcessingInstructionImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMRangeImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMStringListImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMStringPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMTextImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMTreeWalkerImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMTypeInfoImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMLSSerializerImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMLSInputImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMLSOutputImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMXPathExpressionImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMXPathNSResolverImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/DOMXPathResultImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/dom/impl" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/dom/impl/XSDElementNSImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/BinOutputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/LocalFileFormatTarget.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/LocalFileInputSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/MemBufFormatTarget.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/MemBufInputSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/MemoryManager.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/PSVIAttribute.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/PSVIAttributeList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/PSVIElement.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/PSVIHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/PSVIItem.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSAnnotation.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSAttributeDeclaration.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSAttributeGroupDefinition.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSAttributeUse.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSComplexTypeDefinition.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSConstants.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSElementDeclaration.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSFacet.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSIDCDefinition.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSModel.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSModelGroup.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSModelGroupDefinition.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSMultiValueFacet.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSNamedMap.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSNamedMap.c")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSNamespaceItem.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSNotationDeclaration.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSObject.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSParticle.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSSimpleTypeDefinition.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSTypeDefinition.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSValue.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework/psvi" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/psvi/XSWildcard.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/StdInInputSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/StdOutFormatTarget.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/URLInputSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/ValidationContext.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/Wrapper4DOMLSInput.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/Wrapper4InputSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLAttDef.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLAttDefList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLAttr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLBuffer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLBufferMgr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLContentModel.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLDocumentHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLDTDDescription.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLElementDecl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLEntityDecl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLEntityHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLErrorCodes.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLErrorReporter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLFormatter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLGrammarDescription.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLGrammarPool.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLGrammarPoolImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLNotationDecl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLPScanToken.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLRecognizer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLRefInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLSchemaDescription.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/framework" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/framework/XMLValidityCodes.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/BinFileOutputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/BinMemOutputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/CharTypeTables.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/DGXMLScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/ElemStack.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/EndOfEntityException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/IANAEncodings.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/IGXMLScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/MemoryManagerImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/ReaderMgr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/SGXMLScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/ValidationContextImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/VecAttributesImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/VecAttrListImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/WFXMLScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XMLInternalErrorHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XMLReader.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XMLScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XMLScannerResolver.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XProtoType.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XSAXMLScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XSerializable.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XSerializationException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XSerializeEngine.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XSObjectFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/internal" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/internal/XTemplateSerializer.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/parsers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/parsers/AbstractDOMParser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/parsers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/parsers/DOMLSParserImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/parsers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/parsers/SAX2XMLFilterImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/parsers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/parsers/SAX2XMLReaderImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/parsers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/parsers/SAXParser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/parsers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/parsers/XercesDOMParser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/AttributeList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/DocumentHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/DTDHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/EntityResolver.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/ErrorHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/HandlerBase.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/InputSource.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/Locator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/Parser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/SAXException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax/SAXParseException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/Attributes.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/ContentHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/DeclHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/DefaultHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/LexicalHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/SAX2XMLFilter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/SAX2XMLReader.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/sax2" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/sax2/XMLReaderFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/AllContentModel.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMAny.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMBinaryOp.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMLeaf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMRepeatingLeaf.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMNode.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMStateSet.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/CMUnaryOp.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/ContentLeafNameTypeVector.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/ContentSpecNode.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/DFAContentModel.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/Grammar.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/GrammarResolver.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/MixedContentModel.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/common" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/common/SimpleContentModel.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/AbstractNumericFacetValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/AbstractNumericValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/AbstractStringValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/AnySimpleTypeDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/AnyURIDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/Base64BinaryDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/BooleanDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DatatypeValidatorFactory.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DateDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DateTimeDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DateTimeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DayDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DecimalDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DoubleDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/DurationDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/ENTITYDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/FloatDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/HexBinaryDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/IDDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/IDREFDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/InvalidDatatypeFacetException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/InvalidDatatypeValueException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/ListDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/MonthDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/MonthDayDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/NameDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/NCNameDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/NOTATIONDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/QNameDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/StringDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/TimeDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/UnionDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/XMLCanRepGroup.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/YearDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/datatype" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/datatype/YearMonthDatatypeValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DocTypeHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDAttDef.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDAttDefList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDElementDecl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDEntityDecl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDGrammar.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDScanner.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/DTDValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/DTD" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/DTD/XMLDTDDescriptionImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/ComplexTypeInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/GeneralAttributeCheck.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/FieldActivator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/FieldValueMap.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IC_Field.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IC_Key.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IC_KeyRef.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IC_Selector.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IC_Unique.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IdentityConstraint.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/IdentityConstraintHandler.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/ValueStore.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/ValueStoreCache.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/XercesXPath.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/XPathException.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/XPathMatcher.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/XPathMatcherStack.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema/identity" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/identity/XPathSymbols.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/NamespaceScope.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/PSVIDefs.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaAttDef.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaAttDefList.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaElementDecl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaGrammar.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaSymbols.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SchemaValidator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/SubstitutionGroupComparator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/TraverseSchema.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XercesAttGroupInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XercesElementWildcard.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XercesGroupInfo.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XMLSchemaDescriptionImpl.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XSDDOMParser.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XSDErrorReporter.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XSDLocator.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/validators/schema" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/validators/schema/XUtil.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/xinclude" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/xinclude/XIncludeDOMDocumentProcessor.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/xinclude" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/xinclude/XIncludeLocation.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/xinclude" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/xinclude/XIncludeUtils.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/NetAccessors/Socket" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/NetAccessors/Socket" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/NetAccessors" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/NetAccessors/BinHTTPInputStreamCommon.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/Transcoders/IconvGNU" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/Transcoders/IconvGNU/IconvGNUTransService.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/MsgLoaders/InMemory" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/MsgLoaders/InMemory/InMemMsgLoader.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/MsgLoaders/InMemory" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/MsgLoaders/InMemory/XercesMessages_en_US.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/MutexManagers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/MutexManagers/StdMutexMgr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util/FileManagers" TYPE FILE FILES "/home/rb/xerces-c-3.2.3/src/xercesc/util/FileManagers/PosixFileMgr.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xercesc/util" TYPE FILE FILES
    "/home/rb/xerces-c-3.2.3/bld/src/xercesc/util/Xerces_autoconf_config.hpp"
    "/home/rb/xerces-c-3.2.3/bld/src/xercesc/util/XercesVersion.hpp"
    )
endif()

