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
 * Implementation of the exception class for the configuration manager.
 */
//------------------------------------------------------------------------------


#include "ConfigManagerException.hpp" // class's header file


//------------------------------------------------------------------------------
// ConfigManagerException(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param details Text string detailing the problem that threw the exception.
 */
//------------------------------------------------------------------------------
ConfigManagerException::ConfigManagerException(const std::string &details) :
    BaseException       ("ConfigManager Exception Thrown: ", details)
{
}

//------------------------------------------------------------------------------
// ~ConfigManagerException()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ConfigManagerException::~ConfigManagerException()
{
}

