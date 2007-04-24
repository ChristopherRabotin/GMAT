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
   this->m_desc = desc;
   this->dataType = type;
}


//------------------------------------------------------------------------------
// wxString GetDesc()
//------------------------------------------------------------------------------
/**
 * Gets the description of note.
 *
 * @return String description.
 */
//------------------------------------------------------------------------------
wxString GmatTreeItemData::GetDesc()
{
   return m_desc;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetDataType()
//------------------------------------------------------------------------------
/**
 * Gets the data type of the node.
 *
 * @return int corresponding to the type of data.
 */
//------------------------------------------------------------------------------
GmatTree::ItemType GmatTreeItemData::GetDataType()
{
   return dataType;
}


//------------------------------------------------------------------------------
// int SetDesc(wxString description)
//------------------------------------------------------------------------------
/**
 * Set the description of the node.
 *
 * @param <description> input string for description of node.
 */
//------------------------------------------------------------------------------
void GmatTreeItemData::SetDesc(wxString description)
{
   m_desc = description;
}


//------------------------------------------------------------------------------
// int SetDataType(GmatTree::ItemType type)
//------------------------------------------------------------------------------
/**
 * Set the data type of the node.
 *
 * @param <type> input int for data type of node.
 */
//------------------------------------------------------------------------------
void GmatTreeItemData::SetDataType(GmatTree::ItemType type)
{
   dataType = type;
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

