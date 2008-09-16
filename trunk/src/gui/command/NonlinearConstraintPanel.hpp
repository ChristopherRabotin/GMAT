#ifndef NONLINEARCONSTRAINTPANEL_HPP_
#define NONLINEARCONSTRAINTPANEL_HPP_

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "NonlinearConstraint.hpp"

class NonlinearConstraintPanel : public GmatPanel
{
public:
   NonlinearConstraintPanel(wxWindow *parent, GmatCommand *cmd);
   ~NonlinearConstraintPanel(); 
    
private:  
   GuiItemManager *theGuiManager;
           
   wxTextCtrl *mLHSTextCtrl;
   wxTextCtrl *mRHSTextCtrl;
   wxTextCtrl *mTolTextCtrl;

   wxButton *mLeftChooseButton;
   wxButton *mRightChooseButton;

   wxComboBox *mSolverComboBox;
   wxComboBox *mComparisonComboBox;
   
   wxArrayString mObjectTypeList;
   NonlinearConstraint *mNonlinearConstraintCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
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

#endif /*NONLINEARCONSTRAINTPANEL_HPP_*/
