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
// Author: Linda Jun (NASA/GSFC)
// Created: 2016.01.22
//
/**
 *  Class implementation for the Display command.
 */
//------------------------------------------------------------------------------


#include "Display.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"       // for GetArrayIndex()
#include <sstream>

//#define DEBUG_DISPLAY_IA
//#define DEBUG_DISPLAY_OBJ
//#define DEBUG_DISPLAY_SET
//#define DEBUG_DISPLAY_INIT
//#define DEBUG_DISPLAY_EXEC
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_OBJECT_MAP
//#define DEBUG_RENAME

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
const std::string
Display::PARAMETER_TEXT[DisplayParamCount - GmatCommandParamCount] =
{
   "Add"
};

const Gmat::ParameterType
Display::PARAMETER_TYPE[DisplayParamCount - GmatCommandParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,   // "Add",
};


//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
//  Display()
//------------------------------------------------------------------------------
/**
 * Constructs the Display Command (default constructor).
 */
//------------------------------------------------------------------------------
Display::Display() :
   GmatCommand  ("Display"),
   numElements  (0)
{
   // GmatBase data
   objectTypeNames.push_back("Display");
}


//------------------------------------------------------------------------------
//  ~Display()
//------------------------------------------------------------------------------
/**
 * Destroys the Display Command (destructor).
 */
//------------------------------------------------------------------------------
Display::~Display()
{
   DeleteElements();
}


//------------------------------------------------------------------------------
//  Display(const Display &dispCmd)
//------------------------------------------------------------------------------
/**
 * Constructs the Display Command based on another instance (copy constructor).
 * 
 * @param dispCmd The Display that is copied.
 */
//------------------------------------------------------------------------------
Display::Display(const Display &dispCmd) :
   GmatCommand    (dispCmd),
   numElements      (0)
{
   elementNames = dispCmd.elementNames;
   actualElementNames = dispCmd.actualElementNames;
   elementWrappers.clear();
}


//------------------------------------------------------------------------------
//  Display& operator=(const Display &dispCmd)
//------------------------------------------------------------------------------
/**
 * Sets this Display Command to match another instance (Assignment operator).
 * 
 * @param dispCmd The Display that is copied.
 * 
 * @return This instance, configured to match the other and ready for 
 *         initialization.
 */
//------------------------------------------------------------------------------
Display& Display::operator=(const Display &dispCmd)
{
   if (this != &dispCmd)
   {
      elementNames = dispCmd.elementNames;
      actualElementNames = dispCmd.actualElementNames;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
//  void InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Display command has the following syntax:
 *
 *     Display elementName
 */
//------------------------------------------------------------------------------
bool Display::InterpretAction()
{
   #ifdef DEBUG_DISPLAY_IA
   MessageInterface::ShowMessage
      ("Display::InterpretAction(), generatingString = %s\n",
       generatingString.c_str());
   #endif
   
   StringArray chunks = InterpretPreface();
   
   #ifdef DEBUG_DISPLAY_IA
   for (UnsignedInt i=0; i<chunks.size(); i++)
      MessageInterface::ShowMessage("   %d: '%s'\n", i, chunks[i].c_str());
   #endif
   
   if (chunks.size() < 2)
      throw CommandException("Missing information for MissionSave command.\n");

   StringArray items = GmatStringUtil::SeparateBy(chunks[1], ", ");
   
   for (unsigned int i = 0; i < items.size(); i++)
   {
      AddElements(items[i], i);
      
      #ifdef DEBUG_DISPLAY_IA
      MessageInterface::ShowMessage
         ("   elementNames[%d] = '%s'\n", i, elementNames[i].c_str());
      #endif
   }
   
   #ifdef DEBUG_DISPLAY_IA
   MessageInterface::ShowMessage("Display::InterpretAction() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Display::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < DisplayParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Display::GetStringParameter(const Integer id) const
{
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Display::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Display::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case ADD:
      #ifdef DEBUG_DISPLAY_SET
      MessageInterface::ShowMessage
         ("Display::SetStringParameter() Adding parameter '%s'\n", value.c_str());
      #endif
      return AddElements(value, numElements);
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool Display::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Display::SetStringParameter(const Integer id, const std::string &value,
                                 const Integer index)
{
   switch (id)
   {
   case ADD:
      return AddElements(value, index);
   default:
      return GmatCommand::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Display::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Display::GetStringArrayParameter(const Integer id) const
{
   #ifdef DEBUG_DISPLAYFILE_GET
   MessageInterface::ShowMessage
      ("Display::GetStringArrayParameter() id=%d, actualElementNames.size()=%d, "
       "numElements=%d\n", id, actualElementNames.size(), numElements);
   #endif
   
   switch (id)
   {
   case ADD:
      return actualElementNames;
   default:
      return GmatCommand::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& Display::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& Display::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();
   wrapperObjectNames.insert(wrapperObjectNames.end(), actualElementNames.begin(),
                             actualElementNames.end());
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Display::SetElementWrapper(ElementWrapper *toWrapper,
                                const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Display::SetElementWrapper() this=<%p> '%s' entered, toWrapper=<%p>, "
       "withName='%s'\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       toWrapper, withName.c_str());
   #endif
   
   if (toWrapper == NULL)
      return false;
   
   // Do we need any type checking?
   // CheckDataType(toWrapper, Gmat::REAL_TYPE, "Display", true);
   
   bool retval = false;
   ElementWrapper *ew;
   std::vector<ElementWrapper*> wrappersToDelete;
   
   //-------------------------------------------------------
   // check element names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   Checking %d elements\n", actualElementNames.size());
   for (UnsignedInt i=0; i<actualElementNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", actualElementNames[i].c_str());
   #endif
   
   Integer sz = actualElementNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (actualElementNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in actualElementNames\n   "
             "elementWrappers.at(%d)=<%p>", withName.c_str(), i, elementWrappers.at(i));
         #endif
         if (elementWrappers.at(i) != NULL)
         {
            ew = elementWrappers.at(i);
            elementWrappers.at(i) = toWrapper;
            // if wrapper not found, add to the list to delete
            if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ew) ==
                wrappersToDelete.end())
               wrappersToDelete.push_back(ew);
         }
         else
         {
            elementWrappers.at(i) = toWrapper;
         }
         
         retval = true;
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
   
   // Delete old ElementWrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Display::SetElementWrapper()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Display::SetElementWrapper() exiting with %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Display::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Display::ClearWrappers() this=<%p> '%s' entered\n   There are %d wrappers "
       "allocated, these will be deleted if not NULL\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), elementWrappers.size());
   #endif
   
   std::vector<ElementWrapper*> wrappersToDelete;
   
   // delete wrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = elementWrappers.begin();
        ewi < elementWrappers.end(); ewi++)
   {
      if ((*ewi) == NULL)
         continue;
      
      // if wrapper not found, add to the list to delete
      if (find(wrappersToDelete.begin(), wrappersToDelete.end(), (*ewi)) ==
          wrappersToDelete.end())
         wrappersToDelete.push_back((*ewi));
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool Display::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Display::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      elementNames.clear();
      actualElementNames.clear();

      // I think we also need to clear wrappers here (loj: 2008.11.24)
      ClearWrappers();
      elementWrappers.clear();      
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool GetRefObjectName(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the reference object names.
 * 
 * @param type The type of the reference object.
 * 
 * @return the name of the object.
 */
//------------------------------------------------------------------------------
std::string Display::GetRefObjectName(const Gmat::ObjectType type) const
{
   return GmatCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& Display::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   static StringArray refObjectNames;
   refObjectNames.clear();
   
   return GmatCommand::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the DisplayFile and Element objects used bt the Display command.
 * 
 * This method received the global instances of the objects used in the Display
 * command.  It checks their types and stores their names, so that the the 
 * objects can be retrieved from the local store in the Sandbox during 
 * initiialization.  It also tells the DisplayFile instance that it will need
 * to be ready to receive data from a DisplayCommand, so that the DisplayFile does
 * not erroneously inform the user that no data will be written to the 
 * DisplayFile.
 * 
 * @param <obj> Pointer to the reference object.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return true if object successfully set, throws otherwise.
 */
//------------------------------------------------------------------------------
bool Display::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name, const Integer index)
{
   if (obj == NULL)
   {
      #ifdef DEBUG_DISPLAY_OBJ
      MessageInterface::ShowMessage
         ("\nDisplay::SetRefObject() this=<%p> returning false since input object is NULL\n");
      #endif
      return false;
   }
   
   #ifdef DEBUG_DISPLAY_OBJ
   MessageInterface::ShowMessage
      ("\nDisplay::SetRefObject() this=<%p> entered, obj=<%p><%s>'%s'\n   type=%d<%s>, "
       "name='%s', index=%d\n", this, obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       type, GetObjectTypeString(type).c_str(), name.c_str(), index);
   #endif
   
   
   #ifdef DEBUG_DISPLAY_OBJ
   MessageInterface::ShowMessage("Display::SetRefObject() returning true\n");
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//                      const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool Display::RenameRefObject(const Gmat::ObjectType type,
                              const std::string &oldName,
                              const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Display::RenameRefObject() entered, type=%d<%s>, oldName='%s', newName='%s'\n",
       type, GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   // Go through Parameter names
   for (UnsignedInt i=0; i<elementNames.size(); i++)
   {
      if (elementNames[i].find(oldName) != oldName.npos)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   => Before rename, name: '%s'\n", elementNames[i].c_str());
         #endif
         
         elementNames[i] = GmatStringUtil::ReplaceName(elementNames[i], oldName, newName);
         
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("      After  rename, name: '%s'\n", elementNames[i].c_str());
         #endif
      }
   }
   
   // Go through actual Parameter names including array index
   for (UnsignedInt i=0; i<actualElementNames.size(); i++)
   {
      if (actualElementNames[i].find(oldName) != oldName.npos)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   => Before rename, actual name: '%s'\n", actualElementNames[i].c_str());
         #endif
         
         actualElementNames[i] = GmatStringUtil::ReplaceName(actualElementNames[i], oldName, newName);
         
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("      After  rename, actual name: '%s'\n", actualElementNames[i].c_str());
         #endif
      }
   }
   
   // Go through wrappers
   for (WrapperArray::iterator i = elementWrappers.begin(); i < elementWrappers.end(); i++)
   {
      std::string desc = (*i)->GetDescription();
      if (desc.find(oldName) != oldName.npos)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   => Before rename, wrapper desc = '%s'\n", desc.c_str());
         #endif
         
         (*i)->RenameObject(oldName, newName);
         desc = (*i)->GetDescription();
         
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("      After  rename, wrapper desc = '%s'\n", desc.c_str());
         #endif
      }
   }
   
   // Go through generating string
   generatingString = GmatStringUtil::ReplaceName(generatingString, oldName, newName);
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("Display::RenameRefObject() leaving\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of this Display.
 *
 * @return clone of the Display.
 */
//------------------------------------------------------------------------------
GmatBase* Display::Clone() const
{
   return (new Display(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& Display::GetGeneratingString(Gmat::WriteMode mode,
                                                const std::string &prefix,
                                                const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Display::GetGeneratingString() entered, prefix='%s', useName='%s'\n",
       prefix.c_str(), useName.c_str());
   #endif
   
   std::string gen = prefix + "Display ";
   UnsignedInt numElem = actualElementNames.size();
   
   if (numElem > 1)
   {
      for (UnsignedInt i=0; i<numElem-1; i++)
         gen += actualElementNames[i] + " ";
      
      gen += actualElementNames[numElem-1];
   }
   else if (numElem == 1)
   {
      gen += actualElementNames[0];
   }
   
   generatingString = gen + ";";
   
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("   generatingString=<%s>, \n   now returning GmatCommand::GetGeneratingString()\n",
       generatingString.c_str());
   #endif
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Display command.
 *
 * @return true if the Display is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Display::Initialize()
{
   if (GmatCommand::Initialize() == false)
      return false;

   #ifdef DEBUG_DISPLAY_INIT
   MessageInterface::ShowMessage
      ("\nDisplay::Initialize() this=<%p> entered, has %d parameter names\n",
       this, elementNames.size());
   #endif
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   // parms.clear();
   GmatBase *mapObj = NULL;
   
   for (StringArray::iterator i = elementNames.begin(); i != elementNames.end(); ++i)
   {
      std::string itemName = *i;
      
      #ifdef DEBUG_DISPLAY_INIT
      MessageInterface::ShowMessage("   Now find object for '%s'\n", itemName.c_str());
      #endif
      
      mapObj = FindObject(itemName);
      if (mapObj == NULL)
      {
         // Check if it is an object property
         std::string type, owner, dep;
         GmatStringUtil::ParseParameter(itemName, type, owner, dep);
         #ifdef DEBUG_DISPLAY_INIT
         MessageInterface::ShowMessage
            ("   owner='%s', type='%s', dep='%s'\n", owner.c_str(), type.c_str(), dep.c_str());
         #endif
         if (owner != "" && type != "")
            mapObj = FindObject(owner);
         
         if (mapObj == NULL)
         {
            std::string msg = "Object named \"" + (*i) +
               "\" cannot be found for the Display command '" +
               GetGeneratingString(Gmat::NO_COMMENTS) + "'";
            #ifdef DEBUG_DISPLAY_INIT
            MessageInterface::ShowMessage("**** ERROR **** %s\n", msg.c_str());
            #endif
            throw CommandException(msg);
         }
      }
   }
   
   // Set Wrapper references
   for (WrapperArray::iterator i = elementWrappers.begin(); i < elementWrappers.end(); i++)
   {
      #ifdef DEBUG_DISPLAY_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*i)->GetDescription().c_str());
      #endif
      
      if (SetWrapperReferences(*(*i)) == false)
         return false;      
   }
   
   #ifdef DEBUG_DISPLAY_INIT
   MessageInterface::ShowMessage("Display::Initialize() this=<%p> returning true\n", this);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Write the display data to a DisplayFile.
 *
 * @return true if the Command runs to completion, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Display::Execute()
{
   #ifdef DEBUG_DISPLAY_EXEC
   MessageInterface::ShowMessage
      ("\nDisplay::Execute() this=<%p> <%s> entered\n",
       this, GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif

   bool retval = true;
      
   // Write to Message Window using elementWrappers
   for (WrapperArray::iterator i = elementWrappers.begin(); i < elementWrappers.end(); i++)
   {
      #ifdef DEBUG_DISPLAY_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*i)->GetDescription().c_str());
      #endif
      
      ElementWrapper *wrapper = *i;
      if (wrapper)
      {
         MessageInterface::ShowMessage
            ("%s =\n%s\n\n", wrapper->GetDescription().c_str(), wrapper->ToString().c_str());
      }
   }
   
   BuildCommandSummary(true);
   
   #ifdef DEBUG_DISPLAY_EXEC
   MessageInterface::ShowMessage
      ("Display::Execute() this=<%p> returning %d\n", this, retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
void Display::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("Display::RunComplete() this=<%p> '%s' entered\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// bool AddElements(const std::string &elementName, Integer index)
//------------------------------------------------------------------------------
/**
 * Adds elements to the display list.
 */
//------------------------------------------------------------------------------
bool Display::AddElements(const std::string &elementName, Integer index)
{
   #ifdef DEBUG_DISPLAY_SET
   MessageInterface::ShowMessage
      ("Display::AddElements() this=<%p>, Adding element '%s', index=%d, "
       "numElements=%d\n", this, elementName.c_str(), index, numElements);
   #endif
   
   if (elementName == "")
   {
      #ifdef DEBUG_DISPLAY_SET
      MessageInterface::ShowMessage
         ("Display::AddElements() returning false, input elementName is blank\n");
      #endif
      return false;
   }
   
   if (index < 0)
   {
      #ifdef DEBUG_DISPLAY_SET
      MessageInterface::ShowMessage
         ("Display::AddElements() returning false, the index %d is less than 0\n");
      #endif
      return false;
   }
   
   if (index > numElements)
   {
      #ifdef DEBUG_DISPLAY_SET
      MessageInterface::ShowMessage
         ("Display::AddElements() returning false, the index %d is out of bounds, "
          "it must be between 0 and %d\n", index, numElements);
      #endif
      return false;
   }
   
   // Handle Array indexing first
   Integer row, col;
   std::string newName;      
   GmatStringUtil::GetArrayIndex(elementName, row, col, newName);
   
   elementNames.push_back(newName);
   actualElementNames.push_back(elementName);
   elementWrappers.push_back(NULL);
   numElements = actualElementNames.size();
   
   #ifdef DEBUG_DISPLAY_SET
   MessageInterface::ShowMessage
      ("   Added '%s', size=%d\n", elementName.c_str(), numElements);
   MessageInterface::ShowMessage
      ("Display::AddElements() this=<%p> returning true\n", this);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void DeleteElements()
//------------------------------------------------------------------------------
/**
 * Delete elements from the display list and wrappers.
 */
//------------------------------------------------------------------------------
void Display::DeleteElements()
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Display::DeleteElements() this=<%p> '%s' entered\n   There are %d wrappers "
       "allocated, these will be deleted if not NULL\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), elementWrappers.size());
   #endif
   
   std::vector<ElementWrapper*> wrappersToDelete;
   
   // Add wrappers to the list to delete
   for (std::vector<ElementWrapper*>::iterator ewi = elementWrappers.begin();
        ewi < elementWrappers.end(); ewi++)
   {
      if ((*ewi) == NULL)
         continue;
      
      // If wrapper not found, add to the list to delete
      if (find(wrappersToDelete.begin(), wrappersToDelete.end(), (*ewi)) ==
          wrappersToDelete.end())
         wrappersToDelete.push_back((*ewi));
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
   
   // Delete wrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Display::DeleteElements()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   elementWrappers.clear();   
   actualElementNames.clear();
}

