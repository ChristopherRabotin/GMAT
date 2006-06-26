//$Header$
//------------------------------------------------------------------------------
//                                    VaryPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/12
//
/**
 * This class contains the setup for Target Vary command.
 */
//------------------------------------------------------------------------------

#ifndef VaryPanel_hpp
#define VaryPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Vary.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

class VaryPanel : public GmatPanel
{
public:
   VaryPanel(wxWindow *parent, GmatCommand *cmd);
   ~VaryPanel(); 
    
private:             
   
   struct SolverType
   {
      wxString  solverName;
      wxString  varName;
      Real      initialValue;
      Real      pert;
      Real      minValue;
      Real      maxValue;
      Real      maxStep;
      Parameter *varParam;
   };
   
   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mInitialTextCtrl;
   wxTextCtrl *mPertTextCtrl;
   wxTextCtrl *mMaxStepTextCtrl;
   wxTextCtrl *mMinValueTextCtrl;
   wxTextCtrl *mMaxValueTextCtrl;
   
   wxButton *mViewVarButton;
   wxComboBox *mSolverComboBox;

   wxArrayString mObjectTypeList;
   SolverType mSolverData;
   Vary *mVaryCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void ShowVariableSetup();
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);    
   void OnSolverSelection(wxCommandEvent &event);
   void OnButton(wxCommandEvent& event);
   
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

#endif // SolverGoalsPanel_hpp

