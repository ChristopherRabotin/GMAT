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


#ifndef SPACEOBJECTEXCEPTION_HPP
#define SPACEOBJECTEXCEPTION_HPP

#include "BaseException.hpp"

class SpaceObjectException : public BaseException{
public:

	SpaceObjectException(std::string details);
	virtual ~SpaceObjectException();
   SpaceObjectException(const SpaceObjectException &soe);
};

#endif // SPACEOBJECTEXCEPTION_H
