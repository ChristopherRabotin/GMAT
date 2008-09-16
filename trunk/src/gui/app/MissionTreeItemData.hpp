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
   MissionTreeItemData(const wxString desc, GmatTree::ItemType type,
                       const wxString &name = "", GmatCommand *cmd = NULL);
   virtual GmatCommand* GetCommand();
   virtual void SetCommand(GmatCommand *cmd);
   virtual wxString GetCommandName();
    
protected:
private:
   GmatCommand *theCommand;
   wxString theCommandName;
};
#endif // MissionTreeItemData_hpp
