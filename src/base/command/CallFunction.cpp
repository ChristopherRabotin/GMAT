//$Id$
//------------------------------------------------------------------------------
//                                 CallFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallFunction command class
 */
//------------------------------------------------------------------------------
#include "CallFunction.hpp"
#include "BeginFunction.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "MessageInterface.hpp"

#include <sstream>

#if defined __USE_MATLAB__
#include "MatlabInterface.hpp"     // for Matlab Engine functions
#endif

//#define DEBUG_CALL_FUNCTION
//#define DEBUG_SEND_PARAM
//#define DEBUG_UPDATE_VAR
//#define DEBUG_UPDATE_OBJECT
//#define DEBUG_USE_ARRAY
//#define DEBUG_GMAT_FUNCTION_INIT
//#define DEBUG_GET_OUTPUT


//---------------------------------
// static data
//---------------------------------
const std::string
CallFunction::PARAMETER_TEXT[CallFunctionParamCount - GmatCommandParamCount] =
{
   "FunctionName",
   "AddInput",
   "AddOutput",
   "CommandStream",
};


const Gmat::ParameterType
CallFunction::PARAMETER_TYPE[CallFunctionParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::OBJECT_TYPE,
};


//------------------------------------------------------------------------------
// CallFunction::CallFunction()
//------------------------------------------------------------------------------
CallFunction::CallFunction() :
   GmatCommand      ("CallFunction"),
   callcmds         (NULL),
   mFunction        (NULL),
   mFunctionName    (""),
   internalCoordSys (NULL)
{
   mNumInputParams = 0;
   mNumOutputParams = 0;

   parameterCount = CallFunctionParamCount;
}


//------------------------------------------------------------------------------
// ~CallFunction()
//------------------------------------------------------------------------------
CallFunction::~CallFunction()
{
   if (callcmds)
      delete callcmds;
}


//------------------------------------------------------------------------------
// CallFunction(const CallFunction& cf) :
//------------------------------------------------------------------------------
CallFunction::CallFunction(const CallFunction& cf) :
   GmatCommand     (cf),
   callcmds        (NULL),
   mFunction       (cf.mFunction),
   mFunctionName   (cf.mFunctionName),
   internalCoordSys  (NULL)
{
   mNumInputParams = cf.mNumInputParams;
   mNumOutputParams = cf.mNumOutputParams;

   objectArray = cf.objectArray;
   mInputList = cf.mInputList;
   mOutputList = cf.mOutputList;
   callcmds = NULL;           // Commands must be reinitialized
   
   mInputListNames = cf.mInputListNames;
   mOutputListNames = cf.mOutputListNames;

   parameterCount = CallFunctionParamCount;
}


//------------------------------------------------------------------------------
// CallFunction& operator=(const CallFunction& cf)
//------------------------------------------------------------------------------
CallFunction& CallFunction::operator=(const CallFunction& cf)
{
   if (this == &cf)
      return *this;
   
   GmatCommand::operator=(cf);
   
   mFunction = cf.mFunction;
   mFunctionName = cf.mFunctionName;
   mNumInputParams = cf.mNumInputParams;
   mNumOutputParams = cf.mNumOutputParams;
   
   objectArray = cf.objectArray;
   mInputList = cf.mInputList;
   mOutputList = cf.mOutputList;
   callcmds = NULL;           // Commands must be reinitialized
   internalCoordSys = NULL;
   
   mInputListNames = cf.mInputListNames;
   mOutputListNames = cf.mOutputListNames;

   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CallFunction.
 *
 * @return clone of the CallFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CallFunction::Clone() const
{
   return (new CallFunction(*this));
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetParameterText(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::GetParameterText\n");
   #endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name.  (Used to
 *                indent commands)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this CallFunction.
 */
//------------------------------------------------------------------------------
const std::string& CallFunction::GetGeneratingString(Gmat::WriteMode mode,
                                                     const std::string &prefix,
                                                     const std::string &useName)
{
   std::string gen;
   
   // Build the local string
   if (mode != Gmat::NO_COMMENTS)
      gen = prefix + "GMAT ";
   
   if (mOutputListNames.size() > 0)
   {
      gen += "[";
      for (StringArray::iterator i = mOutputListNames.begin();
           i != mOutputListNames.end(); ++i)
      {
         if (i != mOutputListNames.begin())
            gen += ", ";
         gen += *i;
      }
      gen += "] = ";
   }
   
   gen += mFunctionName;
   
   if (mInputListNames.size() > 0)
   {
      gen += "(";
      for (StringArray::iterator i = mInputListNames.begin();
           i != mInputListNames.end(); ++i)
      {
         if (i != mInputListNames.begin())
            gen += ", ";
         gen += *i;
      }
      gen += ")";
   }
   
   generatingString = gen + ";";
   
   if (mode == Gmat::NO_COMMENTS)
      return generatingString;
   
   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer CallFunction::GetParameterID(const std::string &str) const
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::GetParameterID \n");
   #endif

   for (int i=GmatCommandParamCount; i<CallFunctionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType CallFunction::GetParameterType(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::GetParameterType\n");
   #endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetParameterTypeString(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::GetParameterTypeString\n");
   #endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - GmatCommandParamCount)];
   else
      return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::GetStringParameter\n");
   #endif

   switch (id)
   {
   case FUNCTION_NAME:
      return mFunctionName;
   default:
      return GmatCommand::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string CallFunction::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::SetStringParameter\n");
   #endif

   switch (id)
   {
   case FUNCTION_NAME:
      mFunctionName = value;
      return true;
   case ADD_INPUT:
      return AddInputParameter(value, mNumInputParams);
   case ADD_OUTPUT:
      return AddOutputParameter(value, mNumOutputParams);
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
   case ADD_INPUT:
      return AddInputParameter(value, index);
   case ADD_OUTPUT:
      return AddOutputParameter(value, index);
   default:
      return GmatCommand::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& CallFunction::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD_INPUT:
      return mInputListNames;
   case ADD_OUTPUT:
      return mOutputListNames;
   default:
      return GmatCommand::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& CallFunction::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool CallFunction::TakeAction(const std::string &action,
                        const std::string &actionData)
{
   if (action == "ClearInput")
   {
      ClearInputParameters();
      return true;
   }
   else if (action == "ClearOutput")
   {
      ClearOutputParameters();
      return true;
   }
   else if (action == "Clear")
   {
      ClearInputParameters();
      ClearOutputParameters();
      objectArray.clear();
      return true;
   }

   return GmatCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// StringArray GetRefObjectNameArray(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
StringArray CallFunction::GetRefObjectNameArray(const Gmat::ObjectType type) const
{
   StringArray result;

   switch (type) {
      case Gmat::PARAMETER:         // Input/Output
         for (unsigned int i=0; i<mInputListNames.size(); i++)
            result.push_back(mInputListNames[i]);
         for (unsigned int i=0; i<mOutputListNames.size(); i++)
            result.push_back(mOutputListNames[i]);
         return result;
      default:
         break;
   }

   return result;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool CallFunction::RenameRefObject(const Gmat::ObjectType type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("CallFunction::RenameRefObject() type=%d, oldName='%s', newName='%s'\n",
       type, oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::FUNCTION)
   {
      if (mFunctionName == oldName)
         mFunctionName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      // parameters - go through input and output
      for (unsigned int i=0; i<mInputListNames.size(); i++)
      {
         if (mInputListNames[i] == oldName)
         {
            mInputListNames[i] = newName;
            break;
         }
      }

      for (unsigned int i=0; i<mOutputListNames.size(); i++)
      {
         if (mOutputListNames[i] == oldName)
         {
            mOutputListNames[i] = newName;
            break;
         }
      }
   }
   // Since parameter name is composed of spacecraftName.dep.paramType or
   // burnName.dep.paramType, check the type first
   else if (type == Gmat::SPACECRAFT || type == Gmat::BURN ||
            type == Gmat::COORDINATE_SYSTEM || type == Gmat::CALCULATED_POINT)
   {
      
      for (UnsignedInt i=0; i<mInputListNames.size(); i++)
         if (mInputListNames[i].find(oldName) != std::string::npos)
            mInputListNames[i] =
               GmatStringUtil::Replace(mInputListNames[i], oldName, newName);
      
      for (UnsignedInt i=0; i<mOutputListNames.size(); i++)
         if (mOutputListNames[i].find(oldName) != std::string::npos)
            mOutputListNames[i] =
               GmatStringUtil::Replace(mOutputListNames[i], oldName, newName);
   }

   return true;
}


// Reference object accessor methods
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* CallFunction::GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name)
{
   switch (type)
   {
      case Gmat::PARAMETER:
         for (int i=0; i<mNumInputParams; i++)
         {
            if (mInputListNames[i] == name)
               return mInputList[i];
         }
         
         for (int i=0; i<mNumOutputParams; i++)
         {
            if (mOutputListNames[i] == name)
               return mOutputList[i];
         }
         
         throw GmatBaseException("ReportFile::GetRefObject() the object name: "
                           + name + "not found\n");
         
      case Gmat::FUNCTION:
         return mFunction;
         
      case Gmat::COMMAND:
         return callcmds;
         
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return GmatCommand::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type, ...
//------------------------------------------------------------------------------
/**
 * Sets reference object pointer.
 *
 * @return true if object successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool CallFunction::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   switch (type)
   {
   case Gmat::PARAMETER:
      for (int i=0; i<mNumInputParams; i++)
      {
         if (mInputListNames[i] == name)
         {
            mInputList[i] = (Parameter*)obj;
            return true;
         }
      }

      for (int i=0; i<mNumOutputParams; i++)
      {
         if (mOutputListNames[i] == name)
         {
            mOutputList[i] = (Parameter*)obj;
            return true;
         }
      }
      
   case Gmat::FUNCTION:
      mFunction = (Function *)obj;
      mFunctionName = name;
      return true;
      
   case Gmat::COMMAND:
      if (callcmds)
         delete callcmds;
      callcmds = (GmatCommand*)obj;
      return true;
      
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
ObjectArray& CallFunction::GetRefObjectArray(const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::PARAMETER:
      objectArray.clear();

      for (unsigned int i=0; i<mInputList.size(); i++)
         objectArray.push_back(mInputList[i]);
      
      for (unsigned int i=0; i<mOutputList.size(); i++)
         objectArray.push_back(mOutputList[i]);
      
      return objectArray;
      
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetReferenceObject call
   return GmatCommand::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CallFunction::Initialize()
{
   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("In CallFunction::Initialize()\n");
   #endif

   GmatCommand::Initialize();
   
   #ifdef DEBUG_CALL_FUNCTION_INIT
   std::map<std::string, GmatBase*>::iterator pos;
   GmatBase *obj;
   for (pos = objectMap->begin(); pos != objectMap->end(); ++pos)
   {
      obj = pos->second;
      MessageInterface::ShowMessage
         ("===> obj=%s type=%s, name=%s\n", (pos->first).c_str(),
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   }
   for (pos = globalObjectMap->begin(); pos != globalObjectMap->end(); ++pos)
   {
      obj = pos->second;
      MessageInterface::ShowMessage
         ("===> obj=%s type=%s, name=%s\n", (pos->first).c_str(),
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   }
   #endif
   
   GmatBase *mapObj;
   if ((mapObj = FindObject(mFunctionName))  == NULL)
      throw CommandException("CallFunction command cannot find Function " +
               mFunctionName + "\n");
   mFunction = (Function *)mapObj;

   
   // need to initialize input parameters
   mInputList.clear();
   
   for (StringArray::iterator i = mInputListNames.begin(); i != mInputListNames.end(); ++i)
   {
      if ((mapObj = FindObject(*i))  == NULL)
        throw CommandException("CallFunction command cannot find Parameter " +
           *i + " in script line\n   \"" +
           GetGeneratingString(Gmat::SCRIPTING) + "\"");
      
      #ifdef DEBUG_CALL_FUNCTION
         MessageInterface::ShowMessage("Adding input parameter %s\n", i->c_str());
      #endif
         
          mInputList.push_back((Parameter *)mapObj);
   }

   
   // need to initialize output parameters
   mOutputList.clear();
   
   for (StringArray::iterator i = mOutputListNames.begin();
   i != mOutputListNames.end();++i)
   {
      if ((mapObj = FindObject(*i))  == NULL)
        throw CommandException("CallFunction command cannot find Parameter " + (*i));

      #ifdef DEBUG_CALL_FUNCTION
         MessageInterface::ShowMessage("Adding output parameter %s\n", i->c_str());
      #endif
         
          mOutputList.push_back((Parameter *)mapObj);
   }
   
   
   if (mInputList.size() > 0)
      if (mInputList[0] == NULL)
      {
          MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "CallFunction::Initialize() CallFunction will not be created.\n"
             "The first parameter selected as input for the CallFunction is NULL\n");
          return false;
      }
   
   
   if (mOutputList.size() > 0)
      if (mOutputList[0] == NULL)
      {
          MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "CallFunction::Initialize() CallFunction will not be created.\n"
             "The first parameter selected as output for the CallFunction is NULL\n");
          return false;
      }
   
   
   bool rv = true;  // Initialization return value
   
   // Handle additional initialization for GmatFunctions
   if (mFunction->GetTypeName() == "GmatFunction")
   {
      #ifdef DEBUG_GMAT_FUNCTION_INIT
         MessageInterface::ShowMessage("Initializing GmatFunction '%s'\n",
            mFunction->GetName().c_str());
      #endif
         
      if (callcmds == NULL)
         throw CommandException(
            "Error initializing the function call for this command:\n" +
            GetGeneratingString(Gmat::SCRIPTING));
      
      callcmds->TakeAction("ClearLocalData");
      callcmds->SetPublisher(publisher);
      callcmds->SetObjectMap(objectMap);
      callcmds->SetGlobalObjectMap(globalObjectMap);
      callcmds->SetSolarSystem(solarSys);
      
      if (callcmds->GetTypeName() == "BeginFunction")
         ((BeginFunction *)callcmds)->SetInternalCoordSystem(internalCoordSys);
      
      // Pass in the input and output object names
      for (StringArray::iterator i = mInputListNames.begin();
           i != mInputListNames.end(); ++i)
         callcmds->SetStringParameter("CallFunctionInput", *i);
      
      for (StringArray::iterator i = mOutputListNames.begin();
           i != mOutputListNames.end(); ++i)
         callcmds->SetStringParameter("CallFunctionOutput", *i);
      
      rv = callcmds->Initialize();
   }
   
   return rv;
}


//------------------------------------------------------------------------------
// bool AddInputParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool CallFunction::AddInputParameter(const std::string &paramName, Integer index)
{
   if (paramName != "" && index == mNumInputParams)
   {
      mInputListNames.push_back(paramName);
      mNumInputParams = mInputListNames.size();
      mInputList.push_back(NULL);
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool AddOutputParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool CallFunction::AddOutputParameter(const std::string &paramName, Integer index)
{
   if (paramName != "" && index == mNumOutputParams)
   {
      mOutputListNames.push_back(paramName);
      mNumOutputParams = mOutputListNames.size();
      mOutputList.push_back(NULL);
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool CallFunction::Execute()
{
   bool status = false;

   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallFunction");

   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::Execute()\n");
   #endif

   #ifdef __USE_MATLAB__
      if (mFunction->GetTypeName() == "MatlabFunction")
      {
         status = ExecuteMatlabFunction();

         BuildCommandSummary(true);
         return status;
      }
   #endif
      
   if (mFunction->GetTypeName() == "GmatFunction")
   {
      bool rv = true;  // Initialization return value
      
      // There are still 2 things to do here, but let's get this working first:
      //
      // 1.  Add code to make the method reentrant, so a user can break while
      //     in the GmatFunction.
      // 2.  Add code so that GmatFunctions can nest -- right now, we'll break
      //     when the first EndFunction is encountered.
      
      GmatCommand *current = callcmds;
      while (current->GetTypeName() != "EndFunction")
      {
         // tried to reset objectmap - no need though
         
         MessageInterface::ShowMessage("CallFunction current cmd = %s\n   '%s'\n",
            current->GetTypeName().c_str(),
            current->GetGeneratingString().c_str());
         rv = current->Execute();
         
         if (!rv)
            throw CommandException("Could not execute command\n");
         
         current = current->GetNext();
      }
      
      // Now set the outputs.  current points to the EndFunction command, which
      // knows about these puppies!
      callcmds->TakeAction("SetReturnObjects");
      
      Integer memberNum = 0;
      for (StringArray::iterator i = mOutputListNames.begin();
           i != mOutputListNames.end(); ++i)
      {
         // Get the object that maps to the output name
         GmatBase *obj = callcmds->GetRefObject(Gmat::UNKNOWN_OBJECT, *i);

         // Use the assignment operator, called through the Copy method to set
         // the output value.  We may need to check the Copy methods for this
         // to work...
//      MessageInterface::ShowMessage("mOutputList[memberNum]->Copy(%s) with value %f\n",
//          obj->GetName().c_str(), ((Variable *)obj)->EvaluateReal());
         mOutputList[memberNum] = (Parameter *) obj;


//         mOutputList[memberNum]->Copy(obj);
//      MessageInterface::ShowMessage("mOutputList[memberNum]->Copy(obj)2 with value %f\n",
//          ((Variable *)mOutputList[memberNum])->EvaluateReal());
         ++memberNum;
      }
      status = true;
   }


   #ifdef DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("Executed command\n");
   #endif

   BuildCommandSummary(true);
   return status;
}


//------------------------------------------------------------------------------
// bool ExecuteMatlabFunction()
//------------------------------------------------------------------------------
bool CallFunction::ExecuteMatlabFunction()
{
   bool status = false;

   #ifdef __USE_MATLAB__
      MatlabInterface::Open();

      // set format long so that we don't loose precision between string transmission
      MatlabInterface::EvalString("format long");
      
      // add path to matlab workspace
      // send string to matlab - path(path ,'a:\')
      Integer pathId = mFunction->GetParameterID("FunctionPath");
      std::string thePath = mFunction->GetStringParameter(pathId);      
      
      // Clear last errormsg
      MatlabInterface::EvalString("clear errormsg");
      
      if (thePath != "")
      {
         std::string setPath = "path(path ,'" + thePath + "')";
         MatlabInterface::EvalString(setPath);
      }
      
      // send the in parameters
      for (unsigned int i=0; i<mInputList.size(); i++)
      {
         Parameter *param = (Parameter *)mInputList[i];
         SendInParam(param);
      }
      
      //  Eval String
      std::string evalString = FormEvalString();
      EvalMatlabString(evalString);
      
      // get the value for the out parameters
      GetOutParams();
      
      status = true;
      
   #endif
      
   return status;
}


//------------------------------------------------------------------------------
// void SendInParam(Parameter *param)
//------------------------------------------------------------------------------
void CallFunction::SendInParam(Parameter *param)
{
   #ifdef DEBUG_SEND_PARAM
   MessageInterface::ShowMessage("CallFunction::SendInParam()");
   #endif
   
   #ifdef __USE_MATLAB__
      if (param == NULL)
      {
         MessageInterface::ShowMessage("Parameter was null");
         return;
      }
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         ("Parameter name=%s, type=%s\n", param->GetName().c_str(),
          param->GetTypeName().c_str());
      #endif
      
      if (param->GetTypeName() == "Array")
      {
         Array *array = (Array *)param;
         int numRows = array->GetIntegerParameter("NumRows");
         int numCols = array->GetIntegerParameter("NumCols");
         std::ostringstream os;
         
         os.precision(18);
         
         Rmatrix rmatrix = array->GetRmatrix();
         
         for (int j=0; j<numRows; j++)
         {
            os << "[";
            for (int k=0; k<numCols; k++)
               os << rmatrix(j, k) << ",";
            os << "], \n";
         }
         
         std::string inParamString = array->GetName() + " = [" +os.str() + "];";
         EvalMatlabString(inParamString);
      }
      else if (param->GetTypeName() == "Variable")
      {
          std::ostringstream os;
          os.precision(18);
          os << param->EvaluateReal();
          
          std::string inParamString = param->GetName() +" = " + os.str() +";";
          
          #ifdef DEBUG_SEND_PARAM
          MessageInterface::ShowMessage
             ("Sent string %s to matlab\n", inParamString.c_str());
          #endif
          
          EvalMatlabString(inParamString);
      }
      else if (param->GetTypeName() == "String")
      {
         StringVar *stringVar = (StringVar *)param;
         std::string inParamString = param->GetName() +" = '" +
            stringVar->GetString() +"';";
         
         EvalMatlabString(inParamString);
      }
      else // it is any object
      {
         if (param->GetTypeName() == "Spacecraft")
            param->TakeAction("UpdateEpoch");
         
         std::string inParamString =
            param->GetGeneratingString(Gmat::MATLAB_STRUCT);
         
         #ifdef DEBUG_SEND_PARAM
         MessageInterface::ShowMessage
            ("Generated param string :\n%s\n", inParamString.c_str());
         #endif
         
         EvalMatlabString(inParamString);
      }
   #endif  //__USE_MATLAB__
}


//------------------------------------------------------------------------------
// void GetOutParams()
//------------------------------------------------------------------------------
void CallFunction::GetOutParams()
{
#ifdef __USE_MATLAB__
   try
   {
      for (unsigned int i=0; i<mOutputList.size(); i++)
      {
         Parameter *param = (Parameter *)mOutputList[i];
         std::string varName = param->GetName();
         
         #ifdef DEBUG_GET_OUTPUT
         MessageInterface::ShowMessage
            ("CallFunction::GetOutParams() OutParamType=%s, name=%s\n",
             param->GetTypeName().c_str(), varName.c_str());
         #endif
         
         if (param->GetTypeName() == "Array")
         {
            Array *array = (Array *)param;
            int numRows = array->GetIntegerParameter("NumRows");
            int numCols = array->GetIntegerParameter("NumCols");
            int totalCells = numRows * numCols;            
            double *outArray = new double[totalCells];
            
            MatlabInterface::GetRealArray(varName, totalCells, outArray);
            
            // create rmatrix
            Rmatrix rmatrix = Rmatrix (numRows, numCols);
            
            for (int j=0; j<numCols; j++)
               for (int k=0; k<numRows; k++)
                  rmatrix(k, j) = outArray[(j*numRows) + k];
            
            #ifdef DEBUG_USE_ARRAY
            for (int j=0; j<numRows; j++)
            {
               for (int k=0; k<numCols; k++)
                  MessageInterface::ShowMessage("%f\t", rmatrix(j, k));
               MessageInterface::ShowMessage("\n");
            }
            #endif
            
            // assign rmatrix to array
            array->SetRmatrixParameter("RmatValue", rmatrix);
            
            delete [] outArray;
            
         }
         else if (param->GetTypeName() == "Variable")
         {
            double *outArray = new double[1];
            
            MatlabInterface::GetRealArray(varName, 1, outArray);
            
            param->SetReal(outArray[0]);
            std::ostringstream ss;
            ss.precision(18);
            ss << outArray[0];
            param->SetStringParameter("Expression", ss.str());
            
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               ("The EvaluateReal is %f\n",  param->EvaluateReal());
            MessageInterface::ShowMessage
               ("The GetReal is %f\n", param->GetReal());
            #endif
            
            delete [] outArray;
         }
         else if (param->GetTypeName() == "String")
         {
            // need to output string value to buffer         
            char buffer[512];
            MatlabInterface::OutputBuffer(buffer, 512);
            EvalMatlabString(varName);
            
            // get rid of "var ="
            char *ptr = strtok((char *)buffer, "=");
            ptr = strtok(NULL, "\n");
            
            param->SetStringParameter("Expression", ptr);
         }
         else // objects
         {
            //MessageInterface::ShowMessage("==>Handle Object\n");
            
            char buffer[8192];
            MatlabInterface::OutputBuffer(buffer, 8192);
            
            // need to output string value to buffer
            EvalMatlabString(varName);
            
            //MessageInterface::ShowMessage("==>buffer=\n%s\n", buffer);
            
            // assign new value to object
            UpdateObject(param, buffer);
         }
      }
   }
   catch (BaseException &e)
   {
      std::string moreMsg = e.GetFullMessage() + " in \n" +
         GetGeneratingString(Gmat::SCRIPTING);
      e.SetMessage("");
      e.SetDetails(moreMsg);
      throw;
   }
   
#endif  //__USE_MATLAB__
}


//------------------------------------------------------------------------------
// void EvalMatlabString(std::string evalString)
//------------------------------------------------------------------------------
void CallFunction::EvalMatlabString(std::string evalString)
{
#ifdef __USE_MATLAB__

   MatlabInterface::RunMatlabString(evalString);
   
#endif
}


//------------------------------------------------------------------------------
// std::string FormEvalString()
//  String format
//    [Out1, Out2] = FunctionName(In1, In2, In3);
//------------------------------------------------------------------------------
std::string CallFunction::FormEvalString()
{
   std::string evalString = "";


   // left hand side of evaluation string and equals (if necessary)
   if (mOutputList.size() > 1)
   {
      evalString = evalString + "[";
      Parameter *param = (Parameter *)mOutputList[0];
      evalString = evalString + param->GetName();

      for (unsigned int i=1; i<mOutputList.size(); i++)
      {
         param = (Parameter *)mOutputList[i];
         evalString = evalString +", " + param->GetName();
      }

      evalString = evalString + "] = ";
   }
   else if (mOutputList.size() == 1)
   {
      Parameter *param = (Parameter *)mOutputList[0];
      evalString = evalString + param->GetName();
      evalString = evalString +" = ";
   }
   else if (mOutputList.size() == 0)
   {
      // no left hand side
   }
   else
   {
      // need to throw an exception here
   }


   // right hand side of evaluation string
   // function name and left parenthesis
   evalString = evalString + mFunction->GetName().c_str() + "(";


   // input parameters
   if (mInputList.size() > 0)
   {
      Parameter *param = (Parameter *)mInputList[0];
      evalString = evalString + param->GetName();

      for (unsigned int i=1; i<mInputList.size(); i++)
      {
         param = (Parameter *)mInputList[i];
         evalString = evalString + ", " + param->GetName();
      }
   }

   // right parenthesis and semi-colon
   evalString = evalString + ");";

   return evalString;
}


//------------------------------------------------------------------------------
// void ClearInputParameters()
//------------------------------------------------------------------------------
void CallFunction::ClearInputParameters()
{
   mInputList.clear();
   mInputListNames.clear();
   mNumInputParams = 0;
}


//------------------------------------------------------------------------------
// void ClearOutputParameters()
//------------------------------------------------------------------------------
void CallFunction::ClearOutputParameters()
{
   mOutputList.clear();
   mOutputListNames.clear();
   mNumOutputParams = 0;
}


//------------------------------------------------------------------------------
// bool SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 *  Sets the internal coordinate system used by the Sandbox.
 *
 *  @param <cs> The internal coordinate system.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool CallFunction::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if (!cs)
      return false;


   /// @todo Check initialization and cloning for the internal CoordinateSystem.
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   internalCoordSys = cs;
   return true;
}


//------------------------------------------------------------------------------
// void UpdateObject(GmatBase *obj, char *buffer)
//------------------------------------------------------------------------------
void CallFunction::UpdateObject(GmatBase *obj, char *buffer)
{
   StringTokenizer st(buffer, ": \n");
   StringArray tokens = st.GetAllTokens();

   #ifdef DEBUG_UPDATE_OBJECT
   for (unsigned int i=0; i<tokens.size(); i++)
      MessageInterface::ShowMessage("tokens[%d]=<%s>\n", i, tokens[i].c_str());
   #endif
   
   int id;
   Gmat::ParameterType type;
   std::string newstr;

   // actual parameter starts at 2
   for (unsigned int i=2; i<tokens.size(); i+=2)
   {
      //MessageInterface::ShowMessage("tokens[%d]=<%s>\n", i, tokens[i].c_str());
      id = obj->GetParameterID(tokens[i]);
      type = obj->GetParameterType(id);
      
      switch (type)
      {
      case Gmat::STRING_TYPE:
      {
         //MessageInterface::ShowMessage
         //   ("tokens[i+1]=<%s>, length=%d\n", tokens[i+1].c_str(),
         //    tokens[i+1].length());
                  
         if (((tokens[i+1].c_str())[0] == '\'')&&
            ((tokens[i+1].c_str())[tokens[i+1].length()-1] == '\''))
            newstr = tokens[i+1].substr(1, tokens[i+1].length()-2);
         else if ((tokens[i+1].c_str())[0] == '\'')
         {
            // assume it is a gregorian date then DD MMM YYYY hh:mm:ss.sss
            // this probably isn't the best way, but it will do for now...
            
            // DD
            newstr = tokens[i+1].substr(1, tokens[i+1].length()-1) + " ";
            i++;
            
            // MMM
            newstr = newstr + tokens[i+1].substr(0, tokens[i+1].length()) + " "; 
            i++;
            
            // YYYY
            newstr = newstr + tokens[i+1].substr(0, tokens[i+1].length()) + " "; 
            i++;
            
            // hh:
            newstr = newstr + tokens[i+1].substr(0, tokens[i+1].length()) + ":"; 
            i++;
            
            // mm:                          
            newstr = newstr + tokens[i+1].substr(0, tokens[i+1].length()) + ":"; 
            i++;

            // ss.sss
            newstr = newstr + tokens[i+1].substr(0, tokens[i+1].length()-1); 

         }
         else
            newstr = tokens[i+1].substr(1, tokens[i+1].length()-2);
            
         //MessageInterface::ShowMessage("newstr=<%s>\n", newstr.c_str());
         obj->SetStringParameter(id, newstr);
         break;
      }
      case Gmat::REAL_TYPE:
         obj->SetRealParameter(id, atof(tokens[i+1].c_str()));
         break;
      default:
         throw CommandException
            ("\nCurrently CallFunction cannot update output object for "
             "parameter type: " + GmatBase::PARAM_TYPE_STRING[type] + "\n");
      }
   }

   #ifdef DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage
      ("new %s=\n%s\n", obj->GetName().c_str(),
       obj->GetGeneratingString(Gmat::MATLAB_STRUCT).c_str());
   #endif
}

