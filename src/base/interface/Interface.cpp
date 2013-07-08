//$Id$
//------------------------------------------------------------------------------
//                                  Interface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun/GSFC
// Created: 2010/04/02
//
/**
 * Implementation of Interface class.
 */
//------------------------------------------------------------------------------

#include "Interface.hpp"
#include "InterfaceException.hpp"

//------------------------------------------------------------------------------
//  Interface(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructs Interface instance (default constructor).
 */
//------------------------------------------------------------------------------
Interface::Interface(const std::string &type, const std::string &name) :
   GmatBase (Gmat::INTERFACE, type, name)
{
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("Interface");
}


//------------------------------------------------------------------------------
//  Interface(const Interface &interf)
//------------------------------------------------------------------------------
/**
 * Constructs Interface instance (copy constructor). 
 *
 * @param  interf  Instance that gets copied.
 */
//------------------------------------------------------------------------------
Interface::Interface(const Interface &interf) :
   GmatBase (interf)
{
}


//------------------------------------------------------------------------------
//  ~Interface()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
Interface::~Interface()
{
}


//------------------------------------------------------------------------------
// virtual Integer Open(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Opens interface to other application such as MATLAB
 *
 * @param  name  Name of the interface to be used when opening [""]
 */
//------------------------------------------------------------------------------
Integer Interface::Open(const std::string &name)
{
   throw InterfaceException("Open() not defined for " + typeName +
                            " named \"" + instanceName + "\"\n");
}


//------------------------------------------------------------------------------
// virtual Integer Close(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Closes interface to other application such as MATLAB
 *
 * @param  name  Name of the interface to be used when closing [""]
 */
//------------------------------------------------------------------------------
Integer Interface::Close(const std::string &name)
{
   throw InterfaceException("Close() not defined for " + typeName +
                            " named \"" + instanceName + "\"\n");
}

