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
//                                   int dType)
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
GmatTreeItemData::GmatTreeItemData(wxString desc,
                                   int dType)
{
   this->m_desc = desc;
   this->dataType = dType;
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
// int GetDataType()
//------------------------------------------------------------------------------
/**
 * Gets the data type of the node.
 *
 * @return int corresponding to the type of data.
 */
//------------------------------------------------------------------------------
int GmatTreeItemData::GetDataType()
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

