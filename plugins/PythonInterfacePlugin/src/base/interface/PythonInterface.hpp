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
#ifndef PYTHONINTERFACE_HPP
#define PYTHONINTERFACE_HPP

#include <Python.h>
#include "pythoninterface_defs.hpp"
#include "Interface.hpp"
#include "CommandException.hpp"

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

class PYTHON_API PythonInterface : public Interface
{
public:
   /// The singleton instance
   static PythonInterface* PyInstance();

   /// Load Python engine with all built-in modules
   bool PyInitialize();
   /// Unload Python modules
   bool PyFinalize();
   /// Add Module path to sys.path
   void PyAddModulePath(const StringArray& path);
   /// Python function wrapper
   PyObject* PyFunctionWrapper(const std::string &modName, const std::string &funcName,
                                 const std::vector<void *> &argIn, std::vector<Gmat::ParameterType> paramType, UnsignedInt row, UnsignedInt col, UnsignedInt argSz);
	
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

private:
   static PythonInterface *instance;
	
   PythonInterface(const std::string &name);
   ~PythonInterface();
   PythonInterface(const PythonInterface &pi);
   PythonInterface& operator=(const PythonInterface &pi);

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);

   /// Is Python engine loaded
   bool isPythonInitialized;
   /// Number of Python command referenced in Gmat script
   Integer numPyCommands;
   /// Identify the platform we are running
   char *plF;
   void PyPathSep();
   /// Python error/exception message
   void PyErrorMsg(PyObject* pType, PyObject* pValue, PyObject* pTraceback, std::string &msg);
};

#endif
