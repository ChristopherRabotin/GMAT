//$Header$
//------------------------------------------------------------------------------
//                                  ClassName
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


#ifndef COMMANDEXCEPTION_HPP
#define COMMANDEXCEPTION_HPP

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown from the command subsystem
 */
class CommandException : public BaseException
{
	public:
		// class constructor
		CommandException(std::string details);
		// class destructor
		~CommandException();
		// Copy constructor -- hidden from other classes
		CommandException(const CommandException &);
};

#endif // COMMANDEXCEPTION_HPP

