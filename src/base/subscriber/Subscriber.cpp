//$Header$
//------------------------------------------------------------------------------
//                                  Subscriber
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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
// ***  Copyright Thinking Systems 2003                                   ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                             Source code not yet transferred to GSFC
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
#include "MessageInterface.hpp"

//#define DEBUG_WRAPPER_CODE 1
//#define DEBUG_SUBSCRIBER_PARAM 1

//---------------------------------
// static data
//---------------------------------

const std::string
Subscriber::SOLVER_ITER_OPTION_TEXT[SolverIterOptionCount] =
{
   "All", "Last", "None",
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
   Gmat::STRING_TYPE,            // "SolverIterations"
   Gmat::ON_OFF_TYPE,            // "TargetStatus"
};


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
// Subscriber(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
Subscriber::Subscriber(std::string typeStr, std::string nomme) :
   GmatBase (Gmat::SUBSCRIBER, typeStr, nomme),
   data (NULL),
   next (NULL),
   active (true),
   isEndOfReceive(false),
   isEndOfRun(false),
   runstate(Gmat::IDLE),
   currentProvider(0)
{
   objectTypes.push_back(Gmat::SUBSCRIBER);
   objectTypeNames.push_back("Subscriber");
   
   mSolverIterations = "None";
}


//------------------------------------------------------------------------------
// Subscriber(const Subscriber &copy)
//------------------------------------------------------------------------------
Subscriber::Subscriber(const Subscriber &copy) :
   GmatBase (copy),
   data (NULL),
   next (NULL),
   internalCoordSystem (NULL),
   active (copy.active),
   isEndOfReceive(copy.isEndOfReceive),
   isEndOfRun(copy.isEndOfRun),
   runstate(copy.runstate),
   currentProvider(copy.currentProvider),
   wrapperObjectNames(copy.wrapperObjectNames),
   paramWrappers(copy.paramWrappers)
{
   mSolverIterations = copy.mSolverIterations;
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
   active = rhs.active;
   isEndOfReceive = rhs.isEndOfReceive;
   isEndOfRun = rhs.isEndOfRun;
   runstate = rhs.runstate;
   currentProvider = rhs.currentProvider;
   internalCoordSystem = NULL;
   wrapperObjectNames = rhs.wrapperObjectNames;
   paramWrappers = rhs.paramWrappers;
   mSolverIterations = rhs.mSolverIterations;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~Subscriber()
//------------------------------------------------------------------------------
Subscriber::~Subscriber()
{
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Subscriber::Initialize()
{
   isEndOfReceive = false;
   isEndOfRun = false;   
   return true;
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
   if (!active)        // Not currently processing data
      return true;

   data = datastream;
   if (!Distribute(len))
   {
      data = NULL;
      return false;
   }

   data = NULL;
   return true;
}


//------------------------------------------------------------------------------
// bool ReceiveData(const double *datastream, const int len)
//------------------------------------------------------------------------------
bool Subscriber::ReceiveData(const double *datastream, const int len)
{
   if (!active)        // Not currently processing data
      return true;

   if (len == 0)
      return true;

   if (!Distribute(datastream, len))
   {
      return false;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool FlushData()
//------------------------------------------------------------------------------
bool Subscriber::FlushData()
{
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
   isEndOfRun = true;
   Distribute(0);
   Distribute(NULL, 0);
   isEndOfReceive = false;
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
// Subscriber* Next(void)
//------------------------------------------------------------------------------
Subscriber* Subscriber::Next(void)
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
   active = state;
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
   currentProvider = id;
}


//------------------------------------------------------------------------------
// void SetProviderId(Integer id)
//------------------------------------------------------------------------------
Integer Subscriber::GetProviderId()
{
   return currentProvider;
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Subscriber::SetInternalCoordSystem(CoordinateSystem *cs)
{
   internalCoordSystem = cs;
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
   MessageInterface::ShowMessage(
      "Subscriber::SetElementWrapper() Setting wrapper \"%s\" size=%d in %s \"%s\"\n",
      name.c_str(), wrapperObjectNames.size(), GetTypeName().c_str(), instanceName.c_str());
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
         
         //paramWrappers.push_back(toWrapper);
         
         if (paramWrappers.at(i) != NULL)
         {
            ew = paramWrappers.at(i);
            paramWrappers.at(i) = toWrapper;
            delete ew;
         }
         else
         {
            paramWrappers.at(i) = toWrapper;
         }
         
         return true;
      }
   }
   
   return false;
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
      ("Subscriber::SetWrapperReference() obj=(%p)%s, name=%s, size=%d\n",
       obj, obj->GetName().c_str(), name.c_str(), sz);
   #endif
   
   for (Integer i = 0; i < sz; i++)
   {
      if (paramWrappers[i] == NULL)
         throw SubscriberException
            ("Subscriber::SetWrapperReference() failed to set reference for "
             "object named \"" + name + ".\"\nThe wrapper is NULL.\n");
      
      refname = paramWrappers[i]->GetDescription();
      if (paramWrappers[i]->GetWrapperType() == Gmat::ARRAY_ELEMENT)
         refname = refname.substr(0, refname.find('('));
      if (refname == name)
      {
         nameFound = true;
         break;
      }
   }
   
   if (!nameFound)
      throw SubscriberException
         ("Subscriber::SetWrapperReference() failed to find object named \"" +  name + "\"\n");
   
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
      case Gmat::OBJECT_PROPERTY:
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
      case Gmat::ARRAY_ELEMENT:
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
// void ClearWrappers()
//------------------------------------------------------------------------------
/*
 * Deletes and sets all wrapper pointers to NULL but leaves size unchanged.
 */
//------------------------------------------------------------------------------
void Subscriber::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Subscriber::ClearWrappers() entered\n");
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
         MessageInterface::ShowMessage
            ("   deleting wrapper=(%p)'%s'\n", wrapper,
             wrapper->GetDescription().c_str());
         #endif
         delete wrapper;
      }
      
      depParamWrappers[i] = NULL;
   }
   
   for (UnsignedInt i = 0; i < paramWrappers.size(); ++i)
   {
      wrapper = paramWrappers[i];
      
      if (wrapper != NULL)
      {
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage
            ("   deleting wrapper=(%p)'%s'\n", wrapper,
             wrapper->GetDescription().c_str());
         #endif
         delete wrapper;
      }
      
      paramWrappers[i] = NULL;
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Subscriber::ClearWrappers() leaving\n");
   #endif
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
   if (id >= GmatBaseParamCount && id < SubscriberParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - GmatBaseParamCount)];
   else
      return GmatBase::GetParameterTypeString(id);
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
   #if DEBUG_SUBSCRIBER_PARAM
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
   #if DEBUG_SUBSCRIBER_PARAM
   MessageInterface::ShowMessage
      ("Subscriber::SetStringParameter() id = %d, value = %s \n", id,
       value.c_str());
   #endif
   
   switch (id)
   {
   case SOLVER_ITERATIONS:
      {
         bool itemFound = false;
         for (int i=0; i<SolverIterOptionCount; i++)
         {
            if (value == SOLVER_ITER_OPTION_TEXT[i])
            {
               itemFound = true;
               break;
            }
         }
         
         if (itemFound)
         {
            mSolverIterations = value;
            return true;
         }
         else
         {
            SubscriberException se;
            se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                          PARAMETER_TEXT[id - GmatBaseParamCount].c_str(),
                          "All, Last, None");
            
            if (value == "On" || value == "Off")
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** %s.\n", se.GetFullMessage().c_str());
               
               if (value == "On")
               {
                  mSolverIterations = "All";
                  MessageInterface::ShowMessage
                     ("The value \"On\" has automatically switched to \"All\" but "
                      "this will cause an error in a future build.\n\n");
               }
               else
               {
                  mSolverIterations = "None";
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
   #if DEBUG_SUBSCRIBER_PARAM
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
   #if DEBUG_SUBSCRIBER_PARAM
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
   #if DEBUG_SUBSCRIBER_PARAM
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
   #if DEBUG_SUBSCRIBER_PARAM
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
         mSolverIterations = "All";
      else
         mSolverIterations = "None";
      
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


//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool Distribute(const double *dat, int len)
//------------------------------------------------------------------------------
bool Subscriber::Distribute(const double *dat, int len)
{
   return true;
}


//------------------------------------------------------------------------------
// const std::string* GetSolverIterOptionList()
//------------------------------------------------------------------------------
const std::string* Subscriber::GetSolverIterOptionList()
{
   return SOLVER_ITER_OPTION_TEXT;
}

