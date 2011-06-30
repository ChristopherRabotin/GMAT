//$Id: GmatOdtbx_defs.hpp 9540 2011-05-18 00:02:57Z djcinsb $
//------------------------------------------------------------------------------
//                 Gmat C Function Import/Export Definitions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// ODTBX: Orbit Determination Toolbox
//
// **Legal**
//
// Developed jointly by NASA/GSFC, Emergent Space Technologies, Inc.
// and Thinking Systems, Inc. under the FDSS contract, Task 28
//
// Author: Darrel J. Conway (Thinking Systems)
// Created: 2011/03/22
//
/**
 * Definition for library code interfaces.
 */
//------------------------------------------------------------------------------

#ifndef GmatCFunc_defs_hpp
#define GmatCFunc_defs_hpp


#ifdef _WIN32  // Windows
   #ifdef _MSC_VER  // Microsoft Visual C++

      #define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
      // Windows Header File entry point:
      #include <windows.h>

      #define  _USE_MATH_DEFINES  // makes Msoft define pi

   #endif  // End Microsoft C++ specific block

   #ifdef _DYNAMICLINK  // Only used for Visual C++ Windows DLLs
      #ifdef CINTERFACE_EXPORTS
         #define CINTERFACE_API __declspec(dllexport)
      #else
         #define CINTERFACE_API __declspec(dllimport)
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
#else
   #define CINTERFACE_API
#endif //  End of OS nits

#ifdef IMPEXP_STDSTRING
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<char>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
#endif

#ifndef CINTERFACE_API
   #define CINTERFACE_API
#endif

#endif
