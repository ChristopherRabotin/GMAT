//$Id$
//------------------------------------------------------------------------------
//                            StopConditionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
 *  Implementation code for the StopConditionFactory class, responsible for
 *  creating StopCondition objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "StopConditionFactory.hpp"
#include "StopCondition.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateStopCondition(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested StopCondition class
 *
 * @param <ofType> the StopCondition object to create and return.
 * @param <withName> the name to give the newly-created StopCondition object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
StopCondition* StopConditionFactory::CreateStopCondition(const std::string &ofType,
                                                         const std::string &withName)
{
    if (ofType == "StopCondition")
        return new StopCondition(withName);
    return NULL;
}


//------------------------------------------------------------------------------
//  StopConditionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class StopConditionFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
StopConditionFactory::StopConditionFactory() :
Factory(Gmat::STOP_CONDITION)
{
   if (creatables.empty())
   {
      creatables.push_back("StopCondition");
   }
}

//------------------------------------------------------------------------------
//  StopConditionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class StopConditionFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
StopConditionFactory::StopConditionFactory(StringArray createList) :
Factory(createList,Gmat::STOP_CONDITION)
{
}

//------------------------------------------------------------------------------
//  StopConditionFactory(const StopConditionFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class StopConditionFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
StopConditionFactory::StopConditionFactory(const StopConditionFactory& fact) :
Factory(fact)
{
}

//------------------------------------------------------------------------------
//  StopConditionFactory& operator= (const StopConditionFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the StopConditionFactory class.
 *
 * @param <fact> the StopConditionFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" StopConditionFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
StopConditionFactory& StopConditionFactory::operator= (
                                             const StopConditionFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~StopConditionFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the StopConditionFactory base class.
 *
 */
//------------------------------------------------------------------------------
StopConditionFactory::~StopConditionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





