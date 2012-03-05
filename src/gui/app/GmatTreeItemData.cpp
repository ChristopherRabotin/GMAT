//$Id$
//------------------------------------------------------------------------------
//                             GmatTreeItemData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
// GmatTreeItemData(const wxString &name, GmatTree::ItemType type,
//                  const wxString &title, bool isClonable)
//------------------------------------------------------------------------------
/**
 * Constructs GmatTreeItemData object.
 *
 * @param <name>  input string for name of node which is object name
 * @param <type>  input int for type of object.
 * @param <title> input string for title of node.
 * @param <isClonable> input flag indicating that this name and type is clonable
 *
 * @note Creates the GmatTreeItemData object.
 */
//------------------------------------------------------------------------------
GmatTreeItemData::GmatTreeItemData(const wxString &name, GmatTree::ItemType type,
                                   const wxString &title, bool isClonable)
{
   mItemName = name;
   mItemType = type;
   mItemTitle = title;
   mIsClonable = isClonable;
}


//------------------------------------------------------------------------------
// wxString GetName()
//------------------------------------------------------------------------------
wxString GmatTreeItemData::GetName()
{
   return mItemName;
}


//------------------------------------------------------------------------------
// wxString GetTitle()
//------------------------------------------------------------------------------
wxString GmatTreeItemData::GetTitle()
{
   return mItemTitle;
}


//------------------------------------------------------------------------------
// GmatTree::ItemType GetItemType()
//------------------------------------------------------------------------------
GmatTree::ItemType GmatTreeItemData::GetItemType()
{
   return mItemType;
}


//------------------------------------------------------------------------------
// bool IsClonable()
//------------------------------------------------------------------------------
bool GmatTreeItemData::IsClonable()
{
   return mIsClonable;
}


//------------------------------------------------------------------------------
// void SetName(const wxString &name)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetName(const wxString &name)
{
   mItemName = name;
}


//------------------------------------------------------------------------------
// void SetTitle(const wxString &title)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetTitle(const wxString &title)
{
   mItemTitle = title;
}


//------------------------------------------------------------------------------
// void SetItemType(GmatTree::ItemType type)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetItemType(GmatTree::ItemType type)
{
   mItemType = type;
}


//------------------------------------------------------------------------------
// void SetClonable(bool clonable)
//------------------------------------------------------------------------------
void GmatTreeItemData::SetClonable(bool clonable)
{
   mIsClonable = clonable;
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
