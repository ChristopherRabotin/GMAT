//$Id$
//------------------------------------------------------------------------------
//                             MissionTreeItemData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
// MissionTreeItemData(const wxString &name, GmatTree::ItemType type,
//                     const wxString &title, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Constructs MissionTreeItemData object.
 *
 * @param <name> command name
 * @param <type> input int for type of data.
 * @param <title> input string for description of node.
 * @param <cmd> command pointer
 *
 * @note Creates the MissionTreeItemData object.
 */
//------------------------------------------------------------------------------
MissionTreeItemData::MissionTreeItemData(const wxString &name, GmatTree::ItemType type,
                                         const wxString &title, GmatCommand *cmd)
   : GmatTreeItemData(name, type, title, false)
{
   // Set title as name for mission tree since they are the same
   mItemTitle = name;
   theCommand = cmd;
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

