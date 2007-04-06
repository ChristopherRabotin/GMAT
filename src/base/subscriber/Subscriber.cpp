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
   currentProvider(0)
{
   objectTypes.push_back(Gmat::SUBSCRIBER);
   objectTypeNames.push_back("Subscriber");
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
   currentProvider(copy.currentProvider),
   wrapperObjectNames(copy.wrapperObjectNames),
   paramWrappers(copy.paramWrappers)
{
}


//------------------------------------------------------------------------------
// Subscriber& Subscriber::operator=(const Subscriber& sub)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
Subscriber& Subscriber::operator=(const Subscriber& sub)
{
   if (this == &sub)
      return *this;

   GmatBase::operator=(sub);
    
   data = sub.data;
   next = sub.next;
   active = sub.active;
   isEndOfReceive = sub.isEndOfReceive;
   isEndOfRun = sub.isEndOfRun;
   currentProvider = sub.currentProvider;
   internalCoordSystem = NULL;
   wrapperObjectNames = sub.wrapperObjectNames;
   paramWrappers = sub.paramWrappers;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~Subscriber(void)
//------------------------------------------------------------------------------
Subscriber::~Subscriber(void)
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
   
//    depParamWrappers.clear();
//    paramWrappers.clear();
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Subscriber::ClearWrappers() leaving\n");
   #endif
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
