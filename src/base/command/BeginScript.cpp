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
#include <sstream>

#include "MessageInterface.hpp"


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
    GmatCommand (noop)
{}


//------------------------------------------------------------------------------
//  BeginScript& operator=(const BeginScript&)
//------------------------------------------------------------------------------
/**
 * Sets this BeginScript to match another one (assignment operator).
 * 
 * @param noop The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
BeginScript& BeginScript::operator=(const BeginScript& noop)
{
    return *this;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Executes the BeginScript command (copy constructor).
 * 
 * During mission execution, BeginScript is a null operation -- nothing is done
 * in this command.  It functions as a marker in the script, indicating to the
 * GUI where a block of commands starts that should all be grouped together on a
 * ScriptEvent panel.
 */
//------------------------------------------------------------------------------
bool BeginScript::Execute(void)
{
    return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginScript.
 *
 * @return clone of the BeginScript.
 *
 */
//------------------------------------------------------------------------------
GmatBase* BeginScript::Clone(void) const
{
   return (new BeginScript(*this));
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginScript.
 *
 * @return clone of the BeginScript.
 */
//------------------------------------------------------------------------------
const std::string& BeginScript::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   Integer whichOne, start;
   std::stringstream gen;
   std::string indent = "   ", cmdstr;
   
   gen << "BeginScript\n";
   
   GmatCommand *current = next;
   while (current != NULL) {
      if (current->GetTypeName() != "EndScript") {
         cmdstr = current->GetGeneratingString();
         start = 0;
         while (cmdstr[start] == ' ')
            ++start;
         cmdstr = cmdstr.substr(start);

         gen << indent << cmdstr << "\n";
         
         // Handle the branches for branch commands
         whichOne = 0;
         GmatCommand* child = current->GetChildCommand(whichOne);
         #ifdef DEBUG_BEGIN_SCRIPT
            MessageInterface::ShowMessage("Command %s %s a child command.\n",
               current->GetTypeName().c_str(),
               ((child == NULL) ? "does not have" : "has"));
         #endif
         while ((child != NULL) && (child != current)) {
            gen << GetChildString(indent + "   ", child, current);
            ++whichOne;
            child = current->GetChildCommand(whichOne);
         }

         current = current->GetNext();
         if (current == NULL)
            gen << "EndScript;\n";
      }
      else {
         gen << "EndScript;\n";
         current = NULL;
      }
   }
   
   generatingString = gen.str();
   return generatingString;
}


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
   
   while ((current != parent) && (current != NULL)) {
      cmdstr = current->GetGeneratingString();
      start = 0;
      while (cmdstr[start] == ' ')
         ++start;
      cmdstr = cmdstr.substr(start);
      sstr << prefix << cmdstr << "\n";
      whichOne = 0;
      GmatCommand* child = current->GetChildCommand(whichOne);
      while ((child != NULL) && (child != cmd)) {
         sstr << GetChildString(prefix + "   ", child, current);
         ++whichOne;
         child = current->GetChildCommand(whichOne);
      }
      current = current->GetNext();
   }
   
   return sstr.str();
}
