//$Id$
//------------------------------------------------------------------------------
//                           EventLocator
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
// Created: Jul 6, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef EventLocator_hpp
#define EventLocator_hpp


#include "GmatBase.hpp"
#include "LocatedEventTable.hpp"
#include "EventFunction.hpp"


class GMAT_API EventLocator: public GmatBase
{
public:
   EventLocator(const std::string &typeStr, const std::string &nomme = "");
   virtual ~EventLocator();
   EventLocator(const EventLocator& el);
   EventLocator& operator=(const EventLocator& el);

   // Inherited (GmatBase) methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

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

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);


   virtual void         SetSolarSystem(SolarSystem *ss);
//   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
//   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name);

   virtual bool         Initialize();
   virtual Real         GetTolerance();

   virtual Real *Evaluate();

   UnsignedInt GetFunctionCount();
   void BufferEvent(Integer forEventFunction = 0);
   void BufferEvent(Real epoch, std::string type, bool isStart);
   void ReportEventData();
   void ReportEventStatistics();
   Real *GetEventData(std::string type, Integer whichOne = 0);
   void UpdateEventTable(SortStyle how);
   virtual GmatEpoch GetLastEpoch(Integer index);

protected:
   /// The collection of event functions used by the EventLocator.
   std::vector<EventFunction*> eventFunctions;
   /// The longest event duration encountered by the EventLocator.
   std::vector<Real> maxSpan;
   /// The most recent event duration encountered by the EventLocator.
   std::vector<Real>lastSpan;
   /// The LocatedEventTable for the EventLocator.
   LocatedEventTable eventTable;
   /// Name of the event data file
   std::string filename;
   /// The number of event functions to be processed
   UnsignedInt efCount;
   /// The last data set computed
   Real *lastData;
   /// Epochs of the last events located
   GmatEpoch *lastEpochs;
   /// Flag used to turn the locator on or off (default is on)
   bool isActive;
   /// Flag used to show or hide plot of the data
   bool showPlot;

   /// Names of the "target" spacecraft in the location
   StringArray satNames;
   /// Pointers to the sats -- using SpaceObject so Formations can be supported
   std::vector<SpaceObject*> targets;
   /// Event location tolerance
   Real eventTolerance;
   /// The space environment
   SolarSystem *solarSys;

   /// Published parameters for event locators
    enum
    {
       SATNAMES = GmatBaseParamCount,
       TOLERANCE,
       EVENT_FILENAME,
       IS_ACTIVE,
       SHOW_PLOT,
       EventLocatorParamCount
    };

    /// burn parameter labels
    static const std::string
       PARAMETER_TEXT[EventLocatorParamCount - GmatBaseParamCount];
    /// burn parameter types
    static const Gmat::ParameterType
       PARAMETER_TYPE[EventLocatorParamCount - GmatBaseParamCount];
};

#endif /* EventLocator_hpp */
