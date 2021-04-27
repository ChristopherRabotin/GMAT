//$Id$
//------------------------------------------------------------------------------
//                                 PythonInterface
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
   /// Path separator for the platform
   std::string                plF;

   PythonInterface(const std::string &name);
   ~PythonInterface();

   // Methods inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);


   void                 PyPathSep();
   void                 PyErrorMsg(PyObject* pType, PyObject* pValue, 
                                   PyObject* pTraceback, std::string &msg);
};

#endif
