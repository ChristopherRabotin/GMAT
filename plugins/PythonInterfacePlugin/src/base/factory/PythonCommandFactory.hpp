//$Id$
//------------------------------------------------------------------------------
//                             PythonCommandFactory
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
// Created: 2015/02/23
//
/**
* Factory class that creates the Python commands
*/
//------------------------------------------------------------------------------

#include "pythoninterface_defs.hpp"
#include "Factory.hpp"

/**
 * The factory for Python commands
 */
class PYTHON_API PythonCommandFactory : public Factory
{
public:
   PythonCommandFactory();
   virtual ~PythonCommandFactory();
   PythonCommandFactory(const PythonCommandFactory& elf);
   PythonCommandFactory& operator=(const PythonCommandFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
};
