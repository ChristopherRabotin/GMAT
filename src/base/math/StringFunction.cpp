//$Id$
//------------------------------------------------------------------------------
//                                   StringFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.02.17
//
/**
 * Implements the string functions class.
 */
//------------------------------------------------------------------------------

#include "StringFunction.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_VALIDATE_INPUT
//#define DEBUG_INPUT_OUTPUT

//------------------------------------------------------------------------------
//  StringFunction(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the StringFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
StringFunction::StringFunction(const std::string &typeStr, const std::string &name) :
   BuiltinFunction    (typeStr, name)
{
   objectTypeNames.push_back("StringFunction");
}

//------------------------------------------------------------------------------
//  ~StringFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the StringFunction object (destructor).
 */
//------------------------------------------------------------------------------
StringFunction::~StringFunction()
{
   // all math nodes are deleted in MathTree destructor
}

//------------------------------------------------------------------------------
//  StringFunction(const StringFunction &sf)
//------------------------------------------------------------------------------
/**
 * Constructs the StringFunction object (copy constructor).
 * 
 * @param <sf> Object that is copied
 */
//------------------------------------------------------------------------------
StringFunction::StringFunction(const StringFunction &sf) :
   BuiltinFunction   (sf)
{
}

//------------------------------------------------------------------------------
//  StringFunction& operator=(const StringFunction &sf)
//------------------------------------------------------------------------------
/**
 * Sets one StringFunction object to match another (assignment operator).
 * 
 * @param <sf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
StringFunction& StringFunction::operator=(const StringFunction &sf)
{
   if (this == &sf)
      return *this;
   
   GmatBase::operator=(sf);
   return *this;
}

//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method calls its subnodes and checks to be sure that the subnodes return
 * compatible data for the function.
 */
//------------------------------------------------------------------------------
bool StringFunction::ValidateInputs()
{
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("StringFunction::ValidateInputs() <%p><%s> entered, desc='%s'\n", this,
       GetTypeName().c_str(), desc.c_str());
   #endif
   
   bool retval = true;
   
   // Validate inputs
   std::string noBlankDesc = GmatStringUtil::RemoveAllBlanks(desc);
   if (noBlankDesc == "")
      throw MathException("There are no Input to " + GetTypeName() + "() in " +
                          desc);
   
   StringArray items = GmatStringUtil::DecomposeBy(desc, "(");
   if (items.size() == 0)
      throw MathException("Invalid calling syntax to " + GetTypeName() + "() in " +
                          desc);
   
   #ifdef DEBUG_VALIDATE_INPUT
   for (unsigned int i = 0; i < items.size(); i++)
      MessageInterface::ShowMessage("   items[%d] = '%s'\n", i, items[i].c_str());
   #endif
   
   // Check for closing parenthesis
   if (!GmatStringUtil::EndsWith(items[1], ")"))
      throw MathException("Invalid calling syntax to " + GetTypeName() + "() in " +
                          desc);
   
   #ifdef DEBUG_VALIDATE_INPUT
   MessageInterface::ShowMessage
      ("StringFunction::ValidateInputs() <%p><%s> returning %s\n",
       this, GetTypeName().c_str(), retval ? "true" : "false");
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
//------------------------------------------------------------------------------
void StringFunction::GetOutputInfo(Integer &type, Integer &rowCount,
                                   Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("StringFunction::GetOutputInfo() <%p><%s><%s> entered\n", this,
       GetTypeName().c_str(), GetName().c_str());
   #endif
   
   type = Gmat::STRING_TYPE;
   rowCount = 1;
   colCount = 1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("StringFunction::GetOutputInfo() <%p><%s> leaving, type=%d, "
       "rowCount=%d, colCount=%d\n", this, GetTypeName().c_str(), 
       type, rowCount, colCount);
   #endif
}

