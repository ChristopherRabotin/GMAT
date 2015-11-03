//$Id$
//------------------------------------------------------------------------------
//                            EndFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/08/30
//
/**
 * Implementation code for the EndFunction command, a wrapper that marks the end
 * of the commands in a GMAT function.
 */
//------------------------------------------------------------------------------
 

#include "EndFunction.hpp"


EndFunction::EndFunction() :
   GmatCommand    ("EndFunction"),
   functionName         ("")
{
}


EndFunction::~EndFunction()
{
}


EndFunction::EndFunction(const EndFunction& ef) :
   GmatCommand          (ef),
   functionName         (ef.functionName)
{
}


EndFunction& EndFunction::operator=(const EndFunction& ef)
{
   if (this != &ef)
   {
      functionName = ef.functionName;
   }
   
   return *this;
}
 
GmatBase* EndFunction::GetRefObject(const Gmat::ObjectType type,
                                    const std::string &name)
{
   if (type == Gmat::UNKNOWN_OBJECT)  // Just find it by name
   {
      /// @todo Look up return object based on the name used in function call 
   }
   
   return GmatCommand::GetRefObject(type, name);
}

   
bool EndFunction::RenameRefObject(const Gmat::ObjectType type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   return true;
}


GmatBase* EndFunction::Clone() const
{
   return (new EndFunction(*this));
}


bool EndFunction::Execute()
{
   return true;
}
