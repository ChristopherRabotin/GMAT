//$Header$
//------------------------------------------------------------------------------
//                           SolverEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Event window.
 */
//------------------------------------------------------------------------------

#ifndef SolverEventPanel_hpp
#define SolverEventPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "GmatPanel.hpp"

class SolverEventPanel : public GmatPanel
{
public:
   // constructor
   SolverEventPanel(wxWindow *parent, GmatCommand *cmd);
   ~SolverEventPanel();   
   
private:   
   wxStaticText *item6;
   wxStaticText *item8;
   wxStaticText *item10;
   
   wxButton *correctionsButton;
   
   wxComboBox *item7;
   wxComboBox *item9;
   wxComboBox *item11;
    
   GuiInterpreter *theGuiInterpreter;
   GmatCommand *theCommand;
    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   void Setup(wxWindow *parent);
   void Initialize();
   
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

