//$Id$
//------------------------------------------------------------------------------
//                                   Function
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
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Function base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------

#include "Function.hpp"
#include "FunctionException.hpp"    // for exception
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "Parameter.hpp"            // for GetOwner()
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_IN_OUT
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_FUNCTION_OBJ
//#define DEBUG_FUNCTION_FINALIZE
//#define DEBUG_AUTO_OBJ
//#define DEBUG_OBJECT_MAP
//#define DEBUG_FIND_OBJECT


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

// Since more testing is needed I just added #define here (loj: 2008.12.24)
//#define __COLLECT_AUTO_OBJECTS__


//---------------------------------
// static data
//---------------------------------
const std::string
Function::PARAMETER_TEXT[FunctionParamCount - GmatBaseParamCount] =
{
   "FunctionPath",
   "FunctionName",
   "Input",
   "Output"
};

const Gmat::ParameterType
Function::PARAMETER_TYPE[FunctionParamCount - GmatBaseParamCount] =
{
   Gmat::FILENAME_TYPE,       // "FunctionPath",
   Gmat::STRING_TYPE,         // "FunctionName",
   Gmat::STRINGARRAY_TYPE,    // "Input",
   Gmat::STRINGARRAY_TYPE,    // "Output"
};


//------------------------------------------------------------------------------
//  Function(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Function object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Function::Function(const std::string &typeStr, const std::string &name) :
   GmatBase           (Gmat::FUNCTION, typeStr, name),
   callDescription    (""),
   functionPath       (""),
   functionName       (""),
   objectStore        (NULL),
   globalObjectStore  (NULL),
   solarSys           (NULL),
   internalCoordSys   (NULL),
   forces             (NULL),
   fcs                (NULL),
   fcsInitialized     (false),
   fcsFinalized       (false),
   validator          (NULL),
   wasFunctionBuilt   (false),
   isFunctionIOSet    (false),
   scriptErrorFound   (false),
   objectsInitialized (false)
{
   objectTypes.push_back(Gmat::FUNCTION);
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("Function");
   parameterCount = FunctionParamCount;
}


//------------------------------------------------------------------------------
//  ~Function(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Function object (destructor).
 */
//------------------------------------------------------------------------------
Function::~Function()
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Function::~Function() <%p>[%s]'%s' entered\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
   
   // Delete only output wrappers, input wrappers are set by FunctionManager,
   // so they are deleted there.
   // crashes on nested function if delete output wrappers here
   //ClearInOutArgMaps(false, true);
   
   // Clear original objects
   ClearAutomaticObjects();
   ClearFunctionObjects();
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Function::~Function() <%p>[%s]'%s' leaving\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  Function(const Function &f)
//------------------------------------------------------------------------------
/**
 * Constructs the Function object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
Function::Function(const Function &f) :
   GmatBase           (f),
   callDescription    (f.callDescription),
   functionPath       (f.functionPath),
   functionName       (f.functionName),
   inputNames         (f.inputNames),
   outputNames        (f.outputNames),
   inputArgMap        (f.inputArgMap), // do I want to do this?
   outputArgMap       (f.outputArgMap), // do I want to do this?
   outputWrapperTypes (f.outputWrapperTypes),
   outputRowCounts    (f.outputRowCounts),
   outputColCounts    (f.outputColCounts),
   objectStore        (NULL),
   globalObjectStore  (NULL),
   solarSys           (NULL),
   internalCoordSys   (NULL),
   forces             (NULL),
   fcs                (NULL),
   fcsInitialized     (f.fcsInitialized),
   fcsFinalized       (f.fcsFinalized),
   functionObjectMap  (f.functionObjectMap), // Do I want to do this?
   validator          (f.validator),
   wasFunctionBuilt   (f.wasFunctionBuilt),
   isFunctionIOSet    (f.isFunctionIOSet),
   scriptErrorFound   (false),
   objectsInitialized (false)
{
   parameterCount = FunctionParamCount;
}


//------------------------------------------------------------------------------
//  Function& operator=(const Function &f)
//------------------------------------------------------------------------------
/**
 * Sets one Function object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Function& Function::operator=(const Function &f)
{
   if (this == &f)
      return *this;
   
   GmatBase::operator=(f);
   
   callDescription    = f.callDescription;
   functionPath       = f.functionPath;
   functionName       = f.functionName;
   objectStore        = NULL;
   globalObjectStore  = NULL;
   solarSys           = f.solarSys;
   internalCoordSys   = f.internalCoordSys;
   forces             = f.forces;
   fcs                = NULL;
   fcsInitialized     = f.fcsInitialized;
   fcsFinalized       = f.fcsFinalized;
   validator          = f.validator;
   wasFunctionBuilt   = f.wasFunctionBuilt;
   isFunctionIOSet    = f.isFunctionIOSet;
   scriptErrorFound   = f.scriptErrorFound;
   objectsInitialized = f.objectsInitialized;
   inputNames         = f.inputNames;
   outputNames        = f.outputNames;
   inputArgMap        = f.inputArgMap;   // do I want to do this?
   outputArgMap       = f.outputArgMap;  // do I want to do this?
   outputWrapperTypes = f.outputWrapperTypes;
   outputRowCounts    = f.outputRowCounts;
   outputColCounts    = f.outputColCounts;
   return *this;
}

//------------------------------------------------------------------------------
// void SetCallDescription(const std::string &desc)
//------------------------------------------------------------------------------
void Function::SetCallDescription(const std::string &desc)
{
   callDescription = desc;
}

//------------------------------------------------------------------------------
// virtual WrapperTypeArray GetOutputTypes(IntegerArray &rowCounts,
//                                         IntegeArrayr &colCounts) const
//------------------------------------------------------------------------------
WrapperTypeArray Function::GetOutputTypes(IntegerArray &rowCounts,
                                          IntegerArray &colCounts) const
{
   rowCounts = outputRowCounts;
   colCounts = outputColCounts;
   return outputWrapperTypes;
}


//------------------------------------------------------------------------------
// virtual void SetOutputTypes(WrapperTypeArray &outputTypes,
//                             IntegerArray &rowCounts, IntegerArray &colCounts)
//------------------------------------------------------------------------------
/*
 * Sets function output types. This method is called when parsing the function
 * file from the Interpreter.
 */
//------------------------------------------------------------------------------
void Function::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Function::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif
   
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;   
}

//------------------------------------------------------------------------------
// bool Function::IsNewFunction()
//------------------------------------------------------------------------------
/**
 * Defines the interface that identifies new functions
 *
 * @return true if the function is treated as new, false if not
 *
 * @note The default implementation just returns true.  Classes (like
 * GmatFunction) that need different behavior override this method to produce
 * the desired behavior.
 */
//------------------------------------------------------------------------------
bool Function::IsNewFunction()
{
   return true;
}

//------------------------------------------------------------------------------
// void SetNewFunction(bool flag)
//------------------------------------------------------------------------------
/**
 * Defines the interface that is used to identify new functions.
 *
 * GmatFunctions use this interface to identify when a function is being loaded
 * for the first time.
 *
 * @param flag true for new functions, false if not new
 *
 * @note This default implementation doesn't do anything.
 */
//------------------------------------------------------------------------------
void Function::SetNewFunction(bool flag)
{
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool Function::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   validator = Validator::Instance();
   return true;
}


//------------------------------------------------------------------------------
// bool Function::Execute(ObjectInitializer *objInit)  [default implementation]
//------------------------------------------------------------------------------
bool Function::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   return true;
}


//------------------------------------------------------------------------------
// bool Function::Finalize(bool cleanUp = false)  [default implementation]
//------------------------------------------------------------------------------
void Function::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("\nFunction::Finalize() entered, fcsFinalized=%d\n", fcsFinalized);
   #endif
   
   // Checking fcsFinalized here causes GMT-5160 script failure when same object name
   // as function formal name is passed to a function (LOJ: 2015.08.27)
   #if 0
   if (fcsFinalized)
   {
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("Function::Finalize() leaving, fcs has already finalized\n\n");
      #endif
      return;
   }
   #endif
   
   if (sandboxObjects.size() == 0)
   {
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("Function::Finalize() leaving, there is nothing to do since snadboxObjects are empty\n\n");
      #endif
      return;
   }
   
   if (globalObjectStore == NULL)
   {
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("Function::Finalize() leaving, there is nothing to do since globalObjectStore is NULL\n\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   ShowObjects("In Function::Finalize()");
   #endif
   
   // Set ref Parameter back to original ref Parameter for global ReportFile and XyPlot
   // since these subscribers uses Parameters for reporting and plotting
   GmatBase *sandboxObj = NULL;
   GmatBase *globalObj = NULL;
   GmatBase *autoObj = NULL;
   std::string sandboxObjName;
   std::string globalObjName;
   std::string autoObjName;
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Going through sandboxObjects to see if global Parameters in global "
       "Subscribers need to point back to sandbox pointer\n");
   #endif
   // Go through already managed sandbox objects
   for (UnsignedInt i = 0; i < sandboxObjects.size(); i++)
   {
      sandboxObj = sandboxObjects[i];
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("   sandboxObj=<%p>[%s]'%s'\n", sandboxObj,
          sandboxObj ? sandboxObj->GetTypeName().c_str() : "NULL",
          sandboxObj ? sandboxObj->GetName().c_str() : "NULL");
      #endif
      
      if (sandboxObj == NULL)
         continue;
      
      sandboxObjName = sandboxObj->GetName();
      if (sandboxObj->IsOfType(Gmat::PARAMETER))
      {
         GmatBase *paramOwner = ((Parameter*)sandboxObj)->GetOwner();
         #ifdef DEBUG_FUNCTION_FINALIZE
         MessageInterface::ShowMessage
            ("   sandboxObj is Parameter, paramOwner = <%p>[%s]'%s'\n", paramOwner,
             paramOwner ? paramOwner->GetTypeName().c_str() : "NULL",
             paramOwner ? paramOwner->GetName().c_str() : "NULL");
         #endif
         
         // Go through global ReportFile or XYPlot and reset ref Parameter
         for (ObjectMap::iterator i = globalObjectStore->begin(); i != globalObjectStore->end(); ++i)
         {
            try
            {
               globalObj = i->second;
               globalObjName = i->first;
               #ifdef DEBUG_FUNCTION_FINALIZE
               MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   ", globalObj));
               MessageInterface::ShowMessage("Checking if it is ReportFile or XYPlot\n");
               #endif
               if (globalObj && 
                   (globalObj->IsOfType("ReportFile") || globalObj->IsOfType("XYPlot")))
               {
                  #ifdef DEBUG_FUNCTION_FINALIZE
                  MessageInterface::ShowMessage
                     ("   Found global <%p>[%s]'%s', so calling globalObj->SetRefObject(%s)\n",
                      globalObj, globalObj->GetTypeName().c_str(), globalObjName.c_str(),
                      sandboxObjName.c_str());
                  #endif
                  
                  // We can use globalObj->GetRefObjectNameArray() and go through this list
                  // or ignore exception. Ignoring exception for now.
                  try
                  {
                     globalObj->SetRefObject(sandboxObj, Gmat::PARAMETER, sandboxObjName);
                  }
                  catch (BaseException &be)
                  {
                     // Ignore exception
                     #ifdef DEBUG_FUNCTION_FINALIZE
                     MessageInterface::ShowMessage("   Ignoring exception: %s\n", be.GetFullMessage().c_str());
                     #endif
                  }
               }
            }
            catch (BaseException &be)
            {
               #ifdef DEBUG_FUNCTION_FINALIZE
               MessageInterface::ShowMessage
                  ("Caught exception: %s\n", be.GetFullMessage().c_str());
               #endif
               // If cleanup mode, ignore exception
               if (!cleanUp)
                  throw;
            }
         }
      }
   }
   
   #if 0
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Going through automaticObjectMap to see if ref object of global Parameters "
       "need to point back to original pointer\n");
   #endif
   // Go through automatic objects
   ObjectMap::iterator omi;
   for (omi = automaticObjectMap.begin(); omi != automaticObjectMap.end(); ++omi)
   {
      autoObj = omi->second;;
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("   autoObj=<%p>[%s]'%s'\n", autoObj,
          autoObj ? autoObj->GetTypeName().c_str() : "NULL",
          autoObj ? autoObj->GetName().c_str() : "NULL");
      #endif
      
      if (autoObj == NULL)
         continue;
      
      autoObjName = autoObj->GetName();
      if (autoObj->IsOfType(Gmat::PARAMETER))
      {
         GmatBase *paramOwner = ((Parameter*)autoObj)->GetOwner();
         #ifdef DEBUG_FUNCTION_FINALIZE
         MessageInterface::ShowMessage
            ("   autoObj is Parameter, paramOwner = <%p>[%s]'%s'\n", paramOwner,
             paramOwner ? paramOwner->GetTypeName().c_str() : "NULL",
             paramOwner ? paramOwner->GetName().c_str() : "NULL");
         #endif
         
         if (paramOwner == NULL)
            continue;
         
         // Go through global objects and reset ref Parameter
         for (ObjectMap::iterator i = globalObjectStore->begin(); i != globalObjectStore->end(); ++i)
         {
            globalObj = i->second;
            globalObjName = i->first;
            if (globalObj->GetName() == paramOwner->GetName())
            {
               #ifdef DEBUG_FUNCTION_FINALIZE
               MessageInterface::ShowMessage
                  ("   Found global <%p>[%s]'%s', so calling autoObj->SetRefObject()\n",
                   globalObj, globalObj->GetTypeName().c_str(),
                   globalObjName.c_str());
               #endif
               autoObj->SetRefObject(globalObj, globalObj->GetType(), globalObjName);
            }
         }
      }
   }
   #endif
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage("Function::Finalize() leaving\n\n");
   #endif
}


//------------------------------------------------------------------------------
// bool IsFinalized()
//------------------------------------------------------------------------------
bool Function::IsFcsFinalized()
{
   return fcsFinalized;
}

//------------------------------------------------------------------------------
// void SetObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void Function::SetObjectMap(ObjectMap *objMap)
{
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMap(objMap, "In Function::SetObjectMap", "Input Object Map");
   #endif
   objectStore = objMap;
}

//------------------------------------------------------------------------------
// void SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
void Function::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   globalObjectStore = map;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void Function::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetSolarSystem() entered, this='%s', ss=<%p>\n",
       GetName().c_str(), ss);
   #endif
   
   solarSys = ss;
}

//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Function::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetInternalCoordSystem() entered, this='%s', cs=<%p>\n",
       GetName().c_str(), cs);
   #endif
   
   // if cs is not NULL, set (loj: 2008.10.07)
   if (cs != NULL)
      internalCoordSys = cs;
}

//------------------------------------------------------------------------------
// void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
void Function::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   forces = tf;
}

//------------------------------------------------------------------------------
// bool WasFunctionBuilt()
//------------------------------------------------------------------------------
bool Function::WasFunctionBuilt()
{
   return wasFunctionBuilt;
}

//------------------------------------------------------------------------------
// void SetFunctionWasBuilt(bool built)
//------------------------------------------------------------------------------
void Function::SetFunctionWasBuilt(bool built)
{
   wasFunctionBuilt = built;
}

//------------------------------------------------------------------------------
// bool IsFunctionInputOutputSet()
//------------------------------------------------------------------------------
bool Function::IsFunctionInputOutputSet()
{
   return isFunctionIOSet;
}

//------------------------------------------------------------------------------
// void SetFunctionInputOutputIsSet(bool set)
//------------------------------------------------------------------------------
void Function::SetFunctionInputOutputIsSet(bool set)
{
   isFunctionIOSet = set;
}

//------------------------------------------------------------------------------
// void SetScriptErrorFound(bool errFlag)
//------------------------------------------------------------------------------
void Function::SetScriptErrorFound(bool errFlag)
{
   scriptErrorFound = errFlag;
}

//------------------------------------------------------------------------------
// bool ScriptErrorFound()
//------------------------------------------------------------------------------
bool Function::ScriptErrorFound()
{
   return scriptErrorFound;
}

//------------------------------------------------------------------------------
// bool IsFunctionControlSequenceSet()
//------------------------------------------------------------------------------
bool Function::IsFunctionControlSequenceSet()
{
   if (fcs != NULL) return true;
   return false;
}

//------------------------------------------------------------------------------
// bool SetFunctionControlSequence(GmatCommand *cmd)
//------------------------------------------------------------------------------
bool Function::SetFunctionControlSequence(GmatCommand *cmd)
{
   #ifdef DEBUG_FUNCTION_SET
      if (!cmd) MessageInterface::ShowMessage("Trying to set FCS on %s, but it is NULL!!!\n",
            functionName.c_str());
      else
      {
         MessageInterface::ShowMessage("Setting FCS for function %s with FCS pointer = %p\n",
               functionName.c_str(), cmd);  
         MessageInterface::ShowMessage("First command is a %s\n", (cmd->GetTypeName()).c_str());
      }
   #endif
   fcs = cmd;
   return true;
}

//------------------------------------------------------------------------------
// GmatBase* GetFunctionControlSequence()
//------------------------------------------------------------------------------
GmatCommand* Function::GetFunctionControlSequence()
{
   return fcs;
}

//------------------------------------------------------------------------------
// std::string GetFunctionPathAndName()
//------------------------------------------------------------------------------
std::string Function::GetFunctionPathAndName()
{
   return functionPath;
}

//------------------------------------------------------------------------------
// bool SetInputElementWrapper(const std::string &forName, ElementWrapper *wrapper)
//------------------------------------------------------------------------------
bool Function::SetInputElementWrapper(const std::string &forName, ElementWrapper *wrapper)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Function::SetInputElementWrapper() entered, wrapper name \"%s\"\n", forName.c_str());
   MessageInterface::ShowMessage
      ("   wrapper=<%p>, wrapper type = %d, inputArgMap.size()=%d\n", wrapper,
       wrapper->GetWrapperType(), inputArgMap.size());
   #endif
   
   if (inputArgMap.find(forName) == inputArgMap.end())
   {
      #ifdef DEBUG_FUNCTION_IN_OUT
      MessageInterface::ShowMessage
         ("**** ERROR **** Function::SetInputElementWrapper() throwing exception: "
          "Input name '%s' not found in inputArgMap\n", forName.c_str());
      #endif
      std::string errMsg = "Unknown input argument \"" + forName;
      errMsg += "\" for function \"" + functionName + "\"";
      throw FunctionException(errMsg);
   }
   
   inputArgMap[forName] = wrapper;
   
   //@note old inputWrappers are deleted in the FunctionManager::CreateFunctionArgWrappers()
   // before creates new wrappers for input arguments
   
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Function::SetInputElementWrapper() leaving, wrapper name \"%s\"\n", forName.c_str());
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// virtual ElementWrapper*  GetOutputArgument(Integer argNumber)
//------------------------------------------------------------------------------
/*
 * Implements GMAT FUNCTIONS design 27.2.2.3 GmatFunction Execution
 * step 4 of "Steps Performed on the Firstexecution"
 */
//------------------------------------------------------------------------------
ElementWrapper* Function::GetOutputArgument(Integer argNumber)
{
   if ((argNumber < 0) || (argNumber > (Integer) outputNames.size()) ||
       (argNumber> (Integer) outputArgMap.size()))
   {
      std::string errMsg = "Function error: argument number out-of-range\n";
      throw FunctionException(errMsg);
   }
   std::string argName = outputNames.at(argNumber);
   return GetOutputArgument(argName);
}


//------------------------------------------------------------------------------
// ElementWrapper* GetOutputArgument(const std::string &byName)
//------------------------------------------------------------------------------
ElementWrapper* Function::GetOutputArgument(const std::string &byName)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Function::GetOutputArgument(%s) entered, outputArgMap.size()=%d\n",
       byName.c_str(), outputArgMap.size());
   #endif
   
   if (outputArgMap.find(byName) == outputArgMap.end())
   {
      std::string errMsg = "Function error: output \"" + byName;
      errMsg += "\" from function \"" + functionName;
      errMsg += "\" does not exist.\n";
      throw FunctionException(errMsg);
   }
   
   ElementWrapper *ew = outputArgMap[byName];
   
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Function::GetOutputArgument(%s) returning <%p>, type=%d\n", byName.c_str(),
       ew, ew ? ew->GetDataType() : -999);
   #endif
   
   return ew;
}


//------------------------------------------------------------------------------
// WrapperArray& GetWrappersToDelete()
//------------------------------------------------------------------------------
WrapperArray& Function::GetWrappersToDelete()
{
   return wrappersToDelete;
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "")
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action ("")
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool Function::TakeAction(const std::string &action,
                          const std::string &actionData)
{
   #ifdef DEBUG_FUNCTION_ACTION
   MessageInterface::ShowMessage
      ("Function::TakeAction() action=%s, actionData=%s\n", action.c_str(),
       actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      #ifdef DEBUG_FUNCTION_ACTION
      MessageInterface::ShowMessage("   Clearing input and output argument list\n");
      #endif
      
      // Do we need to also delete input/output ElementWrappers here? 
      // They are deleted in the FunctionManager::ClearInOutWrappers()
      // Let's delete them here for now in TakeAction(). I don't know in what
      // situation TakeAction() will be called(loj: 2008.11.12)
      // input wrappers map
      
      ClearInOutArgMaps(true, true);
      return true;
   }
   
   return false;
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
bool Function::IsParameterReadOnly(const Integer id) const
{
   // We want both path and name when we write out, so skip this parameter
   if ((id == FUNCTION_NAME) || (id == FUNCTION_INPUT) || (id == FUNCTION_OUTPUT))
      return true;
   
   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetParameterText(const Integer id) const
{
   if (id >= FUNCTION_PATH && id < FunctionParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer Function::GetParameterID(const std::string &str) const
{
   for (Integer i = FUNCTION_PATH; i < FunctionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Function::GetParameterType(const Integer id) const
{
   if (id >= FUNCTION_PATH&& id < FunctionParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetParameterTypeString(const Integer id) const
{
   if (id >= FUNCTION_PATH&& id < FunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetStringParameter(const Integer id) const
{
   if (id == FUNCTION_PATH)
   {
      #ifdef DEBUG_GET
      MessageInterface::ShowMessage
         ("Function::GetStringParameter() <%p> returning functionPath '%s'\n",
          this, functionPath.c_str());
      #endif
      return functionPath;
   }
   else if (id == FUNCTION_NAME)
      return functionName;
   
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Function::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//---------------------------------------------------------------------------
std::string Function::GetStringParameter(const Integer id, 
                                         const Integer index) const
{
   switch (id)
   {
   case FUNCTION_INPUT:
      if (index >= (Integer)inputNames.size())
      {
         FunctionException fe;
         fe.SetDetails("The index of %d for field \"%s\" is out of bounds for the "
                       "object named \"%s\"", index, GetParameterText(id).c_str(),
                       GetName().c_str());
         throw fe;
      }
      else
         return inputNames[index];
      
   case FUNCTION_OUTPUT:
      if (index >= (Integer)outputNames.size())
      {
         FunctionException fe;
         fe.SetDetails("The index of %d for field \"%s\" is out of bounds for the "
                       "object named \"%s\"", index, GetParameterText(id).c_str(),
                       GetName().c_str());
         throw fe;
      }
      else
         return outputNames[index];
      
   default:
      return GmatBase::GetStringParameter(id);
   }
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//---------------------------------------------------------------------------
std::string Function::GetStringParameter(const std::string &label,
                                         const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& Function::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case FUNCTION_INPUT:
      return inputNames;
   case FUNCTION_OUTPUT:
      return outputNames;
   default:
      return GmatBase::GetStringArrayParameter(id);
   }
}

//---------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
const StringArray& Function::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const Integer id, const std::string &value)
 */
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
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
bool Function::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetStringParameter() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case FUNCTION_INPUT:
      {
         #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage("   Adding function input name '%s'\n", value.c_str());
         #endif
         
         // Ignore () as input, so that "function MyFunction()" will work as no input (LOJ: 2015.08.19)
         if (value == "()")
            return true;
         
         if (inputArgMap.find(value) == inputArgMap.end())
         {
            inputNames.push_back(value);
            inputArgMap.insert(std::make_pair(value,(ElementWrapper*) NULL));
         }
         else
            throw FunctionException
               ("In function file \"" + functionPath + "\": "
                "The input argument \"" + value + "\" already exists");
         
         return true;
      }
   case FUNCTION_OUTPUT:
      {
         #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage("   Adding function output name '%s'\n", value.c_str());
         #endif
         if (outputArgMap.find(value) == outputArgMap.end())
         {
            outputNames.push_back(value);
            outputArgMap.insert(std::make_pair(value,(ElementWrapper*) NULL));
         }
         else
            throw FunctionException
               ("In function file \"" + functionPath + "\": "
                "The output argument \"" + value + "\" already exists");
         
         return true;
      }
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const std::string &label,
                                  const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// void ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs)
//------------------------------------------------------------------------------
void Function::ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs)
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Function::ClearInOutArgMaps() this=<%p> '%s' entered\n", this,
       GetName().c_str());
   MessageInterface::ShowMessage
      ("inputArgMap.size()=%d, outputArgMap.size()=%d\n", inputArgMap.size(),
       outputArgMap.size());
   #endif
   
   std::vector<ElementWrapper *> wrappersToDelete;
   std::map<std::string, ElementWrapper *>::iterator ewi;
   
   if (deleteInputs)
   {
      // input wrappers map
      for (ewi = inputArgMap.begin(); ewi != inputArgMap.end(); ++ewi)
      {
         if (ewi->second)
         {         
            if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ewi->second) ==
                wrappersToDelete.end())
               wrappersToDelete.push_back(ewi->second);
         }
      }
   }
   
   if (deleteOutputs)
   {
      // output wrappers
      for (ewi = outputArgMap.begin(); ewi != outputArgMap.end(); ++ewi)
      {
         if (ewi->second)
         {
            if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ewi->second) ==
                wrappersToDelete.end())
               wrappersToDelete.push_back(ewi->second);
         }
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
   
   // Delete old ElementWrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Function::ClearInOutArgMaps()",
          "deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   inputArgMap.clear();
   outputArgMap.clear();
}

//LOJ: 2014.12.09
//------------------------------------------------------------------------------
// void ClearFunctionObjects()
//------------------------------------------------------------------------------
void Function::ClearFunctionObjects()
{
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage
      ("Function::ClearFunctionObjects() this=<%p> '%s' entered\n   "
       "functionObjectMap.size()=%d, sandboxObjects.size()=%d, "
       "objectsToDelete.size()=%d\n", this, GetName().c_str(),
       functionObjectMap.size(), sandboxObjects.size(), objectsToDelete.size());
   ShowObjectMap(&functionObjectMap, "In ClearFunctionObjects()", "functionObjectMap");
   #endif
   
   StringArray toDelete;
   ObjectMap::iterator omi = functionObjectMap.begin();
   while (omi != functionObjectMap.end())
   {
      GmatBase *obj = omi->second;
      #ifdef DEBUG_FUNCTION_OBJ
      MessageInterface::ShowMessage
         ("   ==> Checking if <%p>[%s]'%s' can be deleted\n", obj,
          obj ? obj->GetTypeName().c_str() : "NULL", (omi->first).c_str());
      #endif
      
      // If object is not NULL and not this function, delete
      if (obj != NULL && (!obj->IsOfType("GmatFunction")))
      {
         #ifdef DEBUG_FUNCTION_OBJ
         MessageInterface::ShowMessage
            ("   isLocal = %d, isGlobal = %d, isAutomaticGlobal = %d, \n",
             obj->IsLocal(), obj->IsGlobal(), obj->IsAutomaticGlobal());
         #endif
         // @note CelestialBody is added to SolarSystem and it will be deleted when
         // when SolarSystem in use is deleted (LOJ: 2014.12.23)
         if (obj->IsLocal() && !(obj->IsOfType(Gmat::CELESTIAL_BODY)))
         {
            #ifdef DEBUG_MEMORY
            GmatBase *obj = omi->second;
            MemoryTracker::Instance()->Remove
               (obj, obj->GetName(), "Function::ClearFunctionObjects()",
                "deleting functionObj");
            #endif
            #ifdef DEBUG_FUNCTION_OBJ
            MessageInterface::ShowMessage("   Deleting since it is a local object\n");
            #endif
            delete omi->second;
            omi->second = NULL;
            //functionObjectMap.erase(omi);
            //++omi;
         }
      }
      ++omi;
   }
   
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage
      ("Function::ClearFunctionObjects() this=<%p> '%s' leaving\n",
       this, GetName().c_str());
   #endif
}

//LOJ: 2014.12.09
//------------------------------------------------------------------------------
// void AddFunctionObject(GmatBase *obj)
//------------------------------------------------------------------------------
void Function::AddFunctionObject(GmatBase *obj)
{
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage
      ("Function::AddFunctionObject() entered, obj=<%p>[%s]'%s'\n", obj,
       obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   //ShowObjects("Entered Function::AddFunctionObject()");
   #endif
   
   if (obj && obj->GetName() != "")
   {
      std::string objName = obj->GetName();
      if (functionObjectMap.find(objName) == functionObjectMap.end())
         functionObjectMap.insert(std::make_pair(objName, obj));
   }
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage("Function::AddFunctionObject() leaving\n");
   ShowObjectMap(&functionObjectMap, "Leaving AddFunctionObject()", "functionObjectMap");
   #endif
}

//LOJ: 2014.10.10
//------------------------------------------------------------------------------
// GmatBase* FindFunctionObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Function::FindFunctionObject(const std::string &name)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Function::FindFunctionObject() entered, name='%s'\n", name.c_str());
   //ShowObjects("In Function::FindFunctionObject()");
   #endif
   
   // Ignore array index
   std::string newName = name;
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
      newName = name.substr(0, index);
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage("   newName='%s'\n", newName.c_str());
   #endif
   
   GmatBase *obj = NULL;
   
   if (functionObjectMap.find(newName) != functionObjectMap.end())
      obj = functionObjectMap[newName];
   
   // try SolarSystem if obj is still NULL
   if (obj == NULL && solarSys != NULL)
      obj = (GmatBase*)(solarSys->GetBody(newName));
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Function::FindFunctionObject() returning <%p>\n", obj);
   #endif
   
   return obj;
}

//------------------------------------------------------------------------------
// ObjectMap* GetFunctionObjectMap()
//------------------------------------------------------------------------------
ObjectMap* Function::GetFunctionObjectMap()
{
   return &functionObjectMap;
}

//------------------------------------------------------------------------------
// void ClearAutomaticObjects()
//------------------------------------------------------------------------------
void Function::ClearAutomaticObjects()
{
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("Function::ClearAutomaticObjects() this=<%p> '%s' entered\n   "
       "automaticObjectMap.size()=%d, sandboxObjects.size()=%d, "
       "objectsToDelete.size()=%d\n", this, GetName().c_str(),
       automaticObjectMap.size(), sandboxObjects.size(), objectsToDelete.size());
   #endif
   
   StringArray toDelete;
   ObjectMap::iterator omi;
   for (omi = automaticObjectMap.begin(); omi != automaticObjectMap.end(); ++omi)
   {
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("Checking if <%p> '%s' can be deleted\n", omi->second,
          (omi->first).c_str());
      #endif
      if (omi->second != NULL)
      {
         std::string objName = (omi->second)->GetName();
         
         // If same object found in sandboxObjects then skip
         if (!sandboxObjects.empty() &&
             find(sandboxObjects.begin(), sandboxObjects.end(), omi->second) !=
             sandboxObjects.end())
            continue;
         
         // If same object found in functionObjectMap then skip
         if (functionObjectMap.find(objName) != functionObjectMap.end())
         {
            ObjectMap::iterator iobjfound = functionObjectMap.find(objName);
            if (omi->second == iobjfound->second)
               continue;
         }
         
         //------------------------------------------------------
         #ifdef __COLLECT_AUTO_OBJECTS__
         //------------------------------------------------------
         
         // if object is not in objectsToDelete then add
         if (find(objectsToDelete.begin(), objectsToDelete.end(), omi->second) ==
             objectsToDelete.end())
         {
            #ifdef DEBUG_AUTO_OBJ
            MessageInterface::ShowMessage("   Added to objectsToDelete\n");
            #endif
            objectsToDelete.push_back(omi->second);
         }
         else
         {
            #ifdef DEBUG_AUTO_OBJ
            MessageInterface::ShowMessage("   Already in the objectsToDelete\n");
            #endif
         }
         
         //------------------------------------------------------
         #else
         //------------------------------------------------------
         
         // if object not found in sandboxObjects then delete
         if (find(sandboxObjects.begin(), sandboxObjects.end(), omi->second) ==
             sandboxObjects.end())
         {
            #ifdef DEBUG_MEMORY
            GmatBase *obj = omi->second;
            MemoryTracker::Instance()->Remove
               (obj, obj->GetName(), "Function::ClearAutomaticObjects()",
                "deleting autoObj");
            #endif
            delete omi->second;
            omi->second = NULL;
         }
         //------------------------------------------------------
         #endif //__COLLECT_AUTO_OBJECTS__
         //------------------------------------------------------
      }
      else
      {
         #ifdef DEBUG_AUTO_OBJ
         MessageInterface::ShowMessage("   Skipped since sandbox object\n");
         #endif
      }
      toDelete.push_back(omi->first); 
   }
   
   for (unsigned int kk = 0; kk < toDelete.size(); kk++)
      automaticObjectMap.erase(toDelete.at(kk));
   
   #ifdef __COLLECT_AUTO_OBJECTS__
   // delete old automatic objects collected if not already deleted
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("   There are %d automatic objects to delete\n", objectsToDelete.size());
   #endif
   
   ObjectArray::iterator oai;
   for (oai = objectsToDelete.begin(); oai != objectsToDelete.end(); ++oai)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*oai), (*oai)->GetName(), "Function::ClearAutomaticObjects()", "deleting old object");
      #endif
      delete (*oai);
      (*oai) = NULL;
   }
   objectsToDelete.clear();
   #endif //__COLLECT_AUTO_OBJECTS__
   
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("Function::ClearAutomaticObjects() this=<%p> '%s' leaving\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void AddAutomaticObject(const std::string &withName, GmatBase *obj,
//                         bool alreadyManaged)
//------------------------------------------------------------------------------
/*
 * Adds automatic objects such as Parameter, e.g. sat.X, to automatic object map.
 * The GmatFunction::Initialize() calls Validator to validate and to create
 * ElementWrappers for commands in the FCS. The Validator creates automatic
 * Parameters if necessary and calls this method for adding. Since function can be
 * initialized more than one time multiple automatic Parameters can be created
 * which is the case for nested and recursive call.
 *
 * @param  withName  Automatic object name
 * @param  obj  Automatic object pointer
 * @param  alreadyManaged  true if object is Sandbox object
 *
 * @note Make sure that old automatic objects are deleted properly
 */
//------------------------------------------------------------------------------
void Function::AddAutomaticObject(const std::string &withName, GmatBase *obj,
                                  bool alreadyManaged)
{
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("\nFunction::AddAutomaticObject() <%p>'%s' entered\n   name='%s', obj=<%p>[%s]'%s', "
       "alreadyManaged=%d, objectStore=<%p>\n", this, GetName().c_str(),
       withName.c_str(), obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       alreadyManaged, objectStore);
   #endif
   
   // Make sure that the owner of automatic Parameter exist in the objectStore
   // (LOJ: 2009.03.25)
   if (objectStore != NULL)
   {
      std::string type, ownerName, dep;
      GmatStringUtil::ParseParameter(withName, type, ownerName, dep);
      GmatBase *owner = FindObject(ownerName);
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   ownerName='%s', owner=<%p>[%s]'%s'\n", ownerName.c_str(), owner,
          owner ? owner->GetTypeName().c_str() : "NULL",
          owner ? owner->GetName().c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMap(objectStore, "In Function::AddAutomaticObject", "objectStore");
      #endif
      #endif
      
      //========================================================================
      // Do not check for the owner here, it will find it when wrapper is created
      // Fix for GMT-2380 LOJ: 2015.06.18)
      // if (owner == NULL)
      // {
      //    FunctionException fe;;
      //    fe.SetDetails("Cannot find the object named \"%s\" in the function "
      //                  "object store", ownerName.c_str());
      //    throw fe;
      // }
      
      // // Check if owner is from the right object store      
      // GmatBase *refObj = obj->GetRefObject(owner->GetType(), ownerName);
      // #ifdef DEBUG_AUTO_OBJ
      // MessageInterface::ShowMessage
      //    ("   refObj=<%p>[%s]'%s'\n", refObj, refObj ? refObj->GetTypeName().c_str() : "NULL",
      //     refObj ? refObj->GetName().c_str() : "NULL");
      // #endif
      
      // if (owner != refObj)
      // {
      //    #ifdef DEBUG_AUTO_OBJ
      //    MessageInterface::ShowMessage
      //       ("*** WARNING *** The ref object \"%s\" of the Parameter \"%s\" "
      //        "does not point to object in object store. owner=<%p>, refObj=<%p>\n",
      //        ownerName.c_str(), withName.c_str(), owner, refObj);
      //    #endif
         
      //    //================================================================
      //    // Do not throw exception, since objects from Global command will be
      //    // moved to global object store when Global command is executing until
      //    // then the refobj will be the configured object since the Moderator
      //    // sets it. (LOJ: 2015.03.03)
      //    #if 0
      //    FunctionException fe;;
      //    fe.SetDetails("The ref object \"%s\" of the Parameter \"%s\" "
      //                  "does not point to object in object store", ownerName.c_str(),
      //                  withName.c_str());
      //    throw fe;
      //    #endif
      //    //================================================================
      // }
      //========================================================================
   }
   
   // If same name exist in the sandbox, add it to sandboxObjects so that
   // it can be handled appropriately during finalization
   if (alreadyManaged)
   {
      if (find(sandboxObjects.begin(), sandboxObjects.end(), obj) ==
          sandboxObjects.end() &&
          automaticObjectMap.find(withName) == automaticObjectMap.end())         
      {
         #ifdef DEBUG_AUTO_OBJ
         MessageInterface::ShowMessage
            ("   ==> Adding <%p>'%s' to sandboxObjects\n", obj, obj->GetName().c_str());
         #endif
         sandboxObjects.push_back(obj);
      }
   }
   
   #ifdef __COLLECT_AUTO_OBJECTS__
   if (automaticObjectMap.find(withName) != automaticObjectMap.end())
   {
      GmatBase *oldObj = automaticObjectMap[withName];
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   Found oldObj=<%p>[%s] '%s'\n", oldObj, oldObj ?
          oldObj->GetTypeName().c_str() : "NULL",
          oldObj ? oldObj->GetName().c_str() : "NULL");
      #endif
      
      // if old object is not in the sandboxObjects add it to delete list,
      // since objects in the Sandbox objectMap will be deleted from the Sandbox first.
      // (LOJ: 2009.03.16)
      if (oldObj != NULL &&
          (find(sandboxObjects.begin(), sandboxObjects.end(), oldObj) ==
           sandboxObjects.end()))
      {
         if (find(objectsToDelete.begin(), objectsToDelete.end(), oldObj) ==
             objectsToDelete.end())
         {
            #ifdef DEBUG_AUTO_OBJ
            MessageInterface::ShowMessage("   Adding to objectsToDelete\n");
            #endif
            objectsToDelete.push_back(oldObj);
         }
      }
   }
   #endif
   
   // Insert object to automaticObjectMap if not already inserted
   if (automaticObjectMap.find(withName) != automaticObjectMap.end())
   {
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   The object <%p>'%s' is already in automaticObjectMap\n", obj, withName.c_str());
      #endif
   }
   else
   {
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   ==> Inserting <%p>'%s' to automaticObjectMap\n", obj, withName.c_str());
      #endif
      
      // Objects in automaticObjectMap are cloned and add to function objectStore
      // in GmatFunction::Initialize()
      automaticObjectMap.insert(std::make_pair(withName,obj));
   }
   
   #ifdef DEBUG_AUTO_OBJ
   ShowObjectMap(&automaticObjectMap, "In Function::AddAutomaticObject()", "automaticObjectMap");
   MessageInterface::ShowMessage
      ("Function::AddAutomaticObject() <%p>'%s' leaving\n\n", this, GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
// GmatBase* FindAutomaticObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Function::FindAutomaticObject(const std::string &name)
{
   if (automaticObjectMap.find(name) != automaticObjectMap.end())
      return automaticObjectMap[name];
   else
      return NULL;
}


//------------------------------------------------------------------------------
// ObjectMap* GetAutomaticObjectMap()
//------------------------------------------------------------------------------
ObjectMap* Function::GetAutomaticObjectMap()
{
   return &automaticObjectMap;
}

//---------------------------------
// protected
//---------------------------------
//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Function::FindObject(const std::string &name)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Function::FindObject() <%p>'%s' entered, name = '%s'\n", this,
       functionName.c_str(), name.c_str());
   ShowObjectMap(objectStore, "In FindObject()", "objectStore");
   ShowObjectMap(globalObjectStore, "In FindObject()", "globalObjectStore");
   ShowObjectMap(&functionObjectMap, "In FindObject()", "functionObjectMap");
   #endif
   
   std::string newName = name;
   GmatBase *obj = NULL;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find('(');
   if (index != name.npos)
      newName = name.substr(0, index);

   // Check for the object in the Local Object Store (LOS) first
   if (objectStore && objectStore->find(newName) != objectStore->end())
      obj = (*objectStore)[newName];
   
   // If not found in the LOS, check the Global Object Store (GOS)
   else if (globalObjectStore && globalObjectStore->find(newName) != globalObjectStore->end())
      obj = (*globalObjectStore)[newName];
   
   // Let's try SolarSystem (loj: 2008.06.12)
   else if (solarSys && solarSys->GetBody(newName))
      obj = (GmatBase*)(solarSys->GetBody(newName));
   
   // If still not found, try functionObjectMap
   // Should this moved to top?
   else if (functionObjectMap.find(newName) != functionObjectMap.end())
      obj = functionObjectMap[newName];
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("Function::FindObject() <%p>'%s' returning <%p> for '%s'\n", this,
       functionName.c_str(), obj, name.c_str());
   #endif
   return obj;
}


//------------------------------------------------------------------------------
//bool IsAutomaticObjectGlobal(const std::string &autoObjName, GmatBase *owner)
//------------------------------------------------------------------------------
/**
 * Checks if the owner of automatic object such as Parameter is global object.
 *
 * @param autoObjName  Input object name
 * @param *owner        Owner of the automatic object if owner is found
 */
//------------------------------------------------------------------------------
bool Function::IsAutomaticObjectGlobal(const std::string &autoObjName,
                                       GmatBase **owner)
{
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("Function::IsAutomaticObjectGlobal() entered, autoObjName='%s'\n",
       autoObjName.c_str());
   #endif
   
   *owner = NULL;
   std::string type, ownerName, dep;
   GmatStringUtil::ParseParameter(autoObjName, type, ownerName, dep);
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage("   ownerName='%s'\n", ownerName.c_str());
   #endif
   
   bool isParamOwnerGlobal = false;
   if (globalObjectStore->find(ownerName) != globalObjectStore->end())
   {
      *owner = (globalObjectStore->find(ownerName))->second;
      isParamOwnerGlobal = true;
   }
   
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("Function::IsAutomaticObjectGlobal() leaving, autoObjName='%s' is %s, ",
       autoObjName.c_str(), isParamOwnerGlobal ? "global" : "not global");
   MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("owner=", (*owner)));
   #endif
   
   return isParamOwnerGlobal;
}

//------------------------------------------------------------------------------
// void ShowObjectMap(ObjectMap *objMap, const std::string &title,
//                    const std::string &mapName)
//------------------------------------------------------------------------------
void Function::ShowObjectMap(ObjectMap *objMap, const std::string &title,
                             const std::string &mapName)
{
   MessageInterface::ShowMessage("%s\n", title.c_str());
   MessageInterface::ShowMessage("this=<%p>, functionName='%s'\n", this, functionName.c_str());
   if (objMap == NULL)
   {
      MessageInterface::ShowMessage("%s is NULL\n", mapName.c_str());
      return;
   }
   
   std::string objMapName = mapName;
   GmatBase *obj = NULL;
   std::string objName;
   std::string objTypeName = "NULL";
   std::string isGlobal;
   std::string isLocal;
   GmatBase *paramOwner = NULL;
   bool isParameter = false;
   std::string paramOwnerType;
   std::string paramOwnerName;
   
   if (objMapName == "")
      objMapName = "object map";
   
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Here is %s <%p>, it has %d objects\n", objMapName.c_str(), objMap, objMap->size());
   for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
   {
      obj = i->second;
      objName = i->first;
      objTypeName = "";
      isGlobal = "No";
      isLocal = "No";
      paramOwner = NULL;
      isParameter = false;
      paramOwnerType = "";
      paramOwnerName = "";
      
      if (obj)
      {
         objTypeName = obj->GetTypeName();
         if (obj->IsGlobal())
            isGlobal = "Yes";
         if (obj->IsLocal())
            isLocal = "Yes";
         if (obj->IsOfType(Gmat::PARAMETER))
         {
            isParameter = true;
            paramOwner = ((Parameter*)obj)->GetOwner();
            if (paramOwner)
            {
               paramOwnerType = paramOwner->GetTypeName();
               paramOwnerName = paramOwner->GetName();
            }
         }
      }
      MessageInterface::ShowMessage
         ("   %50s  <%p>  %-16s  IsGlobal:%-3s  IsLocal:%-3s", objName.c_str(), obj,
          objTypeName.c_str(), isGlobal.c_str(), isLocal.c_str());
      if (isParameter)
         MessageInterface::ShowMessage
            ("  ParameterOwner: <%p>[%s]'%s'\n", paramOwner, paramOwnerType.c_str(),
             paramOwnerName.c_str());
      else
         MessageInterface::ShowMessage("\n");
   }
}


//------------------------------------------------------------------------------
// void ShowObjects(const std::string &title)
//------------------------------------------------------------------------------
void Function::ShowObjects(const std::string &title)
{
   MessageInterface::ShowMessage("%s\n", title.c_str());
   MessageInterface::ShowMessage("this=<%p>, functionName='%s'\n", this, functionName.c_str());
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("solarSys          = <%p>\n", solarSys);
   MessageInterface::ShowMessage("internalCoordSys  = <%p>\n", internalCoordSys);
   MessageInterface::ShowMessage("forces            = <%p>\n", forces);
   MessageInterface::ShowMessage("sandboxObjects.size() = %d\n", sandboxObjects.size());
   for (UnsignedInt i = 0; i < sandboxObjects.size(); i++)
   {
      GmatBase *obj = sandboxObjects[i];
      MessageInterface::ShowMessage
         ("   <%p>[%s]'%s'", obj, obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         GmatBase *paramOwner = ((Parameter*)obj)->GetOwner();
         MessageInterface::ShowMessage
            ("   paramOwner = <%p>[%s]'%s'\n", paramOwner,
             paramOwner ? paramOwner->GetTypeName().c_str() : "NULL",
             paramOwner ? paramOwner->GetName().c_str() : "NULL");
      }
      else
      {
         MessageInterface::ShowMessage("\n");
      }
   }
   ShowObjectMap(&functionObjectMap, "ShowObjects()", "functionObjectMap");
   ShowObjectMap(&automaticObjectMap, "ShowObjects()", "automaticObjectMap");
   ShowObjectMap(objectStore, "ShowObjects()", "objectStore");
   ShowObjectMap(globalObjectStore, "ShowObjects()", "globalObjectStore");
   MessageInterface::ShowMessage("========================================\n");
}

