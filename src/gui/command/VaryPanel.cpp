//$Header$
//------------------------------------------------------------------------------
//                                   VaryPanel
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

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "ParameterSelectDialog.hpp"
#include "VaryPanel.hpp"
#include "Solver.hpp"
#include "GmatBaseException.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_VARY_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(VaryPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON, VaryPanel::OnButton)
   EVT_TEXT(ID_TEXTCTRL, VaryPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBO, VaryPanel::OnSolverSelection)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// VaryPanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
VaryPanel::VaryPanel(wxWindow *parent, GmatCommand *cmd, bool inOptimize)
   : GmatPanel(parent)
{
   mVaryCommand = (Vary *)cmd;
   inOptimizeCmd = inOptimize;
   
   mSolverData.solverName = "";
   mSolverData.varName = "";
   mSolverData.initialValue = 0.0;
   mSolverData.pert = 0.0;
   mSolverData.minValue = -9.999e30;
   mSolverData.maxValue = 9.999e30;
   mSolverData.maxStep = 9.999e30;
   mSolverData.additiveScaleFactor = 0.0;
   mSolverData.multiplicativeScaleFactor = 0.0;
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// ~VaryPanel()
//------------------------------------------------------------------------------
VaryPanel::~VaryPanel()
{
   mObjectTypeList.Clear();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void VaryPanel::Create()
{
   int bsize = 3; // bordersize


   // wxStaticText
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *varStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable"), 
                       wxDefaultPosition, wxSize(40, -1), 0);
   wxStaticText *initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Initial Value"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   pertStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Perturbation"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   maxStepStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Max Step"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *minValueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Lower"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *maxValueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Upper"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   additiveStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Additive Scale Factor"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   multiplicativeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Multiplicative Scale Factor"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxTextCtrl
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   mInitialTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(80,-1), 0);
   mPertTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                  wxDefaultPosition, wxSize(80,-1), 0);
   mMaxStepTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(80,-1), 0);
   mMinValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(80,-1), 0);
   mMaxValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(80,-1), 0);
   mAdditiveTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(80,-1), 0);
   mMultiplicativeTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(80,-1), 0);
   
   // wxString
   wxString emptyArray[] = 
   {
      wxT("No Solver Available")
   };

   // wxComboBox
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
   mViewVarButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   // wx*Sizers
   wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
   wxStaticBox *varSetupStaticBox = new wxStaticBox(this, -1, wxT("Variable Setup"));
   wxStaticBoxSizer *varSetupSizer = new wxStaticBoxSizer(varSetupStaticBox, wxVERTICAL);
   wxFlexGridSizer *valueGridSizer = new wxFlexGridSizer(6, 0, 0);
   wxBoxSizer *solverBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *variableBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxFlexGridSizer *scaleGridSizer = new wxFlexGridSizer(2, 0, 0);
   
   // Add to wx*Sizers
   
   solverBoxSizer->Add(solverStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   solverBoxSizer->Add(mSolverComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   variableBoxSizer->Add(varStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   variableBoxSizer->Add(mVarNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   variableBoxSizer->Add(mViewVarButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   valueGridSizer->Add(40, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   valueGridSizer->Add(initialStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(pertStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(minValueStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(maxValueStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(maxStepStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   
   valueGridSizer->Add(40, 20, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mInitialTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mPertTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mMinValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mMaxValueTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   valueGridSizer->Add(mMaxStepTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);

   scaleGridSizer->Add(additiveStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   scaleGridSizer->Add(mAdditiveTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   scaleGridSizer->Add(multiplicativeStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   scaleGridSizer->Add(mMultiplicativeTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);

   varSetupSizer->Add(variableBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   varSetupSizer->Add(valueGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   panelSizer->Add(solverBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(varSetupSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   panelSizer->Add(scaleGridSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void VaryPanel::LoadData()
{
   #if DEBUG_VARY_PANEL
   MessageInterface::ShowMessage("VaryPanel::LoadData() entered\n");
   MessageInterface::ShowMessage("Command=%s\n", mVaryCommand->GetTypeName().c_str());
   #endif

   mVarNameTextCtrl->Disable(); // we don't want user to edit this box
   mViewVarButton->Enable();
   
   try
   {
      // Set the pointer for the "Show Script" button
      mObject = mVaryCommand;

      std::string solverName =
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("SolverName"));

      #if DEBUG_VARY_PANEL
      MessageInterface::ShowMessage("solverName=%s\n", solverName.c_str());
      #endif
      
      std::string varName =
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Variable"));

      #if DEBUG_VARY_PANEL
      MessageInterface::ShowMessage("varName=%s\n", varName.c_str());
      #endif
   
      Real value;

      mSolverData.solverName = wxT(solverName.c_str());
      mSolverData.varName = wxT(varName.c_str());
      
      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("InitialValue"));
      mSolverData.initialValue = value;

      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("Perturbation"));
      mSolverData.pert = value;

      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("MinimumValue"));
      mSolverData.minValue = value;
         
      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("MaximumValue"));
      mSolverData.maxValue = value;

      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("MaximumChange"));
      mSolverData.maxStep = value;

      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("AdditiveScaleFactor"));
      mSolverData.additiveScaleFactor = value;

      value = mVaryCommand->
         GetRealParameter(mVaryCommand->GetParameterID("MutiplicativeScaleFactor"));
      mSolverData.multiplicativeScaleFactor = value;
            
      if (inOptimizeCmd)
      {   
         pertStaticText->Enable(false);
         maxStepStaticText->Enable(false);
      	 mPertTextCtrl->Enable(false);
      	 mMaxStepTextCtrl->Enable(false);
      }
      else // in target
      {
         // gray out the optimize stuff
         additiveStaticText->Enable(false);
         multiplicativeStaticText->Enable(false);
         mAdditiveTextCtrl->Enable(false);
         mMultiplicativeTextCtrl->Enable(false);
      }


   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("VaryPanel:LoadData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }

   ShowVariableSetup();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void VaryPanel::SaveData()
{   
   
   //-------------------------------------------------------
   // Saving Solver Data
   //-------------------------------------------------------
   std::string solverName = mSolverData.solverName.c_str();
   std::string variableName = mSolverData.varName.c_str();
   
   #if DEBUG_VARY_PANEL
   MessageInterface::ShowMessage
      ("VaryPanel::SaveData() solverName=%s, variableName=%s\n",
       solverName.c_str(), variableName.c_str());
   #endif
   
   Solver *solver = (Solver*)theGuiInterpreter->GetObject(solverName);

   if (solver == NULL)
      throw GmatBaseException("Cannot find the solver: " + solverName);

   try
   {
      solver->SetStringParameter
         (solver->GetParameterID("Variables"), variableName);
   
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("SolverName"), solverName);
   
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("Variable"), variableName);
   
      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("InitialValue"),
          mSolverData.initialValue);
   
      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("Perturbation"),
          mSolverData.pert);
   
      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("MinimumValue"),
          mSolverData.minValue);
   
      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("MaximumValue"),
          mSolverData.maxValue);
   
      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("MaximumChange"),
          mSolverData.maxStep);

      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("AdditiveScaleFactor"),
          mSolverData.additiveScaleFactor);

      mVaryCommand->SetRealParameter
         (mVaryCommand->GetParameterID("MutiplicativeScaleFactor"),
          mSolverData.multiplicativeScaleFactor);
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("VaryPanel:SaveData() error occurred!\n%s\n",
          e.GetMessage().c_str());
   }
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// void ShowVariableSetup()
//------------------------------------------------------------------------------
void VaryPanel::ShowVariableSetup()
{
   mSolverComboBox->SetStringSelection(mSolverData.solverName);
   mVarNameTextCtrl->SetValue(mSolverData.varName);
   
   mInitialTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.initialValue));
   mPertTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.pert));
   mMinValueTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.minValue));
   mMaxValueTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.maxValue));
   mMaxStepTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.maxStep));
   mAdditiveTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.additiveScaleFactor));
   mMultiplicativeTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.multiplicativeScaleFactor));
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void VaryPanel::OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void VaryPanel::OnTextChange(wxCommandEvent& event)
{
   if (mInitialTextCtrl->IsModified())
      mSolverData.initialValue =
         atof(mInitialTextCtrl->GetValue().c_str());
      
   if (mPertTextCtrl->IsModified())
      mSolverData.pert = atof(mPertTextCtrl->GetValue().c_str());
      
   if (mMinValueTextCtrl->IsModified())
      mSolverData.minValue = atof(mMinValueTextCtrl->GetValue().c_str());

   if (mMaxValueTextCtrl->IsModified())
      mSolverData.maxValue = atof(mMaxValueTextCtrl->GetValue().c_str());

   if (mMaxStepTextCtrl->IsModified())
      mSolverData.maxStep = atof(mMaxStepTextCtrl->GetValue().c_str());

   if (mAdditiveTextCtrl->IsModified())
      mSolverData.additiveScaleFactor = atof(mAdditiveTextCtrl->GetValue().c_str());

   if (mMultiplicativeTextCtrl->IsModified())
      mSolverData.multiplicativeScaleFactor = atof(mMultiplicativeTextCtrl->GetValue().c_str());

   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void VaryPanel::OnSolverSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void VaryPanel::OnSolverSelection(wxCommandEvent &event)
{
   mSolverData.solverName = mSolverComboBox->GetStringSelection();
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void VaryPanel::OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void VaryPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mViewVarButton)  
   {
      wxString objType = "ImpulsiveBurn";
      if (theGuiManager->GetNumImpulsiveBurn() == 0)
          objType = "Spacecraft";
      
      ParameterSelectDialog paramDlg(this, mObjectTypeList, objType,
                                     GuiItemManager::SHOW_SETTABLE,
                                     false, false, true, false, false, false);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         mVarNameTextCtrl->SetValue(newParamName);
         mSolverData.varName = newParamName;
         
         theApplyButton->Enable(true);
      }
   }
   else
      event.Skip();
}

