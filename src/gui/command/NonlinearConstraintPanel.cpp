#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "NonlinearConstraintPanel.hpp"
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_NONLINEARCONSTRAINT_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NonlinearConstraintPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, NonlinearConstraintPanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, NonlinearConstraintPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, NonlinearConstraintPanel::OnSolverSelection)
END_EVENT_TABLE()


NonlinearConstraintPanel::NonlinearConstraintPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mNonlinearConstraintCommand = (NonlinearConstraint *)cmd;      
   mObjectTypeList.Add("Spacecraft");
   theGuiManager = GuiItemManager::GetInstance();
   
   Create();
   Show();
   
   theApplyButton->Disable();
}

NonlinearConstraintPanel::~NonlinearConstraintPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
}

void NonlinearConstraintPanel::Create()
{
   int bsize = 2; // bordersize

   // wxStaticText
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(70, -1), 0);
   wxStaticText *lhsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Left Hand Side"), 
                       wxDefaultPosition, wxSize(80, -1), 0);
   wxStaticText *rhsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Right Hand Side"), 
                       wxDefaultPosition, wxSize(80, -1), 0);
   wxStaticText *blankStaticText =
      new wxStaticText(this, ID_TEXT, wxT(""), 
                       wxDefaultPosition, wxSize(60, -1), 0);
   
   // wxTextCtrl
   mLHSTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(140,-1), 0);
   mRHSTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(140,-1), 0);
   
   mSolverComboBox = theGuiManager->GetOptimizerComboBox(this, ID_COMBO,
      			wxSize(120,-1));

   wxString comparisons[] = { wxT("<="), wxT(">="), wxT("==") };
   mComparisonComboBox =
         new wxComboBox(this, ID_COMBO, wxT(comparisons[0]), wxDefaultPosition,
                        wxSize(45,-1), 3, comparisons,
                        wxCB_DROPDOWN|wxCB_READONLY);
                        
   // wxButton
   mLeftChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Choose"), wxDefaultPosition, wxSize(50,-1), 0);
      
   mRightChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Choose"), wxDefaultPosition, wxSize(50,-1), 0);

   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *solverSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *lhsSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *lhsInterSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *rhsSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *rhsInterSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *conditionSizer = new wxBoxSizer(wxVERTICAL);
   
   solverSizer->Add(solverStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   solverSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   lhsInterSizer->Add(mLHSTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   lhsInterSizer->Add(mLeftChooseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   lhsSizer->Add(lhsStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   lhsSizer->Add(lhsInterSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   rhsInterSizer->Add(mRHSTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   rhsInterSizer->Add(mRightChooseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   rhsSizer->Add(rhsStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   rhsSizer->Add(rhsInterSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   conditionSizer->Add(blankStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   conditionSizer->Add(mComparisonComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   panelSizer->Add(solverSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(lhsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(conditionSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(rhsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}

void NonlinearConstraintPanel::LoadData()
{
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("NonlinearConstraintPanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mNonlinearConstraintCommand->GetTypeName().c_str());
   #endif

   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mNonlinearConstraintCommand;
      
      std::string loadedSolverName = mNonlinearConstraintCommand->
         GetStringParameter(mNonlinearConstraintCommand->GetParameterID("OptimizerName"));
      
      mSolverComboBox->SetStringSelection(wxT(loadedSolverName.c_str()));

//      std::string loadedVariableName = mNonlinearConstraintCommand->
//         GetStringParameter(mNonlinearConstraintCommand->GetParameterID("ConstrainedVariableName"));
//
//      mLHSTextCtrl->SetValue(wxT(loadedVariableName.c_str()));

//      Real loadedValue = mNonlinearConstraintCommand->
//         GetRealParameter(mNonlinearConstraintCommand->GetParameterID("ConstrainedVariableName"));
//
//      mLHSTextCtrl->SetValue(wxT(loadedValue));
     
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("NonlinearConstraintPanel:LoadData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }

}

void NonlinearConstraintPanel::SaveData()
{   
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("NonlinearConstraintPanel::SaveData() entered\n");
   #endif
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
      
   mNonlinearConstraintCommand->SetStringParameter
      (mNonlinearConstraintCommand->GetParameterID("OptimizerName"),
       mSolverComboBox->GetValue().c_str());

//   mNonlinearConstraintCommand->SetStringParameter
//      (mNonlinearConstraintCommand->GetParameterID("ConstrainedVariableName"),
//       mLHSTextCtrl->GetValue().c_str());

//   mNonlinearConstraintCommand->SetRealParameter
//      (mNonlinearConstraintCommand->GetParameterID("ConstrainedValue"),
//       mRHSTextCtrl->GetValue().ToDouble());
   
   theApplyButton->Disable();
}

void NonlinearConstraintPanel::OnTextChange(wxCommandEvent& event)
{
        /* from AchievePanel
   if (mGoalValueTextCtrl->IsModified())
   {
      mSolverData.goalValue = mGoalValueTextCtrl->GetValue();
   }
   
   if (mToleranceTextCtrl->IsModified())
      mSolverData.tolerance = atof(mToleranceTextCtrl->GetValue().c_str());
*/
   EnableUpdate(true);
}

void NonlinearConstraintPanel::OnSolverSelection(wxCommandEvent &event)
{
   theApplyButton->Enable();
}

void NonlinearConstraintPanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mLeftChooseButton)
   {      
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mLeftChooseButton)
         {
            mLHSTextCtrl->SetValue(newParamName);
//            variableName = newParamName;
         }
         
         theApplyButton->Enable(true);
      }
   }
   else if (event.GetEventObject() == mRightChooseButton)
   {      
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
                   //GuiItemManager::SHOW_PLOTTABLE, true, true);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mRightChooseButton)
         {
            mRHSTextCtrl->SetValue(newParamName);
//            variableName = newParamName;
         }
         
         theApplyButton->Enable(true);
      }
   }
   else
   {
      event.Skip();
   }
}

