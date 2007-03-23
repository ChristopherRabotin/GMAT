//$Header$
//------------------------------------------------------------------------------
//                                  CommandException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Command hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef CommandException_hpp
#define CommandException_hpp

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown from the command subsystem
 */
class CommandException : public BaseException
{
public:
   // class constructor
   CommandException(const std::string &details = "");
   // class destructor
   ~CommandException();
   // Copy constructor
   CommandException(const CommandException &ce);
};

#endif // CommandException_hpp

