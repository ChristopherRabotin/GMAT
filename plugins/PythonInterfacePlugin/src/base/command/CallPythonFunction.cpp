//$Id$
//------------------------------------------------------------------------------
//                                 CallPythonFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II .
//
// Author: Farideh Farahnak
// Created: 2015/02/23
//
/**
* Definition for the CallPythonFunction command class
*/
//------------------------------------------------------------------------------
#include "CallPythonFunction.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------
const std::string
CallPythonFunction::PARAMETER_TEXT[PythonFunctionParamCount - CallFunctionParamCount] =
{
      "PythonModule",
      "PythonFunction"
};


const Gmat::ParameterType
CallPythonFunction::PARAMETER_TYPE[PythonFunctionParamCount - CallFunctionParamCount] =
{
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE
};


CallPythonFunction::CallPythonFunction() :
   CallFunction      ("CallPythonFunction"),
   moduleName        (""),
   functionName      (""),
   pythonIf          (NULL)
{
}

CallPythonFunction::CallPythonFunction(const CallPythonFunction& cpf) :
   CallFunction      (cpf),
   moduleName        (cpf.moduleName),
   functionName      (cpf.functionName),
   pythonIf          (cpf.pythonIf)
{
}

CallPythonFunction& CallPythonFunction::operator =(
      const CallPythonFunction& cpf)
{
   if (this != &cpf)
   {
      CallFunction::operator=(cpf);
      moduleName   = cpf.moduleName;
      functionName = cpf.functionName;
      pythonIf = cpf.pythonIf;
   }

   return *this;
}

CallPythonFunction::~CallPythonFunction()
{
}

std::string CallPythonFunction::GetParameterText(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TEXT[id - CallFunctionParamCount];
   else
      return CallFunction::GetParameterText(id);
}

Integer CallPythonFunction::GetParameterID(const std::string& str) const
{
   for (Integer i = CallFunctionParamCount; i < PythonFunctionParamCount; ++i)
      if (str == PARAMETER_TEXT[i - CallFunctionParamCount])
         return i;

   return CallFunction::GetParameterID(str);
}

Gmat::ParameterType CallPythonFunction::GetParameterType(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TYPE[id - CallFunctionParamCount];
   else
      return CallFunction::GetParameterType(id);
}

std::string CallPythonFunction::GetParameterTypeString(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - CallFunctionParamCount)];
   else
      return CallFunction::GetParameterTypeString(id);
}

bool CallPythonFunction::IsParameterReadOnly(const Integer id) const
{
   return CallFunction::IsParameterReadOnly(id);
}

bool CallPythonFunction::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

std::string CallPythonFunction::GetStringParameter(const Integer id) const
{
   if (id == MODULENAME)
      return moduleName;
   if (id == FUNCTIONNAME)
      return functionName;

   return CallFunction::GetStringParameter(id);
}

bool CallPythonFunction::SetStringParameter(const Integer id, const char* value)
{
   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      functionName = value;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}

bool CallPythonFunction::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      functionName = value;
      mFunctionName = "Python." + moduleName + "." + functionName;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}

//std::string CallPythonFunction::GetStringParameter(const Integer id,
//      const Integer index) const
//{
//   return CallFunction::GetStringParameter(id, index);
//}

//bool CallPythonFunction::SetStringParameter(const Integer id, const char* value,
//      const Integer index)
//{
//   return CallFunction::SetStringParameter(id, value, index);
//}

//bool CallPythonFunction::SetStringParameter(const Integer id,
//      const std::string& value, const Integer index)
//{
//   return CallFunction::GetStringParameter(id, value, index);
//}

std::string CallPythonFunction::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool CallPythonFunction::SetStringParameter(const std::string& label,
      const char* value)
{
   return SetStringParameter(GetParameterID(label), value);
}

bool CallPythonFunction::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//std::string CallPythonFunction::GetStringParameter(const std::string& label,
//      const Integer index) const
//{
//   return GetStringParameter(GetParameterID(label), index);
//}

//bool CallPythonFunction::SetStringParameter(const std::string& label,
//      const std::string& value, const Integer index)
//{
//   return SetStringParameter(GetParameterID(label), value, index);
//}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Initialize Python engine
*
* This method will initialize python by loading Python engine and setting the 
* PYTHONPATH.
*
* @param none
*
* @return bool
*/
//------------------------------------------------------------------------------
bool CallPythonFunction::Initialize()
{
	bool ret = false;

   MessageInterface::ShowMessage("  Calling CallPythonFunction::Initialize()\n");

   ret = CallFunction::Initialize();

   MessageInterface::ShowMessage("  Base class init complete\n");

	pythonIf = PythonInterface::PyInstance();
   FileManager *fm = FileManager::Instance();

   MessageInterface::ShowMessage("  pythonIf:  %p\n", pythonIf);
   
	//Initialize Python engine
	pythonIf->PyInitialize();

	// Get all Python module paths from the startup file
	StringArray paths = fm->GetAllPythonModulePaths();

   MessageInterface::ShowMessage("  Adding %d python paths\n", paths.size());

	pythonIf->PyAddModulePath(paths);
   
   //Fill in Inputlist
   Integer sizeIn = FillInputList();
   MessageInterface::ShowMessage("  SizeIn is %d\n", sizeIn);

   //Fill in Outputlist
   Integer sizeOut = FillOutputList();
   MessageInterface::ShowMessage("  SizeOut is %d\n", sizeOut);

	return ret;
}

//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
* Execute as a Python wrapper
*
* This method will execute the Python functions in a Python module.
*
* @param none
*
* @return bool
*/
//------------------------------------------------------------------------------
bool CallPythonFunction::Execute()
{
   // send the in parameters
   std::string formatIn("");
   std::vector<void *> argIn;
   // send the out parameters
   std::vector<void *> argOut;

   // Prepare the format string specifier (const char *format) to build 
   // Python object.
   SendInParam(formatIn, argIn);

  // Next call Python function Wrapper
   PyObject* pyRet = pythonIf->PyFunctionWrapper(moduleName, functionName, formatIn, argIn);
 
   /*------------------------------------------------------------------------------------*/
   // Python Requirements on output paramerters from PythonInterfaceNotes_2015_01_14.txt
   /*------------------------------------------------------------------------------------*/
   /* GMAT will recieve Python data following these rules:

   * Floats are passed to GMAT Variables
   
   * Ints are passed to GMAT Variables
   
   * Strings are passed to GMAT strings
   * Lists are passed to GMAT arrays (and must be lists of floats) 
   
   * Lists of lists are passed to GMAT arrays (and must be lists of lists of floats,
   * all of the same dimension)
   
   * Tuples must contain numerical data, and are passed to GMAT one-dimensional arrays
   
   * Tuples of tuples must contain numerical data, and are passed to GMAT 2D arrays
  
   ************************************************************************************/
   Real ret = 0;
   if (pyRet)
   {
      //if the Python module returns a float value without using []
      if (PyFloat_Check(pyRet))
      {
         ret = PyFloat_AsDouble(pyRet);
         argOut.push_back(&ret);
         MessageInterface::ShowMessage("  ret:  %f\n", ret);
      }
      // else if the Python module returns a string
#ifdef IS_PY3K
      else if (PyUnicode_Check(pyRet))
      {
      }
#else
      else if (PyBytes_Check(pyRet))
      {
      }
#endif
      // else if the Python module returns a list of floats
      else if (PyList_Check(pyRet))
      {
         for (unsigned int i = 0; i < mOutputList.size(); i++)
         {
            PyObject* pyItem = PyList_GetItem(pyRet, i);
            if (PyFloat_Check(pyItem))
            {
               ret = PyFloat_AsDouble(pyItem);
               MessageInterface::ShowMessage("Python object converted to Real Type as: %f\n", ret);
               argOut.push_back(&ret);
            }
         }
      }
      // else if the Python module returns a tuple of numerical values
      else if (PyTuple_Check(pyRet))
      {
      }
         
      Py_DECREF(pyRet);

      // Fill out the parameter out
      GetOutParams(argOut);

      // clean up the argIns.
      std::vector<void *>::iterator it;
      for (it = argIn.begin(); it != argIn.end(); ++it)
          delete *it;
   }

   MessageInterface::ShowMessage("  pyRet:  %p\n", pyRet); 

	return true;
}

void CallPythonFunction::RunComplete()
{
	return;
}


//------------------------------------------------------------------------------
// Integer FillInputList()
//------------------------------------------------------------------------------
/**
* Fills in input parameters
*
* This method will fill in input parameters declared in script.
*
* @param none
*
* @return Integer
*/
//------------------------------------------------------------------------------
Integer CallPythonFunction::FillInputList()
{
   GmatBase *mapObj;

   mInputList.clear();
  
   StringArray ar = GetStringArrayParameter(ADD_INPUT);
   StringArray::iterator it;

   for (it = ar.begin(); it != ar.end(); ++it)
   {
      if ((mapObj = FindObject(*it)) == NULL)
      {
         throw CommandException("   CallPythonFunction command cannot find Parameter " +
                                       *it + " in script line\n   \"" +
                                          GetGeneratingString(Gmat::SCRIPTING) + "\"");
      }
         
      if (mapObj->IsOfType(Gmat::PARAMETER))
         mInputList.push_back((Parameter *)mapObj);
   }
      
   return mInputList.size();
}


//------------------------------------------------------------------------------
// Integer FillOutputList()
//------------------------------------------------------------------------------
/**
* Fills in output parameters
*
* This method will fill in output parameters declared in script.
*
* @param none
*
* @return Integer
*/
//------------------------------------------------------------------------------
Integer CallPythonFunction::FillOutputList()
{
   GmatBase *mapObj;

   mOutputList.clear();

   StringArray ar = GetStringArrayParameter(ADD_OUTPUT);
   StringArray::iterator it;

   for (it = ar.begin(); it != ar.end(); ++it)
   {
      if ((mapObj = FindObject(*it)) == NULL)
      {
         throw CommandException("   CallPythonFunction command cannot find Parameter " +
                                       *it + " in script line\n   \"" +
                                          GetGeneratingString(Gmat::SCRIPTING) + "\"");
      }

      if (mapObj->IsOfType(Gmat::PARAMETER))
         mOutputList.push_back((Parameter *)mapObj);
   }

   return mOutputList.size();
}


//------------------------------------------------------------------------------
// void SendInParam()
//------------------------------------------------------------------------------
/**
* Fills in format string and input parameters for Python function
*
* This method will fill in format string and input parameters
*
* @param none
*
* @return void
*/
//------------------------------------------------------------------------------
void CallPythonFunction::SendInParam(std::string &formatIn, std::vector<void *> &argIn)
{
   for (unsigned int i = 0; i < mInputList.size(); i++)
   {
      Parameter *param = mInputList[i];
      Gmat::ParameterType type = param->GetReturnType();
      
      switch (type)
      {
         case Gmat::REAL_TYPE:
         {
            if (i == 0)
               formatIn.append("(f");
            else
               formatIn.append("f");
        
            Real *r = new Real;
            *r = param->EvaluateReal();
            argIn.push_back(r);

            break;
         }
         case Gmat::STRING:
         {
            if (i == 0)
               formatIn.append("(s");
            else
               formatIn.append("s");

            char *str = new char[64];
            str = const_cast<char *>(param->EvaluateString().c_str());
            argIn.push_back(str);

            break; 
         }
         case Gmat::ARRAY:
            break;
      }
   }

   formatIn.append(")");

}

//------------------------------------------------------------------------------
// void GetOutParams()
//------------------------------------------------------------------------------
/**
* Get output parameters
*
* This method will fill in output parameters
*
* @param vector<void *> argOut
*
* @return void
*/
//------------------------------------------------------------------------------
void CallPythonFunction::GetOutParams(const std::vector<void *> &argOut)
{
   
   for (unsigned int i = 0; i < mOutputList.size(); i++)
   {
      Parameter *param = mOutputList[i];
      Gmat::ParameterType type = param->GetReturnType();

      switch (type)
      {
         case Gmat::REAL_TYPE:
         {
            param->SetReal(*(Real*)argOut.at(i));
            break;
         }

      }
   }
            
}