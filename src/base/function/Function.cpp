//$Id$
//------------------------------------------------------------------------------
//                                   Function
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_IN_OUT
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_AUTO_OBJ
//#define DEBUG_OBJECT_MAP

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
   functionPath       (""),
   functionName       (""),
   objectStore        (NULL),
   globalObjectStore  (NULL),
   solarSys           (NULL),
   internalCoordSys   (NULL),
   forces             (NULL),
   fcs                (NULL),
   fcsFinalized       (false),
   validator          (NULL),
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
   // delete only output wrappers, input wrappers are set by FunctionManager,
   // so they are deleted there.
   // crashes on nested function if delete output wrappers here
   //ClearInOutArgMaps(false, true);
   
   ClearAutomaticObjects();
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
   functionPath       (f.functionPath),
   functionName       (f.functionName),
   inputNames         (f.inputNames),
   outputNames        (f.outputNames),
   //inputArgMap       (f.inputArgMap), // do I want to do this?
   //outputArgMap      (f.outputArgMap), // do I want to do this?
   objectStore        (NULL),
   globalObjectStore  (NULL),
   solarSys           (NULL),
   internalCoordSys   (NULL),
   forces             (NULL),
   fcs                (NULL),
   fcsFinalized       (f.fcsFinalized),
   validator          (f.validator),
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
   
   functionPath       = f.functionPath;
   functionName       = f.functionName;
   objectStore        = NULL;
   globalObjectStore  = NULL;
   solarSys           = f.solarSys;
   internalCoordSys   = f.internalCoordSys;
   forces             = f.forces;
   fcs                = NULL;
   fcsFinalized       = f.fcsFinalized;
   validator          = f.validator;
   scriptErrorFound   = f.scriptErrorFound;
   objectsInitialized = f.objectsInitialized;
   inputNames         = f.inputNames;
   outputNames        = f.outputNames;
   //inputArgMap       = f.inputArgMap;   // do I want to do this?
   //outputArgMap      = f.outputArgMap;  // do I want to do this?
   
   return *this;
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
   #ifdef DEBUG_FUNCTION_SET
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
// bool Initialize()  [default implementation]
//------------------------------------------------------------------------------
bool Function::Initialize()
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
// bool Function::Finalize()  [default implementation]
//------------------------------------------------------------------------------
void Function::Finalize()
{
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
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetInputElementWrapper - for wrapper name \"%s\"\n", forName.c_str());
   MessageInterface::ShowMessage
      ("   wrapper=<%p>, wrapper type = %d\n", wrapper, wrapper->GetWrapperType());
   #endif
   if (inputArgMap.find(forName) == inputArgMap.end())
   {
      std::string errMsg = "Unknown input argument \"" + forName;
      errMsg += "\" for function \"" + functionName + "\"";
      throw FunctionException(errMsg);
   }
   
   inputArgMap[forName] = wrapper;
   
   //@note old inputWrappers are deleted in the FunctionManager::CreateFunctionArgWrappers()
   // before creates new wrappers for input arguments
   
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
      MessageInterface::ShowMessage("Function::GetOutputArgument - asking for  \"%s\"\n",
            byName.c_str());
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
       ew, ew->GetDataType());
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
      ("Function::AddAutomaticObject() <%p>'%s' entered, name='%s', obj=<%p> '%s', "
       "alreadyManaged=%d, objectStore=<%p>\n", this, GetName().c_str(),
       withName.c_str(), obj, obj->GetName().c_str(), alreadyManaged, objectStore);
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
         ("Function::AddAutomaticObject(), ownerName='%s', owner=<%p><%s>'%s'\n",
          ownerName.c_str(), owner, owner ? owner->GetTypeName().c_str() : "NULL",
          owner ? owner->GetName().c_str() : "NULL");
         #ifdef DEBUG_OBJECT_MAP
         ShowObjectMap(objectStore, "In Function::AddAutomaticObject", "objectStore");
         #endif
      #endif
      if (owner == NULL)
      {
         FunctionException fe;;
         fe.SetDetails("Cannot find the object named \"%s\" in the function "
                       "object store", ownerName.c_str());
         throw fe;
      }
      
      GmatBase *refObj = obj->GetRefObject(owner->GetType(), ownerName);
      if (owner != refObj)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** The ref object \"%s\" of the Parameter \"%s\""
             "does not points to object in object store", ownerName.c_str(),
             withName.c_str());
         FunctionException fe;;
         fe.SetDetails("The ref object \"%s\" of the Parameter \"%s\""
                       "does not points to object in object store", ownerName.c_str(),
                       withName.c_str());
         throw fe;
      }
   }
   
   if (alreadyManaged)
   {
      if (find(sandboxObjects.begin(), sandboxObjects.end(), obj) ==
          sandboxObjects.end() &&
          automaticObjectMap.find(withName) == automaticObjectMap.end())         
      {
         #ifdef DEBUG_AUTO_OBJ
         MessageInterface::ShowMessage
            ("   Adding <%p>'%s' to sandboxObjects\n", obj, obj->GetName().c_str());
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
         ("   Found oldObj=<%p><%s> '%s'\n", oldObj, oldObj ?
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
   
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("Function::AddAutomaticObject() <%p>'%s' leaving, <%p>'%s' inserted to "
       "automaticObjectMap\n", this, GetName().c_str(), obj, withName.c_str());
   #endif
   
   automaticObjectMap.insert(std::make_pair(withName,obj));
   
   #ifdef DEBUG_AUTO_OBJ
   ShowObjectMap(&automaticObjectMap, "In AddAutomaticObject()", "automaticObjectMap");
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
// ObjectMap& GetAutomaticObjects()
//------------------------------------------------------------------------------
ObjectMap& Function::GetAutomaticObjects()
{
   return automaticObjectMap;
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
   if (id == FUNCTION_NAME)
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
      return functionPath;
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
bool Function::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetStringParameter() entered, id=%d, value=%s\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case FUNCTION_INPUT:
      {
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
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Function::FindObject(const std::string &name)
{
   std::string newName = name;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find('(');
   if (index != name.npos)
      newName = name.substr(0, index);
   
   // Check for the object in the Local Object Store (LOS) first
   if (objectStore && objectStore->find(newName) != objectStore->end())
      return (*objectStore)[newName];
   
   // If not found in the LOS, check the Global Object Store (GOS)
   if (globalObjectStore && globalObjectStore->find(newName) != globalObjectStore->end())
      return (*globalObjectStore)[newName];

   // Let's try SolarSystem (loj: 2008.06.12)
   if (solarSys && solarSys->GetBody(newName))
      return (GmatBase*)(solarSys->GetBody(newName));
   
   return NULL;
}


//------------------------------------------------------------------------------
// void ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs)
//------------------------------------------------------------------------------
void Function::ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs)
{
   #ifdef DEBUG_ARG_MAP
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
      if (omi->second != NULL &&
          find(sandboxObjects.begin(), sandboxObjects.end(), omi->second) ==
          sandboxObjects.end())
      {
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
      MessageInterface::ShowMessage("ObjectMap is NULL\n");
      return;
   }
   
   std::string objMapName = mapName;
   if (objMapName == "")
      objMapName = "object map";
   
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Here is %s <%p>, it has %d objects\n", objMapName.c_str(), objMap, objMap->size());
   for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
      MessageInterface::ShowMessage
         ("   %40s  <%p><%s>\n", i->first.c_str(), i->second,
          i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
}


//------------------------------------------------------------------------------
// void ShowObjects(const std::string &title)
//------------------------------------------------------------------------------
void Function::ShowObjects(const std::string &title)
{
   MessageInterface::ShowMessage("%s\n", title.c_str());
   MessageInterface::ShowMessage("this=<%p>, functionName='%s'\n", this, functionName.c_str());
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("solarSys         = <%p>\n", solarSys);
   MessageInterface::ShowMessage("internalCoordSys = <%p>\n", internalCoordSys);
   MessageInterface::ShowMessage("forces           = <%p>\n", forces);
   MessageInterface::ShowMessage
      ("Here is objectStore <%p>, it has %d objects\n", objectStore,
       objectStore->size());
   for (ObjectMap::iterator i = objectStore->begin(); i != objectStore->end(); ++i)
      MessageInterface::ShowMessage
         ("   %30s  <%p><%s>\n", i->first.c_str(), i->second,
          i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("Here is globalObjectStore <%p>, it has %d objects\n", globalObjectStore,
       globalObjectStore->size());
   for (ObjectMap::iterator i = globalObjectStore->begin(); i != globalObjectStore->end(); ++i)
      MessageInterface::ShowMessage
         ("   %30s  <%p><%s>\n", i->first.c_str(), i->second,
          i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   MessageInterface::ShowMessage("========================================\n");   
}

