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
//#define DEBUG_USE_ARRAY 1

//---------------------------------
// static data
//---------------------------------
const std::string
CallFunction::PARAMETER_TEXT[CallFunctionParamCount - GmatCommandParamCount] =
{
   "FunctionName",
   "AddInput",
   "AddOutput",
};

const Gmat::ParameterType
CallFunction::PARAMETER_TYPE[CallFunctionParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};

CallFunction::CallFunction() :
   GmatCommand     ("CallFunction"),
   mFunction       (NULL),
   mFunctionName   ("")
{
   mNumInputParams = 0;
   mNumOutputParams = 0;

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
   mNumInputParams = cf.mNumInputParams;
   mNumOutputParams = cf.mNumOutputParams;

   objectArray = cf.objectArray;
   mInputList = cf.mInputList;
   mOutputList = cf.mOutputList;
   mParamList = cf.mParamList;

   mInputListNames = cf.mInputListNames;
   mOutputListNames = cf.mOutputListNames;
   mParamListNames = cf.mParamListNames;

   parameterCount = GmatCommandParamCount;
}


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
   mParamList = cf.mParamList;

   mInputListNames = cf.mInputListNames;
   mOutputListNames = cf.mOutputListNames;
   mParamListNames = cf.mParamListNames;
    
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

//loj: 12/7/04 - added
//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool CallFunction::RenameRefObject(const Gmat::ObjectType type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   if (type == Gmat::FUNCTION)
   {
      if (mFunctionName == oldName)
         mFunctionName = newName;
   }
   
   return true;
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
            mInputList.push_back((Parameter *)obj);
            mInputListNames.push_back(obj->GetName());
         }
         else if (index == size)
         {
            mInputList.push_back((Parameter *)obj);
            mInputListNames.push_back(obj->GetName());
         }
         else if (index < size)
         {
            mInputList[index] = (Parameter *)obj;
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
            mOutputList.push_back((Parameter *)obj);
            mOutputListNames.push_back(obj->GetName());
         }
         else if (index == size)
         {
            mOutputList.push_back((Parameter *)obj);
            mOutputListNames.push_back(obj->GetName());
         }
         else if (index < size)
         {
            mOutputList[index] = (Parameter *)obj;
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
      mInputList.push_back((Parameter *)obj);
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

      mInputList.push_back((Parameter *)((*objectMap)[*i]));
   }

    // need to initialize parameters
   mOutputList.clear();

   for (StringArray::iterator i = mOutputListNames.begin(); i != mOutputListNames.end(); ++i)
   {
      if (objectMap->find(*i)  == objectMap->end())
        throw CommandException("CallFunction command cannot find Parameter");

      mOutputList.push_back((Parameter *)((*objectMap)[*i]));
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

   return true;
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


bool CallFunction::Execute()
{
   bool status = false;

   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallFunction");

#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("CallFunction::Execute()\n");
#endif

#if defined __USE_MATLAB__
   if (mFunction->GetTypeName() == "MatlabFunction")
   {
      status = ExecuteMatlabFunction();
   }
#endif

#if DEBUG_CALL_FUNCTION
   MessageInterface::ShowMessage("Excecuted command\n");
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
    MatlabInterface::Open();
    
    // add path to matlab workspace
    // send string to matlab - path(path ,'a:\')
   Integer pathId = mFunction->GetParameterID("FunctionPath");
   std::string thePath = mFunction->GetStringParameter(pathId);
   std::string setPath = "path(path ,'" + thePath + "')";
   EvalMatlabString(setPath);

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

#endif

   return status;
}

void CallFunction::SendInParam(Parameter *param)
{
#if defined __USE_MATLAB__     
  if (param == NULL)
  {
     MessageInterface::ShowMessage("Parameter was null");
     return;
  }

  if (param->GetTypeName() == "Array")
  {
#if DEBUG_USE_ARRAY
     MessageInterface::ShowMessage("Parameter is an array\n");
#endif
     Array *array = (Array *)param;
     int numRows = array->GetIntegerParameter("NumRows");
     int numCols = array->GetIntegerParameter("NumCols");
     std::ostringstream os;

#if DEBUG_USE_ARRAY
     MessageInterface::ShowMessage("Number of rows = %d, number of columns %d\n",
                    numRows, numCols);
#endif

     Rmatrix rmatrix = array->GetMatrix();

     for (int j=0; j<numRows; j++)
     {
        os << "[";
        for (int k=0; k<numCols; k++)
           os << rmatrix(j, k) << ",";
        os << "], \n";
    }

     std::string inParamString = array->GetName() + " = [" +os.str() + "];";
#if DEBUG_USE_ARRAY
     MessageInterface::ShowMessage("The string to send = %s\n", inParamString.c_str());
#endif
     EvalMatlabString(inParamString);
  }
  else if (param->GetTypeName() == "String")
  {
#if DEBUG_CALL_FUNCTION
     MessageInterface::ShowMessage("Parameter type is string\n");
#endif
       
     StringVar *stringVar = (StringVar *)param;
     std::string inParamString = param->GetName() +" = '" + 
                               stringVar->GetString() +"';";
#if DEBUG_CALL_FUNCTION
     MessageInterface::ShowMessage("Message sent to matlab: %s\n",
           inParamString.c_str());
#endif

      EvalMatlabString(inParamString);
  }
  else
  {
#if DEBUG_CALL_FUNCTION
     MessageInterface::ShowMessage("Parameter type is not array or string: %s\n",
           param->GetTypeName().c_str());
#endif
     try
     {
        std::ostringstream os;
        os << param->EvaluateReal();
//            os << param->GetReal();

#if DEBUG_CALL_FUNCTION
     MessageInterface::ShowMessage("Got param real\n");
#endif
        std::string inParamString = param->GetName() +" = " + os.str() +";";
        EvalMatlabString(inParamString);

#if DEBUG_UPDATE_VAR
  MessageInterface::ShowMessage("Sent string %s to matlab\n",
                    inParamString.c_str());
#endif
     }
     catch (BaseException &ex)
     {
        throw CommandException("Unknown parameter type\n");
     }
  }
#endif  //__USE_MATLAB__
}

void CallFunction::GetOutParams()
{
#if defined __USE_MATLAB__     
   for (unsigned int i=0; i<mOutputList.size(); i++)
   {
      Parameter *param = (Parameter *)mOutputList[i];
//      MessageInterface::ShowMessage("Out param type = %s\n", param->GetTypeName().c_str());

      if (param->GetTypeName() == "Array")
      {
         std::string varName = param->GetName();
         Array *array = (Array *)param;
         int numRows = array->GetIntegerParameter("NumRows");
         int numCols = array->GetIntegerParameter("NumCols");
         int totalCells = numRows * numCols;

         double outArray[totalCells];
         //status = 
         MatlabInterface::GetVariable(varName, totalCells, outArray);

         // create rmatrix
         Rmatrix rmatrix = Rmatrix (numRows, numCols);

         for (int j=0; j<numCols; j++)
            for (int k=0; k<numRows; k++)
               rmatrix(k, j) = outArray[(j*numRows) + k];

#if DEBUG_USE_ARRAY
         for (int j=0; j<numRows; j++)
         {
           for (int k=0; k<numCols; k++)
             MessageInterface::ShowMessage("%f\t", rmatrix(j, k));
           MessageInterface::ShowMessage("\n");
         }
#endif

         // assign rmatrix to array
         array->SetRmatrixParameter("RmatValue", rmatrix);
      }
      else if (param->GetTypeName() == "String")
      {  
         std::string varName = param->GetName();
         double buffer[128];
         
//         MatlabInterface::GetVariable(varName, 128, buffer);         
         // need to output string value to buffer
         MatlabInterface::OutputBuffer((char *)buffer, 128);
        EvalMatlabString(varName);
         
         // get rid of "var ="
         char *ptr = strtok((char *)buffer, "=");
         ptr = strtok(NULL, "\n");
         
         param->SetStringParameter("Expression", ptr);
#if DEBUG_CALL_FUNCTION
         MessageInterface::ShowMessage("Set the expression to %s\n", ptr);
#endif
      }  
      else
//      if (param->GetTypeName() == "Variable")
      {
         std::string varName = param->GetName();
         double outArray[500];   // array size???

//          int status = 
         MatlabInterface::GetVariable(varName, 1, outArray);
         //loj: 1/26/05 "Param1" has been removed
         //param->SetRealParameter("Param1", outArray[0]);
         param->SetReal(outArray[0]);
         std::ostringstream ss;
         ss << outArray[0];
         param->SetStringParameter("Expression", ss.str());
         
#if DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage("The EvaluateReal is %f\n", param->EvaluateReal());
            MessageInterface::ShowMessage("The GetReal is %f\n", param->GetReal());
            //loj: 1/26/05 "Param1" has been removed
            //MessageInterface::ShowMessage("The GetRealParameter is %f\n", param->GetRealParameter("Param1"));
            MessageInterface::ShowMessage("The GetRealParameter is %f\n", param->GetReal());
#endif

      }
   }
#endif  //__USE_MATLAB__
}


void CallFunction::EvalMatlabString(std::string evalString)
{
#if DEBUG_UPDATE_VAR
   MessageInterface::ShowMessage("Eval string is %s\n", evalString.c_str());
#endif

   // try to call the function
   evalString = "try,\n" + evalString + "catch\n errormsg = lasterr;\n end";
   MatlabInterface::EvalString(evalString);

   double errormsg[128];
   // if there was an error throw an exception
   if (MatlabInterface::GetVariable("errormsg", 1, errormsg))
   {
      double buffer[128];

      MatlabInterface::OutputBuffer((char *)buffer, 128);
      MatlabInterface::EvalString("errormsg");

      // get rid of "errormsg ="
      char *ptr = strtok((char *)buffer, "=");
      ptr = strtok(NULL, "\n");

      throw CommandException(ptr);
   }
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
// void InterpretAction(void)
//------------------------------------------------------------------------------
bool CallFunction::InterpretAction(void)
{
   return true;
}

Integer CallFunction::GetNumInputParams()
{
   mNumInputParams = mInputList.size();
   return mNumInputParams;
}

Integer CallFunction::GetNumOutputParams()
{
   mNumOutputParams = mOutputList.size();
   return mNumOutputParams;
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

