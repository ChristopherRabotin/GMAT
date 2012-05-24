//$Id: ContactLocator.hpp 2264 2012-04-05 22:12:37Z djconway@NDC $
//------------------------------------------------------------------------------
//                           ContactLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 7, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef ContactLocator_hpp
#define ContactLocator_hpp


#include "EventLocator.hpp"
#include "Contact.hpp"


/**
 * Event locator used for station contact events
 *
 * This class manages station contacts for a set of target SpaceObjects
 * (typically Spacecraft).  COntact events require that two types of event
 * function be satisfied: an elevation function, placing the target above the
 * station's horizon, and (zero or more) line-of-sight event functions, ensuring
 * that there is no obstructing object between the station and the target.
 *
 * @note: Work on this event type is temporarily on hold.  The current state is
 * a set of event functions coded but untested, and a containing ContactLocator
 * which has untested initialization but is missing the reference object
 * methods.
 */
class ContactLocator : public EventLocator
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

   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);

   virtual GmatBase*    Clone() const;
   virtual bool         Initialize();
   virtual void         ClearContacts();

   DEFAULT_TO_NO_CLONES

protected:
   /// List of stations
   StringArray stationNames;
   /// Collection of stations
   ObjectArray stations;
   /// List of line of sight obstructors
   StringArray bodyNames;
   /// Collection of bodies
   ObjectArray bodies;

   /// The Contact event functions
   std::vector<Contact*>     contacts;

   /// Published parameters for contact locators
    enum
    {
       STATIONS = EventLocatorParamCount,
       BODIES,
       ContactLocatorParamCount
    };

    /// burn parameter labels
    static const std::string
       PARAMETER_TEXT[ContactLocatorParamCount - EventLocatorParamCount];
    /// burn parameter types
    static const Gmat::ParameterType
       PARAMETER_TYPE[ContactLocatorParamCount - EventLocatorParamCount];
};

#endif /* ContactLocator_hpp */
