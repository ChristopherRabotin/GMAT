//$Id$
//------------------------------------------------------------------------------
//                                 AchievePanel
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
 * This class contains setup for Target Achieve command.
 */
//------------------------------------------------------------------------------

#ifndef AchievePanel_hpp
#define AchievePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Achieve.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

class AchievePanel : public GmatPanel
{
public:
   AchievePanel(wxWindow *parent, GmatCommand *cmd);
   ~AchievePanel(); 
    
private:             

   Achieve *mAchieveCommand;
   
   bool mIsTextModified;
   
   //Real      mTolerance;
   wxString  mTolerance;
   wxString  mSolverName;
   wxString  mGoalName;
   wxString  mGoalValue;
   Parameter *mGoalParam;
   
   wxTextCtrl *mGoalNameTextCtrl;
   wxTextCtrl *mGoalValueTextCtrl;
   wxTextCtrl *mToleranceTextCtrl;
   
   wxButton *mViewGoalButton;
   wxButton *mViewGoalValueButton;
   wxButton *mViewToleranceButton;
   
   wxComboBox *mSolverComboBox;
   
   wxArrayString mObjectTypeList;
   
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
   };
};

#endif // SolverGoalsPanel_hpp

