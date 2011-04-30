//$Id$
//------------------------------------------------------------------------------
//                            CalculatedPointFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy Shoan
// Created: 2004/01/29
//
/**
 *  Implementation code for the CalculatedPointFactory class, responsible for
 *  creating CalculatedPoint objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CalculatedPointFactory.hpp"
#include "LibrationPoint.hpp"
#include "Barycenter.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateCalculatedPoint(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested CalculatedPoint 
 * class.
 *
 * @param <ofType>   the CalculatedPoint object to create and return.
 * @param <withName> the name to give the newly-created CalculatedPoint object.
 *
 * @return a new CalculatedPoint object.
 *
 */
//------------------------------------------------------------------------------
CalculatedPoint* CalculatedPointFactory::CreateCalculatedPoint(
                                         const std::string &ofType,
                                         const std::string &withName)
{
   if (ofType == "LibrationPoint")
      return new LibrationPoint(withName);
   else if (ofType == "Barycenter")
      return new Barycenter(withName);
   else
   {
      return NULL;   // doesn't match any known type of CalculatedPoint
   }
}

//------------------------------------------------------------------------------
//  CalculatedPointFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CalculatedPointFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
CalculatedPointFactory::CalculatedPointFactory() :
Factory(Gmat::CALCULATED_POINT)
{
   if (creatables.empty())
   {
      creatables.push_back("LibrationPoint");
      creatables.push_back("Barycenter");
   }
}

//------------------------------------------------------------------------------
//  CalculatedPointFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CalculatedPointFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
CalculatedPointFactory::CalculatedPointFactory(const StringArray &createList) :
Factory(createList,Gmat::CALCULATED_POINT)
{
   if (creatables.empty())
   {
      creatables.push_back("LibrationPoint");
      creatables.push_back("Barycenter");
   }
}

//------------------------------------------------------------------------------
//  CalculatedPointFactory(const CalculatedPointFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CalculatedPointFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CalculatedPointFactory::CalculatedPointFactory(const CalculatedPointFactory &fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("LibrationPoint");
      creatables.push_back("Barycenter");
   }
}

//------------------------------------------------------------------------------
//  CalculatedPointFactory& operator= (const CalculatedPointFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CalculatedPointFactory class.
 *
 * @param <fact> the CalculatedPointFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" CalculatedPointFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CalculatedPointFactory& CalculatedPointFactory::operator= (
                                             const CalculatedPointFactory &fact)
{
   if (this == &fact) return *this;
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("LibrationPoint");
      creatables.push_back("Barycenter");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~CalculatedPointFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the CalculatedPointFactory base class.
 *
 */
//------------------------------------------------------------------------------
CalculatedPointFactory::~CalculatedPointFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





