//$Header$
//------------------------------------------------------------------------------
//                              MinimizePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2006/09/20
/**
 * This class contains the Minimize command setup window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "MinimizePanel.hpp"
#include "Array.hpp"
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

//------------------------------------------------------------------------------
// MinimizePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
MinimizePanel::MinimizePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mMinimizeCommand = (Minimize *)cmd;
   theGuiManager = GuiItemManager::GetInstance();
   
   solverName = "";
   minParam = NULL;
   mVarNameChanged = false;
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~MinimizePanel()
//------------------------------------------------------------------------------
MinimizePanel::~MinimizePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Optimizer", mSolverComboBox);
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void MinimizePanel::LoadData()
{
   #if DEBUG_MINIMIZE_PANEL
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

      #if DEBUG_MINIMIZE_PANEL
      MessageInterface::ShowMessage("solverName=%s\n", loadedSolverName.c_str());
      MessageInterface::ShowMessage("variable=%s\n", loadedVariableName.c_str());
      #endif
      
      solverName = wxT(loadedSolverName.c_str());      
      variableName = wxT(loadedVariableName.c_str());
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
   }
   
   ShowGoalSetup();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void MinimizePanel::SaveData()
{   
   #if DEBUG_MINIMIZE_PANEL
   MessageInterface::ShowMessage("MinimizePanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check input value - Variable, Array element, Spacecraft Parameter
   //-----------------------------------------------------------------
   
   if (mVarNameChanged)
   {
      std::string varName = variableName.c_str();
      
      int retval = theGuiManager->IsValidVariable(varName, Gmat::SPACECRAFT);
      
      if (retval == -1)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The variable \"" + varName + "\" does not exist.\n"
             "Press \"Choose\" or create from the resource tree.");
         
         canClose = false;
      }
      else if (retval == 0)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, mMsgFormat.c_str(),
             variableName.c_str(), "Variable to be Minimized",
             "Variable, Array element, Spacecraft parameter");
         
         canClose = false;
      }
      else
      {
         mVarNameChanged = false;
      }      
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      mMinimizeCommand->SetStringParameter
         (mMinimizeCommand->GetParameterID("OptimizerName"),
          std::string(solverName.c_str()));
      
      mMinimizeCommand->SetStringParameter
         (mMinimizeCommand->GetParameterID("ObjectiveName"),
          std::string(variableName.c_str()));
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void ShowGoalSetup()
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MinimizePanel::OnTextChange(wxCommandEvent& event)
{
   if (mVariableTextCtrl->IsModified())
   {
      variableName = mVariableTextCtrl->GetValue();
      mVarNameChanged = true;
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void MinimizePanel::OnSolverSelection(wxCommandEvent &event)
{
   solverName = mSolverComboBox->GetStringSelection();
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
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
         
         EnableUpdate(true);
      }
   }
}

