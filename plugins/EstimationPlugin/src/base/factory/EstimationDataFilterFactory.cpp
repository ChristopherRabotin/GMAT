//$Id: EstimationDataFilterFactory.cpp 1398 2015-03-03 15:42:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         EstimationDataFilterFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2009/03/03
//
/**
 * Factory used to create EstimationDataFilter objects.
 */
//------------------------------------------------------------------------------


#include "EstimationDataFilterFactory.hpp"
#include "StatisticAcceptFilter.hpp"
#include "StatisticRejectFilter.hpp"
//#include "EstimationAcceptFilter.hpp"
//#include "EstimationRejectFilter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONSTRUCTOR

//-----------------------------------------------------------------------------
// EstimationDataFilterFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::EstimationDataFilterFactory() :
   Factory     (Gmat::DATA_FILTER)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory default constructor\n");
#endif

   if (creatables.empty())
   {
      creatables.push_back("StatisticsAcceptFilter");
      creatables.push_back("StatisticsRejectFilter");
      //creatables.push_back("EstimationAcceptFilter");
      //creatables.push_back("EstimationRejectFilter");
   }
}


//-----------------------------------------------------------------------------
// ~EstimationDataFilterFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::~EstimationDataFilterFactory()
{
}


//-----------------------------------------------------------------------------
// EstimationDataFilterFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::EstimationDataFilterFactory(StringArray createList) :
   Factory        (createList, Gmat::DATA_FILTER)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory(names) default constructor\n");
#endif

   if (creatables.empty())
   {
      creatables.push_back("StatisticsAcceptFilter");
      creatables.push_back("StatisticsRejectFilter");
      //creatables.push_back("EstimationAcceptFilter");
      //creatables.push_back("EstimationRejectFilter");
   }
}


//-----------------------------------------------------------------------------
// EstimationDataFilterFactory(const EstimationDataFilterFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::EstimationDataFilterFactory(const EstimationDataFilterFactory& fact) :
   Factory        (fact)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory copy constructor\n");
#endif

   if (creatables.empty())
   {
      creatables.push_back("StatisticsAcceptFilter");
      creatables.push_back("StatisticsRejectFilter");
      //creatables.push_back("EstimationAcceptFilter");
      //creatables.push_back("EstimationRejectFilter");
   }
}


//-----------------------------------------------------------------------------
// EstimationDataFilterFactory& operator= (const EstimationDataFilterFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory& EstimationDataFilterFactory::operator= (const EstimationDataFilterFactory& fact)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory operator =\n");
#endif

   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("StatisticsAcceptFilter");
         creatables.push_back("StatisticsRejectFilter");
         //creatables.push_back("EstimationAcceptFilter");
         //creatables.push_back("EstimationRejectFilter");
      }
   }

   return *this;
}


//-----------------------------------------------------------------------------
// DataFilter* CreateDataFilter(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a DataFilter object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
DataFilter* EstimationDataFilterFactory::CreateDataFilter(const std::string &ofType,
      const std::string &withName)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory::CreateDataFilter()\n");
#endif

   DataFilter *retval = NULL;

   if (ofType == "StatisticsAcceptFilter")
      retval = new StatisticAcceptFilter(withName);
   if (ofType == "StatisticsRejectFilter")
      retval = new StatisticRejectFilter(withName);
   //if (ofType == "EstimationAcceptFilter")
   //   retval = new EstimationAcceptFilter(withName);
   //if (ofType == "EstimationRejectFilter")
   //   retval = new EstimationRejectFilter(withName);

   return retval;
}


//-----------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a DataFilter object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
GmatBase* EstimationDataFilterFactory::CreateObject(const std::string &ofType,
      const std::string &withName)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory::CreateObject()\n");
#endif

   return (GmatBase*) CreateDataFilter(ofType, withName);
}
