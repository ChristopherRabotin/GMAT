//$Id$
//------------------------------------------------------------------------------
//                                  CommandException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
class GMAT_API CommandException : public BaseException
{
public:
   // class constructor
   CommandException(const std::string &details = "",
         // Change to this when the repeated strings are fixed:
//         Gmat::MessageType mt = Gmat::ERROR_);
         // Change to this if it's problematic:
         Gmat::MessageType mt = Gmat::GENERAL_);
   // class destructor
   ~CommandException();
   // Copy constructor
   CommandException(const CommandException &ce);
};

#endif // CommandException_hpp

