//$Id$
//------------------------------------------------------------------------------
//                            Display
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
// number S-67573-G
//
// Author: Linda Jun, NASA/GSFC
// Created: 2016.01.22
//
/**
 *  Class definition for the Display command.
 */
//------------------------------------------------------------------------------
 
#ifndef Display_hpp
#define Display_hpp

#include "GmatCommand.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"

/**
 * The Display command is used to write data to a GUI Message Window.
 */
class GMAT_API Display : public GmatCommand
{
public:
   Display();
   virtual ~Display();
   Display(const Display &dispCmd);
   Display& operator=(const Display &dispCmd);
   
   // Inherited from GmatCommand
   virtual bool         InterpretAction();
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &withName);
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   virtual void         ClearWrappers();
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   bool                 Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // Inherited from GmatBase
   virtual GmatBase*    Clone() const;
   
   // Parameter accessor methods
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
     
   // Object accessor methods
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   
   // Generating string method
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   DEFAULT_TO_NO_CLONES

protected:
   /// Array of parameter names
   StringArray                elementNames;
   /// Array of actual parameter names including index
   StringArray                actualElementNames;
   /// Number of parameters
   Integer                    numElements;
   /// ElementWraper pointers of parameters
   std::vector<ElementWrapper*> elementWrappers;
   
   bool AddElements(const std::string &paramName, Integer index);
   void DeleteElements();
   
   enum
   {
      ADD = GmatCommandParamCount,
      DisplayParamCount  /// Count of the parameters for this class
   };
   
private:

   static const std::string
      PARAMETER_TEXT[DisplayParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[DisplayParamCount - GmatCommandParamCount];
};

#endif      // Display_hpp
