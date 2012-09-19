//$Id: MatlabFunctionFactory.cpp 9513 2011-04-30 21:23:06Z djcinsb $
//------------------------------------------------------------------------------
//                              MatlabFunctionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/9/22
//
/**
 * Implementation code for the MatlabFunctionFactory class, responsible for
 * creating MATLAB Function objects.
 */
//------------------------------------------------------------------------------


#include "MatlabFunctionFactory.hpp"
#include "MatlabFunction.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateFunction(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class
 *
 * @param <ofType> the Function object to create and return.
 */
//------------------------------------------------------------------------------
Function* MatlabFunctionFactory::CreateFunction(const std::string &ofType,
         const std::string &withName)
{
   if (ofType == "MatlabFunction")
      return new MatlabFunction(withName);
   // add more here .......
   else {
      return NULL;   // doesn't match any known type of Function
   }
   
}


//------------------------------------------------------------------------------
//  MatlabFunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabFunctionFactory.
 * (default constructor)
 *
 */
//------------------------------------------------------------------------------
MatlabFunctionFactory::MatlabFunctionFactory() :
    Factory     (Gmat::FUNCTION)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabFunction");
   }
}

//------------------------------------------------------------------------------
//  MatlabFunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabFunctionFactory.
 *
 * @param <createList> list of creatable Function objects
 *
 */
//------------------------------------------------------------------------------
MatlabFunctionFactory::MatlabFunctionFactory(StringArray createList) :
    Factory     (createList, Gmat::FUNCTION)
{
}

//------------------------------------------------------------------------------
//  MatlabFunctionFactory(const MatlabFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class MatlabFunctionFactory (called by
   * copy constructors of derived classes).  (copy constructor)
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
MatlabFunctionFactory::MatlabFunctionFactory(const MatlabFunctionFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabFunction");
   }
}

//------------------------------------------------------------------------------
//  MatlabFunctionFactory& operator= (const MatlabFunctionFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the MatlabFunctionFactory base class.
   *
   * @param <fact> the MatlabFunctionFactory object whose data to assign to "this" factory.
   *
   * @return "this" MatlabFunctionFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
MatlabFunctionFactory& MatlabFunctionFactory::operator=(const MatlabFunctionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~MatlabFunctionFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the MatlabFunctionFactory class.
   *
   */
//------------------------------------------------------------------------------
MatlabFunctionFactory::~MatlabFunctionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

