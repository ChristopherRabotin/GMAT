//$Id$
//------------------------------------------------------------------------------
//                            CommandEchoDefs
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2012 Thinking Systems, Inc.
// Free usage granted to all users; this is shell code.  Adapt and enjoy.  
// Attribution is appreciated.
//
// Author: Joshua J. Raymond, Thinking Systems, Inc.
// Created: June 23, 2017
//
/**
* DLL interface definitions for a CommandEcho GMAT plugin
*/
//------------------------------------------------------------------------------


#ifndef SampleDefs_hpp
#define SampleDefs_hpp

#include "gmatdefs.hpp"

#ifdef _WIN32  // Windows
#ifdef _MSC_VER  // Microsoft Visual C++

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
// Windows Header File entry point:
#include <windows.h>

#define  _USE_MATH_DEFINES  // makes Msoft define pi

#endif  // End Microsoft C++ specific block

#ifdef _DYNAMICLINK  // Only used for Visual C++ Windows DLLs
#ifdef SCRIPTTOOLS_EXPORTS
#define ScriptTools_API __declspec(dllexport)
#else
#define ScriptTools_API __declspec(dllimport)
#endif

// Provide the storage class specifier (extern for an .exe file, null
// for DLL) and the __declspec specifier (dllimport for .an .exe file,
// dllexport for DLL).
// You must define EXP_STL when compiling the DLL.
// You can now use this header file in both the .exe file and DLL - a
// much safer means of using common declarations than two different
// header files.
#ifdef EXP_STL
#    define DECLSPECIFIER __declspec(dllexport)
#    define EXPIMP_TEMPLATE
#else
#    define DECLSPECIFIER __declspec(dllimport)
#    define EXPIMP_TEMPLATE extern
#endif

#define EXPORT_TEMPLATES
#endif
#endif //  End of OS nits

#ifndef ScriptTools_API
#define ScriptTools_API
#endif

#endif /* CommandEchoDefs_hpp */