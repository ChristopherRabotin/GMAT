//$Id: ErrorModelFactory.cpp 1398 2015-01-07 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         ErrorModelFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/01/07
//
/**
 * Factory used to create ErrorModel objects.
 */
//------------------------------------------------------------------------------


#include "ErrorModelFactory.hpp"
#include "ErrorModel.hpp"


//-----------------------------------------------------------------------------
// ErrorModelFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
ErrorModelFactory::ErrorModelFactory() :
   Factory     (Gmat::ERROR_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ErrorModel");
   }
   GmatType::RegisterType(Gmat::ERROR_MODEL, "ErrorModel");
}


//-----------------------------------------------------------------------------
// ~ErrorModelFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
ErrorModelFactory::~ErrorModelFactory()
{
}


//-----------------------------------------------------------------------------
// ErrorModelFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
ErrorModelFactory::ErrorModelFactory(StringArray createList) :
   Factory        (createList, Gmat::ERROR_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ErrorModel");
   }
}


//-----------------------------------------------------------------------------
// ErrorModelFactory(const ErrorModelFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
ErrorModelFactory::ErrorModelFactory(const ErrorModelFactory& fact) :
   Factory        (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ErrorModel");
   }
}


//-----------------------------------------------------------------------------
// ErrorModelFactory& operator= (const ErrorModelFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
ErrorModelFactory& ErrorModelFactory::operator= (const ErrorModelFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("ErrorModel");
      }
   }

   return *this;
}


//-----------------------------------------------------------------------------
// ErrorModel* CreateErrorModel(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a ErrorModel object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
ErrorModel* ErrorModelFactory::CreateErrorModel(const std::string &ofType,
      const std::string &withName)
{
   ErrorModel *retval = NULL;

   if (ofType == "ErrorModel")
      retval = new ErrorModel(withName);

   return retval;
}


GmatBase* ErrorModelFactory::CreateObject(const std::string &ofType,
                                const std::string &withName)
{
   return (GmatBase*) CreateErrorModel(ofType, withName);
}

