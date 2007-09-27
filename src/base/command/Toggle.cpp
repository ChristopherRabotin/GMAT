//$Header$
//------------------------------------------------------------------------------
//                                  Toggle
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/mm/dd
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Insert descriptive text here.
 *
 * @note Any notes here.
 */
//------------------------------------------------------------------------------


// Class automatically generated by Dev-C++ New Class wizard

#include "Toggle.hpp" // class's header file
#include "Publisher.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_RENAME 1
//#define DEBUG_TOGGLE

// class constructor
//------------------------------------------------------------------------------
// Toggle()
//------------------------------------------------------------------------------
Toggle::Toggle() :
   GmatCommand    ("Toggle"),
   toggleState    (true),
   subscriberID   (parameterCount),
   toggleStateID  (++parameterCount)
{
   ++parameterCount;
}

// class destructor
//------------------------------------------------------------------------------
// ~Toggle()
//------------------------------------------------------------------------------
Toggle::~Toggle()
{
   // insert your code here
}


//------------------------------------------------------------------------------
// Toggle(const Toggle& t)
//------------------------------------------------------------------------------
Toggle::Toggle(const Toggle& t) :
   GmatCommand        (t),
   toggleState        (t.toggleState),
   subscriberID       (t.subscriberID),
   toggleStateID      (t.toggleStateID) //loj: 4/405 Added
{
   parameterCount = t.parameterCount;
   subNames.clear();
   subs.clear();
}


//------------------------------------------------------------------------------
// Toggle& operator=(const Toggle& t)
//------------------------------------------------------------------------------
Toggle& Toggle::operator=(const Toggle& t)
{
   return *this;
   
   if (this == &t)
      return *this;
    
   GmatCommand::operator=(t);
   toggleState = t.toggleState;
   subNames.clear();
   subs.clear();
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool Toggle::InterpretAction()
{
   /// @todo: Clean up this hack for the Toggle::InterpretAction method
   // Sample string:  "Toggle Report On"
    
   Integer loc = generatingString.find("Toggle", 0) + 6;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;
   Integer subEnd, commentStart;
   if ((commentStart = (Integer) generatingString.find_first_of("%", 0)) != 
       (Integer)generatingString.npos)  
      subEnd = commentStart;
   else 
      subEnd = (Integer) generatingString.size();
   std::string str1 = generatingString.substr(loc, subEnd-loc);
   #ifdef DEBUG_TOGGLE
      MessageInterface::ShowMessage("In InterpretAction, str1 = \n");
      MessageInterface::ShowMessage("   %s\n", str1.c_str());
   #endif

   // this command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(str, false))
   {
      std::string msg = 
         "The Toggle command is not allowed to contain brackets, braces, or "
         "parentheses";
      throw CommandException(msg);
   }
   StringArray parts = GmatStringUtil::SeparateBy(str1," ,", false);
   Integer partsSz = (Integer) parts.size();
   #ifdef DEBUG_TOGGLE
      MessageInterface::ShowMessage("In InterpretAction, parts = \n");
      for (Integer jj = 0; jj < partsSz; jj++)
         MessageInterface::ShowMessage("   %s\n", parts.at(jj).c_str());
   #endif
   if (partsSz < 2) // 'Toggle' already found
      throw CommandException("Missing field in Toggle command");
   if (parts.at(partsSz-1) == "On")       toggleState = true;
   else if (parts.at(partsSz-1) == "Off") toggleState = false;
   else
      throw CommandException("Missing or misplaced 'On' or 'Off' in Toggle command");
   for (Integer ii = 0; ii < partsSz-1; ii++)
   {
      if ((parts.at(ii) == "On") || (parts.at(ii) == "Off"))
         throw CommandException("Too many 'On's or 'Off's in Toggle command");
      #ifdef DEBUG_TOGGLE
         MessageInterface::ShowMessage("Adding subName %s \n", parts.at(ii).c_str());
      #endif
               
      subNames.push_back(parts.at(ii));
   }
   
/*
   Integer cmd = generatingString.find("On", loc);
   while (generatingString.find("On", cmd+1) != std::string::npos)
      cmd = generatingString.find("On", cmd+1);
        
   if ((cmd == (Integer)std::string::npos) || (cmd <= (Integer)(generatingString.length() - 5))) 
   {
      cmd = generatingString.find("Off", loc);
      while (generatingString.find("Off", cmd+1) != std::string::npos)
         cmd = generatingString.find("Off", cmd+1);
            
      if (cmd == (Integer)std::string::npos)
         throw CommandException("Must Toggle either 'On' or 'Off'");
      if (cmd > (Integer)(generatingString.length() - 6)) 
      {
         toggleState = false;
      }
   }
   else
      if (cmd > (Integer)(generatingString.length() - 5)) 
      {
         toggleState = true;
      }
            
   // Find the Subscriber list
   end = generatingString.find(" ", loc);
   std::string sName = generatingString.substr(loc, end-loc);
   subNames.push_back(sName);
    */ 
   // Register with the publisher
   if (publisher == NULL)
      publisher = Publisher::Instance();
   streamID = publisher->RegisterPublishedData(subNames, subNames);
        
   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Toggle::Initialize()
{
   #ifdef DEBUG_TOGGLE
      MessageInterface::ShowMessage("Toggle::Initialize() entered\n");
   #endif
      
   GmatCommand::Initialize();
   
   Subscriber *sub;
   subs.clear();
   
   for (StringArray::iterator s = subNames.begin(); s != subNames.end(); ++s) {
      if ((*objectMap).find(*s) != objectMap->end()) {
         sub = (Subscriber *)(*objectMap)[*s];
         if (sub) {
            subs.push_back(sub);
         }
      }
      else {
         MessageInterface::ShowMessage
            ("Toggle command cannot find subscriber %s; command has no effect for that object\n",
             s->c_str());
      }
   }
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool Toggle::Execute()
{
   #ifdef DEBUG_TOGGLE
      MessageInterface::ShowMessage("Toggle::Execute() entered\n");
   #endif

   for (std::list<Subscriber *>::iterator s = subs.begin(); s != subs.end(); ++s) {
      (*s)->Activate(toggleState);
   }
    
   char data[] = "Toggle executed\n\n";
   publisher->Publish(streamID, data, strlen(data));
   
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Toggle.
 *
 * @return clone of the Toggle.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Toggle::Clone(void) const
{
   return (new Toggle(*this));
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
bool Toggle::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Toggle::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      subNames.clear();
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Achieve.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Toggle::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SUBSCRIBER);
   //refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
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
const StringArray& Toggle::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   // There are only subscribers, so ignore object type
   return subNames;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Toggle::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "Toggle ";
   for (StringArray::iterator i = subNames.begin(); i != subNames.end(); ++i)
      generatingString += (*i) + " ";
   generatingString += ((toggleState == true) ? "On;" : "Off;");
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Toggle::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Toggle::RenameConfiguredItem() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::SUBSCRIBER)
      return true;

   for (unsigned int i=0; i<subNames.size(); i++)
   {
      if (subNames[i] == oldName)
         subNames[i] = newName;
   }

   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Toggle::GetParameterText(const Integer id) const
{
   if (id == subscriberID)
      return "Subscriber";
   else if (id == toggleStateID)
      return "ToggleState";
   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Toggle::GetParameterID(const std::string &str) const
{
   if (str == "Subscriber")
      return subscriberID;
   else if (str == "ToggleState")
      return toggleStateID;
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Toggle::GetParameterType(const Integer id) const
{
   if (id == subscriberID)
      return Gmat::STRING_TYPE;
   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Toggle::GetParameterTypeString(const Integer id) const
{
   if (id == subscriberID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Toggle::GetStringParameter(const Integer id) const
{
//   if (id == subscriberID)
//   {
//      return subNames[0]; //loj: return first subscriber for now
//   }
   //   else if (id == toggleStateID)
   if (id == toggleStateID)
   {
      if (toggleState == true)
         return "On";
      else
         return "Off";
   }
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Toggle::SetStringParameter(const Integer id, const std::string &value)
{

   #ifdef DEBUG_TOGGLE
   MessageInterface::ShowMessage
      ("Toggle::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif

   if (value == "")
      return false;
   
   if (id == subscriberID)
   {
      if (find(subNames.begin(), subNames.end(), value) == subNames.end())
      {
         subNames.push_back(value);
         
         // Register with the publisher
         if (publisher == NULL)
            publisher = Publisher::Instance();
         
         streamID = publisher->RegisterPublishedData(subNames, subNames);
         //MessageInterface::ShowMessage("===> streamID=%d\n", streamID);
      }
      
      return true;
   }
   else if (id == toggleStateID)
   {
      if (value == "On")
         toggleState = true;
      else
         toggleState = false;

      return true;
   }
   
   return GmatCommand::SetStringParameter(id, value);
}

std::string Toggle::GetStringParameter(const Integer id,
                                       const Integer index) const
{
   if (id == subscriberID)
   {
      if (index < 0 || index >= (Integer) subNames.size())
         throw CommandException(
                  "Index out-of-range for subscriber names list for Toggle command.\n");
      return subNames.at(index); 
   }
   return GmatCommand::GetStringParameter(id, index);
}

bool Toggle::SetStringParameter(const Integer id, 
                                const std::string &value,
                                const Integer index)
{
   if (value == "")
      return false;
   
   if (id == subscriberID)
   {
      if (index < 0 || index > (Integer) subNames.size())
         throw CommandException(
                  "Index out-of-range for subscriber names list for Toggle command.\n");
      else
      {
         if (index == (Integer) subNames.size())  subNames.push_back(value);
         else                   subNames.at(index) = value;
         
         if (publisher == NULL)
            publisher = Publisher::Instance();
         
         streamID = publisher->RegisterPublishedData(subNames, subNames);
      }
      
      return true;
   }
   return GmatCommand::SetStringParameter(id, value, index);
}

