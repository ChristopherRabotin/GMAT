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
   
   solverName = "";
   minParam = NULL;
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   
   theApplyButton->Disable();
}

MinimizePanel::~MinimizePanel()
{
   mObjectTypeList.Clear();
}

void MinimizePanel::Create()
{
   int bsize = 3; // bordersize

   // wxStaticText
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *variableStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable to be Minimized"), 
                       wxDefaultPosition, wxSize(40, -1), 0);
   
   // wxTextCtrl
   mVariableTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   
   // wxString
   wxString emptyArray[] = 
   {
      wxT("No Solver Available")
   };

   // wxComboBox
   // Only Optimizers should be included!!
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
   
   // wxButton
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
         GetStringParameter(mMinimizeCommand->GetParameterID("TargeterName"));

      std::string loadedVariableName = mMinimizeCommand->
         GetStringParameter(mMinimizeCommand->GetParameterID("Variable"));

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
      (mMinimizeCommand->GetParameterID("TargeterName"),
       std::string(solverName.c_str()));

   mMinimizeCommand->SetStringParameter
      (mMinimizeCommand->GetParameterID("Variable"),
       std::string(variableName.c_str()));
   
   theApplyButton->Disable();
}

void MinimizePanel::ShowGoalSetup()
{
   wxString str;
   
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
         
         theApplyButton->Enable(true);
      }
   }
   else
   {
      event.Skip();
   }
}

