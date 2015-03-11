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
#ifndef CALLPYTHONFUNCTION_HPP
#define CALLPYTHONFUNCTION_HPP

#include "pythoninterface_defs.hpp"
#include "CallFunction.hpp"

class  PYTHON_API CallPythonFunction : public CallFunction
{
public:

	CallPythonFunction();
	CallPythonFunction(const CallPythonFunction &cpf);
	CallPythonFunction& operator=(const CallPythonFunction &cpf);
	~CallPythonFunction();

	DEFAULT_TO_NO_CLONES


};




#endif
