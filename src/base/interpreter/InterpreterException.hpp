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




#ifndef INTERPRETEREXCEPTION_HPP
#define INTERPRETEREXCEPTION_HPP

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown by the interpereters
 */
class InterpreterException : public BaseException
{
	public:
		// class constructor
		InterpreterException(std::string details);
		// class destructor
		~InterpreterException(void);
};

#endif // INTERPRETEREXCEPTION_HPP

