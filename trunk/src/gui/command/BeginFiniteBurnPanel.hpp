//$Header$
//------------------------------------------------------------------------------
//                              BeginFiniteBurnPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
// Author: Linda Jun
// Created: 2006/07/14
//
/**
 * This class contains the BeginFiniteBurn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef BeginFiniteBurnPanel_hpp
#define BeginFiniteBurnPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include "GuiItemManager.hpp"

class BeginFiniteBurnPanel : public GmatPanel
{
public:
   // constructors
   BeginFiniteBurnPanel(wxWindow *parent, GmatCommand *cmd);
   ~BeginFiniteBurnPanel();
   
protected:
   // member data
   GmatCommand *theCommand;
   
   wxComboBox *mFiniteBurnComboBox;
   wxCheckListBox *mSatCheckListBox;
   
   // member functions
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckListBoxChange(wxCommandEvent& event);
   
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
      
      ID_COMBOBOX,
      ID_CHECKLISTBOX
   };
};

#endif // BeginFiniteBurnPanel_hpp
