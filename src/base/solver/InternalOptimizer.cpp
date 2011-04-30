//$Id$
//------------------------------------------------------------------------------
//                             InternalOptimizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2007/03/27
//
/**
 * Implementation of the internal optimizer. 
 */
//------------------------------------------------------------------------------


#include "InternalOptimizer.hpp"

//-----------------------------------------
// public methods
//-----------------------------------------


//------------------------------------------------------------------------------
// InternalOptimizer()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
InternalOptimizer::InternalOptimizer(std::string type, std::string name) :
   Optimizer      (type, name)
{
   objectTypeNames.push_back("InternalOptimizer");
}

//------------------------------------------------------------------------------
// InternalOptimizer()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
InternalOptimizer::~InternalOptimizer()
{
}

//------------------------------------------------------------------------------
// InternalOptimizer(const InternalOptimizer &opt)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
InternalOptimizer::InternalOptimizer(const InternalOptimizer &opt) :
   Optimizer      (opt)
{
}

//------------------------------------------------------------------------------
// InternalOptimizer& operator=(const InternalOptimizer &jac)
//------------------------------------------------------------------------------
/**
 * InternalOptimizer assignment operator
 * 
 * Copies the data from another InternalOptimizer instance into this one.
 * 
 * @param jac The suppier of the InternalOptimizer data.
 * 
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
InternalOptimizer& InternalOptimizer::operator=(const InternalOptimizer& opt)
{
   if (&opt != this)
   {
      Optimizer::operator=(opt);
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method used to set up the InternalOptimizer data structures prior to use.
 * 
 * @return true if initialization succeeds
 */
//------------------------------------------------------------------------------
bool InternalOptimizer::Initialize()
{
   bool retval = Optimizer::Initialize();
   
   return retval;
}
