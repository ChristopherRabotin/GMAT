//$Id$
//------------------------------------------------------------------------------
//                                   BuiltinGmatFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.05.02
//
/**
 * Implements BuiltinGmatFunction class.
 */
//------------------------------------------------------------------------------

#include "BuiltinGmatFunction.hpp"
#include "Parameter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_INIT

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//---------------------------------
// static data
//---------------------------------


//------------------------------------------------------------------------------
//  BuiltinGmatFunction(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the BuiltinGmatFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction::BuiltinGmatFunction(const std::string &typeStr, const std::string &name) :
   Function(typeStr, name)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("BuiltinGmatFunction");
}


//------------------------------------------------------------------------------
//  ~BuiltinGmatFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the BuiltinGmatFunction object (destructor).
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction::~BuiltinGmatFunction()
{
}


//------------------------------------------------------------------------------
//  BuiltinGmatFunction(const BuiltinGmatFunction &f)
//------------------------------------------------------------------------------
/**
 * Constructs the BuiltinGmatFunction object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction::BuiltinGmatFunction(const BuiltinGmatFunction &f) :
   Function(f)
{
}


//------------------------------------------------------------------------------
//  BuiltinGmatFunction& operator=(const BuiltinGmatFunction &f)
//------------------------------------------------------------------------------
/**
 * Sets one BuiltinGmatFunction object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
BuiltinGmatFunction& BuiltinGmatFunction::operator=(const BuiltinGmatFunction &f)
{
   if (this == &f)
      return *this;
   
   Function::operator=(f);
   
   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool BuiltinGmatFunction::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      ("BuiltinGmatFunction::Initialize() entered, callCount=%d\n", callCount);
   ShowTrace(callCount, t1, "BuiltinGmatFunction::Initialize() entered");
   #endif
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "BuiltinGmatFunction::Initialize() entered for function '%s'\n   reinitialize=%d, "
       "objectsInitialized=%d\n", functionName.c_str(), reinitialize, objectsInitialized);
   MessageInterface::ShowMessage("   FCS is %s set.\n", (fcs? "correctly" : "NOT"));
   MessageInterface::ShowMessage("   Pointer for FCS is %p\n", fcs);
   MessageInterface::ShowMessage("   First command in fcs is %s\n",
                                 fcs ? (fcs->GetTypeName()).c_str() : "NULL");
   MessageInterface::ShowMessage("   internalCS is %p\n", internalCoordSys);
   #endif
      
   Function::Initialize(objInit);
   
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator->SetFunction(this);
   validator->SetSolarSystem(solarSys);
   std::map<std::string, GmatBase *>::iterator omi;
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("   functionObjectMap.size()  = %d\n   automaticObjectMap.size() = %d\n",
       functionObjectMap.size(), automaticObjectMap.size());
   #endif
   
   // Add clones of objects created in the function to the FOS (LOJ: 2014.12.09)
   for (omi = functionObjectMap.begin(); omi != functionObjectMap.end(); ++omi)
   {
      std::string funcObjName = omi->first;
      GmatBase *funcObj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Add clone to objectStore if funcObj <%p>[%s]'%s' is not already in objectStore\n",
          funcObj, funcObj ? funcObj->GetTypeName().c_str() : "NULL", funcObjName.c_str());
      MessageInterface::ShowMessage
         ("   funcObj->IsGlobal=%d, funcObj->IsLocal=%d\n", funcObj->IsGlobal(),
          funcObj->IsLocal());
      #endif
      
      // if name not found, clone it and add to map (loj: 2008.12.15)
      if (objectStore->find(funcObjName) == objectStore->end())
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   About to clone <%p>[%s]'%s'\n", (omi->second),
             (omi->second)->GetTypeName().c_str(), (omi->second)->GetName().c_str());
         #endif
         GmatBase *funcObjClone = (omi->second)->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (funcObjClone, funcObjName, "BuiltinGmatFunction::Initialize()",
             "funcObj = (omi->second)->Clone()");
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT_MORE
         try
         {
            MessageInterface::ShowMessage
               ("   funcObj(%s)->EvaluateReal() = %f\n", funcObjName.c_str(),
                funcObjClone->GetRealParameter("Value"));
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage("%s\n", e.GetFullMessage().c_str());             
         }
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   ==> Adding funcObj clone <%p>'%s' to objectStore\n", funcObjClone,
             funcObjName.c_str());
         #endif
         funcObjClone->SetIsLocal(true);
         objectStore->insert(std::make_pair(funcObjName, funcObjClone));
      }
      else
      {
         std::map<std::string, GmatBase *>::iterator mapi;
         mapi = objectStore->find(funcObjName);
         if (mapi != objectStore->end())
         {
            GmatBase *mapObj = (mapi->second);
            #ifdef DEBUG_FUNCTION_INIT
            MessageInterface::ShowMessage
               ("   '%s' already exist: <%p>[%s]'%s'\n", funcObjName.c_str(), mapObj,
                mapObj->GetTypeName().c_str(), mapObj->GetName().c_str());
            MessageInterface::ShowMessage
               ("   Now checking if input parameter is redefined to different type\n");
            #endif

            // Check if input parameter is redefined in a function
            if (funcObj->GetTypeName() != mapObj->GetTypeName())
            {
               // Check for return type if object type is Parameter?
               // (fix for GMT-5262 LOJ: 2015.09.04)
               if (funcObj->IsOfType(Gmat::PARAMETER) && mapObj->IsOfType(Gmat::PARAMETER))
               {
                  if (((Parameter*)funcObj)->GetReturnType() != ((Parameter*)mapObj)->GetReturnType())
                  {
                     throw FunctionException
                        ("Redefinition of formal input parameter '" + funcObjName +
                         "' to different type is not allowed in GMAT function '" +
                         functionPath + "'.  It's expected type is '" +
                         mapObj->GetTypeName() + "'.\n");
                  }
               }
            }
         }
      }
   }
   
   // add automatic objects such as sat.X to the FOS (well, actually, clones of them)
   for (omi = automaticObjectMap.begin(); omi != automaticObjectMap.end(); ++omi)
   {
      std::string autoObjName = omi->first;
      GmatBase *autoObj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   Add clone to objectStore if autoObj <%p>[%s]'%s' is not already in objectStore\n",
          autoObj, autoObj ? autoObj->GetTypeName().c_str() : "NULL", autoObjName.c_str());
      MessageInterface::ShowMessage
         ("   autoObj->IsGlobal=%d, autoObj->IsLocal=%d\n", autoObj->IsGlobal(),
          autoObj->IsLocal());
      #endif
      
      // If automatic object owner (i.e Parameter owner) is global, set it global
      GmatBase *owner = NULL;
      if (IsAutomaticObjectGlobal(autoObjName, &owner))
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   autoObj <%p>'%s' is global, so setting isGlobal to true\n", autoObj,
             autoObjName.c_str());
         MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   owner=", owner));
         #endif
         autoObj->SetIsGlobal(true);
         
         // Set IsLocal to false to indicate it is not created inside a function (LOJ: 2015.10.03)
         owner->SetIsLocal(false);
         autoObj->SetIsLocal(false);
      }
      
      // if name not found, clone it and add to map (loj: 2008.12.15)
      std::map<std::string, GmatBase *>::iterator foundIter = objectStore->find(autoObjName);
      if (foundIter == objectStore->end())
      {
         GmatBase *clonedAutoObj = autoObj->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (clonedAutoObj, autoObjName, "BuiltinGmatFunction::Initialize()",
             "clonedAutoObj = (omi->second)->Clone()");
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT_MORE
         try
         {
            MessageInterface::ShowMessage
               ("   clonedAutoObj(%s)->EvaluateReal() = %f\n", autoObjName.c_str(),
                clonedAutoObj->GetRealParameter("Value"));
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage("%s\n", e.GetFullMessage().c_str());             
         }
         #endif
         
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   ==> Adding clonedAutoObj <%p>'%s' to objectStore, isGlobal=%d, "
             "isLocal=%d\n", clonedAutoObj, autoObjName.c_str(), clonedAutoObj->IsGlobal(),
             clonedAutoObj->IsLocal());
         if (owner)
         {
            GmatBase *paramOwner = clonedAutoObj->GetRefObject(owner->GetType(), owner->GetName());
            MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   paramOwner=", paramOwner));
         }
         #endif
         
         // Do not set IsLocal to true since it is cloned above (LOJ: 2015.10.03)
         //clonedAutoObj->SetIsLocal(true);
         objectStore->insert(std::make_pair(autoObjName, clonedAutoObj));
      }
   }
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage("   If object is global, move it to globalObjectStore\n");
   #endif
   
   // If object is global, move it to globalObjectStore
   for (omi = objectStore->begin(); omi != objectStore->end(); ++omi)
   {
      std::string objName = omi->first;
      GmatBase *obj = omi->second;
      
      #ifdef DEBUG_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("   obj<%p>'%s' IsGlobal:%s, IsLocal:%s\n", obj, objName.c_str(),
          obj->IsGlobal() ? "Yes" : "No", obj->IsLocal() ? "Yes" : "No");
      #endif
      // Check if it is global but not a local (means passed or locally created object)
      if (obj->IsGlobal() && !obj->IsLocal())
      {
         if (globalObjectStore->find(objName) == globalObjectStore->end())
         {
            #ifdef DEBUG_FUNCTION_INIT
            MessageInterface::ShowMessage
               ("   ==> obj<%p>'%s' is global and not local, so moving it from "
                "objectStore to globalObjectStore\n", obj, objName.c_str());
            #endif
            globalObjectStore->insert(std::make_pair(objName, obj));
            objectStore->erase(objName);
         }
      }
      else
      {
         #ifdef DEBUG_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("   ==> obj<%p>'%s' is global and local, so it stays in objectStore\n",
             obj, objName.c_str());
         #endif
      }
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool BuiltinGmatFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("BuiltinGmatFunction::SetStringParameter() entered, id=%d, value=%s\n", id, value.c_str());
   #endif

   bool retval = false;
   switch (id)
   {
   case FUNCTION_NAME:
      functionName = value;
      retval = true;
      break;
   default:
      retval = Function::SetStringParameter(id, value);
      break;
   }
   
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("BuiltinGmatFunction::SetStringParameter() returning %d\n", retval);
   #endif
   return retval;
}
