//$Id$
//------------------------------------------------------------------------------
//                                   Function
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Funtion base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------

#include "Function.hpp"
#include "FunctionException.hpp" // for exception
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION
//#define DEBUG_FUNCTION_SET

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
   Gmat::STRING_TYPE,         // "FunctionPath",
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
   GmatBase          (Gmat::FUNCTION, typeStr, name),
   functionPath      (""),
   functionName      (""),
   objectStore       (NULL),
   globalObjectStore (NULL),
   solarSys          (NULL),
   forces            (NULL),
   fcs               (NULL)
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
   GmatBase          (f),
   functionPath      (f.functionPath),
   functionName      (f.functionName),
   inputNames        (f.inputNames),
   outputNames       (f.outputNames),
   //inputArgMap       (f.inputArgMap), // do I want to do this?
   //outputArgMap      (f.outputArgMap), // do I want to do this?
   objectStore       (NULL),
   globalObjectStore (NULL),
   solarSys          (NULL),
   forces            (NULL),
   fcs               (NULL)
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
   
   functionPath      = f.functionPath;
   functionName      = f.functionName;
   objectStore       = NULL;
   globalObjectStore = NULL;
   solarSys          = f.solarSys;
   forces            = f.forces;
   fcs               = NULL;
   inputNames        = f.inputNames;
   outputNames       = f.outputNames;
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
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Function::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif
   
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;   
}



//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Function::Initialize()
{
   #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage("Entering Function::Initialize for function %s\n",
            functionName.c_str());
      MessageInterface::ShowMessage("   and FCS is %s set.\n", (fcs? "correctly" : "NOT"));
      MessageInterface::ShowMessage("   Pointer for FCS is %p\n", fcs);
      MessageInterface::ShowMessage("   First command in fcs is %s\n",
            (fcs->GetTypeName()).c_str());         
   #endif
   if (!fcs) return false;
   // Initialize the Validator - I think I need to do this each time - or do I?
   validator.SetSolarSystem(solarSys);
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objectStore->begin(); omi != objectStore->end(); ++omi)
      store.insert(std::make_pair(omi->first, omi->second));
   for (omi = globalObjectStore->begin(); omi != globalObjectStore->end(); ++omi)
      store.insert(std::make_pair(omi->first, omi->second));
   validator.SetObjectMap(&store);
   
   // add automatic objects to the FOS
   for (omi = automaticObjects.begin(); omi != automaticObjects.end(); ++omi)
   {
      if (objectStore->find(omi->first) == objectStore->end())
         objectStore->insert(std::make_pair(omi->first, omi->second));
   }
   
   
   GmatCommand *current = fcs;
   while (current)
   {
      #ifdef DEBUG_FUNCTION
         if (!current)  MessageInterface::ShowMessage("Current is NULL!!!\n");
         else MessageInterface::ShowMessage("   Now about to initialize command %s\n",
               (current->GetName()).c_str());         
      #endif
      current->SetObjectMap(objectStore);
      current->SetGlobalObjectMap(globalObjectStore);
      current->SetSolarSystem(solarSys);
      current->SetTransientForces(forces);      
      if (!(current->Initialize()))
         return false;
      current = current->GetNext();
   }
   return true;
}


//------------------------------------------------------------------------------
// bool Function::Execute()
//------------------------------------------------------------------------------
bool Function::Execute()
{
   if (!fcs) return false;
   GmatCommand *current = fcs;
   while (current)
   {
      if (!(current->Execute()))
         return false;
      current = current->GetNext();
   }
   // create output wrappers and put into map
   GmatBase *obj;
   for (unsigned int jj = 0; jj < outputNames.size(); jj++)
   {
      if (!(obj = FindObject(outputNames.at(jj))))
      {
         std::string errMsg = "Function: Output \"" + outputNames.at(jj);
         errMsg += " not found for function \"" + functionName + "\"";
         throw FunctionException(errMsg);
      }
      ElementWrapper *outWrapper = validator.CreateElementWrapper(outputNames.at(jj));
      outWrapper->SetRefObject(obj); 
      outputArgMap[outputNames.at(jj)] = outWrapper;
      #ifdef DEBUG_FUNCTION // --------------------------------------------------- debug ---
         MessageInterface::ShowMessage("Function: Output wrapper created for %s\n", (outputNames.at(jj)).c_str());
      #endif // -------------------------------------------------------------- end debug ---
   }
   Finalize();
   return true; 
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real Function::Evaluate()
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Function::Evaluate() <%p><%s> entered.\n", this, this->GetName().c_str());
   #endif
   
   if (outputWrapperTypes.size() == 0)
      throw FunctionException
         ("The output argument of function \"" + functionName + "\" is not set.");
   
   if (!Execute())
      throw FunctionException("Failed to execute function \"" + functionName + "\"");

   // To be implemented
   
   Real r = -999.999;
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage("==> Function::Evaluate() returning %.9f\n", r);
   #endif
   
   return r;
}


//------------------------------------------------------------------------------
// Rmatrix MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix Function::MatrixEvaluate()
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Function::MatrixEvaluate() <%p><%s> entered.\n", this, this->GetName().c_str());
   #endif
   
   if (outputWrapperTypes.size() == 0)
      throw FunctionException
         ("The output argument of function \"" + functionName + "\" is not set.");
   
   if (!Execute())
      throw FunctionException("Failed to execute function \"" + functionName + "\"");
   
   // To be implemented
   Rmatrix rmat;
   rmat.SetSize(outputRowCounts[0], outputColCounts[0]);
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Function::MatrixEvaluate() returning %s\n", rmat.ToString().c_str());
   #endif
   
   return rmat;
}

void Function::Finalize()
{
   ; // @todo - finalize anything else that needs it as well
   GmatCommand *current = fcs;
   while (current)
   {
      #ifdef DEBUG_FUNCTION
         if (!current)  MessageInterface::ShowMessage("Current is NULL!!!\n");
         else MessageInterface::ShowMessage("   Now about to finalize (call RunComplete on) command %s\n",
               (current->GetName()).c_str());         
      #endif
      current->RunComplete();
      current = current->GetNext();
   }
}

void Function::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   #ifdef DEBUG_FM_EXECUTE // ------------------------------------------------- debug ---
      MessageInterface::ShowMessage("Entering Function::SetObjectMap:\n");
      std::map<std::string, GmatBase *>::iterator omi;
      GmatBase *objInMap;
      std::string strInMap;
      for (omi = map.begin(); omi != map.end(); ++omi)
      {
         strInMap = omi->first;
         objInMap = omi->second;
         MessageInterface::ShowMessage("  %s, which is of type %s, with pointer %p\n",
               strInMap.c_str(), (objInMap->GetTypeName()).c_str(), objInMap);
      }
   #endif // -------------------------------------------------------------- end debug ---
   objectStore = map;
}

void Function::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   globalObjectStore = map;
}

void Function::SetSolarSystem(SolarSystem *ss)
{
   solarSys = ss;
}

void Function::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   forces = tf;
}

bool Function::IsFunctionControlSequenceSet()
{
   if (fcs != NULL) return true;
   return false;
}

bool Function::SetFunctionControlSequence(GmatCommand *cmd)
{
   #ifdef DEBUG_FUNCTION
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

bool Function::SetInputElementWrapper(const std::string &forName, ElementWrapper *wrapper)
{
   if (inputArgMap.find(forName) == inputArgMap.end())
   {
      std::string errMsg = "Unknown input argument \"" + forName;
      errMsg += "\" for function \"" + functionName + "\"";
      throw FunctionException(errMsg);
   }
   inputArgMap[forName] = wrapper;
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


ElementWrapper* Function::GetOutputArgument(const std::string &byName)
{
   if (outputArgMap.find(byName) == outputArgMap.end())
   {
      std::string errMsg = "Function error: output \"" + byName;
      errMsg += "\" from function \"" + functionName;
      errMsg += "\" does not exist.\n";
      throw FunctionException(errMsg);
   }
   return outputArgMap[byName];
}

void Function::AddAutomaticObject(const std::string &withName, GmatBase *obj)
{
   automaticObjects.insert(std::make_pair(withName,obj));
}

StringObjectMap Function::GetAutomaticObjects() const
{
   return automaticObjects;
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
      
      inputArgMap.clear();
      outputArgMap.clear();
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
            //inputArgMap[value] = NULL;
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
            //outputArgMap[value] = NULL;
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

//bool Function::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                            const std::string &name);
//{
//   if (type == Gmat::FUNCTION)
//      
//}

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
   if (objectStore->find(newName) == objectStore->end())
   {
     // If not found in the LOS, check the Global Object Store (GOS)
      if (globalObjectStore->find(newName) == globalObjectStore->end())
         return NULL;
      else return (*globalObjectStore)[newName];
   }
   else
      return (*objectStore)[newName];
   return NULL; // should never get to this point
}

