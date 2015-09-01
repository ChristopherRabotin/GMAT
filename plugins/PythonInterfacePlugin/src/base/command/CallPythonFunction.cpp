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
#include "InterfaceException.hpp"
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
   inRow             (1),
   inCol             (1),
   outRow            (1),
   outCol            (1),
   pythonIf          (NULL)
{
}

CallPythonFunction::CallPythonFunction(const CallPythonFunction& cpf) :
   CallFunction      (cpf),
   moduleName        (cpf.moduleName),
   functionName      (cpf.functionName),
   inRow             (cpf.inRow),
   inCol             (cpf.inCol),
   outRow            (cpf.outRow),
   outCol            (cpf.outCol),
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
      inRow = cpf.inRow;
      inCol = cpf.inCol;
      outRow = cpf.outRow;
      outCol = cpf.outCol;
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
   MessageInterface::ShowMessage("  Calling CallPythonFunction::Execute()\n");

   // send the in parameters
   std::vector<void *> argIn;
   // send the out parameters
   std::vector<void *> argOut;
   std::vector<Gmat::ParameterType> paramType;
   
   // Prepare the format string specifier (const char *format) to build 
   // Python object.
   SendInParam(argIn, paramType);
  
  // Next, call Python function Wrapper
   PyObject* pyRet = pythonIf->PyFunctionWrapper(moduleName, functionName, argIn, paramType, inRow, inCol, mInputList.size());
  
   /*-----------------------------------------------------------------------------------*/
   // Python Requirements from PythonInterfaceNotes_2015_01_14.txt
   /*-----------------------------------------------------------------------------------*/
   /* GMAT will recieve Python data following these rules:

   * Floats are passed to GMAT Variables
   
   * Ints are passed to GMAT Variables
   
   * Strings are passed to GMAT strings
   * Lists are passed to GMAT arrays (and must be lists of floats) 
   
   * Lists of lists are passed to GMAT arrays (and must be lists of lists of floats,
   * all of the same dimension)
   
   * Tuples must contain numerical data, and are passed to GMAT one-dimensional arrays
   
   * Tuples of tuples must contain numerical data, and are passed to GMAT 2D arrays
  
   ------------------------------------------------------------------------------------*/
   if (pyRet)
   {
      //if the Python module returns a float value without using []
      if (PyFloat_Check(pyRet))
      {
         Real *ret = new Real;
         *ret = PyFloat_AsDouble(pyRet);
         argOut.push_back(ret);
      }
      // else if the Python module returns a string
#ifdef IS_PY3K
      else if (PyUnicode_Check(pyRet))
      {
         MessageInterface::ShowMessage("Python String is returned.\n");
         std::string *pStr = new std::string;
         *pStr = std::string(_PyUnicode_AsString(pyRet));
         argOut.push_back(pStr); 
      }
#else
      else if (PyBytes_Check(pyRet))
      {
         std::string *pStr = new std::string;
         *pStr = std::string(PyBytes_AsString(pyRet));
         argOut.push_back(pStr);
      }
#endif
      // else if the Python module returns a list of floats
      // Or a list of list of floats/Ints.
      // In case of Python Int, we need to convert it to C++ float.
      else if (PyList_Check(pyRet))
      {
         // number of list elements in a list, for example: [ [], [], [] ]
         Integer listSz = PyList_Size(pyRet);
         PyObject *pyItem = PyList_GetItem(pyRet, 0);
         // number of elements in a list, for example: [ 1, 2, 3 ]
         Integer elementSz = PyList_Size(pyItem);

         if (PyList_Check(pyItem))
         {

            MessageInterface::ShowMessage("Python has returned a list of list of Floats/Integers.\n");

            for (Integer i = 1; i < listSz; i++)
            {
               // throw an exception if the output dimension row and column is not what has been returned by Python, or
               // if the size of each list within list is not the same.
               if (PyList_Size(pyItem) != PyList_Size(PyList_GetItem(pyRet, i)) || listSz != outRow || elementSz != outCol)
                  throw InterfaceException(" Python has returned an unexpected array dimension. \n");
            }
            for (Integer i = 0; i < listSz; i++)
            {
               pyItem = PyList_GetItem(pyRet, i);
               std::vector<Real *> *vItem = new std::vector<Real *>;
               for (Integer j = 0; j < elementSz; j++)
               {
                  Real * ret = new Real;
                  PyObject *pyElem = PyList_GetItem(pyItem, j);
                  // if the element is a Python Integer/Long, convert it to C++ double
                  if (PyLong_Check(pyElem))
                     *ret = PyLong_AsDouble(pyElem);
                  // else if the element is a Python Float
                  else if (PyFloat_Check(pyElem))
                     *ret = PyFloat_AsDouble(pyElem);

                  MessageInterface::ShowMessage("Value in output array is %lf\n", *ret);
                  vItem->push_back(ret);
                  
               }

               argOut.push_back(vItem);
            }
         }
         // Python has returned a list of floats
         else if (PyFloat_Check(pyItem))
         {
            for (Integer i = 0; i < listSz; i++)
            {
               Real *ret = new Real;
               
               pyItem = PyList_GetItem(pyRet, i);
               *ret = PyFloat_AsDouble(pyItem);
               
               argOut.push_back(ret);
            }
         }   
      }
      // else if the Python module returns a tuple of numerical values
      else if (PyTuple_Check(pyRet))
      {
      }
      else
      {
         MessageInterface::ShowMessage("An unknown Python type was returned.\n");
      }
         
      Py_DECREF(pyRet);

      // Fill out the parameter out
      GetOutParams(argOut);

      // clean up the argIns.
      for (Integer i = 0; i < argIn.size(); ++i)
         delete argIn.at(i);

      // clean up the argOut
      for (Integer i = 0; i < argOut.size(); ++i)
         delete argOut.at(i);
   }
   else   // when return value is NULL and no exception is caught/handled.
   {
      MessageInterface::ShowMessage("Unknown error happend in Python Interface.\n");
   }

	return true;
}

void CallPythonFunction::RunComplete()
{
   MessageInterface::ShowMessage("  Calling CallPythonFunction::RunComplete()\n");

   if (pythonIf != NULL)
   {
      pythonIf->PyFinalize();
      pythonIf = NULL;
   }

   CallFunction::RunComplete();
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
void CallPythonFunction::SendInParam(std::vector<void *> &argIn, std::vector<Gmat::ParameterType> &paramType)
{
   for (unsigned int i = 0; i < mInputList.size(); i++)
   {
      Parameter *param = mInputList[i];
      Gmat::ParameterType type = param->GetReturnType();
      
      switch (type)
      {
         case Gmat::REAL_TYPE:
         {       
            Real *r = new Real;
            *r = param->EvaluateReal();   
            argIn.push_back(r);

            paramType.push_back(Gmat::REAL_TYPE);

            break;
         }

         case Gmat::STRING_TYPE:
         {
            std::string *pStr = new std::string;
            *pStr = param->EvaluateString();
            argIn.push_back(pStr);
            paramType.push_back(Gmat::STRING_TYPE);

            break;
         }

         case Gmat::RVECTOR_TYPE:
            break;

         case Gmat::RMATRIX_TYPE:
         {
            Array *arr = (Array *) param;
            if (arr != NULL)
            {
               // Fill in the input array dimension
               inRow = arr->GetRowCount();
               inCol = arr->GetColCount();
               
               for (int i = 0; i < inRow; ++i)
               {
                  for (int j = 0; j < inCol; ++j)
                  {
                     Real *ret = new Real;
                     *ret = arr->GetRealParameter(std::string("SingleValue"), i, j);
                     argIn.push_back(ret);
                  }
               }

               paramType.push_back(Gmat::RMATRIX_TYPE);

               break;
            }
         }

         default:
            MessageInterface::ShowMessage("Unkown type: %d\n", type);
            break;
      }
   }

  

   // Fill in the output array dimension
   for (unsigned int i = 0; i < mOutputList.size(); i++)
   {
      Parameter *param = mOutputList[i];
      Gmat::ParameterType type = param->GetReturnType();

      switch (type)
      {
         case Gmat::RMATRIX_TYPE:
         {
            Array *arr = (Array *)param;
            if (arr != NULL)
            {
               outRow = arr->GetRowCount();
               outCol = arr->GetColCount();
            }

            break;
         }
      }
   }

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
         case Gmat::STRING_TYPE:
         {
            MessageInterface::ShowMessage("String message is %s\n", ((std::string*)(argOut.at(i)))->c_str());
            param->SetString(*(std::string*)(argOut.at(0)));
            break;
         }

         case Gmat::REAL_TYPE:
         {
            param->SetReal(*(Real*)argOut.at(i));
            break;
         }

         case Gmat::RMATRIX_TYPE:
         {
            Rmatrix rMatrix;
            rMatrix = param->EvaluateRmatrix();

            for (Integer i = 0; i < argOut.size(); i++)
            {
               std::vector<Real *> *vItem = (std::vector<Real *> *) argOut.at(i);
               for (Integer j = 0; j < vItem->size(); j++)
               {
                  rMatrix(i, j) = *(vItem->at(j));
               }
            }
            param->SetRmatrix(rMatrix);

            break;
         }
      }
   }        
}
