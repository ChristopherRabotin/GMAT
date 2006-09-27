#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "NonLinearConstraintPanel.hpp"
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_NONLINEARCONSTRAINT_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NonLinearConstraintPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, NonLinearConstraintPanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, NonLinearConstraintPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, NonLinearConstraintPanel::OnSolverSelection)
END_EVENT_TABLE()


NonLinearConstraintPanel::NonLinearConstraintPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
//   mNonLinearConstraintCommand = (NonLinearConstraint *)cmd;
   
   solverName = "";
   lhsParam = NULL;
   rhsParam = NULL;
      
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   
   theApplyButton->Disable();
}

NonLinearConstraintPanel::~NonLinearConstraintPanel()
{
   mObjectTypeList.Clear();
}

void NonLinearConstraintPanel::Create()
{
   int bsize = 3; // bordersize

   // wxStaticText
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *lhsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Left Hand Side"), 
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *rhsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Right Hand Side"), 
                       wxDefaultPosition, wxSize(40, -1), 0);
   
   // wxTextCtrl
   mLHSTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   mRHSTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   
   // wxString
   wxString emptyArray[] = 
   {
      wxT("No Solver Available")
   };

   // wxComboBox
   // Only fmincons (and others that handle constraints) should be included!!
   StringArray solverNames;
   solverNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   int solverCount = solverNames.size();
   wxString *solverArray = new wxString[solverCount];

   for (int i=0; i<solverCount; i++)
   {
      solverArray[i] = solverNames[i].c_str();
   }

   if (solverCount > 0)
   {
      mSolverComboBox =
         new wxComboBox(this, ID_COMBO, wxT(solverArray[0]), wxDefaultPosition,
                        wxSize(180,-1), solverCount, solverArray,
                        wxCB_DROPDOWN|wxCB_READONLY);
   }
   else
   {
      mSolverComboBox =
         new wxComboBox(this, ID_COMBO, wxT(emptyArray[0]), wxDefaultPosition,
                        wxSize(180,-1), 1, emptyArray, wxCB_DROPDOWN|wxCB_READONLY);
   }
   
   wxString comparisons[] = { wxT("leq"), wxT("geq"), wxT("eq") };
   mComparisonComboBox =
         new wxComboBox(this, ID_COMBO, wxT(comparisons[0]), wxDefaultPosition,
                        wxSize(180,-1), 3, comparisons,
                        wxCB_DROPDOWN|wxCB_READONLY);
                        
   // wxButton
   mLeftChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Choose"), wxDefaultPosition, wxDefaultSize, 0);
      
   mRightChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Choose"), wxDefaultPosition, wxDefaultSize, 0);

   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *solverSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *lhsSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *lhsInterSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *rhsSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *rhsInterSizer = new wxBoxSizer(wxHORIZONTAL);
   
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
   
   panelSizer->Add(solverSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(lhsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(mComparisonComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(rhsSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}

void NonLinearConstraintPanel::LoadData()
{
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("NonLinearConstraintPanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mNonLinearConstraintCommand->GetTypeName().c_str());
   #endif

   try
   {
/*      // Set the pointer for the "Show Script" button
      mObject = mNonLinearConstraintCommand;
      
      std::string loadedSolverName = mNonLinearConstraintCommand->
         GetStringParameter(mNonLinearConstraintCommand->GetParameterID("TargeterName"));

      std::string loadedVariableName = mNonLinearConstraintCommand->
         GetStringParameter(mNonLinearConstraintCommand->GetParameterID("Variable"));

      #if DEBUG_ACHIEVE_PANEL
      MessageInterface::ShowMessage("solverName=%s\n", loadedSolverName.c_str());
      MessageInterface::ShowMessage("variable=%s\n", loadedVariableName.c_str());
      #endif
      
      solverName = wxT(loadedSolverName.c_str());      
      variableName = wxT(loadedVariableName.c_str());
  */    
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("NonLinearConstraintPanel:LoadData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }

   ShowGoalSetup();
}

void NonLinearConstraintPanel::SaveData()
{   
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("NonLinearConstraintPanel::SaveData() entered\n");
   #endif
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
/*         
   mNonLinearConstraintCommand->SetStringParameter
      (mNonLinearConstraintCommand->GetParameterID("TargeterName"),
       std::string(solverName.c_str()));

   mNonLinearConstraintCommand->SetStringParameter
      (mNonLinearConstraintCommand->GetParameterID("Variable"),
       std::string(variableName.c_str()));
  */ 
   theApplyButton->Disable();
}

void NonLinearConstraintPanel::ShowGoalSetup()
{
   wxString str;
   
   mSolverComboBox->SetStringSelection(solverName);
   mLHSTextCtrl->SetValue(lhs);
   mRHSTextCtrl->SetValue(rhs);
}

void NonLinearConstraintPanel::OnTextChange(wxCommandEvent& event)
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

void NonLinearConstraintPanel::OnSolverSelection(wxCommandEvent &event)
{
   solverName = mSolverComboBox->GetStringSelection();
}

void NonLinearConstraintPanel::OnButtonClick(wxCommandEvent& event)
{
/*   if (event.GetEventObject() == mChooseButton)
   {      
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mChooseButton)
         {
            mVariableTextCtrl->SetValue(newParamName);
            variableName = newParamName;
         }
         
         theApplyButton->Enable(true);
      }
   }
   else
   {*/
      event.Skip();
//   }
}

