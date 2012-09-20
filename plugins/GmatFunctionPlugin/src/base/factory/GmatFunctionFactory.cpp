//$Id$
//------------------------------------------------------------------------------
//                              GmatFunctionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, task order 28.
//
// Author: Darrel Conway (Thinking Systems)
// Created: 2012/09/18
//
/**
 * Implementation code for the GmatFunctionFactory class, responsible for
 * creating GmatFunction objects.
 */
//------------------------------------------------------------------------------


#include "GmatFunctionFactory.hpp"
#include "GmatFunction.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class
 *
 * @param ofType   The Function object to create and return.
 * @param withName The name of the new object
 *
 * @return The new function
 */
//------------------------------------------------------------------------------
Function* GmatFunctionFactory::CreateFunction(const std::string &ofType,
         const std::string &withName)
{
   if (ofType == "GmatFunction")
      return new GmatFunction(withName);
   // add more here .......
   else {
      return NULL;   // doesn't match any known type of Function
   }
   
}


//------------------------------------------------------------------------------
//  GmatFunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class GmatFunctionFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
GmatFunctionFactory::GmatFunctionFactory() :
    Factory     (Gmat::FUNCTION)
{
   if (creatables.empty())
   {
      creatables.push_back("GmatFunction");
   }
}

//------------------------------------------------------------------------------
//  GmatFunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates a GmatFunctionFactory with list of supported objects.
 *
 * @param createList list of creatable Function objects
 */
//------------------------------------------------------------------------------
GmatFunctionFactory::GmatFunctionFactory(StringArray createList) :
    Factory     (createList, Gmat::FUNCTION)
{
}

//------------------------------------------------------------------------------
//  GmatFunctionFactory(const GmatFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class GmatFunctionFactory (called by
   * copy constructors of derived classes).  (copy constructor)
   *
   * @param fact the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
GmatFunctionFactory::GmatFunctionFactory(const GmatFunctionFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("GmatFunction");
   }
}

//------------------------------------------------------------------------------
//  GmatFunctionFactory& operator= (const GmatFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the GmatFunctionFactory base class.
   *
   * @param fact The GmatFunctionFactory object whose data to assign to this
   *             factory.
   *
   * @return "this" GmatFunctionFactory, set to match the input factory fact.
   */
//------------------------------------------------------------------------------
GmatFunctionFactory& GmatFunctionFactory::operator=(const GmatFunctionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~GmatFunctionFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the GmatFunctionFactory class.
 */
//------------------------------------------------------------------------------
GmatFunctionFactory::~GmatFunctionFactory()
{
   // deletes handled by Factory destructor
}
