//$Id$
//------------------------------------------------------------------------------
//                                 CallGmatFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallGmatFunction command class
 */
//------------------------------------------------------------------------------
#include "CallGmatFunction.hpp"
#include "BeginFunction.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "FileManager.hpp"         // for GetAllMatlabFunctionPaths()
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_CALL_FUNCTION_PARAM
//#define DEBUG_CALL_FUNCTION_INIT
//#define DEBUG_CALL_FUNCTION_EXEC

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
// CallGmatFunction::CallGmatFunction()
//------------------------------------------------------------------------------
CallGmatFunction::CallGmatFunction() :
   CallFunction("CallGmatFunction")
{
   objectTypeNames.push_back("CallGmatFunction");
}


//------------------------------------------------------------------------------
// ~CallGmatFunction()
//------------------------------------------------------------------------------
CallGmatFunction::~CallGmatFunction()
{
}


//------------------------------------------------------------------------------
// CallGmatFunction(const CallGmatFunction& cf)
//------------------------------------------------------------------------------
CallGmatFunction::CallGmatFunction(const CallGmatFunction& cf) :
   CallFunction (cf)
{
}


//------------------------------------------------------------------------------
// CallGmatFunction& operator=(const CallGmatFunction& cf)
//------------------------------------------------------------------------------
CallGmatFunction& CallGmatFunction::operator=(const CallGmatFunction& cf)
{
   if (this == &cf)
      return *this;
   
   CallFunction::operator=(cf);
   
   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CallGmatFunction.
 *
 * @return clone of the CallGmatFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CallGmatFunction::Clone() const
{
   return (new CallGmatFunction(*this));
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CallGmatFunction::Initialize()
{
   #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("CallGmatFunction::Initialize() this=<%p> entered, command = '%s'\n   "
          "function type is '%s', callingFunction is '%s'\n", this,
          GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
          callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
   #endif
      
   bool rv = CallFunction::Initialize();
   
   // Handle additional initialization for GmatFunctions
   if (isGmatFunction)
   {
      #ifdef DEBUG_GMAT_FUNCTION_INIT
         MessageInterface::ShowMessage
            ("CallGmatFunction::Initialize: Initializing GmatFunction '%s'\n",
             mFunction->GetName().c_str());
      #endif
      fm.SetSolarSystem(solarSys);
      fm.SetTransientForces(forces);
      fm.SetGlobalObjectMap(globalObjectMap);
   }
   
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool CallGmatFunction::Execute()
{
   bool status = false;
   
   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallGmatFunction");
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      ("=== CallGmatFunction::Execute() entered, '%s' Count = %d\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("CallGmatFunction::Execute() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the start");
      #endif
   #endif
      
   if (isGmatFunction)
   {
      #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("   calling FunctionManager::Execute() with callingFunction='%s'\n",
          callingFunction ? (callingFunction->GetFunctionName()).c_str() : "NULL");
      #endif
      status = fm.Execute(callingFunction);
   }
   else
   {
      // The function is not GmatFunction so throw an exception
      throw CommandException
         ("*** INTERNAL ERROR *** in CallGmatFunction. " +
          GetGeneratingString(Gmat::NO_COMMENTS) + " cannot be executed, "
          "the function type should be GmatFunction, but it is " +
          mFunction->GetTypeName() + ".\n");
   }
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   Executed command, about to build command summery\n");
   #endif
   
   BuildCommandSummary(true);
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("CallGmatFunction::Execute() GmatFunction exiting with %d\n", status);
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the end");
      #endif
   #endif
   
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("=== CallGmatFunction::Execute() exiting, '%s' Count = %d, Run Time: %f seconds\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount, (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   
   return status;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void CallGmatFunction::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallGmatFunction::RunComplete() entered for this=<%p> '%s',\n   "
       "FCS %sfinalized\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       fm.IsFinalized() ? "already " : "NOT ");
   #endif
   
   if (!fm.IsFinalized())
   {
      #ifdef DEBUG_RUN_COMPLETE
      MessageInterface::ShowMessage("   calling FunctionManager::Finalize()\n");
      #endif
      fm.Finalize();
   }
   
   CallFunction::RunComplete();
}

