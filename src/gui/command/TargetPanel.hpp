//$Header$
//------------------------------------------------------------------------------
//                           TargetPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
// Modified:
//    2004/05/06 Allison Greene - to inherit from GmatPanel
//    2004/11/04 Linda Jun - Renamed from SolverEventPanel
/**
 * This class contains the Target setup window.
 */
//------------------------------------------------------------------------------
#ifndef TargetPanel_hpp
#define TargetPanel_hpp

#include "gmatwxdefs.hpp"
#include "Command.hpp"
#include "GmatPanel.hpp"

class TargetPanel : public GmatPanel
{
public:
   // constructor
   TargetPanel(wxWindow *parent, GmatCommand *cmd);
   ~TargetPanel();   
   
private:   
   
   wxButton *correctionsButton;
   
   wxComboBox *mSolverComboBox;
   wxComboBox *mConvComboBox;
   wxComboBox *modeComboBox;
   
   GmatCommand *theCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Text control event method
   void OnTextUpdate(wxCommandEvent& event);
   
   // Combobox event method
   void OnComboSelection(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 51000,
      ID_TEXTCTRL,
      ID_COMBO,
      ID_BUTTON,
   };
};

#endif // SolverEvent_hpp

