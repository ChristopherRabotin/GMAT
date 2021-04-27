// *** File Name : csaltdefs.hpp
// *** Created   : July 27, 2017
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For           :  Flight Dynamics Analysis Branch (Code 595)       ***
// **************************************************************************

#ifndef CSALTDEFS_HPP
#define CSALTDEFS_HPP

//#include "gmatdefs.hpp"
#include "utildefs.hpp"

#ifdef _WIN32  // Windows DLL import/export definitions

   #ifdef _DYNAMICLINK  // Only used for Visual C++ Windows DLLs
      #ifdef CSALT_EXPORTS
         #define CSALT_API __declspec(dllexport)
      #else
         #define CSALT_API __declspec(dllimport)
      #endif
   #endif

#endif //  End of OS nits

#ifndef CSALT_API
   #define CSALT_API
#endif

// Deprecation messages are turned off for R2020a; uncomment to activate
//#ifdef __GNUC__
//#define DEPRECATED(func) func __attribute__ ((deprecated))
//#elif defined(_MSC_VER)
//#define DEPRECATED(func) __declspec(deprecated) func
//#else
//#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
//#define DEPRECATED(func) func
//#endif

#define DEPRECATED(func) func

#endif // CSALTDEFS_HPP
