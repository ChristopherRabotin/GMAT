//$Id$
//------------------------------------------------------------------------------
//                             MissionTreeItemData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/02/06
/**
 * This class provides a data type for tree nodes.
 */
//------------------------------------------------------------------------------
#ifndef MissionTreeItemData_hpp
#define MissionTreeItemData_hpp

#include "gmatwxdefs.hpp"
#include "GmatTreeItemData.hpp"

#include "GmatCommand.hpp"

class MissionTreeItemData : public GmatTreeItemData
{
public:
   MissionTreeItemData(const wxString &name, GmatTree::ItemType type,
                       const wxString &title = "", GmatCommand *cmd = NULL);
   
   virtual GmatCommand* GetCommand();
   virtual void SetCommand(GmatCommand *cmd);
   virtual wxTreeItemId GetNodeId();
   virtual void SetNodeId(const wxTreeItemId &nodeId);
   
protected:
   GmatCommand *theCommand;
   wxTreeItemId theNodeId;
};
#endif // MissionTreeItemData_hpp
