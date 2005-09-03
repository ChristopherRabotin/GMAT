//$Header$
//------------------------------------------------------------------------------
//                            BeginFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/08/30
//
/**
 * Implementation code for the BeginFunction command, a wrapper that manages the
 * commands in a GMAT function.
 */
//------------------------------------------------------------------------------
 

#include "BeginFunction.hpp"
#include "MessageInterface.hpp"


const std::string 
BeginFunction::PARAMETER_TEXT[BeginFunctionParamCount - GmatCommandParamCount] =
{
   "FunctionName",
   "Input",
   "Output",
   "CallFunctionInput",
   "CallFunctionOutput"
};

const Gmat::ParameterType 
BeginFunction::PARAMETER_TYPE[BeginFunctionParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE
};



BeginFunction::BeginFunction() :
   GmatCommand          ("BeginFunction"),
   functionName         (""),
   gfun                 (NULL),
   transientForces      (NULL)
{
}


BeginFunction::~BeginFunction()
{
   ClearReturnObjects();
}


BeginFunction::BeginFunction(const BeginFunction& bf) :
   GmatCommand          (bf),
   functionName         (bf.functionName),
   gfun                 (NULL),
   transientForces      (NULL)
{
}


BeginFunction& BeginFunction::operator=(const BeginFunction& bf)
{
   if (this != &bf)
   {
      functionName = bf.functionName;
      transientForces = bf.transientForces;
   }
   
   return *this;
}
 
   
GmatBase* BeginFunction::Clone() const
{
   return new BeginFunction(*this);
}

// Function used to put objects in the local onject map
bool BeginFunction::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name)
{
   bool retval = false;
   if (localMap.find(name) == localMap.end())
   {
      localMap[name] = obj;
      retval = true;
   }
   else
   {
      MessageInterface::ShowMessage(
         "Attempted to add the object %s which is already in the "
         "GmatFunction %s\n", name.c_str(), functionName.c_str());
   }
   
   return retval;
}


bool BeginFunction::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   return true;
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
std::string BeginFunction::GetParameterText(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < BeginFunctionParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
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
Integer BeginFunction::GetParameterID(const std::string &str) const
{
   for (int i = 0; i < BeginFunctionParamCount - GmatCommandParamCount; ++i)
      if (PARAMETER_TEXT[i] == str)
         return i+GmatCommandParamCount;
         
   return GmatCommand::GetParameterID(str);
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
Gmat::ParameterType BeginFunction::GetParameterType(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < BeginFunctionParamCount))
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   return GmatCommand::GetParameterType(id);
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
std::string BeginFunction::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool BeginFunction::IsParameterReadOnly(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < BeginFunctionParamCount))
      return true;
   return GmatCommand::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Text description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool BeginFunction::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
std::string BeginFunction::GetStringParameter(const Integer id) const
{
   if (id == FUNCTION_NAME)
   {
      return functionName;
   }
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <index> Index for array parameter values.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const Integer id, 
                                              const Integer index) const
{
   return GmatCommand::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets an array do std::strings.
 * 
 * @param <id> Integer ID of the array.
 * 
 * @return A reference to the array.
 */
//------------------------------------------------------------------------------
const StringArray& BeginFunction::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {      
      case INPUTS:
         return inputs;
      
      case OUTPUTS:
         return outputs;
      
      case INPUT_OBJECT_NAMES:
         return inputObjects;
      
      case OUTPUT_OBJECT_NAMES:
         return outputObjects;
      
      default:
         ;     // Do nothing if it isn't a BeginFunction parameter
   }
   
   return GmatCommand::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const Integer id, 
                                       const std::string &value)
{
   bool retval = false;
   
   switch (id)
   {
      case FUNCTION_NAME:
         if (value != "")
         {
            functionName = value;
            retval = true;
         }
         break;
      
      case INPUTS:
         if ((value != "") && 
             (find(inputs.begin(), inputs.end(), value) == inputs.end()))
         {
            inputs.push_back(value);
            retval = true;
         }
         break;
      
      case OUTPUTS:
         if ((value != "") && 
             (find(outputs.begin(), outputs.end(), value) == outputs.end()))
         {
            outputs.push_back(value);
            retval = true;
         }
         break;
      
      case INPUT_OBJECT_NAMES:
         if ((value != "") && 
             (find(inputObjects.begin(), inputObjects.end(), value) == 
              inputObjects.end()))
         {
            inputObjects.push_back(value);
            retval = true;
         }
         break;
      
      case OUTPUT_OBJECT_NAMES:
         if ((value != "") && 
             (find(outputObjects.begin(), outputObjects.end(), value) == 
              outputObjects.end()))
         {
            outputObjects.push_back(value);
            retval = true;
         }
         break;
      
      default:
         ;     // Do nothing if it isn't a BeginFunction parameter
   }
   
   if (retval)
      return true;
   
   return GmatCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value, 
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter in an array.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index into the array for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const Integer id, 
                                       const std::string &value, 
                                       const Integer index)
{
   return GmatCommand::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <label> String identifier for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter in an array.
 * 
 * @param <label> String identifier for the parameter.
 * @param <index> Index into the array for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const std::string &label, 
                                              const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets an array do std::strings.
 * 
 * @param <label> Text name for the array.
 * 
 * @return A reference to the array.
 */
//------------------------------------------------------------------------------
const StringArray& 
         BeginFunction::GetStringArrayParameter(const std::string &label) const
{
   return BeginFunction::GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <label> String identifier for the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const std::string &label, 
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const Real value, 
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter in an array.
 * 
 * @param <label> String identifier for the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index into the array for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const std::string &label, 
                                       const std::string &value, 
                                       const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


bool BeginFunction::TakeAction(const std::string &action, 
                               const std::string &actionData)
{
   if (action == "ClearLocalData")
   {
      inputObjects.clear();
      outputObjects.clear();
      ClearInputMap();
   }
   
   return true;
}


void BeginFunction::ClearInputMap()
{
   for (std::map <std::string, GmatBase *>::iterator i = localMap.begin(); 
        i != localMap.end(); ++i)
   {
      delete i->second;
   }
   localMap.clear();
}


void BeginFunction::ClearReturnObjects()
{
   for (ObjectArray::iterator i = returnObjects.begin(); 
        i != returnObjects.end(); ++i)
   {
      delete *i;
   }
   returnObjects.clear();
}


void BeginFunction::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   transientForces = tf;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the command stream for a GmatCommand.
 * 
 * This method maps the objects in the function call to the names used in the
 * function definition, and initializes the commands in the command stream using 
 * these objects.  The objects are cloned; hence copy constructors must be 
 * correct for successful initialization.
 * 
 * @return true always.
 */
//------------------------------------------------------------------------------
bool BeginFunction::Initialize()
{
   // Find the GmatFunction
   if (objectMap->find(functionName) == objectMap->end())
      throw CommandException("Error setting up GMAT function '" + functionName +
         "'; there is no GmatFunction object with that name.");
   
   gfun = (GmatFunction *)((*objectMap)[functionName]);
   
   if (gfun->GetTypeName() != "GmatFunction")
      throw CommandException("Object type Error: The object named '" + 
         functionName +
         "' is a " + gfun->GetTypeName() + ", but a GmatFunction is required.");
   
   if (inputObjects.size() != inputs.size())
   {
      std::string errmsg = 
         "Size mismatch between the inputs and expected inputs to the Gmat "
         "function '";
      errmsg += functionName + "'; \n  Inputs: (";
      for (StringArray::iterator i = inputObjects.begin();
           i != inputObjects.end(); ++i)
         errmsg += (*i) + ", ";
      errmsg += ")\n  Expected: (";
      for (StringArray::iterator i = inputs.begin();
           i != inputs.end(); ++i)
         errmsg += (*i) + ", ";
      errmsg += ")";
            
      throw CommandException(errmsg);
   }
   
   // Fill in the local object array with clones of the expected inputs
   for (UnsignedInt i = 0; i < inputObjects.size(); ++i)
   {
      if (objectMap->find(inputObjects[i]) == objectMap->end())
      {
         std::string errmsg = "Error initializing GmatFunction '";
         throw CommandException(errmsg + functionName + "': could not find " +
            "an object named '" + (inputObjects[i]) + "'");
      }
      GmatBase *inobj = (*objectMap)[inputObjects[i]]->Clone();
      // Give the clone the local variable name
      inobj->SetName(inputs[i]);
      SetRefObject(inobj, Gmat::UNKNOWN_OBJECT, inputs[i]);
   }
   
   // Now pretend like we're a Sandbox!
   GmatBase *obj;
   GmatCommand *cmd;
   std::map<std::string, GmatBase *>::iterator omi;

   // Init local objects
   for (omi = localMap.begin(); omi != localMap.end(); ++omi)
   {
      obj = omi->second;
      obj->SetSolarSystem(solarSys);
      
      // Set J2000 Body for all SpacePoint derivatives before anything else
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Setting J2000 Body for %s in the Sandbox\n",
               obj->GetName().c_str());
         #endif
         SpacePoint *spObj = (SpacePoint *)obj;
         SpacePoint *j2k = FindSpacePoint(spObj->GetJ2000BodyName());
         if (j2k)
            spObj->SetJ2000Body(j2k);
         else
            throw CommandException("GmatFunction did not find the "
               "Spacepoint \"" +
               spObj->GetJ2000BodyName() + "\"");
      }
   }
   
   // Add the parameters and global coordinate systems from the Sandbox
   for (omi = objectMap->begin(); omi != objectMap->end(); ++omi)
   {
      obj = omi->second;
      // Set J2000 Body for all SpacePoint derivatives before anything else
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         SetRefObject(obj, obj->GetType(), obj->GetName());
      }
      if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
      {
         std::string csName = obj->GetName();
         if ((csName == "EarthMJ2000Eq") || 
             (csName == "EarthMJ2000Ec") || 
             (csName == "EarthFixed")) 
            SetRefObject(obj, obj->GetType(), obj->GetName());
      }
   }
   
   cmd = next;
   while (cmd)
   {
      cmd->SetObjectMap(&localMap);
      cmd->SetSolarSystem(solarSys);

      // Handle nested GmatFunctions
      if (cmd->GetTypeName() == "CallFunction")
      {
         // Check to see if it is a GmatFunction
         std::string funName = cmd->GetStringParameter("FunctionName");
         if (objectMap->find(funName) == objectMap->end())
            throw CommandException("The GmatFunction '" + functionName +
               "' references the function '" + funName + 
               "', which cannot be found.");
         GmatFunction *fun = (GmatFunction*)(*objectMap)[funName];
         if (fun->GetTypeName() == "GmatFunction")
         {
//            /// @todo Make the GmatFunction file name handling more robust
//            std::string pathAndName = 
//               fun->GetStringParameter(fun->GetParameterID("FunctionPath"));
//            if (pathAndName == "")
//               pathAndName = funName + ".gmf";
//            
//            GmatCommand *funStream = 
//               moderator->InterpretGmatFunction(pathAndName);
//            if (!current->SetRefObject(funStream, Gmat::COMMAND, ""))
//            {
//               std::string errstr = "Error setting the GmatFunction commands "
//                  "for the script line\n  '";
//               throw SandboxException(errstr +
//                  current->GetGeneratingString(Gmat::SCRIPTING) + "'");
//            }
            throw CommandException(
               "Nesting is currently disabled for GmatFunctions");
         }
      }
      
      bool rv = cmd->Initialize();
      if (!rv)
         return false;

      cmd->SetTransientForces(transientForces);
      cmd = cmd->GetNext();
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Runs the command.  This is a NoOp for the BeginFunction command
 * 
 * @return true always.
 */
//------------------------------------------------------------------------------
bool BeginFunction::Execute()
{
   return true;
}


//------------------------------------------------------------------------------
// SpacePoint* FindSpacePoint(const std::string &spname)
//------------------------------------------------------------------------------
/**
 *  Finds a SpacePoint by name.
 *
 *  @param <spname> The name of the SpacePoint.
 *
 *  @return A pointer to the SpacePoint, or NULL if it does not exist in the
 *          Sandbox.
 */
//------------------------------------------------------------------------------
SpacePoint* BeginFunction::FindSpacePoint(const std::string &spName)
{
   SpacePoint *sp = solarSys->GetBody(spName);

   if (sp == NULL)
   {
      if (objectMap->find(spName) != objectMap->end())
      {
         if ((*objectMap)[spName]->IsOfType(Gmat::SPACE_POINT))
            sp = (SpacePoint*)((*objectMap)[spName]);
      }
   }

   return sp;
}

