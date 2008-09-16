//$Header$
//------------------------------------------------------------------------------
//                              InterfaceException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy Shoan/GSFC
// Created: 2006.09.15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Interface classes.
 */
//------------------------------------------------------------------------------


#include "InterfaceException.hpp" // class's header file

//------------------------------------------------------------------------------
//  InterfaceException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs InterfaceException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
InterfaceException::InterfaceException(const std::string &details) :
    BaseException           ("Interface Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~InterfaceException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
InterfaceException::~InterfaceException()
{
}


//------------------------------------------------------------------------------
//  InterfaceException(const InterfaceException &ie)
//------------------------------------------------------------------------------
/**
 * Constructs InterfaceException instance (copy constructor). 
 *
 * @param ie Instance that gets copied.
 */
//------------------------------------------------------------------------------
InterfaceException::InterfaceException(const InterfaceException &ie) :
    BaseException       (ie)
{}

