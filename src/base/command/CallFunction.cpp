//$Header$
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
#include "Moderator.hpp"         // for GetParameter()

#if defined __USE_MATLAB__
#include "MatlabInterface.hpp"   // for Matlab Engine functions
#endif

//#define DEBUG_CALL_FUNCTION 1
//#define DEBUG_UPDATE_VAR 1
//---------------------------------
// static data
//---------------------------------
const std::string
CallFunction::PARAMETER_TEXT[CallFunctionParamCount - GmatCommandParamCount] =
{
   "FunctionName",
};

const Gmat::ParameterType
CallFunction::PARAMETER_TYPE[CallFunctionParamCount - GmatCommandParamCount] =
{
	Gmat::STRING_TYPE,
};

CallFunction::CallFunction() :
    GmatCommand     ("CallFunction"),
    mFunction       (NULL),
    mFunctionName   ("")
{
   parameterCount = GmatCommandParamCount;
}

CallFunction::~CallFunction()
{
}

CallFunction::CallFunction(const CallFunction& cf) :
    GmatCommand     (cf),
    mFunction       (cf.mFunction),
    mFunctionName   (cf.mFunctionName)
{
   parameterCount = GmatCommandParamCount;
}


CallFunction& CallFunction::operator=(const CallFunction& cf)
{
    if (this == &cf)
        return *this;
    
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
#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("CallFunction::GetParameterText\n");
#endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer CallFunction::GetParameterID(const std::string &str) const
{
#if DEBUG_CALL_FUNCTION
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
#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("CallFunction::GetParameterType\n");
#endif

   if (id >= 0 && id < CallFunctionParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetParameterTypeString(const Integer id) const
{
#if DEBUG_CALL_FUNCTION
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
#if DEBUG_CALL_FUNCTION
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
#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("CallFunction::SetStringParameter\n");
#endif

   switch (id)
   {
   case FUNCTION_NAME:
      mFunctionName = value;
      return true;
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
   if (action == "Clear")
   {
   }

   return GmatCommand::TakeAction(action, actionData);
}

std::string CallFunction::GetRefObjectName(const Gmat::ObjectType type) const
{
   /// @todo Figure out how to send back the arrays of names
   switch (type) {
      case Gmat::PARAMETER:
         return "Input";
      case Gmat::FUNCTION:
         return mFunctionName;
      default:
         break;
   }

   return GmatCommand::GetRefObjectName(type);
}

StringArray CallFunction::GetRefObjectNameArray(const Gmat::ObjectType type) const
{
   StringArray result;

   switch (type) {
      case Gmat::PARAMETER:         // Input/Output
         result.push_back("Input");
         result.push_back("Output");
         return result;
      default:
         break;
   }

   return result;
}

bool CallFunction::SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name)
{
   switch (type) {
     case Gmat::FUNCTION:
         mFunctionName = name;
         return true;
     default:
         break;
   }

   return GmatCommand::SetRefObjectName(type, name);
}

// Reference object accessor methods
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//                        const Integer index
//------------------------------------------------------------------------------
GmatBase* CallFunction::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name, const Integer index)
{
   switch (type)
   {
      case Gmat::PARAMETER:
         if (name == "Input")
         {
            if (index < (Integer)mInputList.size())
               return mInputList[index];
            else
               throw CommandException("CallFunction::GetRefObject invalid index\n");
         }
         else if (name == "Output")
         {
            if (index < (Integer)mOutputList.size())
               return mOutputList[index];
            else
               throw CommandException("CallFunction::GetRefObject invalid index\n");
         }

      case Gmat::FUNCTION:
         return mFunction;
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return GmatCommand::GetRefObject(type, name, index);
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
                             const std::string &name, const Integer index)
{
   switch (type)
   {
   case Gmat::PARAMETER:
      if (name == "Input")
      {
         Integer size = mInputList.size();
         if (mInputList.empty() && index == 0)
         {
            mInputList.push_back((Variable *)obj);
            mInputListNames.push_back(obj->GetName());
         }
         else if (index == size)
         {
            mInputList.push_back((Variable *)obj);
            mInputListNames.push_back(obj->GetName());
         }
         else if (index < size)
         {
            mInputList[index] = (Variable *)obj;
            mInputListNames[index] = obj->GetName();
         }
         else
         {
            MessageInterface::ShowMessage
               ("CallFunction::SetRefObject() index=%d is not next available "
                "index=%d. Setting %s:%s failed\n", index, size,
                obj->GetTypeName().c_str(), obj->GetName().c_str());
            return false;
         }
         return true;
      }
      else if (name == "Output")
      {
         Integer size = mOutputList.size();
         if (mOutputList.empty() && index == 0)
         {
            mOutputList.push_back((Variable *)obj);
            mOutputListNames.push_back(obj->GetName());
         }
         else if (index == size)
         {
            mOutputList.push_back((Variable *)obj);
            mOutputListNames.push_back(obj->GetName());
         }
         else if (index < size)
         {
            mOutputList[index] = (Variable *)obj;
            mOutputListNames[index] = obj->GetName();
         }
         else
         {
            MessageInterface::ShowMessage
               ("CallFunction::SetRefObject() index=%d is not next available "
                "index=%d. Setting %s:%s failed\n", index, size,
                obj->GetTypeName().c_str(), obj->GetName().c_str());
            return false;
         }
         return true;
      }
   case Gmat::FUNCTION:
      mFunction = (Function *)obj;
      mFunctionName = name;
      return true;
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return GmatCommand::SetRefObject(obj, type, name, index);
}

//------------------------------------------------------------------------------
// virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
ObjectArray& CallFunction::GetRefObjectArray(const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::PARAMETER:

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
// bool SetObject(GmatBase *obj, const Gmat::ObjectType type)
//------------------------------------------------------------------------------
bool CallFunction::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("CallFunction::SetObject()\n");
#endif

   switch (type)
   {
   case Gmat::PARAMETER:
      /// @todo what about output list?
      mInputList.push_back((Variable *)obj);
      return true;
   case Gmat::FUNCTION:
      mFunction = (Function *)obj;
      return true;
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetObject call
   return GmatCommand::SetObject(obj, type);
}

//------------------------------------------------------------------------------
// void ClearObject(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
void CallFunction::ClearObject(const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::PARAMETER:
      mInputList.clear();
      mInputListNames.clear();
      mOutputList.clear();
      mOutputListNames.clear();
      objectArray.clear();
      break;

   default:
      break;
   }
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const Gmat::ObjectType type, ...
//------------------------------------------------------------------------------
GmatBase* CallFunction::GetObject(const Gmat::ObjectType type,
                               const std::string objName)
{
   switch(type)
   {
      case Gmat::FUNCTION:
         return mFunction;
      case Gmat::PARAMETER:
         if (objName == "Input")
            return mInputList[0];
         else if (objName == "Output")
            return mOutputList[0];
      default:
         break;
   }

   return GmatCommand::GetObject(type, objName);
}

bool CallFunction::Initialize()
{
#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("In Initialized\n");
#endif
   GmatCommand::Initialize();

    // need to initialize parameters
   mInputList.clear();

   for (StringArray::iterator i = mInputListNames.begin(); i != mInputListNames.end(); ++i)
   {
      if (objectMap->find(*i)  == objectMap->end())
        throw CommandException("CallFunction command cannot find Parameter");

      mInputList.push_back((Variable *)((*objectMap)[*i]));
   }

    // need to initialize parameters
   mOutputList.clear();

   for (StringArray::iterator i = mOutputListNames.begin(); i != mOutputListNames.end(); ++i)
   {
      if (objectMap->find(*i)  == objectMap->end())
        throw CommandException("CallFunction command cannot find Parameter");

      mOutputList.push_back((Variable *)((*objectMap)[*i]));
   }


   return true;
}


bool CallFunction::Execute()
{
   bool status = false;

#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("CallFunction::Execute()\n");
#endif

#if defined __USE_MATLAB__
   if (mFunction->GetTypeName() == "MatlabFunction")
   {
      return ExecuteMatlabFunction();
   }
#endif

   return status;
}

//------------------------------------------------------------------------------
// bool ExecuteMatlabFunction()
//------------------------------------------------------------------------------
bool CallFunction::ExecuteMatlabFunction()
{
   bool status = false;

#if defined __USE_MATLAB__

   // check if we have a pointer to matlab engine
   if (!MatlabInterface::IsOpen())
   {
      MatlabInterface::Open();
   }

   // ag:  having trouble getting the string parameter and typename
   // for input params
   for (unsigned int i=0; i<mInputList.size(); i++)
   {
      Variable *param = (Variable *)mInputList[i];
      std::string varName =  param->GetName();
      double arrayToSend[] = {param->GetReal()};
#if DEBUG_UPDATE_VAR
//      MessageInterface::ShowMessage("The EvaluateReal for %s is %f\n", varName.c_str(), param->EvaluateReal());
//      MessageInterface::ShowMessage("The parameter value for %s is %f\n", varName.c_str(), param->GetRealParameter("Param1"));
//      MessageInterface::ShowMessage("The type for this parameter is %f\n", param->GetTypeName().c_str());
//      MessageInterface::ShowMessage("The string parameter is %f\n", param->GetStringParameter("Expression").c_str());
#endif
      status = MatlabInterface::PutVariable(varName, 1, arrayToSend);
#if DEBUG_UPDATE_VAR
      MessageInterface::ShowMessage("Put %s with value %f in matlab\n", varName.c_str(),
                  arrayToSend[0]);

      // try updating variable to 5
//      param->SetRealParameter("Param1", 5);
//      MessageInterface::ShowMessage("The parameter should be updated to 5...\n");
//      MessageInterface::ShowMessage("The EvaluateReal for %s is %f\n", varName.c_str(), param->EvaluateReal());
//      MessageInterface::ShowMessage("The parameter value for %s is %f\n", varName.c_str(), param->GetRealParameter("Param1"));
#endif
   }

   //  Eval String
   std::string evalString = FormEvalString();
#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("Eval string is %s\n", evalString.c_str());
#endif
   status =  MatlabInterface::EvalString(evalString);

   // ag: Not updating the variable correctly (setrealparameter)
   // for output params
   for (unsigned int i=0; i<mOutputList.size(); i++)
   {
      Variable *param = (Variable *)mOutputList[i];
      std::string varName = param->GetName();
      double outArray[500];   // array size???
#if DEBUG_UPDATE_VAR
      MessageInterface::ShowMessage("Returning param %s\n", varName.c_str());
#endif
      status = MatlabInterface::GetVariable(varName, 1, outArray);
#if DEBUG_UPDATE_VAR
      MessageInterface::ShowMessage("The value is %f\n", outArray[0]);
#endif
      param->SetRealParameter("Param1", outArray[0]);
#if DEBUG_UPDATE_VAR
      MessageInterface::ShowMessage("The GetReal is %f\n", param->GetReal());
//      MessageInterface::ShowMessage("The EvaluateReal is %f\n", param->EvaluateReal());
//      MessageInterface::ShowMessage("The parameter value is %f\n", param->GetRealParameter("Param1"));
#endif
   }

#endif

   return status;
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
      Variable *param = (Variable *)mOutputList[0];
      evalString = evalString + param->GetName();

      for (unsigned int i=1; i<mOutputList.size(); i++)
      {
         param = (Variable *)mOutputList[i];
         evalString = evalString +", " + param->GetName();
      }

      evalString = evalString + "] = ";
   }
   else if (mOutputList.size() == 1)
   {
      Variable *param = (Variable *)mOutputList[0];
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
      Variable *param = (Variable *)mInputList[0];
      evalString = evalString + param->GetName();

      for (unsigned int i=1; i<mInputList.size(); i++)
      {
         param = (Variable *)mInputList[i];
         evalString = evalString + ", " + param->GetName();
      }
   }

   // right parenthesis and semi-colon
   evalString = evalString + ");";

   return evalString;
}

//------------------------------------------------------------------------------
// void InterpretAction(void)
//------------------------------------------------------------------------------
bool CallFunction::InterpretAction(void)
{
   return true;
}

Integer CallFunction::GetNumInputParams()
{
   return mInputList.size();
}

Integer CallFunction::GetNumOutputParams()
{
   return mOutputList.size();
}

