//$Id$
//------------------------------------------------------------------------------
//                                 CallPythonFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
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

//#define DEBUG_CONSTRUCTOR
//#define DEBUG_SETGET
//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION

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


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CallPythonFunction()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
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
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("CallPythonFunction default constructor.\n");
#endif

}


//------------------------------------------------------------------------------
// CallPythonFunction(const CallPythonFunction& cpf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param cpf The object being copied
 */
//------------------------------------------------------------------------------
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
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("CallPythonFunction copy constructor from <%p>.\n", cpf);
#endif

}


//------------------------------------------------------------------------------
// CallPythonFunction& operator =(const CallPythonFunction& cpf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param cpf The object used to set structures in this one
 *
 * @return This command, configured to match cpf
 */
//------------------------------------------------------------------------------
CallPythonFunction& CallPythonFunction::operator=(const CallPythonFunction& cpf)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("CallPythonFunction assignment command from <%p>.\n", cpf);
#endif

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


//------------------------------------------------------------------------------
// ~CallPythonFunction
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
CallPythonFunction::~CallPythonFunction()
{
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script string used for each parameter
 *
 * @param id The ID for the parameter
 *
 * @return The descriptor
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetParameterText(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TEXT[id - CallFunctionParamCount];

   return CallFunction::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a parameter from its string descrition
 *
 * @param str The string associated with the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer CallPythonFunction::GetParameterID(const std::string& str) const
{
   for (Integer i = CallFunctionParamCount; i < PythonFunctionParamCount; ++i)
      if (str == PARAMETER_TEXT[i - CallFunctionParamCount])
         return i;

   return CallFunction::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the type for a parameter
 *
 * @param id The parameter ID
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType CallPythonFunction::GetParameterType(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TYPE[id - CallFunctionParamCount];
   else
      return CallFunction::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the string description of a parameter's type
 *
 * @param id The ID for the parameter
 *
 * @return The string describing the type
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetParameterTypeString(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - CallFunctionParamCount)];
   else
      return CallFunction::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The id for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetStringParameter(const Integer id) const
{
#ifdef DEBUG_SETGET
   MessageInterface::ShowMessage("CallPythonFunction::GetStringParameter(id = %d)\n", id);
#endif
   if (id == MODULENAME)
      return moduleName;
   if (id == FUNCTIONNAME)
      return functionName;

   return CallFunction::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const char* value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param id The id for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const Integer id, const char* value)
{
#ifdef DEBUG_SETGET
   MessageInterface::ShowMessage("CallPythonFunction::SetStringParameter(id = %d, value = <%s>)\n", id, value);
#endif

   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      /// @todo: Determine if this code should match SetStringParameter below
      functionName = value;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param id The id for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const Integer id,
      const std::string& value)
{
#ifdef DEBUG_SETGET
   MessageInterface::ShowMessage("CallPythonFunction::SetStringParameter(id = %d, std::string &value = <%s>)\n", id, value.c_str());
#endif

   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      /// @todo: Determine if this code should match SetStringParameter above
      /// @note I think this is the one used in the script engine
      functionName = value;
      mFunctionName = "Python." + moduleName + "." + functionName;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The script label for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const char* value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const std::string& label,
      const char* value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


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
      FileManager *fileMgr = FileManager::Instance();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("  pythonIf:  %p\n", pythonIf);
      #endif

	   //Initialize Python engine
	   pythonIf->PyInitialize();

	   // Get all Python module paths from the startup file
	   StringArray paths = fileMgr->GetAllPythonModulePaths();

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
      throw CommandException("An unhandled Python exception was thrown during "
            "initialization");
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
      throw CommandException("An unhandled Python exception was thrown during "
         "execution");
   }

   /*------------------------------------------------------------------------*/
   /* GMAT receives Python data following these rules:
    *
    * Floats are passed to GMAT Variables
    *
    * Ints are passed to GMAT Variables
    *
    * Strings are passed to GMAT strings
    *
    * Lists are passed to GMAT arrays (and must be lists of floats) 
    *
    * Lists of lists are passed to GMAT arrays (and must be lists of lists of
    * floats, all of the same dimension)
    *
    * @note: The tuple handling is different from that described here, because
    * Python returns multiple parameters in tuples, and that means mixed types.
    *
    * Tuples must contain numerical data, and are passed to GMAT one-
    * dimensional arrays
    *
    * Tuples of tuples must contain numerical data, and are passed to GMAT 2D 
    * arrays
   --------------------------------------------------------------------------*/
   dataReturn.clear();
   
   if (pyRet)
   {
      if (!BuildReturnFromPyObject(pyRet))
      {
         if (PyTuple_Check(pyRet))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Python has returned a tuple of "
                     "values.\n");
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
               MessageInterface::ShowMessage("Python has returned a memory"
                     "view object\n");
            #endif
         }
         else if (PyLong_Check(pyRet))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Python has returned an Integer "
                     "object\n");
            #endif
         }
         else
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("An unhandled Python type was "
                  "returned.\n");
            #endif
         }
      }

      Py_DECREF(pyRet);

      // Fill in the output parameters
      GetOutParams();

      // clean up the argIns.
      for (Integer i = 0; i < argIn.size(); ++i)
         delete argIn.at(i);
   }
   else   // when return value is NULL and no exception is caught/handled.
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("Unknown error happened in Python "
               "Interface.\n");
      #endif
   }

   BuildCommandSummary(true);

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

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("BuildReturnFromPyObject(%p)\n", member);
   #endif

   if (member != Py_None)
   {
      try // Since we are making Python calls here, wrap in a handler
      {
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
         else if (PyLong_Check(member))
         {
            PyReturnValue rv;
            rv.toType = Gmat::REAL_TYPE;
            rv.floatData.push_back(PyLong_AsDouble(member));
            dataReturn.push_back(rv);
            retval = true;
         }

         // Strings
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

         // Lists of floats/ints or lists of lists of floats/ints
         else if (PyList_Check(member))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Return was a list of size %d\n", 
                  PyList_Size(member));
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
                  MessageInterface::ShowMessage("Python has returned a list of list "
                     "of Floats/Integers.\n");
               #endif

               rv.toType = Gmat::RMATRIX_TYPE;

               for (Integer i = 1; i < listSz; i++)
               {
                  // throw an exception if the output dimension row and column is not
                  // what has been returned by Python, or if the size of each list
                  // within the top list is not the same.
                  if ((PyList_Size(pyItem) != PyList_Size(PyList_GetItem(member, i))) ||
                     (listSz != outRow) || (elementSz != outCol))
                     throw CommandException("The dimension of the array returned "
                     "from Python does not match the dimension of the "
                     "receiving array in GMAT on the script line\n   \"" +
                     GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
               }

               for (Integer i = 0; i < listSz; i++)
               {
                  pyItem = PyList_GetItem(member, i);
                  RealArray vItem;
                  for (Integer j = 0; j < elementSz; j++)
                  {
                     Real ret;
                     PyObject *pyElem = PyList_GetItem(pyItem, j);

                     // If element is a Python Integer/Long, convert to Real
                     if (PyLong_Check(pyElem))
                        ret = PyLong_AsDouble(pyElem);
                     else if (PyFloat_Check(pyElem))
                        ret = PyFloat_AsDouble(pyElem);
                     else
                        throw CommandException("An array member received from Python "
                        "is neither a float nor an integer, so GMAT cannot "
                        "process the value returned on the script line\n   \"" +
                        GetGeneratingString(Gmat::NO_COMMENTS) + "\"");

                     #ifdef DEBUG_EXECUTION
                        MessageInterface::ShowMessage("Array element [%d, %d] value in "
                           "output array is %lf\n", i, j, ret);
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
                  MessageInterface::ShowMessage("Python has returned a list of "
                     "floats.\n");
               #endif
               rv.toType = Gmat::RMATRIX_TYPE;
               for (Integer i = 0; i < listSz; ++i)
               {
                  pyItem = PyList_GetItem(member, i);
                  rv.floatData.push_back(PyFloat_AsDouble(pyItem));

                  #ifdef DEBUG_EXECUTION
                     MessageInterface::ShowMessage("Value is %lf\n", 
                        rv.floatData[rv.floatData.size()-1]);
                  #endif
               }
               dataReturn.push_back(rv);
               retval = true;
            }
            else if (PyLong_Check(pyItem))
            {
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("Python has returned a list of Integers.\n");
               #endif
               rv.toType = Gmat::REAL_TYPE;
               for (Integer i = 0; i < listSz; ++i)
               {
                  pyItem = PyList_GetItem(member, i);
                  rv.floatData.push_back(PyLong_AsDouble(pyItem));
               }
               dataReturn.push_back(rv);
               retval = true;
            }
            else
            {
               // The return type is not handled
               throw CommandException("The list member returned from the Python "
                  "call on the script line\n   \"" +
                  GetGeneratingString(Gmat::NO_COMMENTS) + "\"\nis a type that "
                  "GMAT does not handle.");
            }
         }
         else if (!PyTuple_Check(member))
         {
            // The return type is not handled
            throw CommandException("The returned value from the Python call is a "
               "type not handled by GMAT");
         }
      }
      catch (BaseException &ex)
      {
         throw CommandException(ex.GetFullMessage() + " on the script line\n   \"" +
            GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
      }
      catch (...)
      {
         throw CommandException("An error was encountered processing return data "
            "from Python");
      }
   }
   else 
      // Empty returns are allowed
      retval = true;

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
         throw CommandException("The CallPythonFunction command cannot find "
               "the parameter " + *it + " in script line\n   \"" +
               GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
      }
         
      if (mapObj->IsOfType(Gmat::PARAMETER))
         mInputList.push_back((Parameter *)mapObj);
      else
         throw CommandException("The input field " + (*it) + " was not "
               "recognized as a valid input to the Python interface.");
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
         throw CommandException("The CallPythonFunction command cannot find "
                                "the parameter " + *it + " in script line\n   \"" +
                                GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
      }

      if (mapObj->IsOfType(Gmat::PARAMETER))
         mOutputList.push_back((Parameter *)mapObj);
      else
         throw CommandException("The output field " + (*it) + " was not "
               "recognized as a valid output from the Python interface on the "
               "script line\n   \"" + GetGeneratingString(Gmat::NO_COMMENTS) +
               "\"");
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
 */
//------------------------------------------------------------------------------
void CallPythonFunction::SendInParam(std::vector<void *> &argIn, std::vector<Gmat::ParameterType> &paramType)
{
   for (unsigned int i = 0; i < mInputList.size(); i++)
   {
      Parameter *param = mInputList[i];
      Gmat::ParameterType type = param->GetReturnType();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Parameter %d, %s, type %d\n", i,
               param->GetName().c_str(), type);
      #endif

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

         case Gmat::RMATRIX_TYPE:
         {
            // Array or array element
            bool entireArray = (mInputNames[i].find("(") == std::string::npos);

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("%d: %s, an array%s\n", i,
                     mInputNames[i].c_str(), (entireArray ? "" : " element"));
            #endif

            Array *arr = (Array *) param;
            inRow = arr->GetRowCount();
            inCol = arr->GetColCount();

            if (entireArray)
            {
               if (arr != NULL)
               {
                  if ((inRow > 1) && (inCol > 1))
                     throw CommandException("The parameter " + param->GetName() +
                                 " is a two-dimensional array.  GMAT's Python "
                                 "interface does not support input arrays with "
                                 "more than one dimension.");

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
               }
               else
                  throw CommandException("The Python input parameter " + param->GetName() +
                        " should contain an array, but does not");
            }
            else  // Array element: Passes as a float/Real
            {
               std::string elementStr = mInputNames[i].substr(mInputNames[i].find("(")+1);
               if (elementStr.find(")") == std::string::npos)
                  throw CommandException("The parameter " + param->GetName() +
                        " appears to be an array element, but matching "
                        "parentheses were not found in the scripting \"" +
                        mInputNames[i] + "\" on line\n   " +
                        GetGeneratingString(Gmat::NO_COMMENTS));
               elementStr = elementStr.substr(0, elementStr.find(")"));

               // Scripted row/col values (indexed from 1)
               UnsignedInt row = 1, col = 1;
               bool is2D = (elementStr.find(",") != std::string::npos);
               std::stringstream data(elementStr);
               data >> row;
               if (is2D)
               {
                  data.str(elementStr.substr(elementStr.find(",")+1));
                  data >> col;
               }

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Row %d, col %d\n", row, col);
               #endif

               // In GMAT, 1D arrays are a single row, and indexed by col number
               if (inRow == 1)
               {
                  col = row;
                  row = 1;
               }

               if ((row > inRow) || (col > inCol))
                  throw CommandException("The input array element " + mInputNames[i] +
                        " exceeds the dimensions of the specified array on line\n   " +
                        GetGeneratingString(Gmat::NO_COMMENTS));

               Real *r = new Real;
               *r = arr->GetRealParameter(std::string("SingleValue"), row-1, col-1);;
               argIn.push_back(r);

               paramType.push_back(Gmat::REAL_TYPE);
            }
            break;
         }

         default:
            throw CommandException("The parameter " + param->GetName() +
                        ", with type " + PARAM_TYPE_STRING[type] +
                        ", is not a valid input type for GMAT's Python "
                        "interface.");
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
         default:
            std::cout <<
               "type not RMATRIX_TYPE in CallPythonFunction::SendInParam";
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
                              << rvMatrix.GetNumColumns() << " columns.\n";
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
      throw CommandException(messages.str() + "See the script line\n   \"" +
            GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
}
