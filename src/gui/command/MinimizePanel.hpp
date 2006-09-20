#ifndef MINIMIZEPANEL_HPP_
#define MINIMIZEPANEL_HPP_

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Minimize.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

class MinimizePanel : public GmatPanel
{
public:
   MinimizePanel(wxWindow *parent, GmatCommand *cmd);
   ~MinimizePanel(); 
    
private:             

   wxString  solverName;
   wxString variableName;
   Parameter *minParam;
   
   wxTextCtrl *mVariableTextCtrl;
   
   wxButton *mChooseButton;
   
   wxComboBox *mSolverComboBox;
   
   wxArrayString mObjectTypeList;
   Minimize *mMinimizeCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void ShowGoalSetup();
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);    
   void OnSolverSelection(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
  
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 53000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBO,
      ID_GRID,
   };
};

#endif /*MINIMIZEPANEL_HPP_*/

