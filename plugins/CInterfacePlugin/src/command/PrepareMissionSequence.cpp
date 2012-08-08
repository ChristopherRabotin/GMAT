//$Id: PrepareMissionSequence.cpp 8788 2010-11-26 21:36:26Z djcinsb $
//------------------------------------------------------------------------------
//                           PrepareMissionSequence
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2010/03/30
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS task 28
//
/**
 * Null operator used to begin mission sequence -- used (optionally) to toggle
 * command mode
 */
//------------------------------------------------------------------------------


#include "PrepareMissionSequence.hpp" // class's header file
#include "Propagate.hpp"


//------------------------------------------------------------------------------
//  PrepareMissionSequence()
//------------------------------------------------------------------------------
/**
 * Constructs the PrepareMissionSequence command (default constructor).
 */
//------------------------------------------------------------------------------
PrepareMissionSequence::PrepareMissionSequence() :
    GmatCommand("PrepareMissionSequence")
{
   objectTypeNames.push_back("PrepareMissionSequence");
}


//------------------------------------------------------------------------------
//  ~PrepareMissionSequence()
//------------------------------------------------------------------------------
/**
 * Destroys the PrepareMissionSequence command.
 */
//------------------------------------------------------------------------------
PrepareMissionSequence::~PrepareMissionSequence()
{
}


//------------------------------------------------------------------------------
//  PrepareMissionSequence(const PrepareMissionSequence& bms)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the PrepareMissionSequence command (copy constructor).
 *
 * @param bms The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
PrepareMissionSequence::PrepareMissionSequence(const PrepareMissionSequence& bms) :
    GmatCommand (bms)
{
}


//------------------------------------------------------------------------------
//  PrepareMissionSequence& operator=(const PrepareMissionSequence& bms)
//------------------------------------------------------------------------------
/**
 * Sets this PrepareMissionSequence to match another one (assignment operator).
 * 
 * @param bms The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
PrepareMissionSequence& PrepareMissionSequence::operator=(
      const PrepareMissionSequence& bms)
{
   if (this != &bms)
   {
      GmatCommand::operator=(bms);
   }
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the PrepareMissionSequence command (copy constructor).
 * 
 * PrepareMissionSequence is a null operation -- nothing is done in this
 * command.  It functions to toggle GMAT into command mode when needed.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool PrepareMissionSequence::Execute()
{
   BuildCommandSummary(true);
//   GmatCommand *cmd = GetNext();
//   while (cmd != NULL)
//   {
//      if (cmd->GetTypeName() == "Propagate")
//      {
//         ((Propagate*)cmd)->PrepareToPropagate();
//         break;
//      }
//      cmd = cmd->GetNext();
//   }
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the PrepareMissionSequence.
 *
 * @return clone of the PrepareMissionSequence.
 */
//------------------------------------------------------------------------------
GmatBase* PrepareMissionSequence::Clone() const
{
   return (new PrepareMissionSequence(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
bool PrepareMissionSequence::RenameRefObject(const Gmat::ObjectType type,
                                           const std::string &oldName,
                                           const std::string &newName)
{
   return true;
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& PrepareMissionSequence::GetGeneratingString(
                                             Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   generatingString = prefix + "PrepareMissionSequence;";
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

