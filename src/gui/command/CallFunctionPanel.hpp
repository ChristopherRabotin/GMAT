//$Header$
//------------------------------------------------------------------------------
//                              CallFunctionPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/9/15
/**
 * This class contains the Matlab Command Setup window.
 */
//------------------------------------------------------------------------------

#ifndef CallFunctionPanel_hpp
#define CallFunctionPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CallFunction.hpp"

class CallFunctionPanel : public GmatPanel
{
public:
   // constructors
   CallFunctionPanel( wxWindow *parent, GmatCommand *cmd);
   ~CallFunctionPanel();

private:
   CallFunction *theCommand;

   wxGrid *inputGrid;
   wxGrid *outputGrid;

   wxArrayString inputStrings;
   wxArrayString outputStrings;

   wxStaticText *nameStaticText;
   wxComboBox *functionComboBox;

   wxTextCtrl *inputTextCtrl;
   wxTextCtrl *outputTextCtrl;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void OnComboChange();
   void OnCellClick(wxGridEvent& event);

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXTCTRL = 44000,
      ID_COMBO,
      ID_BUTTON,
      ID_CHECKBOX,
      ID_GRID,
      MENU_INSERT_P,
      MENU_DELETE_P,
      MENU_CLEAR_P,
      ID_TEXT,
   };
};

#endif // CallFunctionPanel_hpp

