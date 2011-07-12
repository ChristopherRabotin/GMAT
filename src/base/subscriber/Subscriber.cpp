//$Id$
//------------------------------------------------------------------------------
//                                  Subscriber
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// *** File Name : Subscriber.cpp
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
 * Implementation code for the Subscriber base class
 */
//------------------------------------------------------------------------------

#include "Subscriber.hpp"
#include "SubscriberException.hpp"
#include "Parameter.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "MessageInterface.hpp"

// Cloning wrapper is not ready
//#define __ENABLE_CLONING_WRAPPERS__

//#define DEBUG_WRAPPER_CODE
//#define DEBUG_SUBSCRIBER
//#define DEBUG_SUBSCRIBER_PARAM
//#define DEBUG_RECEIVE_DATA
//#define DEBUG_RENAME

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/// Available show foot print options
StringArray Subscriber::solverIterOptions;

const std::string
Subscriber::SOLVER_ITER_OPTION_TEXT[SolverIterOptionCount] =
{
   "All", "Current", "None",
};

const std::string
Subscriber::PARAMETER_TEXT[SubscriberParamCount - GmatBaseParamCount] =
{
   "SolverIterations",
   "TargetStatus",
};

const Gmat::ParameterType
Subscriber::PARAMETER_TYPE[SubscriberParamCount - GmatBaseParamCount] =
{
   Gmat::ENUMERATION_TYPE,         // "SolverIterations"
   Gmat::ON_OFF_TYPE,              // "TargetStatus"
};


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
// Subscriber(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
Subscriber::Subscriber(const std::string &typeStr, const std::string &nomme) :
   GmatBase (Gmat::SUBSCRIBER, typeStr, nomme),
   data(NULL),
   next(NULL),
   theInternalCoordSystem(NULL),
   theDataCoordSystem(NULL),
   theDataMJ2000EqOrigin(NULL),
   theSolarSystem(NULL),
   currentProvider(NULL),
   active(true),
   isManeuvering(false),
   isEndOfReceive(false),
   isEndOfDataBlock(false),
   isEndOfRun(false),
   isInitialized(false),
   isFinalized(false),
   isDataOn(true),
   isDataStateChanged(false),
   runstate(Gmat::IDLE),
   currProviderId(0)
{
   objectTypes.push_back(Gmat::SUBSCRIBER);
   objectTypeNames.push_back("Subscriber");
   
   mSolverIterations = "Current";
   mSolverIterOption = SI_CURRENT;
   
   wrappersCopied = false;
   
   solverIterOptions.clear();
   for (UnsignedInt i = 0; i < SolverIterOptionCount; i++)
      solverIterOptions.push_back(SOLVER_ITER_OPTION_TEXT[i]);
}


//------------------------------------------------------------------------------
// Subscriber(const Subscriber &copy)
//------------------------------------------------------------------------------
Subscriber::Subscriber(const Subscriber &copy) :
   GmatBase(copy),
   data(NULL),
   next(NULL),
   theInternalCoordSystem(copy.theInternalCoordSystem),
   theDataCoordSystem(copy.theDataCoordSystem),
   theDataMJ2000EqOrigin(copy.theDataMJ2000EqOrigin),
   theSolarSystem(copy.theSolarSystem),
   currentProvider(NULL),
   active(copy.active),
   isManeuvering(copy.isManeuvering),
   isEndOfReceive(copy.isEndOfReceive),
   isEndOfRun(copy.isEndOfRun),
   isInitialized(copy.isInitialized),
   isFinalized(copy.isFinalized),
   isDataOn(copy.isDataOn),
   isDataStateChanged(copy.isDataStateChanged),
   runstate(copy.runstate),
   currProviderId(copy.currProviderId),
   wrapperObjectNames(copy.wrapperObjectNames)
{
   mSolverIterations = copy.mSolverIterations;
   mSolverIterOption = copy.mSolverIterOption;
   wrappersCopied = true;
   
#ifdef __ENABLE_CLONING_WRAPPERS__
   // Create new wrappers by cloning (LOJ: 2009.03.10)
   CloneWrappers(depParamWrappers, copy.depParamWrappers);
   CloneWrappers(paramWrappers, copy.paramWrappers);   
#else
   // Copy wrappers
   depParamWrappers = copy.depParamWrappers;
   paramWrappers = copy.paramWrappers;
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Subscriber(copy) copied wrappers\n");
   WriteWrappers();
   #endif
#endif
}


//------------------------------------------------------------------------------
// Subscriber& Subscriber::operator=(const Subscriber& rhs)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
Subscriber& Subscriber::operator=(const Subscriber& rhs)
{
   if (this == &rhs)
      return *this;
   
   GmatBase::operator=(rhs);
   
   data = rhs.data;
   next = rhs.next;
   
   theInternalCoordSystem = rhs.theInternalCoordSystem;
   theDataCoordSystem = rhs.theDataCoordSystem;
   theDataMJ2000EqOrigin = rhs.theDataMJ2000EqOrigin;
   theSolarSystem = rhs.theSolarSystem;
   currentProvider = NULL;
   
   active = rhs.active;
   active = rhs.active;
   isManeuvering = rhs.isManeuvering;
   isEndOfRun = rhs.isEndOfRun;
   isInitialized = rhs.isInitialized;
   isFinalized = rhs.isFinalized;
   isDataOn = rhs.isDataOn;
   isDataStateChanged = rhs.isDataStateChanged;
   runstate = rhs.runstate;
   currProviderId = rhs.currProviderId;
   
   wrapperObjectNames = rhs.wrapperObjectNames;
   mSolverIterations = rhs.mSolverIterations;
   mSolverIterOption = rhs.mSolverIterOption;
   wrappersCopied = true;
   
#ifdef __ENABLE_CLONING_WRAPPERS__
   // Clear old wrappers
   ClearWrappers();
   // Create new wrappers by cloning (LOJ: 2009.03.10)
   CloneWrappers(depParamWrappers, rhs.depParamWrappers);
   CloneWrappers(paramWrappers, rhs.paramWrappers);
#else
   // Copy wrappers
   depParamWrappers = rhs.depParamWrappers;
   paramWrappers = rhs.paramWrappers;
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Subscriber(=) copied wrappers\n");
   WriteWrappers();
   #endif
#endif
   
   return *this;
}


//------------------------------------------------------------------------------
// ~Subscriber()
//------------------------------------------------------------------------------
Subscriber::~Subscriber()
{
#ifdef __ENABLE_CLONING_WRAPPERS__
   ClearWrappers();
#else
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("~Subscriber() <%p>'%s' entered, wrappersCopied = %d\n", this, GetName().c_str(),
       wrappersCopied);
   #endif
   
   // Since we just copies wrappers, we can only delete if it is not a cloned
   if (!wrappersCopied)
   {
      ClearWrappers();
   }
   else
   {
      //@todo We should delete wrappers here
      //Func_BallisticMassParamTest.script leaves memory trace
      //If we clear wrappers, APT_AttitudeTest.script crashes
      //ClearWrappers();
   }
#endif
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Subscriber::Initialize()
{
   isEndOfReceive = false;
   isEndOfDataBlock = false;
   isEndOfRun = false;
   isInitialized = false;
   isFinalized = false;
   isDataOn = true;
   isDataStateChanged = false;
   return true;
}


//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
bool Subscriber::IsInitialized()
{
   return isInitialized;
}


//------------------------------------------------------------------------------
// bool ReceiveData(const char *datastream)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const char *datastream)
{
   if (!active)        // Not currently processing data
      return true;
   
   data = datastream;
   return true;
}


//------------------------------------------------------------------------------
// bool ReceiveData(const char *datastream,  const int len)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const char *datastream,  const int len)
{
   #ifdef DEBUG_RECEIVE_DATA
   MessageInterface::ShowMessage
      ("Subscriber::ReceiveData(char*, int) <%p>'%s' entered, active=%d, len=%d\n"
       "data='%s'\n", this, GetName().c_str(), active, len, datastream);
   #endif
   
   if (!active)        // Not currently processing data
   {
      #ifdef DEBUG_RECEIVE_DATA
      MessageInterface::ShowMessage
         ("Subscriber::ReceiveData() '%s' is not active, so just returning true\n",
          GetName().c_str());
      #endif
      return true;
   }
   
   data = datastream;
   if (!Distribute(len))
   {
      data = NULL;
      #ifdef DEBUG_RECEIVE_DATA
      MessageInterface::ShowMessage
         ("Subscriber::ReceiveData() '%s' failed to distribute, so just returning false\n",
          GetName().c_str());
      #endif
      return false;
   }
   
   data = NULL;
   
   #ifdef DEBUG_RECEIVE_DATA
   MessageInterface::ShowMessage
      ("Subscriber::ReceiveData() '%s' was successful, returning true\n",
       GetName().c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ReceiveData(const double *datastream, const int len)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const double *datastream, const int len)
{
   #ifdef DEBUG_RECEIVE_DATA
   MessageInterface::ShowMessage
      ("Subscriber::ReceiveData(double*) <%p>'%s' entered, active=%d, len=%d\n",
       this, GetName().c_str(), active, len);
   if (len > 0)
      MessageInterface::ShowMessage("   data[0]=%f\n", datastream[0]);
   #endif
   
   if (!active)        // Not currently processing data
   {
      #ifdef DEBUG_RECEIVE_DATA
      MessageInterface::ShowMessage
         ("Subscriber::ReceiveData() '%s' is not active, so just returning true\n",
          GetName().c_str());
      #endif
      return true;
   }
   
   if (len == 0)
      return true;
   
   if (!Distribute(datastream, len))
   {
      return false;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool FlushData(bool endOfDataBlock = true)
//------------------------------------------------------------------------------
bool Subscriber::FlushData(bool endOfDataBlock)
{
   isEndOfDataBlock = endOfDataBlock;
   isEndOfReceive = true;
   Distribute(0);
   Distribute(NULL, 0);
   isEndOfReceive = false;
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetEndOfRun()
//------------------------------------------------------------------------------
bool Subscriber::SetEndOfRun()
{
   isEndOfReceive = true;
   isEndOfDataBlock = true;
   isEndOfRun = true;
   Distribute(0);
   Distribute(NULL, 0);
   isEndOfReceive = false;
   isEndOfDataBlock = false;
   isEndOfRun = false;
   return true;
}


//------------------------------------------------------------------------------
// void SetRunState(Gmat::RunState rs)
//------------------------------------------------------------------------------
/**
 * Sets the run state to the input value.
 * 
 * @param rs The new run state.
 */
//------------------------------------------------------------------------------
void Subscriber::SetRunState(Gmat::RunState rs)
{
   runstate = rs;
}


//------------------------------------------------------------------------------
// void SetManeuvering(GmatBase *originator, bool flag, Real epoch,
//                     const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft maneuvering flag.
 * 
 * @param originator  The maneuver command pointer who is maneuvering
 * @param flag  Set to true if maneuvering
 * @param epoch  Epoch of maneuver
 * @param satName  Name of the maneuvering spacecraft
 * @param desc  Description of maneuver (e.g. impulsive or finite)
 */
//------------------------------------------------------------------------------
void Subscriber::SetManeuvering(GmatBase *originator, bool flag, Real epoch,
                                const std::string &satName,
                                const std::string &desc)
{
   static StringArray satNames;
   satNames.clear();
   isManeuvering = flag;
   satNames.push_back(satName);
   HandleManeuvering(originator, flag, epoch, satNames, desc);
}


//------------------------------------------------------------------------------
// void SetManeuvering(GmatBase *originator, bool flag, Real epoch,
//                     const StringArray &satNames, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft maneuvering flag.
 * 
 * @param originator  The maneuver command pointer who is maneuvering
 * @param flag Set to true if maneuvering
 * @param epoch Epoch of maneuver
 * @param satNames Names of the maneuvering spacecraft
 * @param desc Description of maneuver (e.g. impulsive or finite)
 */
//------------------------------------------------------------------------------
void Subscriber::SetManeuvering(GmatBase *originator, bool flag, Real epoch,
                                const StringArray &satNames,
                                const std::string &desc)
{
   isManeuvering = flag;
   HandleManeuvering(originator, flag, epoch, satNames, desc);
}


//------------------------------------------------------------------------------
// void SetScPropertyChanged(GmatBase *originator, Real epoch,
//                           const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft property change.
 * 
 * @param originator  The assignment command pointer who is setting
 * @param epoch  Epoch of spacecraft at property change
 * @param satName  Name of the spacecraft
 * @param desc  Description of property change
 */
//------------------------------------------------------------------------------
void Subscriber::SetScPropertyChanged(GmatBase *originator, Real epoch,
                                      const std::string &satName,
                                      const std::string &desc)
{
   HandleScPropertyChange(originator, epoch, satName, desc);
}


//------------------------------------------------------------------------------
// Subscriber* Next()
//------------------------------------------------------------------------------
Subscriber* Subscriber::Next()
{
    return next;
}


//------------------------------------------------------------------------------
// bool Add(Subscriber *s)
//------------------------------------------------------------------------------
bool Subscriber::Add(Subscriber *s)
{
   if (next)
      return next->Add(s);

   next = s;
   return true;
}


//------------------------------------------------------------------------------
// bool Remove(Subscriber *s, const bool del)
//------------------------------------------------------------------------------
bool Subscriber::Remove(Subscriber *s, const bool del)
{
   Subscriber *temp = next;

   if (next == s)
   {
      next = next->Next();
      if (del)
         delete temp;
      return true;
   }
   return false;
}


//------------------------------------------------------------------------------
// void Activate(bool state)
//------------------------------------------------------------------------------
void Subscriber::Activate(bool state)
{
   #ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage
      ("Subscriber::Activate() entered, state=%d, active=%d, isDataOn=%d, "
       "isDataStateChanged=%d, isInitialized=%d\n", state, active, isDataOn,
       isDataStateChanged, isInitialized);
   #endif
   
   isDataStateChanged = false;
   if (active != state)
   {
      isDataStateChanged = true;
      active = state;
      isDataOn = state;
   }
   
   #ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage
      ("Subscriber::Activate() leaving, state=%d, active=%d, isDataOn=%d, "
       "isDataStateChanged=%d, isInitialized=%d\n", state, active, isDataOn,
       isDataStateChanged, isInitialized);
   #endif
   
}


//------------------------------------------------------------------------------
// bool IsActive()
//------------------------------------------------------------------------------
bool Subscriber::IsActive()
{
   return active;
}


//------------------------------------------------------------------------------
// void SetProviderId(Integer id)
//------------------------------------------------------------------------------
void Subscriber::SetProviderId(Integer id)
{
   #ifdef DEBUG_SUBSCRIBER_PROVIDER
   MessageInterface::ShowMessage
      ("Subscriber::SetProviderId() <%s> entered, id=%d\n", GetName().c_str(), id);
   #endif
   
   currProviderId = id;
}


//------------------------------------------------------------------------------
// void SetProviderId(Integer id)
//------------------------------------------------------------------------------
Integer Subscriber::GetProviderId()
{
   return currProviderId;
}

//------------------------------------------------------------------------------
// virtual void SetProvider(GmatBase *provider);
//------------------------------------------------------------------------------
void Subscriber::SetProvider(GmatBase *provider)
{
   currentProvider = provider;
}

//------------------------------------------------------------------------------
// void SetDataLabels(const StringArray& elements)
//------------------------------------------------------------------------------
void Subscriber::SetDataLabels(const StringArray& elements)
{
   #ifdef DEBUG_SUBSCRIBER_SET_LABELS
   MessageInterface::ShowMessage
      ("==> Subscriber::SetDataLabels() Using new Publisher code\n");
   #endif
   
   // Publisher new code always sets current labels
   if (theDataLabels.empty())
      theDataLabels.push_back(elements);
   else
      theDataLabels[0] = elements;
   
   #ifdef DEBUG_SUBSCRIBER_DATA
   MessageInterface::ShowMessage
      ("Subscriber::SetDataLabels() <%s> leaving, theDataLabels.size()=%d, "
       "first label is '%s'\n", GetName().c_str(), theDataLabels.size(),
       elements[0].c_str());
   #endif
}


//------------------------------------------------------------------------------
// void ClearDataLabels()
//------------------------------------------------------------------------------
void Subscriber::ClearDataLabels()
{
   #ifdef DEBUG_SUBSCRIBER_DATA
   MessageInterface::ShowMessage("Subscriber::ClearDataLabels() entered\n");
   #endif
   
   theDataLabels.clear();
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Subscriber::SetInternalCoordSystem(CoordinateSystem *cs)
{
   theInternalCoordSystem = cs;

   // Set data coordinate system to internal coordinate system
   // Internal coordinate system is MJ2000Eq of j2000body as origin.
   // Current j2000body of current internal coordinate system is Earth.
   if (theDataCoordSystem == NULL)
      theDataCoordSystem = cs;
}


//------------------------------------------------------------------------------
// virtual void SetDataCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Subscriber::SetDataCoordSystem(CoordinateSystem *cs)
{
   //#ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage
      ("Subscriber::SetDataCoordSystem()<%s> set to %s<%p>\n",
       instanceName.c_str(), cs->GetName().c_str(), cs);
   //#endif
   
   theDataCoordSystem = cs;
}


//------------------------------------------------------------------------------
// virtual void SetDataMJ2000EqOrigin(CelestialBody *cb)
//------------------------------------------------------------------------------
void Subscriber::SetDataMJ2000EqOrigin(CelestialBody *cb)
{
   theDataMJ2000EqOrigin = cb;
}


//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void Subscriber::SetSolarSystem(SolarSystem *ss)
{
   theSolarSystem = ss;
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray()
//------------------------------------------------------------------------------
const StringArray& Subscriber::GetWrapperObjectNameArray()
{
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper* toWrapper, const std::string &name)
//------------------------------------------------------------------------------
bool Subscriber::SetElementWrapper(ElementWrapper* toWrapper,
                                   const std::string &name)
{
   ElementWrapper *ew;
   
   if (toWrapper == NULL)
      return false;
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Subscriber::SetElementWrapper() <%p>'%s' entered, toWrapper=<%p>, name='%s'\n",
       this, GetName().c_str(), toWrapper, name.c_str());
   #endif
   
   Integer sz = wrapperObjectNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (wrapperObjectNames.at(i) == name)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\", wrapper=%p\n", name.c_str(), paramWrappers.at(i));
         #endif
         
         if (paramWrappers.at(i) != NULL)
         {
            ew = paramWrappers.at(i);
            paramWrappers.at(i) = toWrapper;
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (ew, ew->GetDescription(), "Subscriber::SetElementWrapper",
                "deleting old wrapper");
            #endif
            delete ew;
         }
         else
         {
            paramWrappers.at(i) = toWrapper;
         }
         
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage
            ("   Set wrapper name \"%s\" to wrapper=%p\n", name.c_str(), paramWrappers.at(i));
         #endif
         
         return true;
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
/*
 * Deletes and sets all wrapper pointers to NULL but leaves size unchanged.
 */
//------------------------------------------------------------------------------
void Subscriber::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Subscriber::ClearWrappers() <%p>'%s' entered\n", this, GetName().c_str());
   WriteWrappers();
   #endif
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < depParamWrappers.size(); ++i)
   {
      wrapper = depParamWrappers[i];
      
      // check if wrapper is in the paramWrappers
      // if wrapper found in paramWrappers, delete in the paramWrappers
      if (wrapper != NULL &&
          find(paramWrappers.begin(), paramWrappers.end(), wrapper) ==
          paramWrappers.end())
      {
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage("   deleting depParamWrapper = <%p>\n", wrapper);
         #endif
         
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (wrapper, wrapper->GetDescription(), "Subscriber::ClearWrappers()",
             "deleting old dep wrapper");
         #endif
         delete wrapper;
         wrapper = NULL;
      }
      
      depParamWrappers[i] = NULL;
   }
   
   for (UnsignedInt i = 0; i < paramWrappers.size(); ++i)
   {
      wrapper = paramWrappers[i];
      
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   deleting paramWrapper = <%p>\n", wrapper);
      #endif
      
      if (wrapper != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (wrapper, wrapper->GetDescription(), "Subscriber::ClearWrappers()",
             "deleting old wrapper");
         #endif
         delete wrapper;
         wrapper = NULL;
      }
      
      paramWrappers[i] = NULL;
   }
   
   depParamWrappers.clear();
   paramWrappers.clear();
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Subscriber::ClearWrappers() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//---------------------------------------------------------------------------
// virtual bool RenameRefObject(const Gmat::ObjectType type,
//                              const std::string &oldName,
//                              const std::string &newName)
//---------------------------------------------------------------------------
bool Subscriber::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Subscriber::RenameRefObject() type=%d, oldName=<%s>, newName=<%s>\n",
       type, oldName.c_str(), newName.c_str());
   #endif
   
   // go through wrapper object names
   Integer sz = wrapperObjectNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      std::string name = wrapperObjectNames[i];
      
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   old name=<%s>\n", name.c_str());
      #endif
      
      if (name == oldName)
      {
         wrapperObjectNames[i] = newName;
      }
      else if (name.find(oldName) != name.npos)
      {
         name = GmatStringUtil::Replace(name, oldName, newName);
         wrapperObjectNames[i] = name;
      }
   }
   
   #ifdef DEBUG_RENAME
   for (Integer i = 0; i < sz; i++)
      MessageInterface::ShowMessage
         ("   new name=<%s>\n", wrapperObjectNames[i].c_str());
   #endif
   
   // now go through wrappers
   sz = paramWrappers.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (paramWrappers[i] == NULL)
         throw SubscriberException
            ("Subscriber::SetWrapperReference() \"" + GetName() +
             "\" failed to set reference for object named \"" + oldName +
             ".\" The wrapper is NULL.\n");
      
      std::string desc = paramWrappers[i]->GetDescription();
      
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   old desc=<%s>\n", desc.c_str());
      #endif
      
      if (desc == oldName)
      {
         paramWrappers[i]->SetDescription(newName);
      }
      else if (desc.find(oldName) != desc.npos)
      {
         desc = GmatStringUtil::Replace(desc, oldName, newName);
         paramWrappers[i]->SetDescription(desc);         
      }
   }
   
   #ifdef DEBUG_RENAME
   for (Integer i = 0; i < sz; i++)
      MessageInterface::ShowMessage
         ("   new desc=<%s>\n", paramWrappers[i]->GetDescription().c_str());
   MessageInterface::ShowMessage
      ("Subscriber::RenameRefObject() returning true\n");
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Subscriber::IsParameterReadOnly(const Integer id) const
{
   if (id == TARGET_STATUS)
      return true;
   
   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Subscriber::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SubscriberParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Subscriber::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<SubscriberParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Subscriber::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SubscriberParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Subscriber::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Subscriber::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case SOLVER_ITERATIONS:
      return mSolverIterations;
   default:
      return GmatBase::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Subscriber::GetStringParameter(const std::string &label) const
{
   #ifdef DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Subscriber::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::SetStringParameter() id = %d, value = %s \n", id,
       value.c_str());
   #endif
   
   switch (id)
   {
   case SOLVER_ITERATIONS:
      {
         bool itemFound = false;
         int index = -1;
         for (int i=0; i<SolverIterOptionCount; i++)
         {
            if (value == SOLVER_ITER_OPTION_TEXT[i])
            {
               itemFound = true;
               index = i;
               break;
            }
         }
         
         if (itemFound)
         {
            mSolverIterations = value;
            mSolverIterOption = (SolverIterOption)index;
            return true;
         }
         else
         {
            SubscriberException se;
            se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                          PARAMETER_TEXT[id - GmatBaseParamCount].c_str(),
                          "All, Current, None");
            
            if (value == "On" || value == "Off")
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** %s.\n", se.GetFullMessage().c_str());
               
               if (value == "On")
               {
                  mSolverIterations = "All";
                  mSolverIterOption = SI_ALL;
                  MessageInterface::ShowMessage
                     ("The value \"On\" has automatically switched to \"All\" but "
                      "this will cause an error in a future build.\n\n");
               }
               else
               {
                  mSolverIterations = "None";
                  mSolverIterOption = SI_NONE;
                  MessageInterface::ShowMessage
                     ("The value \"Off\" has automatically switched to \"None\" but "
                      "this will cause an error in a future build.\n\n");
               }
               
               return true;
            }
            else
            {
               throw se;
            }
         }
      }
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool Subscriber::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
   #ifdef DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::SetStringParameter() label = %s, value = %s \n",
       label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Subscriber::SetStringParameter(const Integer id, const std::string &value,
                                    const Integer index)
{
   #ifdef DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::SetStringParameter() id = %d, value = %s, index = %d\n",
       id, value.c_str(), index);
   #endif
   
   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Subscriber::SetStringParameter(const std::string &label,
                                    const std::string &value,
                                    const Integer index)
{
   #ifdef DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::SetStringParameter() label = %s, value = %s, index = %d\n",
       label.c_str(), value.c_str(), index);
   #endif
   
   return SetStringParameter(GetParameterID(label), value, index);
}

//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string Subscriber::GetOnOffParameter(const Integer id) const
{
   switch (id)
   {
   case TARGET_STATUS:
      return mSolverIterations;
   default:
      return GmatBase::GetOnOffParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string Subscriber::GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Subscriber::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool Subscriber::SetOnOffParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::SetOnOffParameter() id = %d, value = %s \n", id,
       value.c_str());
   #endif
   
   // Write only one message per session
   static bool writeTargetStatus = true;
   
   switch (id)
   {
   case TARGET_STATUS:
      if (writeTargetStatus)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"TargetStatus\" is deprecated and will be "
             "removed from a future build; please use \"SolverIterations\" "
             "instead.\n");
         writeTargetStatus = false;
      }
      
      if (value == "On")
      {
         mSolverIterations = "All";
         mSolverIterOption = SI_ALL;
      }
      else
      {
         mSolverIterations = "None";
         mSolverIterOption = SI_NONE;
      }
      
      return true;
      
   default:
      return GmatBase::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool Subscriber::SetOnOffParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const std::string* GetSolverIterOptionList()
//------------------------------------------------------------------------------
const std::string* Subscriber::GetSolverIterOptionList()
{
   return SOLVER_ITER_OPTION_TEXT;
}


//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool CloneWrappers(WrapperArray &toWrappers, const WrapperArray &fromWrappers);
//------------------------------------------------------------------------------
bool Subscriber::CloneWrappers(WrapperArray &toWrappers,
                               const WrapperArray &fromWrappers)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Subscriber::CloneWrappers() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   for (UnsignedInt i=0; i<fromWrappers.size(); i++)
   {
      if (fromWrappers[i] != NULL)
      {
         ElementWrapper *ew = fromWrappers[i]->Clone();
         toWrappers.push_back(ew);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (ew, ew->GetDescription(), "Subscriber::CloneWrappers()",
             "ew = fromWrappers[i]->Clone()");
         #endif         
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetWrapperReference(GmatBase *obj, const std::string &name)
//------------------------------------------------------------------------------
bool Subscriber::SetWrapperReference(GmatBase *obj, const std::string &name)
{
   Integer sz = paramWrappers.size();
   std::string refname, desc;
   Gmat::WrapperDataType wrapperType;
   bool nameFound = false;
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Subscriber::SetWrapperReference() obj=<%p>'%s', name='%s', size=%d\n",
       obj, obj->GetName().c_str(), name.c_str(), sz);
   #endif
   
   for (Integer i = 0; i < sz; i++)
   {
      if (paramWrappers[i] == NULL)
         throw SubscriberException
            ("Subscriber::SetWrapperReference() \"" + GetName() +
             "\" failed to set reference for object named \"" + name +
             ".\" The wrapper is NULL.\n");
      
      refname = paramWrappers[i]->GetDescription();
      if (paramWrappers[i]->GetWrapperType() == Gmat::ARRAY_ELEMENT_WT)
         refname = refname.substr(0, refname.find('('));
      if (refname == name)
      {
         nameFound = true;
         break;
      }
   }
   
   if (!nameFound)
      throw SubscriberException
         ("Subscriber::SetWrapperReference() \"" + GetName() +
          "\" failed to find object named \"" +  name + "\"\n");
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage("   setting ref object of wrappers\n");
   #endif
   
   // set ref object of wrappers
   for (Integer i = 0; i < sz; i++)
   {
      desc = paramWrappers[i]->GetDescription();
      wrapperType = paramWrappers[i]->GetWrapperType();
      
      #ifdef DEBUG_WRAPPER_CODE   
      MessageInterface::ShowMessage
         ("   paramWrappers[%d]=\"%s\", wrapperType=%d\n", i, desc.c_str(),
          wrapperType);
      #endif
      
      refname = desc;
      
      switch (wrapperType)
      {
      case Gmat::OBJECT_PROPERTY_WT:
         if (refname == name)
         {
            Parameter *param = (Parameter*)obj;
            StringArray onames = paramWrappers[i]->GetRefObjectNames();
            for (UnsignedInt j=0; j<onames.size(); j++)
            {
               paramWrappers[i]->
                  SetRefObject(param->GetRefObject(param->GetOwnerType(), onames[j]));
            }
            return true;
         }
      case Gmat::ARRAY_ELEMENT_WT:
         // for array element, we need to go through all array elements set
         // ref object, so break insted of return
         refname = refname.substr(0, refname.find('('));
         if (refname == name)
            paramWrappers[i]->SetRefObject(obj);
         break;
      default:
         // Others, such as VARIABLE, PARAMETER_OBJECT
         if (refname == name)
         {
            #ifdef DEBUG_WRAPPER_CODE   
            MessageInterface::ShowMessage
               ("   Found wrapper name \"%s\" in SetWrapperReference\n", name.c_str());
            #endif
            
            paramWrappers[i]->SetRefObject(obj);
            return true;
         }
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Subscriber::SetWrapperReference() ArrayElement set. Leaving\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void WriteWrappers()
//------------------------------------------------------------------------------
void Subscriber::WriteWrappers()
{
   MessageInterface::ShowMessage
      ("Subscriber::WriteWrappers() <%p>'%s' has %d depParamWrappers and %d "
       "paramWrappers\n", this, GetName().c_str(), depParamWrappers.size(),
       paramWrappers.size());
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < depParamWrappers.size(); ++i)
   {
      wrapper = depParamWrappers[i];
      MessageInterface::ShowMessage
         ("   depPaWrapper = <%p> '%s'\n", wrapper, wrapper->GetDescription().c_str());
   }
   
   for (UnsignedInt i = 0; i < paramWrappers.size(); ++i)
   {
      wrapper = paramWrappers[i];
      MessageInterface::ShowMessage
         ("   paramWrapper = <%p> '%s'\n", wrapper, wrapper->GetDescription().c_str());
   }
}


//------------------------------------------------------------------------------
// Integer FindIndexOfElement(StringArray &labelArray, const std::string &label)
//------------------------------------------------------------------------------
Integer Subscriber::FindIndexOfElement(StringArray &labelArray,
                                       const std::string &label)
{
   std::vector<std::string>::iterator pos;
   pos = find(labelArray.begin(), labelArray.end(),  label);
   if (pos == labelArray.end())
      return -1;
   else
      return distance(labelArray.begin(), pos);
}



//------------------------------------------------------------------------------
// bool Distribute(Integer len)
//------------------------------------------------------------------------------
bool Subscriber::Distribute(Integer len)
{
   return true;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool Subscriber::Distribute(const Real *dat, Integer len)
{
   return true;
}


//------------------------------------------------------------------------------
// virtual void HandleManeuvering(GmatBase *originator, bool maneuvering, Real epoch,
//                                const StringArray &satNames,
//                                const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Handles maneuvering on or off.
 * 
 * @param originator  The maneuver command pointer who is maneuvering
 * @param maneuvering  Set to true if maneuvering
 * @param epoch  Epoch of maneuver on or off
 * @param satNames  Names of the maneuvering spacecraft
 * @param desc  Description of maneuver (e.g. impulsive or finite)
 */
//------------------------------------------------------------------------------
void Subscriber::HandleManeuvering(GmatBase *originator, bool maneuvering,
                                   Real epoch, const StringArray &satNames,
                                   const std::string &desc)
{
   // do nothing here
}


//------------------------------------------------------------------------------
// void HandleScPropertyChange(GmatBase *originator, Real epoch,
//                             const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Handles spacecraft property change.
 * 
 * @param originator  The assignment command pointer who is setting
 * @param epoch  Epoch of spacecraft at property change
 * @param satName  Name of the spacecraft
 * @param desc  Description of property change
 */
//------------------------------------------------------------------------------
void Subscriber::HandleScPropertyChange(GmatBase *originator, Real epoch,
                                        const std::string &satName,
                                        const std::string &desc)
{
   // do nothing here
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
const StringArray& Subscriber::GetPropertyEnumStrings(const Integer id) const
{
   if (id == SOLVER_ITERATIONS)
      return solverIterOptions;
   
   return GmatBase::GetPropertyEnumStrings(id);
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const std::string &label) const
//---------------------------------------------------------------------------
const StringArray& Subscriber::GetPropertyEnumStrings(const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}

