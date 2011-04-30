//$Id$
//------------------------------------------------------------------------------
//                           ConfigManagerException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Definition of the exception class for the configuration manager.
 */
//------------------------------------------------------------------------------


#ifndef ConfigManagerException_hpp
#define ConfigManagerException_hpp

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown by the Configuration Manager.
 */
class GMAT_API ConfigManagerException : public BaseException
{
	public:
		// class constructor
		ConfigManagerException(const std::string &details);
		// class destructor
		~ConfigManagerException();
};

#endif // ConfigManagerException_hpp

