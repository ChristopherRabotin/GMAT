//$Id: ObTypeFactory.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             ObTypeFactory
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
// Created: 2009/06/19
// 
/**
 * Factory used to create ObType objects
 */
//------------------------------------------------------------------------------


#include "ObTypeFactory.hpp"

#define INCLUDE_TDM


// Supported ObTypes
#include "GmatObType.hpp"
#ifdef INCLUDE_TDM
   #include "TdmObType.hpp"
#endif

/// TBD: Are these needed?
#include "GmatODType.hpp"						// made change by TUAN NGUYEN
#include "GmatODDopplerType.hpp"				// made change by TUAN NGUYEN
#include "RampTableType.hpp"                    // made change by TUAN NGUYEN



//-----------------------------------------------------------------------------
// ObTypeFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
ObTypeFactory::ObTypeFactory() :
   Factory     (Gmat::OBTYPE)
{
   if (creatables.empty())
   {
      creatables.push_back("GMATInternal");
	  creatables.push_back("GMAT_OD");				// made change by TUAN NGUYEN
	  creatables.push_back("GMAT_ODDoppler");		// made change by TUAN NGUYEN
	  creatables.push_back("GMAT_RampTable");		// made change by TUAN NGUYEN
#ifdef INCLUDE_TDM
      creatables.push_back("TDM");
#endif
   }
}


//-----------------------------------------------------------------------------
// ~ObTypeFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
ObTypeFactory::~ObTypeFactory()
{
}


//-----------------------------------------------------------------------------
// ObTypeFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
ObTypeFactory::ObTypeFactory(StringArray createList) :
   Factory        (createList, Gmat::OBTYPE)
{
   if (creatables.empty())
   {
      creatables.push_back("GMATInternal");
	  creatables.push_back("GMAT_OD");				// made change by TUAN NGUYEN
	  creatables.push_back("GMAT_ODDoppler");		// made change by TUAN NGUYEN
	  creatables.push_back("GMAT_RampTable");		// made change by TUAN NGUYEN
#ifdef INCLUDE_TDM
      creatables.push_back("TDM");
#endif
   }
}


//-----------------------------------------------------------------------------
// ObTypeFactory(const ObTypeFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
ObTypeFactory::ObTypeFactory(const ObTypeFactory& fact) :
   Factory        (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("GMATInternal");
	  creatables.push_back("GMAT_OD");				// made change by TUAN NGUYEN
	  creatables.push_back("GMAT_ODDoppler");		// made change by TUAN NGUYEN
	  creatables.push_back("GMAT_RampTable");		// made change by TUAN NGUYEN
#ifdef INCLUDE_TDM
      creatables.push_back("TDM");
#endif
   }
}


//-----------------------------------------------------------------------------
// ObTypeFactory& operator= (const ObTypeFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
ObTypeFactory& ObTypeFactory::operator= (const ObTypeFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("GMATInternal");
		 creatables.push_back("GMAT_OD");				// made change by TUAN NGUYEN
		 creatables.push_back("GMAT_ODDoppler");		// made change by TUAN NGUYEN
		 creatables.push_back("GMAT_RampTable");		// made change by TUAN NGUYEN
#ifdef INCLUDE_TDM
         creatables.push_back("TDM");
#endif
      }
   }

   return *this;
}


//-----------------------------------------------------------------------------
// ObType* CreateObType(const std::string &ofType, const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates an ObType object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
ObType* ObTypeFactory::CreateObType(const std::string &ofType,
      const std::string &withName)
{
   ObType *retval = NULL;

   if (ofType == "GMATInternal")
      retval = new GmatObType(withName);
   else if (ofType == "GMAT_OD")				// made change by TUAN NGUYEN
      retval = new GmatODType(withName);		// made change by TUAN NGUYEN
   else if (ofType == "GMAT_ODDoppler")			// made change by TUAN NGUYEN
      retval = new GmatODDopplerType(withName);	// made change by TUAN NGUYEN
   else if (ofType == "GMAT_RampTable")			// made change by TUAN NGUYEN
      retval = new RampTableType(withName);		// made change by TUAN NGUYEN
#ifdef INCLUDE_TDM
   if (ofType == "TDM")
      retval = new TdmObType(withName);
#endif

   return retval;
}
