//$Header$
//------------------------------------------------------------------------------
//                                  ClassName
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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




#include "InterpreterException.hpp" // class's header file

// class constructor
InterpreterException::InterpreterException(std::string details) :
    BaseException       ("Interpreter Exception: ", details)
{
}

// class destructor
InterpreterException::~InterpreterException(void)
{
}

