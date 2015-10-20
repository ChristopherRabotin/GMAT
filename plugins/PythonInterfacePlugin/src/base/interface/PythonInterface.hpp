//$Id$
//------------------------------------------------------------------------------
//                                 PythonInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
   static PythonInterface* PyInstance();
   bool                    PyInitialize();
   bool                    PyFinalize();
   void                    PyAddModulePath(const StringArray& path);
   PyObject*               PyFunctionWrapper(const std::string &modName, 
                                             const std::string &funcName,
                                             const std::vector<void *> &argIn, 
                                             std::vector<Gmat::ParameterType> paramType, 
                                             UnsignedInt row, UnsignedInt col, 
                                             UnsignedInt argSz);
	
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

private:
   /// The interface singleton
   static PythonInterface     *instance;
   /// Flag indicating if the Python engine is loaded
   bool                       isPythonInitialized;
   /// Number of Python commands referenced in the GMAT script
   Integer                    numPyCommands;
   /// Identity of the the platform being run
   char                       *plF;

   PythonInterface(const std::string &name);
   ~PythonInterface();
   PythonInterface(const PythonInterface &pi);
   PythonInterface&     operator=(const PythonInterface &pi);

   // Methods inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);


   void                 PyPathSep();
   void                 PyErrorMsg(PyObject* pType, PyObject* pValue, 
                                   PyObject* pTraceback, std::string &msg);
};

#endif
