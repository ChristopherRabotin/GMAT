//$Id$
//------------------------------------------------------------------------------
//                                 PythonInterface
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
// Created: 2015/05/22
//
/**
 * Definition for the PythonInterface command class
 */
//------------------------------------------------------------------------------
#include "stdlib.h"

#include "PythonInterface.hpp"
#include "MessageInterface.hpp"
#include "InterfaceException.hpp"
#include "Array.hpp"
#include <iostream>

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION

PythonInterface* PythonInterface::instance = NULL;

//------------------------------------------------------------------------------
// PythonInterface* PyInstance()
//------------------------------------------------------------------------------
/**
 * Access method for the Python interface singleton
 *
 * @return The singleton pointer
 */
//------------------------------------------------------------------------------
PythonInterface* PythonInterface::PyInstance()
{
   if (instance == NULL)
	   instance = new PythonInterface("PythonInterface");

   return instance;
}


//------------------------------------------------------------------------------
// PythonInterface(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PythonInterface::PythonInterface(const std::string &name) : 
   Interface      ("PythonInterface", name)
{
   isPythonInitialized = false;
   numPyCommands = 0;
   plF = new char[2];
   memset(plF, 0, 2);
}


//------------------------------------------------------------------------------
// ~PythonInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PythonInterface::~PythonInterface()
{
   delete[] plF;
}


//------------------------------------------------------------------------------
// PythonInterface(const PythonInterface& pi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param pi The interface copied to this one
 *
 * @note Since this is a singleton, we need to determine if this methos is ever 
 * called
 */
//------------------------------------------------------------------------------
PythonInterface::PythonInterface(const PythonInterface& pi) :
   Interface      (pi)
{
   plF = new char[2];
   memcpy(plF, pi.plF, 2);
}


//------------------------------------------------------------------------------
// PythonInterface& operator=(const PythonInterface& pi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param pi The interface providing data for this one
 *
 * @note Since this is a singleton, we need to determine if this methos is ever
 * called
 */
//------------------------------------------------------------------------------
PythonInterface& PythonInterface::operator=(const PythonInterface& pi)
{
   if (&pi != this)
   {
	   Interface::operator=(pi);
      numPyCommands = pi.numPyCommands;
      isPythonInitialized = pi.isPythonInitialized;
      plF = pi.plF;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
/**
 * Copies settings from the input PythonInterface to this one
 *
 * @param orig The Interface being copied
 *
 * @return The new PythonInterface
 *
 * @note Since this is a singleton, we need to determine if this methos is ever
 * called
 */
//------------------------------------------------------------------------------
void PythonInterface::Copy(const GmatBase* orig)
{
   operator=(*((PythonInterface *)(orig)));
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method clones the current PythonInterface to make a new one.
 *
 * @return The clone
 *
 * @note Since this is a singleton, we need to determine if this methos is ever
 * called
 */
//------------------------------------------------------------------------------
GmatBase* PythonInterface::Clone() const
{
   return (new PythonInterface(*this));
}


//------------------------------------------------------------------------------
// bool PyInitialize()
//------------------------------------------------------------------------------
/**
 * Load Python engine
 *
 * This method will initialize python by loading Python engine.
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool PythonInterface::PyInitialize()
{
   // Initialize Python only once.
   if (!isPythonInitialized)
      Py_Initialize();

   if (Py_IsInitialized())
   {
      if (!isPythonInitialized)

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Python is initialized/Loaded.\n");
      #endif

	   isPythonInitialized = true;
      numPyCommands++;
   }
   else
   {
	   throw CommandException ("Python failed to load properly.");
   }
		
   return isPythonInitialized;
}


//------------------------------------------------------------------------------
// bool PyFinalize()
//------------------------------------------------------------------------------
/**
 * UnLoad Python engine
 *
 * This method will shut down/unload python engine.
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool PythonInterface::PyFinalize()
{
   // When all the Python commands in the Gmat script are run and completed,
   // close and finalize Python.
   if (--numPyCommands == 0)
   {
//	   Py_Finalize();

      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("Python was Finalized and Unloaded.\n");
      #endif
///
//      isPythonInitialized = false;
   }
	
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("numPyCommands: %d  (Python is unloaded "
            "when this counter reaches 0)\n", numPyCommands);
   #endif

   return isPythonInitialized;
}


//------------------------------------------------------------------------------
// void PyPathSep()
//------------------------------------------------------------------------------
/**
 * Operating system environment/system path delimiter
 *
 * This method will differentiate the path delimiter for different OS.
 *
 * @return void
 */
//------------------------------------------------------------------------------
void PythonInterface::PyPathSep()
{
	const char* plForm = Py_GetPlatform();
   std::string str(plForm);
   
   // Unix and Mac separator is ":"
   // Windows separator is ";"
   if (str.find("win") != std::string::npos)
      plF = ";";
   else
      plF = ":";
}

//------------------------------------------------------------------------------
// void PyAddModulePath()
//------------------------------------------------------------------------------
/**
 * Add Python module to the system path.
 *
 * @param StringArray & path
 */
//------------------------------------------------------------------------------
void PythonInterface::PyAddModulePath(const StringArray& path)
{
   wchar_t *s3K = new wchar_t[8192];
   char *destPath = new char[8192];

   // Platform path seperator delimiter
   PyPathSep();

#ifdef IS_PY3K
   //convert wchar_t to char
   wcstombs(destPath, Py_GetPath(), 8192);
   //concatenate the path delimiter (unix , windows and mac)
   strcat(destPath, plF);
#else
   strcpy(destPath, Py_GetPath());
   strcat(destPath, plF);
#endif

   StringArray::const_iterator it;
   for (it = path.begin(); it != path.end(); ++it)
   {
      strcat(destPath, it->c_str());
      strcat(destPath, plF);     
   }
 
#ifdef IS_PY3K
   //convert char to wchar_t
   mbstowcs(s3K, destPath, 8192);
   PySys_SetPath(s3K);
#else
   PySys_SetPath(destPath);
#endif

   delete[] destPath;
   delete[] s3K;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  Leaving PyAddModulePath( ) \n");
   #endif
}


//------------------------------------------------------------------------------
// PyObject* PyFunctionWrapper(const std::string &modName, 
//    const std::string &funcName, const std::vector<void *> &argIn,
//    std::vector<Gmat::ParameterType> paramType, UnsignedInt row, 
//    UnsignedInt col, UnsignedInt argSz)
//------------------------------------------------------------------------------
/**
 * Method that calls the scripted Python function
 *
 * @param modName The name of teh Python file being called
 * @param funcName The Python function in the module
 * @param argIn The input parameters
 * @param paramType The type associated with each input
 * @param row The number of rows in the input (is this used?)
 * @param col The number of columns in the input (is this used?)
 * @param argSz The number of input arguments
 *
 * @return The PyObject containing the returned data
 */
//------------------------------------------------------------------------------
PyObject* PythonInterface::PyFunctionWrapper(const std::string &modName, 
                                             const std::string &funcName,
                                             const std::vector<void *> &argIn, 
                                             std::vector<Gmat::ParameterType> paramType, 
                                             UnsignedInt row, UnsignedInt col, 
                                             UnsignedInt argSz)
{
   PyObject* pyModule = NULL;
   PyObject* pyPluginModule = NULL;
   PyObject* pyFuncAttr = NULL;
   PyObject* pyArgs = NULL;
   PyObject* pyFunc = NULL;
   PyObject* pyTupleObj = NULL;

   //error messages
   PyObject* pType = NULL;
   PyObject* pValue = NULL;
   PyObject* pTraceback = NULL;

   std::string msg;

   // Build the Python Tuple object based on the format string
   pyTupleObj = PyTuple_New(argSz);
   // index to add elements to Tuple
   int i = 0; 
   int n = 0;

#ifdef IS_PY3K
   // create a python Unicode object from an UTF-8 encoded null terminated char buffer
   pyModule = PyUnicode_FromString(modName.c_str() );
#else
   pyModule = PyBytes_FromString(modName.c_str() );
#endif
  
   if (!pyModule)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);
      throw InterfaceException(" Python Exception: " + msg + "\n");
   }

   // import the python module
   pyPluginModule = PyImport_Import(pyModule);   
   Py_DECREF(pyModule);

   if (!pyPluginModule)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);
      throw InterfaceException(" Python Exception: " + msg + "\n");
   }

   // retrieve an attribute named 'funcName' from the python module
   pyFuncAttr = PyObject_GetAttrString(pyPluginModule, funcName.c_str() );
   Py_DECREF(pyPluginModule);

   if (!pyFuncAttr)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);
      throw InterfaceException(" Python Exception: " + msg + "\n");
   }

   /*
    * GMAT supports passing data to Python using the following rules:
    *
    * Variable data is passed into a Python float
    *
    * Array data is passed in memoryview objects for one dimensional arrays
    *
    * Two dimensional arrays are not supported
    *
    * Strings are passed to Python strings
    */
  
   for (UnsignedInt index = 0; index < paramType.size(); ++index)
   {
      Gmat::ParameterType parType = paramType.at(index);

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Paramater Type is %d\n", parType);
         MessageInterface::ShowMessage("INDEX is %d\n", index);
      #endif

      if (parType == Gmat::RMATRIX_TYPE)
      {
         Py_buffer *pybuffer = (Py_buffer *)malloc(sizeof(Py_buffer));
         Real *v = new Real[row*col];
         
         for (UnsignedInt m = 0; m < row; ++m)
         {
            for (UnsignedInt k = 0; k < col; ++k)
            {
               v[m*col+k] = *(Real*)argIn.at(m*col + k +n);

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("v[] is %lf\n", v[m*col+k]);
               #endif
            }
         }
            
         n += col;

         pybuffer->obj = NULL;
         pybuffer->format = "d";
         pybuffer->ndim = (row != 1 && col != 1) ? 2 : 1;
         pybuffer->shape = (Py_ssize_t *)malloc(
                            sizeof(Py_ssize_t)* pybuffer->ndim);
         pybuffer->readonly = 0;
         pybuffer->suboffsets = 0;
         pybuffer->buf = v;

         pybuffer->strides = (Py_ssize_t *)malloc(
                              sizeof(Py_ssize_t)* pybuffer->ndim);
         for (UnsignedInt m = 0; m < row; ++m)
            pybuffer->shape[m] = col;

         PyBuffer_FillContiguousStrides(pybuffer->ndim, pybuffer->shape, 
                                        pybuffer->strides, sizeof(Real), 'C');

         pybuffer->itemsize = sizeof(Real);
         pybuffer->len = (pybuffer->ndim == 1 ?
            pybuffer->shape[0] * pybuffer->itemsize :
            pybuffer->shape[0] * pybuffer->shape[1] * pybuffer->itemsize);

         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("length, shape, strides, itemsize "
                  "values:  %d, %d, %d, %d\n",
                  pybuffer->len, pybuffer->shape[0], pybuffer->strides[0], 
                  pybuffer->itemsize);
         #endif

         int c = PyBuffer_IsContiguous(pybuffer, 'C');
         Py_buffer *view = (Py_buffer *)malloc(sizeof(Py_buffer));

         if (c == 1)
         {
            PyObject *pyobj = NULL;

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Calling PyMemoryView_"
                     "FromBuffer()\n");
            #endif

            pyobj = PyMemoryView_FromBuffer(pybuffer);

            //check the memoryview object and read back the buffer
            int l = 0;
            l = PyMemoryView_Check(pyobj);
            if (l == 1)
            {
               int ret = PyBuffer_FillInfo(view, pyobj, v, pybuffer->len, 0, 
                                           PyBUF_CONTIG);
               
               if (ret == -1)
               {
                  PyErrorMsg(pType, pValue, pTraceback, msg);
                  throw InterfaceException(" Python Exception: " + msg + "\n");
               }
            }

            PyTuple_SetItem(pyTupleObj, i, pyobj);
            i++;
         }

         // free memory
         /// @todo: Check to see if this leaks
   //      delete[] v;
   //      free(view);
   //      free(pybuffer->shape);
   //      free(pybuffer->strides);
   //      free(pybuffer);
      }
      else if (parType == Gmat::STRING_TYPE)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("A string is passed to Python.\n");
         #endif

         /// @todo: Both python versions need to be implemented.
         PyObject * pyStr = PyUnicode_FromString(((std::string *)argIn.at(n))->c_str());

         PyTuple_SetItem(pyTupleObj, i, pyStr);
         i++;
         n++;
      }
      else if (parType == Gmat::REAL_TYPE)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Reading floats %lf\n", *(Real*)argIn.at(n));
         #endif
         PyObject* pyFloatObj = PyFloat_FromDouble(*(Real*)argIn.at(n));
         PyTuple_SetItem(pyTupleObj, i, pyFloatObj);
            
         i++;
         n++;
      }
      else
         throw InterfaceException("The input parameter  is not a supported input type for GMAT's Python interface");
   }

   /// @todo: Check to see if this leaks
   // delete[] v;
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Executing the function\n", *(Real*)argIn.at(n));
   #endif

   // Call the python function   
   pyFunc = PyObject_CallObject(pyFuncAttr, pyTupleObj);

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Function executed\n", *(Real*)argIn.at(n));
   #endif
  
   Py_DECREF(pyFuncAttr);
   Py_DECREF(pyTupleObj);

   if (!pyFunc)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);
      throw InterfaceException("Python Exception: " + msg + "\n");
   }

   return pyFunc;
}


//------------------------------------------------------------------------------
// void PyErrorMsg(PyObject* pType, PyObject* pValue, PyObject* pTraceback, 
//       std::string &msg)
//------------------------------------------------------------------------------
/**
 * Fetches a Python error/exception message
 *
 * @param pType The Python type
 * @param pValue The Python value
 * @param pTraceback The Python traceback object
 * @param msg The message string that is being filled in with the exception 
 *            information
 */
//------------------------------------------------------------------------------
void PythonInterface::PyErrorMsg(PyObject* pType, PyObject* pValue, 
      PyObject* pTraceback, std::string &msg)
{
   int reset_error_state = 0;

   // will own the reference to each variables passed
   if (PyErr_Occurred())
   {
      reset_error_state = 1;
      PyErr_Fetch(&pType, &pValue, &pTraceback);
   }
   
   if (pType != NULL)
   {
      PyObject* v = PyObject_Str(pValue);
      PyObject* t = PyObject_Str(pType);

      if (v)
      {
#ifdef IS_PY3K
         msg = std::string(_PyUnicode_AsString(t));
         msg += ": " + std::string(_PyUnicode_AsString(v));
#else
         msg = std::string(PyString_AsString(t));
         msg += ": " + std::string(PyString_AsString(v));
#endif
      }
   }

   if (reset_error_state) 
   {
      PyErr_Restore(pType, pValue, pTraceback);
   }
}
