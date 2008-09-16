//$Header$
//------------------------------------------------------------------------------
//                                  InterfaceException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy Shoan/GSFC
// Created: 2006.09.15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Interface classes.
 */
//------------------------------------------------------------------------------


#ifndef InterfaceException_hpp
#define InterfaceException_hpp

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown from the interface subsystem
 */
class InterfaceException : public BaseException
{
   public:
      // class constructor
      InterfaceException(const std::string &details = "");
      // class destructor
      ~InterfaceException();
      // Copy constructor
      InterfaceException(const InterfaceException &ce);
};

#endif // InterfaceException_hpp

