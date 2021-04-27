//$Id: ContactLocator.hpp 2264 2012-04-05 22:12:37Z  $
//------------------------------------------------------------------------------
//                           ContactLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 7, 2011
// Updated: 2015  Wendy Shoan / GSFC and Yeerang Lim/KAIST
//
/**
 * Definition of the ContactLocator
 * Updates based on prototype by Joel Parker / GSFC
 */
//------------------------------------------------------------------------------


#ifndef ContactLocator_hpp
#define ContactLocator_hpp


#include "EventLocator.hpp"
#include "EventLocatorDefs.hpp"
#include "ContactResult.hpp"


/**
 * Event locator used for station contact events
 *
 * This class manages station contacts for a set of target SpaceObjects
 * (typically Spacecraft).  Contact events require that two types of event
 * function be satisfied: an elevation function, placing the target above the
 * station's horizon, and (zero or more) line-of-sight event functions, ensuring
 * that there is no obstructing object between the station and the target.
 *
 * @note: Work on this event type is temporarily on hold.  The current state is
 * a set of event functions coded but untested, and a containing ContactLocator
 * which has untested initialization but is missing the reference object
 * methods.
 */
class LOCATOR_API ContactLocator : public EventLocator
{
public:
   ContactLocator(const std::string &name);
   virtual ~ContactLocator();
   ContactLocator(const ContactLocator& el);
   ContactLocator& operator=(const ContactLocator& cl);

   // Inherited (GmatBase) methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const ObjectTypeArray&
                        GetTypesForList(const Integer id);
   virtual const ObjectTypeArray&
                        GetTypesForList(const std::string &label);

   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name);

   virtual bool         HasRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();


   virtual bool         RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName);

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");


   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual bool         Initialize();
   virtual bool         ReportEventData(const std::string &reportNotice = "");

   DEFAULT_TO_NO_CLONES

protected:
   /// List of stations
   StringArray stationNames;
   /// Collection of stations
   ObjectArray stations;
   /// Light time Direction
   std::string lightTimeDirection;

   // The stored results
   std::vector<ContactResult*> contactResults;


   /// Published parameters for contact locators
    enum
    {
       STATIONS = EventLocatorParamCount,
       LIGHT_TIME_DIRECTION,
       ContactLocatorParamCount
    };

    /// burn parameter labels
    static const std::string
       PARAMETER_TEXT[ContactLocatorParamCount - EventLocatorParamCount];
    /// burn parameter types
    static const Gmat::ParameterType
       PARAMETER_TYPE[ContactLocatorParamCount - EventLocatorParamCount];

    static const std::string LT_DIRECTIONS[2];

    virtual void         FindEvents();
    virtual std::string  GetAbcorrString();
};

#endif /* ContactLocator_hpp */
