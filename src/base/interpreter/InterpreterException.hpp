//$Id$
//------------------------------------------------------------------------------
//                                  ClassName
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/11/04
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Insert descriptive text here.
 *
 * @note Any notes here.
 */
//------------------------------------------------------------------------------

#ifndef InterpreterException_hpp
#define InterpreterException_hpp

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown by the interpreters
 */
class GMAT_API InterpreterException : public BaseException
{
public:
   // class constructor
   InterpreterException(const std::string &details = "",
         // Change to this when the repeated instances are fixed:
//         Gmat::MessageType mt = Gmat::ERROR_);
         // Change to this if it's problematic:
         Gmat::MessageType mt = Gmat::GENERAL_);

   // class destructor
   ~InterpreterException();
};

#endif // InterpreterException_hpp

