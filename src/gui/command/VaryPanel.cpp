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
   mSolverData.initialValue = "0.0";
   mSolverData.pert = "0.0";
   mSolverData.minValue = "-9.999e30";
   mSolverData.maxValue = "9.999e30";
   mSolverData.maxStep = "9.999e30";
   mSolverData.additiveScaleFactor = "0.0";
   mSolverData.multiplicativeScaleFactor = "1.0";
   
   mObjectTypeList.Add("Spacecraft");
   mObjectTypeList.Add("ImpulsiveBurn");
   
   Create();
   Show();
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// ~VaryPanel()
//------------------------------------------------------------------------------
VaryPanel::~VaryPanel()
{
   mObjectTypeList.Clear();
   theGuiManager->UnregisterComboBox("Solver", mSolverComboBox);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void VaryPanel::Create()
{
   int bsize = 2; // bordersize

   // Targeter
   wxStaticText *solverStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solver"),
                       wxDefaultPosition, wxSize(40, -1), 0);
   
   // Show all userdefined Solvers
   mSolverComboBox =
      theGuiManager->GetSolverComboBox(this, ID_COMBO, wxSize(180,-1));
   
   // Variable
   wxStaticText *varStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable"), 
                       wxDefaultPosition, wxSize(40, -1), 0);
   mVarNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(250,-1), 0);
   mViewVarButton = new
      wxButton(this, ID_BUTTON, wxT("View"), wxDefaultPosition, wxDefaultSize, 0);
   
   // Initial Value
   wxStaticText *initialStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Initial Value"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mInitialTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                     wxDefaultPosition, wxSize(100,-1), 0);

   // Perturbation
   pertStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Perturbation"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mPertTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                  wxDefaultPosition, wxSize(100,-1), 0);

   // Lower
   wxStaticText *minValueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Lower"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mMinValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);

   // Upper
   wxStaticText *maxValueStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Upper"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mMaxValueTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);

   // Max Step
   maxStepStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Max Step"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mMaxStepTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(100,-1), 0);

   // Additive Scale Factor
   additiveStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Additive Scale Factor"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mAdditiveTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);

   // Multiplicative Scale Factor
   multiplicativeStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Multiplicative Scale Factor"), 
                       wxDefaultPosition, wxDefaultSize, 0);
   mMultiplicativeTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                                   wxDefaultPosition, wxSize(100,-1), 0);
   
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
   
      //Real value;

      mSolverData.solverName = wxT(solverName.c_str());
      mSolverData.varName = wxT(varName.c_str());
      
      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("InitialValue"));
      //mSolverData.initialValue = value;
      std::string initVal = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("InitialValue"));
      mSolverData.initialValue = wxT(initVal.c_str());

      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("Perturbation"));
      //mSolverData.pert = value;
      std::string pertVal = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Perturbation"));
      mSolverData.pert = wxT(pertVal.c_str());

      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("Lower"));
      //mSolverData.minValue = value;
      std::string lowerVal = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Lower"));
      mSolverData.minValue = wxT(lowerVal.c_str());
         
      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("Upper"));
      //mSolverData.maxValue = value;
      std::string upperVal = 
      mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("Upper"));
      mSolverData.maxValue = wxT(upperVal.c_str());
         
      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("MaxStep"));
      //mSolverData.maxStep = value;
      std::string maxVal = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("MaxStep"));
      mSolverData.maxStep = wxT(maxVal.c_str());

      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("AdditiveScaleFactor"));
      //mSolverData.additiveScaleFactor = value;
      std::string addVal = 
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("AdditiveScaleFactor"));
      mSolverData.additiveScaleFactor = wxT(addVal.c_str());

      //value = mVaryCommand->
      //   GetRealParameter(mVaryCommand->GetParameterID("MultiplicativeScaleFactor"));
      //mSolverData.multiplicativeScaleFactor = value;
      std::string multVal =
         mVaryCommand->GetStringParameter(mVaryCommand->GetParameterID("MultiplicativeScaleFactor"));
      mSolverData.multiplicativeScaleFactor = wxT(multVal.c_str());
            
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
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }

   ShowVariableSetup();
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void VaryPanel::SaveData()
{   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check input values: Number, Variable, Array element, Parameter
   //-----------------------------------------------------------------
   
   CheckVariable(mSolverData.initialValue.c_str(), Gmat::BURN, "InitialValue",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mSolverData.pert.c_str(), Gmat::BURN, "Perturbation",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mSolverData.minValue.c_str(), Gmat::BURN, "Lower",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mSolverData.maxValue.c_str(), Gmat::BURN, "Upper",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mSolverData.maxStep.c_str(), Gmat::BURN, "MaxStep",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mSolverData.additiveScaleFactor.c_str(), Gmat::BURN,
                 "AdditiveScaleFactor",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   CheckVariable(mSolverData.multiplicativeScaleFactor.c_str(), Gmat::BURN,
                 "MultiplicativeScaleFactor",
                 "Real Number, Variable, Array element, plottable Parameter", true);
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   std::string solverName = mSolverData.solverName.c_str();
   std::string variableName = mSolverData.varName.c_str();
   
   #if DEBUG_VARY_PANEL
   MessageInterface::ShowMessage
      ("VaryPanel::SaveData() solverName=%s, variableName=%s\n",
       solverName.c_str(), variableName.c_str());
   #endif
   
   Solver *solver = (Solver*)theGuiInterpreter->GetConfiguredObject(solverName);

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
      
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("InitialValue"),
          mSolverData.initialValue.c_str());
      
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("Perturbation"),
          mSolverData.pert.c_str());
   
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("Lower"),
          mSolverData.minValue.c_str());
      
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("Upper"),
          mSolverData.maxValue.c_str());
      
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("MaxStep"),
          mSolverData.maxStep.c_str());
      
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("AdditiveScaleFactor"),
          mSolverData.additiveScaleFactor.c_str());
      
      //mVaryCommand->SetRealParameter
      mVaryCommand->SetStringParameter
         (mVaryCommand->GetParameterID("MultiplicativeScaleFactor"),
          mSolverData.multiplicativeScaleFactor.c_str());
      
      if (!theGuiInterpreter->ValidateCommand(mVaryCommand))
          canClose = false;
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
   
   //EnableUpdate(false);
}

//------------------------------------------------------------------------------
// void ShowVariableSetup()
//------------------------------------------------------------------------------
void VaryPanel::ShowVariableSetup()
{
   mSolverComboBox->SetStringSelection(mSolverData.solverName);
   mVarNameTextCtrl->SetValue(mSolverData.varName);
   
   //mInitialTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.initialValue));
   //mPertTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.pert.c_str()));
   //mMinValueTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.minValue));
   //mMaxValueTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.maxValue));
   //mMaxStepTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.maxStep));
   //mAdditiveTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.additiveScaleFactor));
   //mMultiplicativeTextCtrl->SetValue(theGuiManager->ToWxString(mSolverData.multiplicativeScaleFactor));

   mInitialTextCtrl->SetValue(mSolverData.initialValue);
   mPertTextCtrl->SetValue(mSolverData.pert);
   mMinValueTextCtrl->SetValue(mSolverData.minValue);
   mMaxValueTextCtrl->SetValue(mSolverData.maxValue);
   mMaxStepTextCtrl->SetValue(mSolverData.maxStep);
   mAdditiveTextCtrl->SetValue(mSolverData.additiveScaleFactor);
   mMultiplicativeTextCtrl->SetValue(mSolverData.multiplicativeScaleFactor);
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
         mInitialTextCtrl->GetValue();
         //atof(mInitialTextCtrl->GetValue().c_str());
      
   if (mPertTextCtrl->IsModified())
      mSolverData.pert = mPertTextCtrl->GetValue();
      //mSolverData.pert = atof(mPertTextCtrl->GetValue().c_str());
      
   if (mMinValueTextCtrl->IsModified())
      mSolverData.minValue = mMinValueTextCtrl->GetValue();
      //mSolverData.minValue = atof(mMinValueTextCtrl->GetValue().c_str());

   if (mMaxValueTextCtrl->IsModified())
      mSolverData.maxValue = mMaxValueTextCtrl->GetValue();
      //mSolverData.maxValue = atof(mMaxValueTextCtrl->GetValue().c_str());

   if (mMaxStepTextCtrl->IsModified())
      mSolverData.maxStep = mMaxStepTextCtrl->GetValue();
      //mSolverData.maxStep = atof(mMaxStepTextCtrl->GetValue().c_str());

   if (mAdditiveTextCtrl->IsModified())
      mSolverData.additiveScaleFactor = mAdditiveTextCtrl->GetValue();
      //mSolverData.additiveScaleFactor = atof(mAdditiveTextCtrl->GetValue().c_str());

   if (mMultiplicativeTextCtrl->IsModified())
      mSolverData.multiplicativeScaleFactor = mMultiplicativeTextCtrl->GetValue();
      //mSolverData.multiplicativeScaleFactor = atof(mMultiplicativeTextCtrl->GetValue().c_str());

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
      
      ParameterSelectDialog paramDlg(this, mObjectTypeList,
                                     GuiItemManager::SHOW_SETTABLE);
      
      paramDlg.SetObjectType(objType);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         mVarNameTextCtrl->SetValue(newParamName);
         mSolverData.varName = newParamName;
         
         EnableUpdate(true);
      }
   }
   else
      event.Skip();
}

