//$Id$
//------------------------------------------------------------------------------
//                                  Subscriber
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
// *** File Name : Subscriber.hpp
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 10/01/2003 - L. Ruley, Missions Applications Branch
//                             Updated style using GMAT cpp style guide
// **************************************************************************
//
/**
 * Definition for the Subscriber base class.
 */
//------------------------------------------------------------------------------
#ifndef Subscriber_hpp
#define Subscriber_hpp

#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "SolarSystem.hpp"
#include "ElementWrapper.hpp"

class GMAT_API Subscriber : public GmatBase
{
public:
   Subscriber(const std::string &typeStr, const std::string &nomme);
   Subscriber(const Subscriber &);
   Subscriber& operator=(const Subscriber &);
   virtual ~Subscriber(void);
   
   virtual bool         Initialize();
   virtual bool         IsInitialized();
   virtual bool         ReceiveData(const char * datastream);
   virtual bool         ReceiveData(const char * datastream, const Integer len);
   virtual bool         ReceiveData(const Real * datastream, const Integer len = 0);
   virtual bool         FlushData(bool endOfDataBlock = true);
   virtual bool         SetEndOfRun();
   virtual void         SetRunState(Gmat::RunState rs);
   void                 SetManeuvering(GmatBase *maneuver, bool flag, Real epoch,
                                       const std::string &satName,
                                       const std::string &desc);
   void                 SetManeuvering(GmatBase *maneuver, bool flag, Real epoch,
                                       const StringArray &satNames,
                                       const std::string &desc);
   void                 SetScPropertyChanged(GmatBase *originator, Real epoch,
                                             const std::string &satName,
                                             const std::string &desc);
   Subscriber*          Next();
   bool                 Add(Subscriber *s);
   bool                 Remove(Subscriber *s, const bool del);
   
   virtual void         Activate(bool state = true);
   virtual bool         IsActive();
   
   virtual void         SetProviderId(Integer id);
   virtual Integer      GetProviderId();
   
   virtual void         SetProvider(GmatBase *provider);
   virtual void         SetDataLabels(const StringArray& elements);
   virtual void         ClearDataLabels();
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetDataCoordSystem(CoordinateSystem *cs);
   virtual void         SetDataMJ2000EqOrigin(CelestialBody *cb);
   virtual void         SetSolarSystem(SolarSystem *ss);
   
   // methods for element wrappers
   virtual const StringArray&
                        GetWrapperObjectNameArray();
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &name);
   virtual void         ClearWrappers();
   
   // for rename reference objects
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // methods for parameters
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   
   enum SolverIterOption
   {
      SI_ALL,
      SI_CURRENT,
      SI_NONE,
      SolverIterOptionCount
   };
   
protected:
   
   std::string          mSolverIterations;
   SolverIterOption     mSolverIterOption;
   
   /// Arrays used to track elements for published data
   std::vector<StringArray> theDataLabels;
   
   const char           *data;
   Subscriber           *next;
   CoordinateSystem     *theInternalCoordSystem;
   CoordinateSystem     *theDataCoordSystem;
   CelestialBody        *theDataMJ2000EqOrigin;
   SolarSystem          *theSolarSystem;
   GmatBase             *currentProvider;
   
   bool                 active;
   bool                 isManeuvering;
   bool                 isEndOfReceive;
   bool                 isEndOfDataBlock;
   bool                 isEndOfRun;
   bool                 isInitialized;
   bool                 isFinalized;
   bool                 isDataOn;
   bool                 isDataStateChanged;
   
   /// The current run state, so actions based on state can be taken
   Gmat::RunState       runstate;
   Integer              currProviderId;
   
   /// The list of names of Wrapper objects
   StringArray          wrapperObjectNames;
   /// vector of pointers to ElementWrappers for the item
   WrapperArray         depParamWrappers;
   WrapperArray         paramWrappers;
   
   // For ElementWrapper
   bool                 CloneWrappers(WrapperArray &toWrappers,
                                      const WrapperArray &fromWrappers);
   bool                 SetWrapperReference(GmatBase *obj, const std::string &name);
   void                 WriteWrappers();
   Integer              FindIndexOfElement(StringArray &labelArray,
                                           const std::string &label);
   
   // Methods that derived classes can override
   virtual bool         Distribute(Integer len);
   virtual bool         Distribute(const Real *dat, Integer len);
   virtual void         HandleManeuvering(GmatBase *originator,
                                          bool maneuvering, Real epoch,
                                          const StringArray &satNames,
                                          const std::string &desc);
   virtual void         HandleScPropertyChange(GmatBase *originator, Real epoch,
                                               const std::string &satName,
                                               const std::string &desc);
   
public:
   enum
   {
      SOLVER_ITERATIONS = GmatBaseParamCount,
      TARGET_STATUS,
      SubscriberParamCount,
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[SubscriberParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[SubscriberParamCount - GmatBaseParamCount];
   
   // for GUI population
   static StringArray solverIterOptions;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;
   static Integer GetSolverIterOptionCount() { return SolverIterOptionCount; }
   static const std::string* GetSolverIterOptionList();
   
   
private:
   
   static const std::string SOLVER_ITER_OPTION_TEXT[SolverIterOptionCount];
   bool    wrappersCopied;
};
#endif // Subscribe_hpp

