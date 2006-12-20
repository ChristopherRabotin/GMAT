//$Header$
//------------------------------------------------------------------------------
//                              BeginScript
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/02/25
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Script tag used to indicate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------


#include "BeginScript.hpp" 
#include "TextParser.hpp"     // for DecomposeBlock()
#include "CommandUtil.hpp"    // for GetNextCommand()
#include <sstream>

#include "MessageInterface.hpp"

//#define DEBUG_BEGIN_SCRIPT
//#define DEBUG_BEGIN_SCRIPT_GEN_STRING

//------------------------------------------------------------------------------
//  BeginScript()
//------------------------------------------------------------------------------
/**
 * Constructs the BeginScript command (default constructor).
 */
//------------------------------------------------------------------------------
BeginScript::BeginScript() :
   GmatCommand("BeginScript")
{
   generatingString = "BeginScript";
}


//------------------------------------------------------------------------------
//  ~BeginScript()
//------------------------------------------------------------------------------
/**
 * Destroys the BeginScript command (default constructor).
 */
//------------------------------------------------------------------------------
BeginScript::~BeginScript()
{
}


//------------------------------------------------------------------------------
//  BeginScript(const BeginScript& noop)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the BeginScript command (copy constructor).
 */
//------------------------------------------------------------------------------
BeginScript::BeginScript(const BeginScript& noop) :
    GmatCommand       (noop)
{
        generatingString = noop.generatingString;
}


//------------------------------------------------------------------------------
//  BeginScript& operator=(const BeginScript&)
//------------------------------------------------------------------------------
/**
 * Sets this BeginScript to match another one (assignment operator).
 * 
 * @param noop The original used to set parameters for this one.
 *
 * @return this object.
 */
//------------------------------------------------------------------------------
BeginScript& BeginScript::operator=(const BeginScript& noop)
{
   if (this == &noop)
      return *this;

   GmatCommand::operator=(noop);
   generatingString = noop.generatingString;
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the BeginScript command (copy constructor).
 * 
 * During mission execution, BeginScript is a null operation -- nothing is done
 * in this command.  It functions as a marker in the script, indicating to the
 * GUI where a block of commands starts that should all be grouped together on a
 * ScriptEvent panel.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool BeginScript::Execute()
{
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginScript.
 *
 * @return clone of the BeginScript.
 */
//------------------------------------------------------------------------------
GmatBase* BeginScript::Clone() const
{
   return (new BeginScript(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginScript.
 *
 * @param <mode>    Specifies the type of serialization requested. (Not yet
 *                  used in commands)
 * @param <prefix>  Optional prefix appended to the object's name.  (Not yet
 *                  used in commands)
 * @param <useName> Name that replaces the object's name.  (Not yet used in
 *                  commands)
 *
 * @return The string that reproduces this command.
 */
//------------------------------------------------------------------------------
const std::string& BeginScript::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   //Note: This method is called only once from the ScriptInterpreter::WriteScript()
   // So all nested ScriptEvent generating string should be handled here
   
   std::stringstream gen;
   //    Integer start;
   std::string indent = "   ", cmdstr;
   std::string commentLine = GetCommentLine();
   std::string inlineComment = GetInlineComment();
   std::string newPrefix = prefix + indent;
   
   if (commentLine != "")
      gen << commentLine;
   
   gen << prefix << "BeginScript";
   
   if (inlineComment != "")
      gen << inlineComment << "\n";
   else
      gen << "\n";
   
   #ifdef DEBUG_BEGIN_SCRIPT_GEN_STRING
   MessageInterface::ShowMessage
      ("BeginScript::GetGeneratingString() this=(%p)%s, mode=%d, prefix='%s', "
       "useName='%s'\n", this, this->GetTypeName().c_str(), mode, prefix.c_str(),
       useName.c_str());
   #endif
   
   GmatCommand *current = next;
   
   while (current != NULL)
   {
      #ifdef DEBUG_BEGIN_SCRIPT_GEN_STRING
      MessageInterface::ShowMessage
         ("BeginScript::GetGeneratingString() current=(%p)%s\n", current,
          current->GetTypeName().c_str());
      #endif
      
      if (current->GetTypeName() != "EndScript")
      {
         // Indent whole block within Begin/EndScript (loj: 12/15/06)
         cmdstr = current->GetGeneratingString(mode, prefix, useName);
         
         //start = 0;
         //while (cmdstr[start] == ' ')
         //   ++start;
         //cmdstr = cmdstr.substr(start);
         
         #ifdef DEBUG_BEGIN_SCRIPT_GEN_STRING
         MessageInterface::ShowMessage
            ("BeginScript::GetGeneratingString() cmdstr=\n   \"%s\"\n", cmdstr.c_str());
         #endif
         
         //gen << indent << prefix << cmdstr << "\n";
         TextParser tp;
         StringArray textArray = tp.DecomposeBlock(cmdstr);
         
         for (UnsignedInt i=0; i<textArray.size(); i++)
         {
            gen << indent << textArray[i];
            if (textArray[i].find("\n") == cmdstr.npos &&
                textArray[i].find("\r") == cmdstr.npos)
               gen << "\n";
         }
         
         // Get command after EndScript
         //current = current->GetNext();
         current = GmatCommandUtil::GetNextCommand(current);
         
         if (current == NULL)
            gen << prefix << "EndScript;\n";
      }
      else
      {
         //gen << prefix << "EndScript;\n";         
         gen << prefix << "EndScript;";
         current = NULL;
      }
   }
   
   generatingString = gen.str();
   
   #ifdef DEBUG_BEGIN_SCRIPT_GEN_STRING
   MessageInterface::ShowMessage
      ("BeginScript::GetGeneratingString() return generatingString=\n%s\n\n",
       generatingString.c_str());
   #endif
   
   return generatingString;
}


//------------------------------------------------------------------------------
// const std::string BeginScript::GetChildString(const std::string &prefix,
//                                               GmatCommand *cmd,
//                                               GmatCommand *parent)
//------------------------------------------------------------------------------
/**
 * Iteratively recurses through the command tree, building the strings to build
 * the child commands.
 *
 * @param <prefix> Prefix appended to the child command's string (typically used
 *                 to indent the child command strings).
 * @param <cmd>    The first child command in the current nesting level.
 * @param <parent> The command that has this command as a child.
 *
 * @return The string that reproduces the commands at the child's level.
 */
//------------------------------------------------------------------------------
const std::string BeginScript::GetChildString(const std::string &prefix,
                                              GmatCommand *cmd,
                                              GmatCommand *parent)
{
   #ifdef DEBUG_BEGIN_SCRIPT
      MessageInterface::ShowMessage("BeginScript::GetChildString entered\n");
   #endif
   
   std::stringstream sstr;
   std::string cmdstr;
   Integer whichOne, start;
   GmatCommand *current = cmd;
   
   while ((current != parent) && (current != NULL))
   {
      cmdstr = current->GetGeneratingString();
      start = 0;
      while (cmdstr[start] == ' ')
         ++start;
      cmdstr = cmdstr.substr(start);
      sstr << prefix << cmdstr << "\n";
      whichOne = 0;
      GmatCommand* child = current->GetChildCommand(whichOne);
      while ((child != NULL) && (child != cmd))
      {
         sstr << GetChildString(prefix + "   ", child, current);
         ++whichOne;
         child = current->GetChildCommand(whichOne);
      }
      current = current->GetNext();
   }
   
   return sstr.str();
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool BeginScript::RenameRefObject(const Gmat::ObjectType type,
                                  const std::string &oldName,
                                  const std::string &newName)
{   
   GmatCommand *current = next;
   
   while (current != NULL)
   {
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("BeginScript::RenameRefObject() current=%s\n",
          current->GetTypeName().c_str());
      #endif
      
      if (current->GetTypeName() != "EndScript")
      {
         current->RenameRefObject(type, oldName, newName);
         current = current->GetNext();
      }
      else
      {
         current = NULL;
      }
   }
   
   return true;
}
