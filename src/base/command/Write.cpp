//$Id$
//------------------------------------------------------------------------------
//                            Write
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
 *  Class implementation for the Write command.
 */
//------------------------------------------------------------------------------


#include "Write.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"       // for GetArrayIndex()
#include <sstream>

//#define DEBUG_Write_IA
//#define DEBUG_Write_OBJ
//#define DEBUG_Write_SET
//#define DEBUG_Write_INIT
//#define DEBUG_Write_EXEC
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
Write::PARAMETER_TEXT[WriteParamCount - GmatCommandParamCount] =
{
   "Add"
};

const Gmat::ParameterType
Write::PARAMETER_TYPE[WriteParamCount - GmatCommandParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,   // "Add",
};


//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
//  Write()
//------------------------------------------------------------------------------
/**
 * Constructs the Write Command (default constructor).
 */
//------------------------------------------------------------------------------
Write::Write() :
   GmatCommand       ("Write"),
   numElements       (0),
   reportFile        (""),
   reporter          (NULL),
   messageWindowOn   (true),
   outputStyle       (CONCISE),
   logFileOn         (false)
{
   // GmatBase data
   objectTypeNames.push_back("Write");
}


//------------------------------------------------------------------------------
//  ~Write()
//------------------------------------------------------------------------------
/**
 * Destroys the Write Command (destructor).
 */
//------------------------------------------------------------------------------
Write::~Write()
{
   DeleteElements();
}


//------------------------------------------------------------------------------
//  Write(const Write &dispCmd)
//------------------------------------------------------------------------------
/**
 * Constructs the Write Command based on another instance (copy constructor).
 * 
 * @param dispCmd The Write that is copied.
 */
//------------------------------------------------------------------------------
Write::Write(const Write &cmd) :
   GmatCommand       (cmd),
   numElements       (0),
   reportFile        (cmd.reportFile),
   reporter          (NULL),
   messageWindowOn   (cmd.messageWindowOn),
   outputStyle       (cmd.outputStyle),
   logFileOn         (cmd.logFileOn)
{
   elementNames = cmd.elementNames;
   actualElementNames = cmd.actualElementNames;
   elementWrappers.clear();
}


//------------------------------------------------------------------------------
//  Write& operator=(const Write &dispCmd)
//------------------------------------------------------------------------------
/**
 * Sets this Write Command to match another instance (Assignment operator).
 * 
 * @param dispCmd The Write that is copied.
 * 
 * @return This instance, configured to match the other and ready for 
 *         initialization.
 */
//------------------------------------------------------------------------------
Write& Write::operator=(const Write &cmd)
{
   if (this != &cmd)
   {
      elementNames = cmd.elementNames;
      actualElementNames = cmd.actualElementNames;
      outputStyle = cmd.outputStyle;
      reportFile = cmd.reportFile;
      reporter = NULL;
      messageWindowOn = cmd.messageWindowOn;
      logFileOn = cmd.logFileOn;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
//  void InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Write command has the following syntax:
 *
 *     Write elementName
 */
//------------------------------------------------------------------------------
bool Write::InterpretAction()
{
   #ifdef DEBUG_Write_IA
   MessageInterface::ShowMessage
      ("Write::InterpretAction(), generatingString = %s\n",
       generatingString.c_str());
   #endif

   StringArray blocks = parser.DecomposeBlock(generatingString);

   StringArray chunks = parser.SeparateBrackets(blocks[0], "{}", " ", false);

#ifdef DEBUG_Write_IA
   MessageInterface::ShowMessage("Chunks from \"%s\":\n",
      blocks[0].c_str());
   for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
      MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
#endif

   if (chunks[0] != typeName)
      throw CommandException(typeName + "::InterpretAction() does not identify "
      "the command in line\n" + generatingString);

   // delete old elements before adding the new elements
   DeleteElements();

   bool optionsParsed = false;
   for (unsigned int i = 1; i < chunks.size(); i++)
   {
      if (GmatStringUtil::IsEnclosedWithBraces(chunks[i]))
      {
         optionsParsed = true;
         CheckForOptions(chunks[i]);
      }
      else if (optionsParsed)
      {
         DeleteElements();
         throw CommandException(typeName + "::InterpretAction() requires all resources and variables "
            "are specified before options are specified\n" + generatingString);
      }
      else
      {
         AddElements(chunks[i], i - 1);

#ifdef DEBUG_Write_IA
         MessageInterface::ShowMessage
            ("   elementNames[%d] = '%s'\n", i, elementNames[i].c_str());
#endif
      }
   }

   return true;

}


//------------------------------------------------------------------------------
// void CheckForOptions(std::string &opts)
//------------------------------------------------------------------------------
void Write::CheckForOptions(std::string &opts)
{
   StringArray chunks = parser.SeparateBrackets(opts, "{}", ", ", true);

#ifdef DEBUG_Write_IA
   MessageInterface::ShowMessage("Chunks from \"%s\":\n", opts.c_str());
   for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
      MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
#endif

   for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
   {
      StringArray option = parser.SeparateBy(*i, "= ");

#ifdef DEBUG_Write_IA
      MessageInterface::ShowMessage("Options from \"%s\":\n", i->c_str());
      for (StringArray::iterator i = option.begin(); i != option.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
#endif

      if (option.size() != 2)
         throw CommandException(typeName + "::InterpretAction() Write options "
         "are not in the form option = value in line\n" +
         generatingString);

      std::string opt0 = GmatStringUtil::ToUpper(option[0]);
      std::string opt1 = GmatStringUtil::ToUpper(option[1]);
      if (opt0 == "MESSAGEWINDOW")
      {
         if (opt1 == "TRUE")
            messageWindowOn = true;
         else if (opt1 == "FALSE")
            messageWindowOn = false;
         else
            throw CommandException(typeName + "::InterpretAction() Write Command "
            "MessageWindow option " + option[1] +
            " is not a recognized value on line\n" + generatingString +
            "\nAllowed values are \"true\" and \"false\"\n");
      }
      else if (opt0 == "LOGFILE")
      {
         if (opt1 == "TRUE")
            logFileOn = true;
         else if (opt1 == "FALSE")
            logFileOn = false;
         else
            throw CommandException(typeName + "::InterpretAction() Write Command "
            "LogFile option " + option[1] +
            " is not a recognized value on line\n" + generatingString +
            "\nAllowed values are \"true\" and \"false\"\n");
      }
      else if (opt0 == "STYLE")
      {
         if (opt1 == "CONCISE")
            outputStyle = OutputStyle::CONCISE;
         else if (opt1 == "VERBOSE")
            outputStyle = OutputStyle::VERBOSE;
         else if (opt1 == "SCRIPT")
            outputStyle = OutputStyle::SCRIPTABLE;
         else
            throw CommandException(typeName + "::InterpretAction() Write Command "
            "Style option " + option[1] +
            " is not a recognized value on line\n" + generatingString +
            "\nAllowed values are \"Concise\", \"Verbose\" and \"Script\"\n");
      }
      else if (opt0 == "REPORTFILE")
      {
         // will not work without sandbox
         //GmatBase *mapObj = FindObject(option[1]);
         //if ((mapObj == NULL) || (mapObj->GetTypeName() != "ReportFile"))
         //   throw CommandException(typeName + "::InterpretAction() Write Command "
         //   "ReportFile option \"" + option[1] +
         //   "\" is not a ReportFile\n");
         reportFile = option[1];
      }
      else
      {
         throw CommandException(typeName +
            "::InterpretAction() Write option " + option[0] +
            " is not a recognized option on line\n" + generatingString
            );
      }
   }
#ifdef DEBUG_Write_IA
   MessageInterface::ShowMessage("Write::CheckForOptions:: Append flag is %s\n",
      (appendFlag ? "true" : "false"));
#endif
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Write::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < WriteParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Write::GetStringParameter(const Integer id) const
{
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Write::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Write::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case ADD:
      #ifdef DEBUG_Write_SET
      MessageInterface::ShowMessage
         ("Write::SetStringParameter() Adding parameter '%s'\n", value.c_str());
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
bool Write::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Write::SetStringParameter(const Integer id, const std::string &value,
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
bool Write::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Write::GetStringArrayParameter(const Integer id) const
{
   #ifdef DEBUG_WriteFILE_GET
   MessageInterface::ShowMessage
      ("Write::GetStringArrayParameter() id=%d, actualElementNames.size()=%d, "
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
const StringArray& Write::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& Write::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();
   wrapperObjectNames.insert(wrapperObjectNames.end(), actualElementNames.begin(),
                             actualElementNames.end());
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Write::SetElementWrapper(ElementWrapper *toWrapper,
                                const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Write::SetElementWrapper() this=<%p> '%s' entered, toWrapper=<%p>, "
       "withName='%s'\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       toWrapper, withName.c_str());
   #endif
   
   if (toWrapper == NULL)
      return false;
   
   // Do we need any type checking?
   // CheckDataType(toWrapper, Gmat::REAL_TYPE, "Write", true);
   
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
         ((*ewi), (*ewi)->GetDescription(), "Write::SetElementWrapper()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Write::SetElementWrapper() exiting with %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Write::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Write::ClearWrappers() this=<%p> '%s' entered\n   There are %d wrappers "
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
bool Write::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Write::TakeAction() action=%s, actionData=%s\n",
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
std::string Write::GetRefObjectName(const Gmat::ObjectType type) const
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
const StringArray& Write::GetRefObjectNameArray(const Gmat::ObjectType type)
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
 * Sets the WriteFile and Element objects used bt the Write command.
 * 
 * This method received the global instances of the objects used in the Write
 * command.  It checks their types and stores their names, so that the the 
 * objects can be retrieved from the local store in the Sandbox during 
 * initiialization.  It also tells the WriteFile instance that it will need
 * to be ready to receive data from a WriteCommand, so that the WriteFile does
 * not erroneously inform the user that no data will be written to the 
 * WriteFile.
 * 
 * @param <obj> Pointer to the reference object.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return true if object successfully set, throws otherwise.
 */
//------------------------------------------------------------------------------
bool Write::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name, const Integer index)
{
   if (obj == NULL)
   {
      #ifdef DEBUG_Write_OBJ
      MessageInterface::ShowMessage
         ("\nWrite::SetRefObject() this=<%p> returning false since input object is NULL\n");
      #endif
      return false;
   }
   
   #ifdef DEBUG_Write_OBJ
   MessageInterface::ShowMessage
      ("\nWrite::SetRefObject() this=<%p> entered, obj=<%p><%s>'%s'\n   type=%d<%s>, "
       "name='%s', index=%d\n", this, obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       type, GetObjectTypeString(type).c_str(), name.c_str(), index);
   #endif
   
   
   #ifdef DEBUG_Write_OBJ
   MessageInterface::ShowMessage("Write::SetRefObject() returning true\n");
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
bool Write::RenameRefObject(const Gmat::ObjectType type,
                              const std::string &oldName,
                              const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Write::RenameRefObject() entered, type=%d<%s>, oldName='%s', newName='%s'\n",
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
   MessageInterface::ShowMessage("Write::RenameRefObject() leaving\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of this Write.
 *
 * @return clone of the Write.
 */
//------------------------------------------------------------------------------
GmatBase* Write::Clone() const
{
   return (new Write(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& Write::GetGeneratingString(Gmat::WriteMode mode,
   const std::string &prefix,
   const std::string &useName)
{
#ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Write::GetGeneratingString() entered, prefix='%s', useName='%s'\n",
      prefix.c_str(), useName.c_str());
#endif

   std::string gen = prefix + "Write ";
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
   gen += " { ";
   switch (outputStyle)
   {
      case CONCISE:     gen += "Style = Concise, "; break;
      case VERBOSE:     gen += "Style = Verbose, "; break;
      case SCRIPTABLE:  gen += "Style = Script, "; break;
   }
   if (logFileOn)
      gen += "LogFile = true, ";
   else
      gen += "LogFile = false, ";
   if (messageWindowOn)
      gen += "MessageWindow = true";
   else
      gen += "MessageWindow = false";
   if (reportFile != "")
      gen += ", ReportFile = \"" + reportFile + "\" }";
   else
      gen += " }";

   generatingString = gen;
   
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
 * Performs the initialization needed to run the Write command.
 *
 * @return true if the Write is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Write::Initialize()
{
   if (GmatCommand::Initialize() == false)
      return false;

   #ifdef DEBUG_Write_INIT
   MessageInterface::ShowMessage
      ("\nWrite::Initialize() this=<%p> entered, has %d parameter names\n",
       this, elementNames.size());
   #endif
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   // parms.clear();
   GmatBase *mapObj = NULL;

   if (reportFile != "")
   {
      mapObj = FindObject(reportFile);
      if ((mapObj == NULL) || (mapObj->GetTypeName() != "ReportFile"))
         throw CommandException(typeName + "::Initialize() Write Command "
         "ReportFile option \"" + reportFile +
         "\" is not a ReportFile\n");
      reporter = (ReportFile *)mapObj;
      // Tell the ReportFile object that a command has requested its services.
      // Added this here so that ReportFile::Initialize() doesn't throw exception
      // when there is no paramters to report 
      reporter->TakeAction("PassedToReport");
   }
   
   for (StringArray::iterator i = elementNames.begin(); i != elementNames.end(); ++i)
   {
      std::string itemName = *i;
      
      #ifdef DEBUG_Write_INIT
      MessageInterface::ShowMessage("   Now find object for '%s'\n", itemName.c_str());
      #endif
      
      mapObj = FindObject(itemName);
      if (mapObj == NULL)
      {
         // Check if it is an object property
         std::string type, owner, dep;
         GmatStringUtil::ParseParameter(itemName, type, owner, dep);
         #ifdef DEBUG_Write_INIT
         MessageInterface::ShowMessage
            ("   owner='%s', type='%s', dep='%s'\n", owner.c_str(), type.c_str(), dep.c_str());
         #endif
         if (owner != "" && type != "")
            mapObj = FindObject(owner);
         
         if (mapObj == NULL)
         {
            std::string msg = "Object named \"" + (*i) +
               "\" cannot be found for the Write command '" +
               GetGeneratingString(Gmat::NO_COMMENTS) + "'";
            #ifdef DEBUG_Write_INIT
            MessageInterface::ShowMessage("**** ERROR **** %s\n", msg.c_str());
            #endif
            throw CommandException(msg);
         }
      }
   }
   
   // Set Wrapper references
   for (WrapperArray::iterator i = elementWrappers.begin(); i < elementWrappers.end(); i++)
   {
      #ifdef DEBUG_Write_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*i)->GetDescription().c_str());
      #endif
      
      if (SetWrapperReferences(*(*i)) == false)
         return false;      
   }
   
   #ifdef DEBUG_Write_INIT
   MessageInterface::ShowMessage("Write::Initialize() this=<%p> returning true\n", this);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ExecuteReport()
//------------------------------------------------------------------------------
/**
* Write the data to report file
*
* @return true if the Command runs to completion, false if an error occurs.
*/
//------------------------------------------------------------------------------
void Write::ExecuteReport()
{
   if (reporter == NULL)
      throw CommandException("Reporter is not yet set\n");

#ifdef DEBUG_Write_EXEC
   MessageInterface::ShowMessage
      ("\nReport::Execute() this=<%p> <%s> entered, reporter <%s> '%s' has %d Parameters\n",
      this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
      reporter->GetName().c_str(), reporter->GetStringParameter("Filename").c_str(),
      parms.size());
#endif

   // Build the data as a string
   std::stringstream datastream;

   // Set the stream to use the settings in the ReportFile.
   // Note that this is done here, rather than during initialization, in case
   // the user has changed the values during the run.
   Integer prec = reporter->GetIntegerParameter(reporter->GetParameterID("Precision"));
   datastream.precision(prec);

   bool leftJustify = false;
   if (reporter->GetOnOffParameter(reporter->GetParameterID("LeftJustify")) == "On")
      leftJustify = true;

   bool zeroFill = false;
   if (reporter->GetOnOffParameter(reporter->GetParameterID("ZeroFill")) == "On")
      zeroFill = true;

   int colWidth = reporter->GetIntegerParameter(reporter->GetParameterID("ColumnWidth"));

#ifdef DEBUG_Write_EXEC
   MessageInterface::ShowMessage
      ("   precision=%d, leftJustify=%d, zeroFill=%d, colWidth=%d, needsHeaders=%d\n",
      prec, leftJustify, zeroFill, colWidth, needsHeaders);
#endif

   if (leftJustify)
      datastream.setf(std::ios::left);

   // first time through, use cmd setting; after that, ask reporter
   if (hasExecuted)
      needsHeaders = reporter->TakeAction("CheckHeaderStatus");
   else
      needsHeaders = reporter->GetBooleanParameter(
      reporter->GetParameterID("WriteHeaders"));

   if (needsHeaders)
      WriteHeaders(datastream, colWidth);

   // if zero fill, show decimal point
   // showing decimal point automatically filles zero
   if (zeroFill)
      datastream.setf(std::ios::showpoint);

   // Write to report file using ReportFile::WriateData().
   // This method takes ElementWrapper array to write data to stream
   reporter->TakeAction("ActivateForReport", "On");
   bool retval = reporter->WriteData(elementWrappers);
   reporter->TakeAction("ActivateForReport", "Off");
   hasExecuted = true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Write the data to output targets
 *
 * @return true if the Command runs to completion, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Write::Execute()
{
   #ifdef DEBUG_Write_EXEC
   MessageInterface::ShowMessage
      ("\nWrite::Execute() this=<%p> <%s> entered\n",
       this, GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif

   bool retval = true;
   bool logging = MessageInterface::GetLogEnable();
      
   std::string prefix;
   std::string wrapperToStr;
   GmatBase *gb;

   // write to report file
   if (reporter != NULL)
      ExecuteReport();

   // Write to Message Window using elementWrappers
   for (WrapperArray::iterator i = elementWrappers.begin(); i < elementWrappers.end(); i++)
   {
      #ifdef DEBUG_Write_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*i)->GetDescription().c_str());
      #endif
      
      ElementWrapper *wrapper = *i;
      if (wrapper)
      {
         // get the strings to write to the destinations
         switch (outputStyle)
         {
            case CONCISE:     
               prefix = "";          
               wrapperToStr = wrapper->ToString();
               break;
            case VERBOSE:     
               prefix = wrapper->GetDescription() + " ="; 
               wrapperToStr = wrapper->ToString();
               break;
            case SCRIPTABLE:  
               prefix = "";
               gb = wrapper->GetRefObject();
               if (gb->GetTypeName() == "Variable" || gb->GetTypeName() == "String")
               {
                  prefix = "Create " + gb->GetTypeName() + " " + gb->GetName() + ";";
                  wrapperToStr = "GMAT " + gb->GetName() + " = " + wrapper->ToString() + ";";
               }
               else
               {
                  wrapperToStr = gb->GetGeneratingString(Gmat::NO_COMMENTS, "", "");
               }
               break;
         }

         // write to message window
         if (messageWindowOn)
         {
            // make sure we do not write to log file.  It will be done later
            MessageInterface::SetLogEnable(false);
            try
            {
               if (prefix != "")
                  MessageInterface::ShowMessage
                  ("%s\n", prefix.c_str());
               MessageInterface::ShowMessage
                  ("%s\n", wrapperToStr.c_str());
               MessageInterface::SetLogEnable(logging);
            }
            catch (...)
            {
               MessageInterface::SetLogEnable(logging);
               throw;
            }
         }
         // write to log file
         if (logFileOn)
         {
            MessageInterface::SetLogEnable(true);
            try
            {
               if (prefix != "")
                  MessageInterface::LogMessage
                  ("%s\n", prefix.c_str());
               MessageInterface::LogMessage
                  ("%s\n", wrapperToStr.c_str());
               MessageInterface::SetLogEnable(logging);
            }
            catch (...)
            {
               MessageInterface::SetLogEnable(logging);
               throw;
            }
         }
      }
   }

   BuildCommandSummary(true);
   
   #ifdef DEBUG_Write_EXEC
   MessageInterface::ShowMessage
      ("Write::Execute() this=<%p> returning %d\n", this, retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
void Write::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("Write::RunComplete() this=<%p> '%s' entered\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// bool AddElements(const std::string &elementName, Integer index)
//------------------------------------------------------------------------------
/**
 * Adds elements to the Write list.
 */
//------------------------------------------------------------------------------
bool Write::AddElements(const std::string &elementName, Integer index)
{
   #ifdef DEBUG_Write_SET
   MessageInterface::ShowMessage
      ("Write::AddElements() this=<%p>, Adding element '%s', index=%d, "
       "numElements=%d\n", this, elementName.c_str(), index, numElements);
   #endif
   
   if (elementName == "")
   {
      #ifdef DEBUG_Write_SET
      MessageInterface::ShowMessage
         ("Write::AddElements() returning false, input elementName is blank\n");
      #endif
      return false;
   }
   
   if (index < 0)
   {
      #ifdef DEBUG_Write_SET
      MessageInterface::ShowMessage
         ("Write::AddElements() returning false, the index %d is less than 0\n");
      #endif
      return false;
   }
   
   if (index > numElements)
   {
      #ifdef DEBUG_Write_SET
      MessageInterface::ShowMessage
         ("Write::AddElements() returning false, the index %d is out of bounds, "
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
   
   #ifdef DEBUG_Write_SET
   MessageInterface::ShowMessage
      ("   Added '%s', size=%d\n", elementName.c_str(), numElements);
   MessageInterface::ShowMessage
      ("Write::AddElements() this=<%p> returning true\n", this);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void DeleteElements()
//------------------------------------------------------------------------------
/**
 * Delete elements from the Write list and wrappers.
 */
//------------------------------------------------------------------------------
void Write::DeleteElements()
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Write::DeleteElements() this=<%p> '%s' entered\n   There are %d wrappers "
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
         ((*ewi), (*ewi)->GetDescription(), "Write::DeleteElements()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   elementWrappers.clear();   
   actualElementNames.clear();
}


//------------------------------------------------------------------------------
// void WriteHeaders(std::stringstream &datastream, Integer colWidth)
//------------------------------------------------------------------------------
/**
* Writes column header by calling ReportFile::ReceiveData() with composed
* headers
*/
//------------------------------------------------------------------------------
void Write::WriteHeaders(std::stringstream &datastream, Integer colWidth)
{
#ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage
      ("Report::WriteHeaders() entered, colWidth = %d, needsHeaders = %d\n",
      colWidth, needsHeaders);
#endif

   reporter->TakeAction("ActivateForReport", "On");
   for (StringArray::iterator i = actualElementNames.begin();
      i != actualElementNames.end(); ++i)
   {
      datastream.width(colWidth);
      datastream << (*i);
      datastream << "   ";
   }

   std::string header = datastream.str();
   reporter->ReceiveData(header.c_str(), header.length());
   datastream.str("");

#ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage
      ("Report::WriteHeaders() leaving, needsHeaders set to false\n");
#endif

   reporter->TakeAction("HeadersWritten");
   needsHeaders = false;
}


