//$Header$
//------------------------------------------------------------------------------
//                              PublisherException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
class PublisherException : public BaseException
{
	public:
		// class constructor
		PublisherException(std::string details);
		// class destructor
		~PublisherException();
};

#endif // PUBLISHEREXCEPTION_HPP

