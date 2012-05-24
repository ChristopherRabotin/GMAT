//$Id: EclipseLocator.hpp 2264 2012-04-05 22:12:37Z djconway@NDC $
//------------------------------------------------------------------------------
//                           EclipseLocator
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
// Created: Sep 2, 2011
//
/**
 * Definition of the EventLocator used for eclipses
 */
//------------------------------------------------------------------------------


#ifndef EclipseLocator_hpp
#define EclipseLocator_hpp

#include "EventLocator.hpp"
#include "Umbra.hpp"
#include "Penumbra.hpp"
#include "Antumbra.hpp"


/**
 * The EventLocator used for shadow entry and exit location
 */
class LOCATOR_API EclipseLocator : public EventLocator
{
public:
   EclipseLocator(const std::string &name);
   virtual ~EclipseLocator();
   EclipseLocator(const EclipseLocator& el);
   EclipseLocator& operator=(const EclipseLocator& el);

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
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData);

   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);

   const ObjectTypeArray& GetTypesForList(const Integer id);
   const ObjectTypeArray& GetTypesForList(const std::string &label);

   virtual GmatBase*    Clone() const;
   virtual bool         Initialize();

   DEFAULT_TO_NO_CLONES

protected:
   /// List of occulting bodies
   StringArray occulters;

   // Member event functions
   /// The Umbra event functions
   std::vector<Umbra*>        umbras;
   /// The Penumbra event functions
   std::vector<Penumbra*>     penumbras;
   /// The Antumbra event functions
   std::vector<Antumbra*>     antumbras;

   /// Published parameters for eclipse locators
    enum
    {
       OCCULTERS = EventLocatorParamCount,
       EclipseLocatorParamCount
    };

    /// burn parameter labels
    static const std::string
       PARAMETER_TEXT[EclipseLocatorParamCount - EventLocatorParamCount];
    /// burn parameter types
    static const Gmat::ParameterType
       PARAMETER_TYPE[EclipseLocatorParamCount - EventLocatorParamCount];
};

#endif /* EclipseLocator_hpp */
