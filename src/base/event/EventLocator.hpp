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
 * Definition of the the event locator base class
 */
//------------------------------------------------------------------------------


#ifndef EventLocator_hpp
#define EventLocator_hpp


#include "GmatBase.hpp"
#include "LocatedEventTable.hpp"
#include "EventFunction.hpp"


/**
 * Base class for the event locators.
 *
 * EventLocators are container classes that collect together a set of event
 * functions defining a specific type of location process.  For example, the
 * EclipseLocator class collects together Penumbra, Umbra, and Antumbra event
 * functions.
 */
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

   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);
   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);

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

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   virtual const ObjectTypeArray& GetTypesForList(const Integer id);
   virtual const ObjectTypeArray& GetTypesForList(const std::string &label);



   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual bool         SetOrigin(SpacePoint *bod, 
                              const std::string &forObject = "");
//   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
//   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                              const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);

   // Accessors for the owned subscribers
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);

   virtual bool         Initialize();
   virtual Real         GetTolerance();

   virtual Real         *Evaluate(GmatEpoch atEpoch = -1.0,
                                  Real *forState = NULL);

   UnsignedInt GetFunctionCount();
   void BufferEvent(Integer forEventFunction = 0);
   void BufferEvent(Real epoch, std::string type, bool isStart);
   void ReportEventData(const std::string &reportNotice = "");
   virtual bool FileWasWritten();
   void ReportEventStatistics();
   Real *GetEventData(std::string type, Integer whichOne = 0);
   void UpdateEventTable(SortStyle how);
   virtual GmatEpoch GetLastEpoch(Integer index);
   virtual void SetFoundEventBrackets(Integer index,
         GmatEpoch early, GmatEpoch late);

   // Methods used in integration
   virtual bool HasAssociatedStateObjects();
   virtual std::string GetAssociateName(UnsignedInt val = 0);
   std::string GetTarget(UnsignedInt forFunction);
   StringArray GetDefaultPropItems();
   Integer SetPropItem(const std::string &propItem);
   Integer GetPropItemSize(const Integer item);
   void SetStateIndices(UnsignedInt forFunction, Integer index,
         Integer associate);

protected:
   /// The collection of event functions used by the EventLocator.
   std::vector<EventFunction*> eventFunctions;
   /// Earliest time bracket for last event boundary found
   Real *earlyBound;
   /// Latest time bracket for last event boundary found
   Real *lateBound;
//   /// The longest event duration encountered by the EventLocator.
//   std::vector<Real> maxSpan;
//   /// The most recent event duration encountered by the EventLocator.
//   std::vector<Real> lastSpan;
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
   /// Flag set when a run writes data to the event report
   bool fileWasWritten;

   /// Names of the "target" spacecraft in the location
   StringArray satNames;
   /// Pointers to the sats -- using SpaceObject so Formations can be supported
   std::vector<SpaceObject*> targets;
   /// Event location tolerance
   Real eventTolerance;
   /// The space environment
   SolarSystem *solarSys;
   /// Indices for values/derivatives in the state/derivative vectors
   std::vector<Integer> stateIndices;
   /// Start indices for the associated state data
   std::vector<Integer> associateIndices;
   /// Vector of event function values used in integration
   Rvector functionValues;

   /// Published parameters for event locators
    enum
    {
       SATNAMES = GmatBaseParamCount,
       TOLERANCE,
       EVENT_FILENAME,
       IS_ACTIVE,
       SHOW_PLOT,
       EPOCH,
       EVENT_FUNCTION,
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
