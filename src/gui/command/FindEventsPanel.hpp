//$Id$
//------------------------------------------------------------------------------
//                              FindEventsPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2015.03/27
//
/**
 * This class contains the FindEvents Setup window.
 */
//------------------------------------------------------------------------------

#ifndef FindEventsPanel_hpp
#define FindEventsPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiItemManager.hpp"

class FindEventsPanel : public GmatPanel
{
public:
   // constructors
   FindEventsPanel(wxWindow *parent, GmatCommand *cmd);
   ~FindEventsPanel();

protected:
   // member data
   GmatCommand *theCommand;

   wxComboBox *locatorCB;
   wxCheckBox *appendCheckBox;

   // member functions
   void OnLocatorComboBoxChange(wxCommandEvent& event);
   void OnAppendCheckBoxChange(wxCommandEvent &event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 80000,
      ID_LOCATOR_COMBOBOX,
      ID_APPEND_CHECKBOX
   };
};

#endif // FindEventsPanel_hpp
