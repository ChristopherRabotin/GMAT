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
// MissionTreeItemData(wxString desc, int type, const wxString &name,
//                     Command *cmd)
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
MissionTreeItemData::MissionTreeItemData(wxString desc, int type,
                                         const wxString &name, Command *cmd)
    : GmatTreeItemData(desc, type)
{
    theCommand = cmd;
    theCommandName = name;
}

//------------------------------------------------------------------------------
// Command* GetCommand()
//------------------------------------------------------------------------------
/**
 * @return Command pointer
 */
//------------------------------------------------------------------------------
Command* MissionTreeItemData::GetCommand()
{
    return theCommand;
}

//------------------------------------------------------------------------------
// wxString GetCommandName()
//------------------------------------------------------------------------------
/**
 * @return command name
 */
//------------------------------------------------------------------------------
wxString MissionTreeItemData::GetCommandName()
{
    return theCommandName;
}

