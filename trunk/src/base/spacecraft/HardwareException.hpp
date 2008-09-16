//$Header$
//------------------------------------------------------------------------------
//                                  HardwareException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Exception class used by the Hardware hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef HARDWAREEXCEPTION_HPP
#define HARDWAREEXCEPTION_HPP

#include "BaseException.hpp"

class HardwareException : public BaseException{
public:

	HardwareException(std::string details);
	virtual ~HardwareException();
   HardwareException(const HardwareException &soe);
};

#endif // HARDWAREEXCEPTION_HPP
