//$Header$
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "MinimizePanel.hpp"
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MINIMIZE_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MinimizePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, MinimizePanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, MinimizePanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, MinimizePanel::OnSolverSelection)
END_EVENT_TABLE()


MinimizePanel::MinimizePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mMinimizeCommand = (Minimize *)cmd;
   theGuiManager = GuiItemManager::GetInstance();
   
   solverName = "";
   minParam = NULL;
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   
   EnableUpdate(false);
//   theApplyButton->Disable();
}

MinimizePanel::~MinimizePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
}

void MinimizePanel::Create()
{
   int bsize = 2; // bordersize

   // Optimizer
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Optimizer"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mSolverComboBox = theGuiManager->GetOptimizerComboBox(this, ID_COMBO,
      			wxSize(120,-1));
   
   // Variable to be Minimized
   wxStaticText *variableStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable to be Minimized"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mVariableTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   
   // Choose button
   mChooseButton = new
      wxButton(this, ID_BUTTON, wxT("Choose"), wxDefaultPosition, wxDefaultSize, 0);
 
   wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *solverSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *variableSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *variableInterfaceSizer = new wxBoxSizer(wxHORIZONTAL);
   
   solverSizer->Add(solverStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   solverSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   variableInterfaceSizer->Add(mVariableTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   variableInterfaceSizer->Add(mChooseButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   variableSizer->Add(variableStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   variableSizer->Add(variableInterfaceSizer, 0, wxALIGN_CENTER|wxALL, bsize);

   panelSizer->Add(solverSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(variableSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}

void MinimizePanel::LoadData()
{
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("MinimizePanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mMinimizeCommand->GetTypeName().c_str());
   #endif

   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mMinimizeCommand;
      
      std::string loadedSolverName = mMinimizeCommand->
         GetStringParameter(mMinimizeCommand->GetParameterID("OptimizerName"));

      std::string loadedVariableName = mMinimizeCommand->
         GetStringParameter(mMinimizeCommand->GetParameterID("ObjectiveName"));

      #if DEBUG_ACHIEVE_PANEL
      MessageInterface::ShowMessage("solverName=%s\n", loadedSolverName.c_str());
      MessageInterface::ShowMessage("variable=%s\n", loadedVariableName.c_str());
      #endif
      
      solverName = wxT(loadedSolverName.c_str());      
      variableName = wxT(loadedVariableName.c_str());
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("MinimizePanel:LoadData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }

   ShowGoalSetup();
}

void MinimizePanel::SaveData()
{   
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("MinimizePanel::SaveData() entered\n");
   #endif
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
   
   mMinimizeCommand->SetStringParameter
      (mMinimizeCommand->GetParameterID("OptimizerName"),
       std::string(solverName.c_str()));

   mMinimizeCommand->SetStringParameter
      (mMinimizeCommand->GetParameterID("ObjectiveName"),
       std::string(variableName.c_str()));
   
   EnableUpdate(false);
//   theApplyButton->Disable();
}

void MinimizePanel::ShowGoalSetup()
{
   wxString str;
   
   if (solverName == "")
   {
      mSolverComboBox->SetSelection(0);
      solverName = mSolverComboBox->GetStringSelection();
   }
   else
      mSolverComboBox->SetStringSelection(solverName);
      
   mVariableTextCtrl->SetValue(variableName);
}

void MinimizePanel::OnTextChange(wxCommandEvent& event)
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

void MinimizePanel::OnSolverSelection(wxCommandEvent &event)
{
   solverName = mSolverComboBox->GetStringSelection();
   EnableUpdate(true);
}

void MinimizePanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mChooseButton)
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
         
//         theApplyButton->Enable(true);
           EnableUpdate(true);
      }
   }
   else
   {
      event.Skip();
   }
}

