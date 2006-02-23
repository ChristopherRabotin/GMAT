//$Header$
//------------------------------------------------------------------------------
//                              TogglePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/20
//
/**
 * This class contains the Toggle setup window.
 */
//------------------------------------------------------------------------------

#ifndef TogglePanel_hpp
#define TogglePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"

class TogglePanel : public GmatPanel
{
public:
   // constructors
   TogglePanel(wxWindow *parent, GmatCommand *cmd);
   
protected:
   // member data
   GmatCommand *theCommand;
   
   wxRadioButton *mOnRadioButton;
   wxRadioButton *mOffRadioButton;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnRadioButtonChange(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,        
      ID_RADIOBUTTON
   };
};

#endif // TogglePanel_hpp
