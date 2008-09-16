//$Header$
//------------------------------------------------------------------------------
//                           ConfigManagerException
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
 * Definition of the exception class for the configuration manager.
 */
//------------------------------------------------------------------------------


#ifndef ConfigManagerException_hpp
#define ConfigManagerException_hpp

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown by the Configuration Manager.
 */
class ConfigManagerException : public BaseException
{
	public:
		// class constructor
		ConfigManagerException(const std::string &details);
		// class destructor
		~ConfigManagerException();
};

#endif // ConfigManagerException_hpp

