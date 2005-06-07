//$Header$
//------------------------------------------------------------------------------
//                              SavePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/14
//
/**
 * This class contains the Save Setup window.
 */
//------------------------------------------------------------------------------

#ifndef SavePanel_hpp
#define SavePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "Command.hpp"
#include "GuiItemManager.hpp"

class SavePanel : public GmatPanel
{
public:
   SavePanel(wxWindow *parent, GmatCommand *cmd);
   ~SavePanel();
   
protected:
   GmatCommand *theCommand;

   wxComboBox *mObjectComboBox;
    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // event handling
   void OnComboBoxChange(wxCommandEvent& event);

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,        
      ID_COMBOBOX
   };
};

#endif // SavePanel_hpp
