//$Id$
//------------------------------------------------------------------------------
//                              PublisherException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/07/29
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P.
//
/**
 * Exception class used by the Publisher.
 */
//------------------------------------------------------------------------------



#ifndef PUBLISHEREXCEPTION_HPP
#define PUBLISHEREXCEPTION_HPP

#include "BaseException.hpp" // inheriting class's header file

/**
 * Publisher Exception class
 */
class GMAT_API PublisherException : public BaseException
{
	public:
		// class constructor
		PublisherException(std::string details);
		// class destructor
		~PublisherException();
};

#endif // PUBLISHEREXCEPTION_HPP

