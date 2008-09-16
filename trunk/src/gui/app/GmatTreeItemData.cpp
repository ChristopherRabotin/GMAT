//$Header$
//------------------------------------------------------------------------------
//                             GmatTreeItemData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/10/08
/**
 * This class provides a data type for tree nodes.
 */
//------------------------------------------------------------------------------
#include "GmatTreeItemData.hpp"

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatTreeItemData::GmatTreeItemData(wxString desc,
//                                    GmatTree::ItemType dType)
//------------------------------------------------------------------------------
/**
 * Constructs GmatTreeItemData object.
 *
 * @param <desc> input string for description of node.
 * @param <dtype> input int for type of data.
 *
 * @note Creates the GmatTreeItemData object.
 */
//------------------------------------------------------------------------------
GmatTreeItemData::GmatTreeItemData(wxString desc, GmatTree::ItemType type)
{
   this->mDesc = desc;
   this->mItemType = type;
}


//------------------------------------------------------------------------------
// Command* GetCommand()
//------------------------------------------------------------------------------
/**
 * @return Command pointer
 */
//------------------------------------------------------------------------------
GmatCommand* GmatTreeItemData::GetCommand()
{
   return NULL;
}


//------------------------------------------------------------------------------
// wxString GetCommandName()
//------------------------------------------------------------------------------
/**
 * @return command name
 */
//------------------------------------------------------------------------------
wxString GmatTreeItemData::GetCommandName()
{
   return "";
}

