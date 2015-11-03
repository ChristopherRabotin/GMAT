//$Id$
//------------------------------------------------------------------------------
//                                  ISolverListener
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// number NNG04CC06P
//
// Author: Thomas G. Grubb/GSFC
// Created: 2015.06.19
//
/**
 * Definition of the ISolverListener class.  This is the virtual class/interface
 * for a class that listens for changed values
 *
 */
//------------------------------------------------------------------------------

#ifndef ISolverListener_hpp
#define ISolverListener_hpp

#include "IChangeListener.hpp"
#include "gmatdefs.hpp"
#include "GmatBase.hpp"

// forward references

class GMAT_API ISolverListener: public IChangeListener
{
public:
        virtual ~ISolverListener() {}
        virtual void ConstraintChanged(std::string name, Real desiredValue, 
           Real value, Integer condition = 0) = 0;
        virtual void Convergence(bool value, std::string info = "") = 0;
        virtual void ObjectiveChanged(std::string name, Real value) = 0;
        virtual void VariabledChanged(std::string name, Real value) = 0;
        virtual void VariabledChanged(std::string name, std::string &value) = 0;
};

#endif // ISolverListener_hpp