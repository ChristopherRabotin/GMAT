//$Id$
//------------------------------------------------------------------------------
//                              ManageObjectPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/10/14
//
/**
 * This class contains the Save Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ManageObjectPanel_hpp
#define ManageObjectPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiItemManager.hpp"

class ManageObjectPanel : public GmatPanel
{
public:
   ManageObjectPanel(wxWindow *parent, GmatCommand *cmd);
   ~ManageObjectPanel();
   
protected:
   GmatCommand *theCommand;

   wxCheckListBox *mObjectCheckListBox;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // event handling
   void OnCheckListBoxChange(wxCommandEvent& event);

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,        
      ID_CHECKLISTBOX
   };
};

#endif // ManageObjectPanel_hpp
