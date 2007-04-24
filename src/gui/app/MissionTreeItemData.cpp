//$Header$
//------------------------------------------------------------------------------
//                             MissionTreeItemData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/02/06
/**
 * Implements MissionTreeItemData class.
 */
//------------------------------------------------------------------------------
#include "MissionTreeItemData.hpp"

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// MissionTreeItemData(wxString desc, GmatTree::ItemType type, const wxString &name,
//                     GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Constructs MissionTreeItemData object.
 *
 * @param <desc> input string for description of node.
 * @param <type> input int for type of data.
 * @param <name> command name
 * @param <cmd> command pointer
 *
 * @note Creates the MissionTreeItemData object.
 */
//------------------------------------------------------------------------------
MissionTreeItemData::MissionTreeItemData(wxString desc, GmatTree::ItemType type,
                                         const wxString &name, GmatCommand *cmd)
    : GmatTreeItemData(desc, type)
{
   theCommand = cmd;
   theCommandName = name;
}

//------------------------------------------------------------------------------
// virtual GmatCommand* GetCommand()
//------------------------------------------------------------------------------
/**
 * @return Command pointer
 */
//------------------------------------------------------------------------------
GmatCommand* MissionTreeItemData::GetCommand()
{
   return theCommand;
}

//------------------------------------------------------------------------------
// virtual void SetCommand(GmatCommand* cmd)
//------------------------------------------------------------------------------
/**
 * Sets command
 */
//------------------------------------------------------------------------------
void MissionTreeItemData::SetCommand(GmatCommand* cmd)
{
   theCommand = cmd;
}

//------------------------------------------------------------------------------
// virtual wxString GetCommandName()
//------------------------------------------------------------------------------
/**
 * @return command name
 */
//------------------------------------------------------------------------------
wxString MissionTreeItemData::GetCommandName()
{
   return theCommandName;
}

