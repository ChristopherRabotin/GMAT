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

PythonInterface* PythonInterface::instance = NULL;

PythonInterface* PythonInterface::PyInstance()
{
   if (instance == NULL)
	   instance = new PythonInterface("PythonInterface");

   return instance;
}

PythonInterface::PythonInterface(const std::string &name) : Interface("PythonInterface", name)
{
   isPythonInitialized = false;
   numPyCommands = 0;
   plF = new char[2];
   memset(plF, 0, 2);
}

PythonInterface::~PythonInterface()
{
   delete[] plF;
}

PythonInterface::PythonInterface(const PythonInterface& pi) : Interface(pi)
{
   plF = new char[2];
   memcpy(plF, pi.plF, 2);
}


//------------------------------------------------------------------------------
// PythonInterface& operator=(const PythonInterface& pi)
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

void PythonInterface::Copy(const GmatBase* orig)
{
   operator=(*((PythonInterface *)(orig)));
}

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
* @param none
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
         MessageInterface::ShowMessage("Python is initialized/Loaded.\n");

	   isPythonInitialized = true;
      
	   numPyCommands++;
   }
   else
   {
	   throw CommandException ("  Python is not installed or loaded properly.");
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
* @param none
*
* @return bool
*/
//------------------------------------------------------------------------------
bool PythonInterface::PyFinalize()
{
   // when all the Python commands in Gmat script is run and completed
   // close and finalize the Python.
   if (--numPyCommands == 0)
   {
	   Py_Finalize();
      MessageInterface::ShowMessage("Python is Finalized/Unloaded.\n");

	   isPythonInitialized = false;
   }
	
   MessageInterface::ShowMessage("numPyCommands is %d: \n", numPyCommands);

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
* @param none
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
*
* @return void
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

   MessageInterface::ShowMessage("  Leaving PyAddModulePath( ) \n");

}

//------------------------------------------------------------------------------
// PyObject* PyFunctionWrapper()
//------------------------------------------------------------------------------
/**
* Call Python Function
*
* @param modName, funcName, formatIn, argIn, paramType
*
* @return PyObject
*/
//------------------------------------------------------------------------------
PyObject* PythonInterface::PyFunctionWrapper(const std::string &modName, const std::string &funcName,
                                             const std::string &formatIn, const std::vector<void *> &argIn,                                              Gmat::ParameterType paramType, Integer row, Integer col)
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

#ifdef IS_PY3K
   // create a python Unicode object from an UTF-8 encoded null terminated char buffer
   pyModule = PyUnicode_FromString(modName.c_str() );
#else
   pyModule = PyBytes_FromString(modName.c_str() );
#endif
  
   if (!pyModule)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);

      throw InterfaceException(" Python Exception Type: " + msg + "\n");
   }
 
   // import the python module
   pyPluginModule = PyImport_Import(pyModule);   
   Py_DECREF(pyModule);

   if (!pyPluginModule)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);

      throw InterfaceException(" Python Exception Type: " + msg + "\n");
   }

   // retrieve an attribute named 'funcName' from imported python module
   pyFuncAttr = PyObject_GetAttrString(pyPluginModule, funcName.c_str() );
   Py_DECREF(pyPluginModule);

   if (!pyFuncAttr)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);

      throw InterfaceException(" Python Exception Type:" + msg + "\n");
   }
   
   // The Array requirement which is listed below needs to be rephrased  in PythonInterfaceNotes_2015-01-14.txt
   // Array data is passed into a list of floats for one dimensional arrays
   if (paramType == Gmat::RMATRIX_TYPE)
   {
      Py_buffer *pybuffer = (Py_buffer *)malloc(sizeof(Py_buffer));
      Real *v = new Real[argIn.size()];
      std::vector<void *>::const_iterator it = argIn.begin();
      Integer i = 0;
      for (it, i; it != argIn.end(); ++it,++i)
         v[i] = *(Real*)*it;

      
      pybuffer->obj = NULL;
      pybuffer->format = "d";
      pybuffer->ndim = (row != 1 && col != 1) ? 2 : 1;
      pybuffer->shape = (Py_ssize_t *)malloc(sizeof(Py_ssize_t)* pybuffer->ndim);
      pybuffer->readonly = 0;
      pybuffer->suboffsets = 0;
      pybuffer->buf = v;
    
      pybuffer->strides = (Py_ssize_t *)malloc(sizeof(Py_ssize_t)* pybuffer->ndim);
      for (UnsignedInt m = 0; m < row; ++m)
         pybuffer->shape[m] = col;

      PyBuffer_FillContiguousStrides(pybuffer->ndim, pybuffer->shape, pybuffer->strides, sizeof(Real), 'C');
   
      pybuffer->itemsize = sizeof(Real);
      pybuffer->len = (pybuffer->ndim == 1 ?                        pybuffer->shape[0] * pybuffer->itemsize :                        pybuffer->shape[0] * pybuffer->shape[1] * pybuffer->itemsize);

      MessageInterface::ShowMessage("length, shape, strides, itemsize values:  %d, %d, %d, %d\n",             pybuffer->len, pybuffer->shape[0], pybuffer->strides[0], pybuffer->itemsize);
      
      int c =  PyBuffer_IsContiguous(pybuffer, 'C');
      Py_buffer *view = (Py_buffer *)malloc(sizeof(Py_buffer));

      if (c == 1)
      {
         PyObject *pyobj = NULL;
         MessageInterface::ShowMessage("calling PyMemoryView_FromBuffer() \n");
         pyobj = PyMemoryView_FromBuffer(pybuffer);
         
         //check the memory view object and read back the buffer we created earlier
         int l = 0;
         l = PyMemoryView_Check(pyobj);
         if (l == 1)
         {
            
            int ret = PyBuffer_FillInfo(view, pyobj, v, pybuffer->len, 0, PyBUF_CONTIG);
            if (ret != -1)
            {
               MessageInterface::ShowMessage("Fifth value is %lf\n", ((Real*)(view->buf))[4]);
            }
         }

         // Create a Tuple to pass it to Python function
         pyTupleObj = PyTuple_New(1);
         PyTuple_SetItem(pyTupleObj, 0, pyobj);

         pyFunc = PyObject_CallObject(pyFuncAttr, pyTupleObj);
         MessageInterface::ShowMessage("pyObject_CallObject() is called\n");
      }

      // free memory
      delete[] v;
      free(view);
      free(pybuffer->shape);
      free(pybuffer->strides);
      free(pybuffer);
   }
   else if (paramType == Gmat::STRING_TYPE)
   {
      MessageInterface::ShowMessage("A string is passed to Python.\n");
      //attention for both python version needs to be implemented.
      PyObject * pyStr = PyUnicode_FromString(((std::string *)argIn.at(0))->c_str());
      // Create a Tuple to pass it to Python function
      pyTupleObj = PyTuple_New(1);
      PyTuple_SetItem(pyTupleObj, 0, pyStr);

      pyFunc = PyObject_CallObject(pyFuncAttr, pyTupleObj);
      MessageInterface::ShowMessage("pyObject_CallObject() is called\n");
   }
   else
   {
      // Build the Python Tuple object based on the format string
      pyTupleObj = PyTuple_New(argIn.size());
      char* pch = NULL;
      char key[3] = "ds";
      int i = 0;
      //Locate characters in string
      pch = (char*)strpbrk(formatIn.c_str(), key);

      //Fill in the Python Tuple object with parameter Ins
      while (pch != NULL)
      {
         std::string str(pch);
         if (str.find_first_of("d") != std::string::npos)
         {
            PyObject* pyFloatObj = PyFloat_FromDouble(*(Real*)argIn.at(i));
            PyTuple_SetItem(pyTupleObj, i, pyFloatObj);
         }
         else if (str.find_first_of("s") != std::string::npos)
         {
#ifdef IS_PY3K
            PyObject* pyStringObj = PyUnicode_FromString((char*)argIn.at(i));
#else
            pyObject* pyStringObj = PyBytes_FromString((char*)argIn.at(i));
#endif
            PyTuple_SetItem(pyTupleObj, i, pyStringObj);
         }

         i++;
         pch = strpbrk(pch + 1, key);
      }

      if (!pyTupleObj)
      {
         PyErrorMsg(pType, pValue, pTraceback, msg);
         Py_DECREF(pyFuncAttr);

         throw InterfaceException(" Python Exception Type:" + msg + "\n");
      }

      // Call the python function   
      pyFunc = PyObject_CallObject(pyFuncAttr, pyTupleObj);
      
   }

   Py_DECREF(pyFuncAttr);
   Py_DECREF(pyTupleObj);

   if (!pyFunc)
   {
      PyErrorMsg(pType, pValue, pTraceback, msg);

      throw InterfaceException(" Python Exception Type:" + msg + "\n");
   }

   return pyFunc;
}

//------------------------------------------------------------------------------
// bool PyErrorMsg
//------------------------------------------------------------------------------
/**
* Fetch Internal Python error/exception messages
*
* @param pType, pValue, pTraceback of type PyObject, msg as string
*
* @return void
*/
//------------------------------------------------------------------------------
void PythonInterface::PyErrorMsg(PyObject* pType, PyObject* pValue, PyObject* pTraceback, std::string &msg)
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