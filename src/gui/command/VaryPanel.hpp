//$Id$
//------------------------------------------------------------------------------
//                                    VaryPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
   VaryPanel(wxWindow *parent, GmatCommand *cmd, bool inOptimize = false);
   ~VaryPanel(); 
   
protected:
   Vary *mVaryCommand;
   std::string solverName;
   std::string variableName;
   bool        inOptimizeCmd;
   bool        solverChanged;
   bool        variableChanged;
   
   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mInitialTextCtrl;
   wxTextCtrl *mPertTextCtrl;
   wxTextCtrl *mMaxStepTextCtrl;
   wxTextCtrl *mLowerValueTextCtrl;
   wxTextCtrl *mUpperValueTextCtrl;
   wxTextCtrl *mAdditiveTextCtrl;
   wxTextCtrl *mMultiplicativeTextCtrl;

   wxStaticText *lowerValueStaticText;
   wxStaticText *upperValueStaticText;
   wxStaticText *pertStaticText;
   wxStaticText *maxStepStaticText;
   wxStaticText *additiveStaticText;
   wxStaticText *multiplicativeStaticText;
   
   wxButton *mViewVarButton;
   wxComboBox *mSolverComboBox;
   
   wxArrayString mObjectTypeList;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);    
   void OnSolverSelection(wxCommandEvent &event);
   void OnButton(wxCommandEvent& event);
   
   void SetControlEnabling(GmatBase *slvr);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 53000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBO,
   };
};

#endif // SolverGoalsPanel_hpp

