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

// #define DEBUG_INITIALIZATION
// #define DEBUG_EXECUTION

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
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialize Python engine
 *
 * This method initializes Python by loading the Python engine and setting the 
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

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  Calling CallPythonFunction::Initialize()\n");
   #endif

   ret = CallFunction::Initialize();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  Base class init complete\n");
   #endif

   try
   {
      pythonIf = PythonInterface::PyInstance();
      FileManager *fm = FileManager::Instance();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("  pythonIf:  %p\n", pythonIf);
      #endif

	   //Initialize Python engine
	   pythonIf->PyInitialize();

	   // Get all Python module paths from the startup file
	   StringArray paths = fm->GetAllPythonModulePaths();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("  Adding %d python paths\n", paths.size());
      #endif

	   pythonIf->PyAddModulePath(paths);
   }
   catch (BaseException &ex)
   {
      throw CommandException("Error in the CallPython command initializaton:\n" +
         ex.GetFullMessage());
   }
   catch (...)
   {
      throw CommandException("An unhandled Python exception was thrown during initialization");
   }

   //Fill in Inputlist
   Integer sizeIn = FillInputList();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  SizeIn is %d\n", sizeIn);
   #endif

   //Fill in Outputlist
   Integer sizeOut = FillOutputList();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  SizeOut is %d\n", sizeOut);
   #endif

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
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("  Calling CallPythonFunction::Execute()\n");
   #endif

   // send the in parameters
   std::vector<void *> argIn;
//   // send the out parameters
//   std::vector<void *> argOut;
   std::vector<Gmat::ParameterType> paramType;
   
   // Prepare the format string specifier (const char *format) to build 
   // Python object.
   SendInParam(argIn, paramType);
   PyObject* pyRet;

  // Next, call Python function Wrapper
   try
   {
      pyRet = pythonIf->PyFunctionWrapper(moduleName, functionName, argIn,
            paramType, inRow, inCol, mInputList.size());
   }
   catch (BaseException &ex)
   {
      throw CommandException("Error in the CallPython command execution:\n" +
         ex.GetFullMessage());
   }
   catch (...)
   {
      throw CommandException("An unhandled Python exception was thrown during execution");
   }

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
   dataReturn.clear();

   if (pyRet)
   {
      if (!BuildReturnFromPyObject(pyRet))
      {
         if (PyTuple_Check(pyRet))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Python has returned a tuple of values.\n");
            #endif
            Integer tupleSz = PyTuple_Size(pyRet);
            for (Integer i = 0; i < tupleSz; ++i)
            {
               PyObject *member = PyTuple_GetItem(pyRet, i);
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("   %d: %p\n", i, member);
               #endif
               BuildReturnFromPyObject(member);
            }
         }
         else if (PyMemoryView_Check(pyRet))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Python has returned a memoryview object\n");
            #endif
         }
         else if (PyLong_Check(pyRet))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Python has returned an Integer object\n");
            #endif
         }
         else
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("An unknown Python type was returned.\n");
            #endif
         }
      }

      Py_DECREF(pyRet);

      // Fill out the parameter out
      GetOutParams();

      // clean up the argIns.
      for (Integer i = 0; i < argIn.size(); ++i)
         delete argIn.at(i);
   }
   else   // when return value is NULL and no exception is caught/handled.
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("Unknown error happened in Python Interface.\n");
      #endif
   }

	return true;
}


//------------------------------------------------------------------------------
// bool CallPythonFunction::BuildReturnFromPyObject(PyObject* member)
//------------------------------------------------------------------------------
/**
 * Builds a PyReturnValue struct and pushes it to the vector of returned data
 *
 * @param PyObject The object being handled.
 *
 * Note that Tuple objects are not handled here -- the calling function breaks
 * the tuple into separate PyObjects and passes them in one at a time.
 *
 * @return true If a PyReturnValue was pushed onto the returned data vector.
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::BuildReturnFromPyObject(PyObject* member)
{
   bool retval = false;

   // Reals
   if (PyFloat_Check(member))
   {
      PyReturnValue rv;
      rv.toType = Gmat::REAL_TYPE;
      rv.floatData.push_back(PyFloat_AsDouble(member));
      dataReturn.push_back(rv);
      retval = true;
   }

   // Integers, passed into real number containers
   if (PyLong_Check(member))
   {
      PyReturnValue rv;
      rv.toType = Gmat::REAL_TYPE;
      rv.floatData.push_back(PyLong_AsDouble(member));
      dataReturn.push_back(rv);
      retval = true;
   }

   // Strings
   // else if the Python module returns a string
#ifdef IS_PY3K
   else if (PyUnicode_Check(member))
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("A Python String was returned.\n");
      #endif
      PyReturnValue rv;
      rv.toType = Gmat::STRING_TYPE;
      rv.stringData = _PyUnicode_AsString(member);
      dataReturn.push_back(rv);
      retval = true;
   }
#else
   else if (PyBytes_Check(member))
   {
      PyReturnValue rv;
      rv.toType = Gmat::STRING_TYPE;
      rv.stringData.push_back(PyBytes_AsString(member));
      dataReturn.push_back(rv);
      retval = true;
   }
#endif

   // else if the Python module returns a list of floats
   // Or a list of list of floats/Ints.
   // In case of Python Int, we need to convert it to C++ float.
   else if (PyList_Check(member))
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("Return was a list of size %d\n", PyList_Size(member));
      #endif

      // number of list elements in a list, for example: [ [], [], [] ]
      Integer listSz = PyList_Size(member);
      PyObject *pyItem = PyList_GetItem(member, 0);
      // number of elements in a list, for example: [ 1, 2, 3 ]
      Integer elementSz = PyList_Size(pyItem);
      PyReturnValue rv;

      if (PyList_Check(pyItem))
      {

         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Python has returned a list of list of Floats/Integers.\n");
         #endif

         rv.toType = Gmat::RMATRIX_TYPE;

         for (Integer i = 1; i < listSz; i++)
         {
            // throw an exception if the output dimension row and column is not what has been returned by Python, or
            // if the size of each list within list is not the same.
            if (PyList_Size(pyItem) != PyList_Size(PyList_GetItem(member, i)) || listSz != outRow || elementSz != outCol)
               throw CommandException("Python has returned an unexpected array dimension. \n");
         }
         for (Integer i = 0; i < listSz; i++)
         {
            pyItem = PyList_GetItem(member, i);
            RealArray vItem;
            for (Integer j = 0; j < elementSz; j++)
            {
               Real ret;
               PyObject *pyElem = PyList_GetItem(pyItem, j);

               // if the element is a Python Integer/Long, convert it to C++ double
               if (PyLong_Check(pyElem))
                  ret = PyLong_AsDouble(pyElem);
               // else if the element is a Python Float
               else if (PyFloat_Check(pyElem))
                  ret = PyFloat_AsDouble(pyElem);

               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("Array element [%d, %d] value in output array is %lf\n", i, j, ret);
               #endif

               vItem.push_back(ret);
            }
            rv.lolData.push_back(vItem);
         }
         dataReturn.push_back(rv);
         retval = true;
      }
      else if (PyFloat_Check(pyItem))
      {
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Python has returned a list of floats.\n");
         #endif
         rv.toType = Gmat::RMATRIX_TYPE;
         for (Integer i = 0; i < listSz; ++i)
         {
            pyItem = PyList_GetItem(member, i);
            rv.floatData.push_back(PyFloat_AsDouble(pyItem));

            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Value is %lf\n", rv.floatData[rv.floatData.size()-1]);
            #endif
         }
         dataReturn.push_back(rv);
         retval = true;
      }
   }

   return retval;
}



//------------------------------------------------------------------------------
// void CallPythonFunction::RunComplete()
//------------------------------------------------------------------------------
/**
* Finalizes and closes the Python interface
*/
//------------------------------------------------------------------------------
void CallPythonFunction::RunComplete()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("  Calling CallPythonFunction::RunComplete()\n");
   #endif
   if (pythonIf != NULL)
   {
      try 
      {
         pythonIf->PyFinalize();
         pythonIf = NULL;
      }
      catch (BaseException &ex)
      {
         throw CommandException("Error in the CallPython command finalizaton:\n" +
            ex.GetFullMessage());
      }
      catch (...)
      {
         throw CommandException("An unhandled Python exception was thrown during finalization");
      }
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
* Checks match between returned data and GMAT expectations, and fills in output parameters
*
* @return void
*/
//------------------------------------------------------------------------------
void CallPythonFunction::GetOutParams()
{
   std::stringstream messages;

   // First validate that the right number of parameters was returned
   if (dataReturn.size() != mOutputList.size())
   {
      messages << "Python returned " << dataReturn.size()
               << " output parameters, and GMAT expected "
               << mOutputList.size() << " returned values.";

      throw CommandException(messages.str());
   }

   Parameter *param;

   for (UnsignedInt i = 0; i < dataReturn.size(); ++i)
   {
      param = mOutputList[i];

      switch (dataReturn[i].toType)
      {
      case Gmat::STRING_TYPE:
         if (param->GetReturnType() != Gmat::STRING_TYPE)
         {
            messages << "The type returned from Python, a string, does not "
                        "match the type expected by GMAT, a "
                     << PARAM_TYPE_STRING[param->GetReturnType()] << ".\n";
            continue;
         }
         param->SetString(dataReturn[i].stringData);
         break;

      case Gmat::REAL_TYPE:
         if (param->GetReturnType() != Gmat::REAL_TYPE)
         {
            messages << "The type returned from Python, a real number, does not "
                        "match the type expected by GMAT, a "
                     << PARAM_TYPE_STRING[param->GetReturnType()] << ".\n";
            continue;
         }
         param->SetReal(dataReturn[i].floatData[0]);
         break;

      case Gmat::RMATRIX_TYPE:
         if (param->GetReturnType() != Gmat::RMATRIX_TYPE)
         {
            messages << "The type returned from Python, an array, does not "
                        "match the type expected by GMAT, a "
                     << PARAM_TYPE_STRING[param->GetReturnType()] << ".\n";
            continue;
         }
         {
            // Validate size matches
            Rmatrix rvMatrix = param->EvaluateRmatrix();
            Integer expectedRowCount = rvMatrix.GetNumRows();
            Integer expectedColCount = rvMatrix.GetNumColumns();

            bool is1D = expectedRowCount == 1 ? true : false;

            if (is1D)
            {
               if (expectedColCount != dataReturn[i].floatData.size())
               {
                  messages << "Size mismatch in the array returned from Python at"
                              " index "
                           << i << ": the returned array has "
                           << dataReturn[i].floatData.size()
                           << " elements and GMAT expected "
                           << expectedColCount << " elements.\n";
                  break;
               }
            }
            else
            {
               if (expectedRowCount != dataReturn[i].lolData.size())
               {
                  messages << "Size mismatch in the array returned from Python at"
                              " index "
                           << i << ": the returned array has "
                           << dataReturn[i].lolData.size()
                           << " rows and GMAT expected "
                           << rvMatrix.GetNumRows() << " rows.\n";
                  continue;
               }
            }

            if (is1D)
               for (UnsignedInt j = 0; j < dataReturn[i].floatData.size(); ++j)
                  rvMatrix(0, j) = dataReturn[i].floatData[j];
            else
            {
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("GMAT is expecting %d rows and %d "
                        "columns.\nThe Python data has %d rows and the first row "
                        "has %d columns\n", expectedRowCount, expectedColCount,
                        dataReturn[i].lolData.size(), dataReturn[i].lolData[0].size());
               #endif

               for (UnsignedInt j = 0; j < dataReturn[i].lolData.size(); ++j)
               {
                  if (expectedColCount != dataReturn[i].lolData[j].size())
                  {
                     messages << "Size mismatch in the array returned from Python at"
                                 " index "
                              << i << ": the returned array has "
                              << dataReturn[i].lolData.size()
                              << " columns and GMAT expected "
                              << rvMatrix.GetNumRows() << " columns.\n";
                     continue;
                  }

                  for (Integer k = 0; k < dataReturn[i].lolData[j].size(); ++k)
                        rvMatrix(j, k) = dataReturn[i].lolData[j][k];
               }
            }

            param->SetRmatrix(rvMatrix);
         }
         break;

      default:
         messages << "Type mismatch in the returned data at index "
                  << i << ": the returned data is not a type handled by the "
                          "Python interface.";
         break;
      }
   }

   if (messages.str() != "")
      throw CommandException(messages.str());
}
