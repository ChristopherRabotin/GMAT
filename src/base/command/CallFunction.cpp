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

   mInputListNames = cf.mInputListNames;
   mOutputListNames = cf.mOutputListNames;

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
      ClearInputParameters();
      ClearOutputParameters();
      objectArray.clear();
      return true;
   }

   return GmatCommand::TakeAction(action, actionData);
}

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
                           + name +
                           "not found\n");

      case Gmat::FUNCTION:
         return mFunction;
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

bool CallFunction::Initialize()
{
   #if DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("In Initialized\n");
   #endif

   GmatCommand::Initialize();

   if (objectMap->find(mFunctionName)  == objectMap->end())
      throw CommandException("CallFunction command cannot find Function " +
               mFunctionName);
   mFunction = (Function *)((*objectMap)[mFunctionName]);

   // need to initialize parameters
   mInputList.clear();

   for (StringArray::iterator i = mInputListNames.begin(); i != mInputListNames.end(); ++i)
   {
      if (objectMap->find(*i)  == objectMap->end())
        throw CommandException("CallFunction command cannot find Parameter");

         #if DEBUG_CALL_FUNCTION
            MessageInterface::ShowMessage("Adding input parameter %s\n", i->c_str());
         #endif

      mInputList.push_back((Parameter *)((*objectMap)[*i]));
   }

    // need to initialize parameters
   mOutputList.clear();

   for (StringArray::iterator i = mOutputListNames.begin(); i != mOutputListNames.end(); ++i)
   {
      if (objectMap->find(*i)  == objectMap->end())
        throw CommandException("CallFunction command cannot find Parameter " + (*i));

        #if DEBUG_CALL_FUNCTION
           MessageInterface::ShowMessage("Adding output parameter %s\n", i->c_str());
        #endif

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

      // DJC, 03/02/05
      status = true;

   #endif

   return status;
}

void CallFunction::SendInParam(Parameter *param)
{
   #if DEBUG_CALL_FUNCTION
      MessageInterface::ShowMessage("CallFunction::SendInParam()");
   #endif

   #if defined __USE_MATLAB__
      if (param == NULL)
      {
         MessageInterface::ShowMessage("Parameter was null");
         return;
      }

      #if DEBUG_CALL_FUNCTION
         MessageInterface::ShowMessage("Parameter %s, type is: %s",
               param->GetName().c_str(),
               param->GetTypeName().c_str());
      #endif

      if (param->GetTypeName() == "Array")
      {
         Array *array = (Array *)param;
         int numRows = array->GetIntegerParameter("NumRows");
         int numCols = array->GetIntegerParameter("NumCols");
         std::ostringstream os;

         Rmatrix rmatrix = array->GetMatrix();

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
      else if (param->GetTypeName() == "String")
      {
         StringVar *stringVar = (StringVar *)param;
         std::string inParamString = param->GetName() +" = '" +
                                   stringVar->GetString() +"';";
         EvalMatlabString(inParamString);
      }
      // needs to be updated when burn, etc are added to dialog
      else if (param->GetTypeName() == "Spacecraft")
      {
          #if DEBUG_CALL_FUNCTION
              MessageInterface::ShowMessage(
                     "Parameter type is : %s\n",
                     param->GetTypeName().c_str());
          #endif

          std::string inParamString =
                     param->GetGeneratingString(Gmat::MATLAB_STRUCT);

          #if DEBUG_CALL_FUNCTION
             MessageInterface::ShowMessage("Generated param string : %s\n",
                     inParamString.c_str());
          #endif

          EvalMatlabString(inParamString);
      }
      else //if (param->GetTypeName() == "Variable")
      {
          std::ostringstream os;
          os << param->EvaluateReal();

          std::string inParamString = param->GetName() +" = " + os.str() +";";

          #if DEBUG_UPDATE_VAR
             MessageInterface::ShowMessage("Sent string %s to matlab\n",
                              inParamString.c_str());
          #endif

          EvalMatlabString(inParamString);
      }
   #endif  //__USE_MATLAB__
}

void CallFunction::GetOutParams()
{
   #if defined __USE_MATLAB__
      for (unsigned int i=0; i<mOutputList.size(); i++)
      {
         Parameter *param = (Parameter *)mOutputList[i];

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

            // need to output string value to buffer
            MatlabInterface::OutputBuffer((char *)buffer, 128);
            EvalMatlabString(varName);

            // get rid of "var ="
            char *ptr = strtok((char *)buffer, "=");
            ptr = strtok(NULL, "\n");

            param->SetStringParameter("Expression", ptr);
         }
         else
         {
            std::string varName = param->GetName();
            double outArray[500];   // array size???

            MatlabInterface::GetVariable(varName, 1, outArray);
            //loj: 1/26/05 "Param1" has been removed
            //param->SetRealParameter("Param1", outArray[0]);
            param->SetReal(outArray[0]);
            std::ostringstream ss;
            ss << outArray[0];
            param->SetStringParameter("Expression", ss.str());

            #if DEBUG_UPDATE_VAR
               MessageInterface::ShowMessage("The EvaluateReal is %f\n",
                           param->EvaluateReal());
               MessageInterface::ShowMessage("The GetReal is %f\n",
                           param->GetReal());
               //loj: 1/26/05 "Param1" has been removed
               //MessageInterface::ShowMessage("The GetRealParameter is %f\n",
               //          param->GetRealParameter("Param1"));
               MessageInterface::ShowMessage("The GetRealParameter is %f\n",
                           param->GetReal());
            #endif
         }
      }
   #endif  //__USE_MATLAB__
}


void CallFunction::EvalMatlabString(std::string evalString)
{
   #if defined __USE_MATLAB__

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
// void InterpretAction(void)
//------------------------------------------------------------------------------
bool CallFunction::InterpretAction(void)
{
   return true;
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

