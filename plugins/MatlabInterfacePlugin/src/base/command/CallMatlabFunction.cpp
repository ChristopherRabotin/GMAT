//$Id: CallMatlabFunction.cpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                                 CallMatlabFunction
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
 * Definition for the CallMatlabFunction command class
 */
//------------------------------------------------------------------------------
#include "CallMatlabFunction.hpp"
#include "BeginFunction.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "FileManager.hpp"         // for GetAllMatlabFunctionPaths()
#include "MessageInterface.hpp"
#include <sstream>
#include "MatlabInterface.hpp"     // for Matlab Engine functions

// if using MatlabInterface::EvalMatlabString() to send array to Matlab workspace
// if using MatlabInterface::EvalMatlabString() for getting string from Matlab workspace
//#define __USE_EVAL_STRING__

//#define DEBUG_CALL_FUNCTION_PARAM
//#define DEBUG_CALL_FUNCTION_INIT
//#define DEBUG_CALL_FUNCTION_EXEC
//#define DEBUG_SEND_PARAM
//#define DEBUG_UPDATE_VAR
//#define DEBUG_UPDATE_OBJECT
//#define DEBUG_SHOW_ARRAY
//#define DEBUG_GMAT_FUNCTION_INIT
//#define DEBUG_GET_OUTPUT
//#define DEBUG_OBJECT_MAP
//#define DEBUG_GLOBAL_OBJECT_MAP
//#define DEBUG_RUN_COMPLETE
//#define DEBUG_MATLAB_EXEC
//#define DEBUG_MATLAB_EVAL
//#define DEBUG_CALLING_OBJECT

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
// CallMatlabFunction::CallMatlabFunction()
//------------------------------------------------------------------------------
CallMatlabFunction::CallMatlabFunction() :
   CallFunction("CallMatlabFunction")
{
   objectTypeNames.push_back("CallMatlabFunction");
}


//------------------------------------------------------------------------------
// ~CallMatlabFunction()
//------------------------------------------------------------------------------
CallMatlabFunction::~CallMatlabFunction()
{
}


//------------------------------------------------------------------------------
// CallMatlabFunction(const CallMatlabFunction& cf) :
//------------------------------------------------------------------------------
CallMatlabFunction::CallMatlabFunction(const CallMatlabFunction& cf) :
   CallFunction (cf)
{
}


//------------------------------------------------------------------------------
// CallMatlabFunction& operator=(const CallMatlabFunction& cf)
//------------------------------------------------------------------------------
CallMatlabFunction& CallMatlabFunction::operator=(const CallMatlabFunction& cf)
{
   if (this == &cf)
      return *this;
   
   CallFunction::operator=(cf);
   
   return *this;
}


//------------------------------------------------------------------------------
// std::string FormEvalString()
//  String format
//    [Out1, Out2] = FunctionName(In1, In2, In3);
//------------------------------------------------------------------------------
std::string CallMatlabFunction::FormEvalString()
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("CallMatlabFunction::FormEvalString() entered, mFunction=<%p>'%s'\n",
       mFunction, mFunction ? mFunction->GetName().c_str() : "NULL");
   #endif
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
      evalString = "[" + evalString + param->GetName() + "]";
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
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CallMatlabFunction.
 *
 * @return clone of the CallMatlabFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CallMatlabFunction::Clone() const
{
   return (new CallMatlabFunction(*this));
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CallMatlabFunction::Initialize()
{
   #ifdef DEBUG_CALL_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::Initialize() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
   #endif
   
   if (!GmatGlobal::Instance()->IsMatlabAvailable())
      throw CommandException
         ("MATLAB is not installed.  GMAT will not run if any CallMatlabFunction"
          " uses MATLAB function");
   
   bool rv = true;  // Initialization return value
   
   rv = CallFunction::Initialize();
   
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps("In CallMatlabFunction::Initialize()");
   #endif
   
   // We need to add all Matlab paths to the bottom of the path using path(path, 'newpath')
   // since FileManager::GetAllMatlabFunctionPaths() returns in top to bottom order
   if (isMatlabFunction)
   {
      FileManager *fm = FileManager::Instance();
      StringArray paths = fm->GetAllMatlabFunctionPaths();
      
      // Open Matlab engine first and set calling function name
      matlabIf = MatlabInterface::Instance();

      #ifdef DEBUG_CALLING_OBJECT
      MessageInterface::ShowMessage
         ("CallMatlabFunction::Initialize() setting calling object '%s'\n",
          mFunctionPathAndName.c_str());
      #endif
      
      matlabIf->SetCallingObjectName(mFunctionPathAndName);
      
      if (!matlabIf->IsOpen())
         matlabIf->Open("GmatMatlab");
      
      #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage("   Found %d matlab path\n", paths.size());
      #endif
      
      // Add path to the top of the Matlab path in reverse order(loj: 2008.10.16)
      std::string pathName;
      StringArray::reverse_iterator rpos = paths.rbegin();
      std::string addPath;
      while (rpos != paths.rend())
      {
         pathName = *rpos;
         if (pathName != "")
         {
            #ifdef DEBUG_CALL_FUNCTION_INIT
            MessageInterface::ShowMessage
               ("   Adding matlab path '%s' to the top\n", pathName.c_str());
            #endif
            addPath = "path('" + pathName + "', path)";
            matlabIf->EvalString(addPath);
         }
         rpos++;
      }
   
      // Initialize input parameters only for MatlabFunction
      // Initialization of GmatFunctions are handled by FunctionManager during
      // execution
      
      // add input/output parameters
      mInputList.clear();
      GmatBase *mapObj;
      // need to initialize input parameters
      for (StringArray::iterator i = mInputNames.begin();
           i != mInputNames.end(); ++i)
      {
         if ((mapObj = FindObject(*i))  == NULL)
            throw CommandException("CallMatlabFunction command cannot find Parameter " +
                                   *i + " in script line\n   \"" +
                                   GetGeneratingString(Gmat::SCRIPTING) + "\"");
         
         #ifdef DEBUG_CALL_FUNCTION_INIT
         MessageInterface::ShowMessage("Adding input parameter %s\n", i->c_str());
         #endif
         
         mInputList.push_back((Parameter *)mapObj);
      }
      
      // need to initialize output parameters
      mOutputList.clear();
      
      for (StringArray::iterator i = mOutputNames.begin();
           i != mOutputNames.end();++i)
      {
         if ((mapObj = FindObject(*i))  == NULL)
            throw CommandException("CallMatlabFunction command cannot find Parameter " + (*i));
         
         #ifdef DEBUG_CALL_FUNCTION_INIT
         MessageInterface::ShowMessage("Adding output parameter %s\n", i->c_str());
         #endif
         
         mOutputList.push_back((Parameter *)mapObj);
      }
      
      if (mInputList.size() > 0)
         if (mInputList[0] == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "CallMatlabFunction::Initialize() CallMatlabFunction will not be created.\n"
                "The first parameter selected as input for the CallMatlabFunction is NULL\n");
            return false;
         }
      
      if (mOutputList.size() > 0)
         if (mOutputList[0] == NULL)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "CallMatlabFunction::Initialize() CallMatlabFunction will not be created.\n"
                "The first parameter selected as output for the CallMatlabFunction is NULL\n");
            return false;
         }
   }
   
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool CallMatlabFunction::Execute()
{
   bool status = false;
   
   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallMatlabFunction");
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      ("=== CallMatlabFunction::Execute() entered, '%s' Count = %d\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("CallMatlabFunction::Execute() this=<%p> entered, command = '%s'\n   "
       "function type is '%s', callingFunction is '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the start");
      #endif
   #endif
      
   if (isMatlabFunction)
   {
      #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage("   calling ExecuteMatlabFunction()\n");
      #endif
      
      status = ExecuteMatlabFunction();
      BuildCommandSummary(true);
      
      #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("CallMatlabFunction::Execute() MatlabFunction exiting with %d\n", status);
         #ifdef DEBUG_OBJECT_MAP
         ShowObjectMaps("object maps at the end");
         #endif
      #endif
   }
   
   
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("=== CallMatlabFunction::Execute() exiting, '%s' Count = %d, Run Time: %f seconds\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount, (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   
   return status;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void CallMatlabFunction::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallMatlabFunction::RunComplete() entered for this=<%p> '%s',\n   "
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
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// bool ExecuteMatlabFunction()
//------------------------------------------------------------------------------
bool CallMatlabFunction::ExecuteMatlabFunction()
{
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage
      ("CallMatlabFunction::ExecuteMatlabFunction() entered, mFunctionName='%s', "
       "function=<%p>\n", mFunctionName.c_str(), mFunction);
   if (mFunction)
      MessageInterface::ShowMessage
         ("   functionPath='%s', functionName='%s'\n",
          mFunction->GetStringParameter("FunctionPath").c_str(),
          mFunction->GetStringParameter("FunctionName").c_str());
   #endif
   
   #ifdef DEBUG_CALLING_OBJECT
   MessageInterface::ShowMessage
      ("CallMatlabFunction::ExecuteMatlabFunction() setting calling object '%s'\n",
       mFunctionPathAndName.c_str());
   #endif
   
   matlabIf->SetCallingObjectName(mFunctionPathAndName);
   
   // open Matlab engine
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Opening Matlab Engine\n");
   #endif
   matlabIf->Open("GmatMatlab");
   
   // set format long so that we don't loose precision between string transmission
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Sending format long\n");
   #endif
   matlabIf->EvalString("format long");
   
   // Clear last errormsg
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Sending clear errormsg\n");
   #endif
   matlabIf->EvalString("clear errormsg");
   
   // add the path to the top of the path list using path('newpath', path)
   Integer pathId = mFunction->GetParameterID("FunctionPath");
   std::string thePath = mFunction->GetStringParameter(pathId);      
   
   if (thePath != "")
   {
      #ifdef DEBUG_MATLAB_EXEC
      MessageInterface::ShowMessage(".....Sending path to use\n");
      #endif
      std::string setPath = "path('" + thePath + "', path)";
      matlabIf->EvalString(setPath);
   }
   
   // send the in parameters
   for (unsigned int i=0; i<mInputList.size(); i++)
   {
      Parameter *param = (Parameter *)mInputList[i];
      #ifdef DEBUG_MATLAB_EXEC
      MessageInterface::ShowMessage
         (".....Sending input parameter <%p> '%s', %d out of %d\n", param,
          mInputNames[i].c_str(), i+1, mNumInputParams);
      #endif
      SendInParam(param);
   }
   
   //  Eval String
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Forming evaluate string\n");
   #endif
   std::string evalString = FormEvalString();
   EvalMatlabString(evalString);
   
   // get the value for the out parameters
   #ifdef DEBUG_MATLAB_EXEC
   MessageInterface::ShowMessage(".....Getting output parameters\n");
   #endif
   GetOutParams();
   
   return true;
   
}


//------------------------------------------------------------------------------
// void SendInParam(Parameter *param)
//------------------------------------------------------------------------------
void CallMatlabFunction::SendInParam(Parameter *param)
{
   #ifdef DEBUG_SEND_PARAM
   MessageInterface::ShowMessage("CallMatlabFunction::SendInParam()");
   #endif
   
   if (param == NULL)
   {
      MessageInterface::ShowMessage("Parameter was null");
      return;
   }
   
   //MessageInterface::ShowMessage("---> CallMatlabFunction::SendInParam() setting calling object '%s'\n", mFunctionPathAndName.c_str());
   //matlabIf->SetCallingObjectName(mFunctionPathAndName);
   
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
      Rmatrix rmatrix = array->GetRmatrix();

      //----------------------------------------------------
      #ifdef __USE_EVAL_STRING__
      //----------------------------------------------------
      std::ostringstream os;
      os.precision(18);
      
      for (int j=0; j<numRows; j++)
      {
         os << "[";
         for (int k=0; k<numCols; k++)
            os << rmatrix(j, k) << ",";
         os << "], \n";
      }
      
      std::string inParamString = array->GetName() + " = [" +os.str() + "];";
      EvalMatlabString(inParamString);
      
      //----------------------------------------------------
      #else
      //----------------------------------------------------
      const Real *realArray = rmatrix.GetDataVector();
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         (".....Putting RealArray data <%p> to Matlab workspace\n", realArray);
      #endif
      matlabIf->PutRealArray(param->GetName(), numRows, numCols, realArray);
      
      //----------------------------------------------------
      #endif
      //----------------------------------------------------
   }
   else if (param->GetTypeName() == "Variable")
   {
      //----------------------------------------------------
      #ifndef __USE_EVAL_STRING__
      //----------------------------------------------------
      std::ostringstream os;
      os.precision(18);
      os << param->EvaluateReal();
      
      std::string inParamString = param->GetName() +" = " + os.str() +";";
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         ("Sent string %s to matlab\n", inParamString.c_str());
      #endif
      
      EvalMatlabString(inParamString);
      
      //----------------------------------------------------
      #else
      //----------------------------------------------------
      static Real *realVal = new Real[1];
      realVal[0] = param->EvaluateReal();
      
      #ifdef DEBUG_SEND_PARAM
      MessageInterface::ShowMessage
         (".....Putting Real data %p to Matlab workspace\n", realVal);
      #endif
      
      matlabIf->PutRealArray(param->GetName(), 1, 1, (const Real*)realVal);
      
      //----------------------------------------------------
      #endif
      //----------------------------------------------------
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
}


//------------------------------------------------------------------------------
// void GetOutParams()
//------------------------------------------------------------------------------
void CallMatlabFunction::GetOutParams()
{
   //MessageInterface::ShowMessage("---> CallMatlabFunction::GetOutParams() setting calling object '%s'\n", mFunctionPathAndName.c_str());
   //matlabIf->SetCallingObjectName(mFunctionPathAndName);
   
   try
   {
      for (unsigned int i=0; i<mOutputList.size(); i++)
      {
         Parameter *param = (Parameter *)mOutputList[i];
         std::string varName = param->GetName();
         
         #ifdef DEBUG_GET_OUTPUT
         MessageInterface::ShowMessage
            ("CallMatlabFunction::GetOutParams() OutParamType=%s, name=%s\n",
             param->GetTypeName().c_str(), varName.c_str());
         #endif
         
         if (param->GetTypeName() == "Array")
         {
            Array *array = (Array *)param;
            int numRows = array->GetIntegerParameter("NumRows");
            int numCols = array->GetIntegerParameter("NumCols");
            int totalCells = numRows * numCols;            
            double *outArray = new double[totalCells];
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "*outArray = new double[totalCells]");
            //MessageInterface::ShowMessage
            //   ("+++ CallMatlabFunction::GetOutParams() double *outArray = new double[%d], <%p>\n",
            //    totalCells, outArray);
            #endif
            
            #ifdef DEBUG_GET_OUTPUT
            MessageInterface::ShowMessage
               ("CallMatlabFunction::GetOutParams() calling MI::GetRealArray()\n");
            #endif
            if (matlabIf->GetRealArray(varName, totalCells, outArray) == 0)
            {
               std::string msg =
                  "CallMatlabFunction::GetOutParams() error occurred in matlabIf->GetRealArray()";
               #ifdef DEBUG_GET_OUTPUT
               MessageInterface::ShowMessage(msg + "\n");
               #endif
               throw CommandException(msg);
            }
            
            // create rmatrix
            Rmatrix rmatrix = Rmatrix (numRows, numCols);
            // copy value
            for (int j=0; j<numCols; j++)
               for (int k=0; k<numRows; k++)
                  rmatrix(k, j) = outArray[(j*numRows) + k];
            
            #ifdef DEBUG_SHOW_ARRAY
            for (int j=0; j<numRows; j++)
            {
               for (int k=0; k<numCols; k++)
                  MessageInterface::ShowMessage("%f\t", rmatrix(j, k));
               MessageInterface::ShowMessage("\n");
            }
            #endif
            
            // assign rmatrix to array
            array->SetRmatrixParameter("RmatValue", rmatrix);
            
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "deletinig outArray");
            //MessageInterface::ShowMessage
            //   ("--- CallMatlabFunction::GetOutParams() deleting outArray <%p>\n",
            //    outArray);
            #endif
            delete [] outArray;
            
         }
         else if (param->GetTypeName() == "Variable")
         {
            double *outArray = new double[1];
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "*outArray = new double[1]");
            //MessageInterface::ShowMessage
            //   ("+++ CallMatlabFunction::GetOutParams() double *outArray = new double[1], <%p>\n",
            //    outArray);
            #endif
            
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               (".....Calling matlabIf->GetRealArray()\n");
            #endif
            matlabIf->GetRealArray(varName, 1, outArray);
            
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
            
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (outArray, "outArray", "CallMatlabFunction::GetOutParams()",
                "deletinig outArray");
            //MessageInterface::ShowMessage
            //   ("--- CallMatlabFunction::GetOutParams() deleting outArray <%p>\n",
            //    outArray);
            #endif
            delete [] outArray;
         }
         else if (param->GetTypeName() == "String")
         {
            //----------------------------------------------
            #ifdef __USE_EVAL_STRING__
            //----------------------------------------------            
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               (".....Calling matlabIf->SetOutputBuffer()\n");
            #endif
            
            // need to output string value to buffer
            int bufSize = matlabIf->SetOutputBuffer(512);
            EvalMatlabString(varName);
            
            char *outBuffer = matlabIf->GetOutputBuffer();
            
            // get rid of "var ="
            char *ptr = strtok((char *)outBuffer, "=");
            ptr = strtok(NULL, "\n");
            
            //----------------------------------------------
            #else
            //----------------------------------------------
            #ifdef DEBUG_UPDATE_VAR
            MessageInterface::ShowMessage
               (".....Calling matlabIf->GetString()\n");
            #endif
            std::string outStr;
            matlabIf->GetString(varName, outStr);
            param->SetStringParameter("Expression", outStr);
            
            //----------------------------------------------
            #endif
            //----------------------------------------------
         }
         else // objects
         {
            #ifdef DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage
               (".....Calling matlabIf->SetOutputBuffer() for object\n");
            #endif
            
            char buffer[8192];
            int bufSize = matlabIf->SetOutputBuffer(8192);
            EvalMatlabString(varName);
            char *outBuffer = matlabIf->GetOutputBuffer();
            
            // copy output to local buffer
            strncpy(buffer, outBuffer, bufSize);
            
            #ifdef DEBUG_UPDATE_OBJECT
            MessageInterface::ShowMessage("   buffer=\n%s\n", buffer);
            #endif
            
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
}


//------------------------------------------------------------------------------
// void EvalMatlabString(std::string evalString)
//------------------------------------------------------------------------------
void CallMatlabFunction::EvalMatlabString(std::string evalString)
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("CallMatlabFunction::EvalMatlabString() calling MI::RunMatlabString() with\n"
       "======================================================================\n"
       "%s\n\n", evalString.c_str());
   #endif

   matlabIf->RunMatlabString(evalString);
}


//------------------------------------------------------------------------------
// void ClearInputParameters()
//------------------------------------------------------------------------------
void CallMatlabFunction::ClearInputParameters()
{
   mInputList.clear();
   mInputNames.clear();
   mNumInputParams = 0;
}


//------------------------------------------------------------------------------
// void ClearOutputParameters()
//------------------------------------------------------------------------------
void CallMatlabFunction::ClearOutputParameters()
{
   mOutputList.clear();
   mOutputNames.clear();
   mNumOutputParams = 0;
}


//------------------------------------------------------------------------------
// void UpdateObject(GmatBase *obj, char *buffer)
//------------------------------------------------------------------------------
void CallMatlabFunction::UpdateObject(GmatBase *obj, char *buffer)
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
            ("\nCurrently CallMatlabFunction cannot update output object for "
             "parameter type: " + GmatBase::PARAM_TYPE_STRING[type] + "\n");
      }
   }

   #ifdef DEBUG_UPDATE_OBJECT
   MessageInterface::ShowMessage
      ("new %s=\n%s\n", obj->GetName().c_str(),
       obj->GetGeneratingString(Gmat::MATLAB_STRUCT).c_str());
   #endif
}

