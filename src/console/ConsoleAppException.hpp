//$Id$
//------------------------------------------------------------------------------
//                                  ClassName
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/mm/dd
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




// Class automatically generated by Dev-C++ New Class wizard

#ifndef CONSOLEAPPEXCEPTION_H
#define CONSOLEAPPEXCEPTION_H

#include "BaseException.hpp" // inheriting class's header file

/**
 * Class used to report exceptions to the console based driver for GMAT
 */
class ConsoleAppException : public BaseException
{
	public:
		// class constructor
		ConsoleAppException(std::string details);
		// class destructor
		~ConsoleAppException();
};

#endif // CONSOLEAPPEXCEPTION_H

