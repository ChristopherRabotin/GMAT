//$Header$
//------------------------------------------------------------------------------
//                               AchievePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/12
//
/**
 * This class contains the setup for Target Achive command.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "ParameterSelectDialog.hpp"
#include "AchievePanel.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ACHIEVE_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AchievePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, AchievePanel::OnButtonClick)
   EVT_TEXT(ID_TEXTCTRL, AchievePanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, AchievePanel::OnSolverSelection)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// AchievePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
AchievePanel::AchievePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   mAchieveCommand = (Achieve *)cmd;

   mIsTextModified = false;
   mSolverName = "";
   mGoalName = "";
   mGoalValue = "";
   mTolerance = 1.0e-6;
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~AchievePanel()
//------------------------------------------------------------------------------
AchievePanel::~AchievePanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("BoundarySolver", mSolverComboBox);
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AchievePanel::Create()
{
   int bsize = 2; // bordersize

   // Solver name
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver Name"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   
   mSolverComboBox =
      theGuiManager->GetBoundarySolverComboBox(this, ID_COMBO, wxSize(180,-1));
   
   // Goal
   wxStaticText *goalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Goal"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mGoalNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   mViewGoalButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   // Value
   wxStaticText *initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Value"),
                       wxDefaultPosition, wxDefaultSize, 0);
   mGoalValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                       wxDefaultPosition, wxSize(250,-1), 0);
   mViewGoalValueButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);

   // Tolerence
   wxStaticText *toleranceStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mToleranceTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                       wxDefaultPosition, wxSize(250,-1), 0);   
   mViewToleranceButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   
   // wx*Sizers
   wxFlexGridSizer *goalGridSizer = new wxFlexGridSizer(3, 0, 0);
   
   // Add to wx*Sizers  
   goalGridSizer->Add(solverStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mSolverComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalGridSizer->Add(goalStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mGoalNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mViewGoalButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalGridSizer->Add(initialStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mGoalValueTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mViewGoalValueButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalGridSizer->Add(toleranceStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mToleranceTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   goalGridSizer->Add(mViewToleranceButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // add page sizer to middle sizer
   theMiddleSizer->Add(goalGridSizer, 0, wxALIGN_CENTRE|wxALL, 5);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AchievePanel::LoadData()
{
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("AchievePanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mAchieveCommand->GetTypeName().c_str());
   #endif
   
   // We don't want user to edit this box
   mGoalNameTextCtrl->Disable();
   
   // When Parameter in commands is implemented, we can enable this
   // according to Input Range Testing Table 3.8.
   mViewToleranceButton->Disable();
   
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mAchieveCommand;
      
      std::string solverName = mAchieveCommand->
         GetStringParameter(mAchieveCommand->GetParameterID("TargeterName"));
      
      std::string goalName = mAchieveCommand->
         GetStringParameter(mAchieveCommand->GetParameterID("Goal"));
      
      std::string goalValue = mAchieveCommand->GetStringParameter
         (mAchieveCommand->GetParameterID("GoalValue"));
      
      #if DEBUG_ACHIEVE_PANEL
      MessageInterface::ShowMessage("solverName=%s\n", solverName.c_str());
      MessageInterface::ShowMessage("goalName=%s\n", goalName.c_str());
      MessageInterface::ShowMessage("goalValue=%s\n", goalValue.c_str());
      #endif
      
      mSolverName = wxT(solverName.c_str());      
      mGoalName = wxT(goalName.c_str());
      mGoalValue = wxT(goalValue.c_str());
      
      mTolerance = mAchieveCommand->GetRealParameter
         (mAchieveCommand->GetParameterID("Tolerance"));
      
      mSolverComboBox->SetValue(mSolverName);
      mGoalNameTextCtrl->SetValue(mGoalName);
      mGoalValueTextCtrl->SetValue(mGoalValue);   
      mToleranceTextCtrl->SetValue(theGuiManager->ToWxString(mTolerance));
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AchievePanel::SaveData()
{   
   #if DEBUG_ACHIEVE_PANEL
   MessageInterface::ShowMessage("AchievePanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string inputString;
   Real tol;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (mIsTextModified)
   {
      inputString = mToleranceTextCtrl->GetValue();      
      CheckReal(tol, inputString, "Tolerence", "Real Number > 0.0");
   }
   
   
   Real value;
   inputString = mGoalValue.c_str();
   
   // goal value can ba a number or a prameter name, so validate
   if (!GmatStringUtil::ToReal(inputString, value))
   {
      if (theGuiInterpreter->GetObject(inputString) == NULL)
      {
         CheckReal(value, inputString, "GoalValue",
                   "Real Number or valid Parameter Name", true);
      }
   }
   
   if (!canClose)
      return;
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
   try
   {
      mAchieveCommand->SetStringParameter
         (mAchieveCommand->GetParameterID("TargeterName"), mSolverName.c_str());
      
      mAchieveCommand->SetStringParameter
         (mAchieveCommand->GetParameterID("Goal"), mGoalName.c_str());
      
      mAchieveCommand->SetStringParameter
         (mAchieveCommand->GetParameterID("GoalValue"), mGoalValue.c_str());
      
      if (mIsTextModified)
      {
         mAchieveCommand->SetRealParameter
            (mAchieveCommand->GetParameterID("Tolerance"), tol);
         
         mIsTextModified = false;
      }
      
      EnableUpdate(false);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
   }
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void AchievePanel::OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Sets mIsTextModified to true if text field for Real value is modified.
 */
//------------------------------------------------------------------------------
void AchievePanel::OnTextChange(wxCommandEvent& event)
{
   if (mGoalValueTextCtrl->IsModified())
   {
      mGoalValue = mGoalValueTextCtrl->GetValue();
   }
   
   if (mToleranceTextCtrl->IsModified())
   {
      mIsTextModified = true;
   }
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void AchievePanel::OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AchievePanel::OnSolverSelection(wxCommandEvent &event)
{
   mSolverName = mSolverComboBox->GetValue();
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void AchievePanel::OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void AchievePanel::OnButtonClick(wxCommandEvent& event)
{
   // show dialog to select parameter
   ParameterSelectDialog paramDlg(this, mObjectTypeList);
   paramDlg.ShowModal();
      
   if (paramDlg.IsParamSelected())
   {
      wxString newParamName = paramDlg.GetParamName();
      if (event.GetEventObject() == mViewGoalButton)
      {
         mGoalNameTextCtrl->SetValue(newParamName);
         mGoalName = newParamName;
      }
      else
      {
         mGoalValueTextCtrl->SetValue(newParamName);
         mGoalValue = newParamName;
      }
         
      EnableUpdate(true);
   }
}

