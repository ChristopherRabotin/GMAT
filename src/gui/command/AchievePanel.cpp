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
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "AchievePanel.hpp"
#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

#define DEBUG_ACHIEVE_PANEL 1

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
   
   mSolverData.solverName = "";
   mSolverData.goalName = "";
   mSolverData.goalValue = "";
   //mSolverData.goalValue = 0.0; //loj: 2/4/05 Made goalValue wxString
   mSolverData.tolerance = 1.0e-6;
   mSolverData.goalParam = NULL;
   
   Create();
   Show();
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// ~AchievePanel()
//------------------------------------------------------------------------------
AchievePanel::~AchievePanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AchievePanel::Create()
{
   int bsize = 3; // bordersize

   // wxStaticText
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *goalStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Goal"), 
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Goal Value"), //loj: 2/4/05 Changed from initial value
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *toleranceStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mGoalNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(150,-1), 0);
   mGoalValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                       wxDefaultPosition, wxSize(150,-1), 0);
   mToleranceTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                       wxDefaultPosition, wxSize(80,-1), 0);
   
   // wxString
   wxString emptyArray[] = 
   {
      wxT("No Solver Available")
   };

   // wxComboBox
   StringArray solverNames;
   solverNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SOLVER);
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
   mViewGoalButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   mViewGoalValueButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   // wx*Sizers
   wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
   wxStaticBox *goalSetupStaticBox = new wxStaticBox(this, -1, wxT("Goal Setup"));
   wxStaticBoxSizer *goalSetupSizer = new wxStaticBoxSizer(goalSetupStaticBox, wxVERTICAL);
   wxFlexGridSizer *valueGridSizer = new wxFlexGridSizer(4, 0, 0);
   wxBoxSizer *solverBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *goalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   // Add to wx*Sizers  
   solverBoxSizer->Add(solverStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   solverBoxSizer->Add(mSolverComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   
   goalBoxSizer->Add(goalStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   goalBoxSizer->Add(mGoalNameTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   goalBoxSizer->Add(mViewGoalButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   valueGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(initialStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(toleranceStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   valueGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mGoalValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mViewGoalValueButton, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mToleranceTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   goalSetupSizer->Add(goalBoxSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   goalSetupSizer->Add(valueGridSizer, 0, wxALIGN_CENTER|wxALL, bsize);

   panelSizer->Add(solverBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(goalSetupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
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

   mGoalNameTextCtrl->Disable(); // we don't want user to edit this box
   
   try
   {
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
      
      mSolverData.solverName = wxT(solverName.c_str());      
      mSolverData.goalName = wxT(goalName.c_str());
      mSolverData.goalValue = wxT(goalValue.c_str());
      
      mSolverData.tolerance = mAchieveCommand->GetRealParameter
         (mAchieveCommand->GetParameterID("Tolerance"));
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("AchievePanel:LoadData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }

   ShowGoalSetup();
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
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
         
   mAchieveCommand->SetStringParameter
      (mAchieveCommand->GetParameterID("TargeterName"),
       std::string(mSolverData.solverName.c_str()));

   mAchieveCommand->SetStringParameter
      (mAchieveCommand->GetParameterID("Goal"),
       std::string(mSolverData.goalName.c_str()));

   // goal value can ba a number or a prameter name, so validate
   Real value;
   if (!mSolverData.goalValue.ToDouble(&value))
   {
      if (theGuiInterpreter->
          GetParameter(std::string(mSolverData.goalValue.c_str())) == NULL)
      {
         wxLogError("The goal value is not a number or a valid parameter name");
         canClose = false;
      }
   }
   
   if (canClose)
   {
      mAchieveCommand->SetStringParameter
         (mAchieveCommand->GetParameterID("GoalValue"),
          std::string(mSolverData.goalValue.c_str()));
   }
   
   mAchieveCommand->SetRealParameter
      (mAchieveCommand->GetParameterID("Tolerance"),
       mSolverData.tolerance);
   
   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void ShowGoalSetup()
//------------------------------------------------------------------------------
void AchievePanel::ShowGoalSetup()
{
   wxString str;
   
   mSolverComboBox->SetStringSelection(mSolverData.solverName);
   mGoalNameTextCtrl->SetValue(mSolverData.goalName);

   mGoalValueTextCtrl->SetValue(mSolverData.goalValue);
   
   str.Printf("%g", mSolverData.tolerance);
   mToleranceTextCtrl->SetValue(str);
   
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void AchievePanel::OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void AchievePanel::OnTextChange(wxCommandEvent& event)
{
   if (mGoalValueTextCtrl->IsModified())
   {
//        // if some character entered bring up ParameterSelectDialog
//        Real value;
//        if (mGoalValueTextCtrl->GetValue() != "" &&
//            !mGoalValueTextCtrl->GetValue().ToDouble(&value))
//        {
//           mGoalValueTextCtrl->SetValue(mSolverData.goalValue);
//           ParameterSelectDialog paramDlg(this);
//           paramDlg.ShowModal();

//           if (paramDlg.IsParamSelected())
//           {
//              wxString newParamName = paramDlg.GetParamName();
//              mGoalValueTextCtrl->SetValue(newParamName);
//              mSolverData.goalValue = newParamName;
//           }
//        }
//        else
//        {
         mSolverData.goalValue = mGoalValueTextCtrl->GetValue();
//        }
   }
   
   if (mToleranceTextCtrl->IsModified())
      mSolverData.tolerance = atof(mToleranceTextCtrl->GetValue().c_str());

   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void AchievePanel::OnSolverSelection()
//------------------------------------------------------------------------------
void AchievePanel::OnSolverSelection()
{
   mSolverData.solverName = mSolverComboBox->GetStringSelection();
}

//------------------------------------------------------------------------------
// void AchievePanel::OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void AchievePanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewGoalButton ||
       event.GetEventObject() == mViewGoalValueButton)
   {       
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this);
      paramDlg.ShowModal();

      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         if (event.GetEventObject() == mViewGoalButton)
         {
            mGoalNameTextCtrl->SetValue(newParamName);
            mSolverData.goalName = newParamName;
         }
         else
         {
            mGoalValueTextCtrl->SetValue(newParamName);
            mSolverData.goalValue = newParamName;
         }
         
         theApplyButton->Enable(true);
      }
   }
   else
   {
      event.Skip();
   }
}

