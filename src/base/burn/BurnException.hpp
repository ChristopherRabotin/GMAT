//$Header$
//------------------------------------------------------------------------------
//                               BurnException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/1/13
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Burn hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef BURNEXCEPTION_H
#define BURNEXCEPTION_H

#include "BaseException.hpp"

class BurnException : public BaseException{
public:

	BurnException(std::string details);
	virtual ~BurnException();
    BurnException(const BurnException &be);
};

#endif // BURNEXCEPTION_H
