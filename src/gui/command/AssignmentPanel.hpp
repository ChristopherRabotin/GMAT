//$Header$
//------------------------------------------------------------------------------
//                              AssignmentPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/12/23
/**
 * This class contains the AssignmentPanel Setup window.
 */
//------------------------------------------------------------------------------

#ifndef AssignmentPanel_hpp
#define AssignmentPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CallFunction.hpp"

class AssignmentPanel : public GmatPanel
{
public:
   // constructors
   AssignmentPanel( wxWindow *parent, GmatCommand *cmd);
   ~AssignmentPanel();

private:
   CallFunction *theCommand;

   wxGrid *inputGrid;
   wxGrid *outputGrid;

   wxArrayString inputStrings;
   wxArrayString outputStrings;

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

#endif // AssignmentPanel_hpp

