//$Header$
//------------------------------------------------------------------------------
//                                SQPSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: 
// Created: 
//
/**
 * Declares SQPSetupPanel.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "SQPSetupPanel.hpp"

#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Solver.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"  // for ToReal()

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SQPSetupPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBOBOX, SQPSetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, SQPSetupPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, SQPSetupPanel::OnCheckboxChange)
END_EVENT_TABLE()

const wxString
SQPSetupPanel::DISPLAY_SCHEMES[4] =
{
   "Iter",
   "Off",
   "Notify",
   "Final"
}; 

//------------------------------------------------------------------------------
// SQPSetupPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
SQPSetupPanel::SQPSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    
   theSolver =
      (Solver*)theGuiInterpreter->GetObject(std::string(name.c_str()));
    
   if (theSolver != NULL)
   {
      Create();
      Show();
   }
   else
   {
      // show error message
   }
}


//------------------------------------------------------------------------------
// ~SQPSetupPanel()
//------------------------------------------------------------------------------
SQPSetupPanel::~SQPSetupPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void SQPSetupPanel::Create()
//------------------------------------------------------------------------------
void SQPSetupPanel::Create()
{
    Setup(this);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void SQPSetupPanel::LoadData()
{
   // load data from the core engine
   try
   {
          mObject = theSolver;
        
      std::string valueStr = theSolver->GetStringParameter("GradObj"); 
      if (valueStr == "On")
         gradObjCB->SetValue(true);
      else
         gradObjCB->SetValue(false);
         
      valueStr = theSolver->GetStringParameter("GradConstr"); 
      if (valueStr == "On")
         gradConstrCB->SetValue(true);
      else
         gradConstrCB->SetValue(false);
         
      valueStr = theSolver->GetStringParameter("DerivativeCheck"); 
      if (valueStr == "On")
         derivativeCheckCB->SetValue(true);
      else
         derivativeCheckCB->SetValue(false);
      
      valueStr = theSolver->GetStringParameter("Diagnostics");
      if (valueStr == "On")
         diagnosticsCB->SetValue(true);
      else
         diagnosticsCB->SetValue(false);
         
      valueStr = theSolver->GetStringParameter("TolFun"); 
      tolFunTextCtrl->SetValue(wxT(valueStr.c_str()));
      
      valueStr = theSolver->GetStringParameter("TolCon"); 
      tolConTextCtrl->SetValue(wxT(valueStr.c_str()));
      
      valueStr = theSolver->GetStringParameter("TolX"); 
      tolXTextCtrl->SetValue(wxT(valueStr.c_str()));
      
      valueStr = theSolver->GetStringParameter("MaxFunEvals"); 
      maxFunEvalsTextCtrl->SetValue(wxT(valueStr.c_str()));
      
      valueStr = theSolver->GetStringParameter("MaxIter"); 
      maxIterTextCtrl->SetValue(wxT(valueStr.c_str()));
      
      valueStr = theSolver->GetStringParameter("DiffMinChange"); 
      diffMinChangeTextCtrl->SetValue(wxT(valueStr.c_str()));
      
      valueStr = theSolver->GetStringParameter("DiffMaxChange"); 
      diffMaxChangeTextCtrl->SetValue(wxT(valueStr.c_str()));
     
      valueStr = theSolver->GetStringParameter("Display");
      displayComboBox->SetValue(wxT(valueStr.c_str()));
        
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("SQPSetupPanel:LoadData() error occurred!\n%s\n", e.GetMessage().c_str());
   }
   
   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void SQPSetupPanel::SaveData()
{   
   try
   {
      Real rvalue;
      Integer ivalue;    
      canClose = true;
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" + 
                         theSolver->GetName() + "\" is not an allowed value. \n"
                        "The allowed values are: [%s].";                        

      if (gradObjCB->IsChecked())
         theSolver->SetStringParameter("GradObj", "On");
      else
         theSolver->SetStringParameter("GradObj", "Off");
            
      if (gradConstrCB->IsChecked())
         theSolver->SetStringParameter("GradConstr", "On");
      else
         theSolver->SetStringParameter("GradConstr", "Off");

      if (derivativeCheckCB->IsChecked())
         theSolver->SetStringParameter("DerivativeCheck", "On");
      else
         theSolver->SetStringParameter("DerivativeCheck", "Off");
             
      if (diagnosticsCB->IsChecked())
         theSolver->SetStringParameter("Diagnostics", "On");
      else
         theSolver->SetStringParameter("Diagnostics", "Off");
          
      // save Tol Fun
      inputString = tolFunTextCtrl->GetValue();      

      // check to see if input is a real
      if (GmatStringUtil::ToReal(inputString,&rvalue))
      {  
         theSolver->SetStringParameter("TolFun", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Tol Fun","Real Number > 0.0");
         canClose = false;
      }

      // save Tol Con
      inputString = tolConTextCtrl->GetValue();      

      // check to see if input is a real
      if (GmatStringUtil::ToReal(inputString,&rvalue))
      {  
         theSolver->SetStringParameter("TolCon", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Tol Con","Real Number > 0.0");
         canClose = false;
      }

      // save Tol X
      inputString = tolXTextCtrl->GetValue();      

      // check to see if input is a real
      if (GmatStringUtil::ToReal(inputString,&rvalue))
      {  
         theSolver->SetStringParameter("TolX", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Tol X","Real Number > 0.0");
         canClose = false;
      }

      // save MaxIter
      inputString = maxIterTextCtrl->GetValue();      

      // check to see if input is a integer
      if (GmatStringUtil::ToInteger(inputString,&ivalue))
      {  
         theSolver->SetStringParameter("MaxIter", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "MaxIter","Integer > 0");
         canClose = false;
      }

      // save MaxFunEvals
      inputString = maxFunEvalsTextCtrl->GetValue();      

      // check to see if input is a integer
      if (GmatStringUtil::ToInteger(inputString,&ivalue))
      {  
         theSolver->SetStringParameter("MaxFunEvals", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "MaxFunEvals","Integer > 0");
         canClose = false;
      }

      // save DiffMinChange
      inputString = diffMinChangeTextCtrl->GetValue();      

      // check to see if input is a real
      if (GmatStringUtil::ToReal(inputString,&rvalue))
      {  
         theSolver->SetStringParameter("DiffMinChange", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "DiffMinChange","Real > 0.0");
         canClose = false;
      }

      // save DiffMaxChange
      inputString = diffMaxChangeTextCtrl->GetValue();      

      // check to see if input is a real
      if (GmatStringUtil::ToReal(inputString,&rvalue))
      {  
         theSolver->SetStringParameter("DiffMaxChange", inputString.c_str());
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "DiffMaxChange","Real > 0.0");
         canClose = false;
      }
//      theSolver->SetStringParameter("TolFun", tolFunTextCtrl->GetValue().c_str());
//      theSolver->SetStringParameter("TolCon", tolConTextCtrl->GetValue().c_str());
//      theSolver->SetStringParameter("TolX", tolXTextCtrl->GetValue().c_str());
//      theSolver->SetStringParameter("MaxFunEvals", maxFunEvalsTextCtrl->GetValue().c_str());
//      theSolver->SetStringParameter("MaxIter", maxIterTextCtrl->GetValue().c_str());
//      theSolver->SetStringParameter("DiffMinChange", diffMinChangeTextCtrl->GetValue().c_str());
//      theSolver->SetStringParameter("DiffMaxChange", diffMaxChangeTextCtrl->GetValue().c_str());

      theSolver->SetStringParameter("Display", displayComboBox->GetValue().c_str());                
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("SQPSetupPanel:SaveData() error occurred!\n%s\n", e.GetMessage().c_str());
      canClose = false;
      return;
   }
   
   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void Setup( wxWindow *parent)
//------------------------------------------------------------------------------
void SQPSetupPanel::Setup( wxWindow *parent)
{   
   // Tol Fun
   tolFunStaticText = new wxStaticText(parent, ID_TEXT, wxT("Tol Fun"));
   tolFunTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
                     wxSize(100,-1));

   // Tol Con
   tolConStaticText = new wxStaticText(parent, ID_TEXT, wxT("Tol Con"));
   tolConTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
                     wxSize(100,-1));

   // Tol X
   tolXStaticText = new wxStaticText(parent, ID_TEXT, wxT("Tol X"));
   tolXTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
                     wxSize(100,-1));

   // Max Fun Evals
   maxFunEvalsStaticText = 
      new wxStaticText(parent, ID_TEXT, wxT("Max Fun Evals"));
   maxFunEvalsTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
                     wxSize(100,-1));

   // Max Iter
   maxIterStaticText = new wxStaticText(parent, ID_TEXT, wxT("Max Iter"));
   maxIterTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
                     wxSize(100,-1));

   // Diff Min Change
   diffMinChangeStaticText = 
      new wxStaticText(parent, ID_TEXT, wxT("Diff Min Change"));
   diffMinChangeTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                      wxSize(100,-1));

   // Diff Max Change
   diffMaxChangeStaticText = 
      new wxStaticText(parent, ID_TEXT, wxT("Diff Max Change"));
   diffMaxChangeTextCtrl = 
      new wxTextCtrl(parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition,
                     wxSize(100,-1));

   // Display
   displayStaticText = new wxStaticText( parent, ID_TEXT, wxT("Display"));
   displayComboBox   = 
      new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, 
                     4, DISPLAY_SCHEMES, wxCB_READONLY);
        
   // Grad Obj
   gradObjCB = new wxCheckBox( parent, ID_CHECKBOX, "Grad Obj");

   // Grad Constr
   gradConstrCB = new wxCheckBox( parent, ID_CHECKBOX, "Grad Constr");

   // Derivative Check
   derivativeCheckCB = new wxCheckBox( parent, ID_CHECKBOX, "Derivative Check");

   // Diagnostics
   diagnosticsCB = new wxCheckBox( parent, ID_CHECKBOX, "Diagnostics");
        
        

   wxFlexGridSizer *fGSMain = new wxFlexGridSizer(2);
   wxBoxSizer *bSCheckBoxes = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *bSDisplay = new wxBoxSizer(wxHORIZONTAL);
   wxGridSizer *gSSpecs = new wxGridSizer(2);

   Integer border = 3;
   //   fGSMain->SetFlexibleDirection(wxVERTICAL);
   
   bSDisplay->Add(displayStaticText, 0, wxALL, border);
   bSDisplay->Add(displayComboBox, 0, wxALL, border);
   
   // add display sizer to checkbox sizer
   bSCheckBoxes->Add(bSDisplay, 0, wxALL|wxALIGN_LEFT, border);
   // add checkboxes to sizer
   bSCheckBoxes->Add(gradObjCB, 0, wxALL|wxALIGN_LEFT, border);
   bSCheckBoxes->Add(gradConstrCB, 0, wxALL|wxALIGN_LEFT, border);
   bSCheckBoxes->Add(derivativeCheckCB, 0, wxALL|wxALIGN_LEFT, border);
   bSCheckBoxes->Add(diagnosticsCB, 0, wxALL|wxALIGN_LEFT, border);
   
   gSSpecs->Add(tolFunStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(tolFunTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(tolConStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(tolConTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(tolXStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(tolXTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(maxFunEvalsStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(maxFunEvalsTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(maxIterStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(maxIterTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(diffMinChangeStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(diffMinChangeTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(diffMaxChangeStaticText, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(diffMaxChangeTextCtrl, 0, wxALL|wxALIGN_LEFT, border);
   
   fGSMain->Add(gSSpecs, 0, wxALL|wxALIGN_RIGHT, border*5);
   fGSMain->Add(bSCheckBoxes, 0, wxALL|wxALIGN_LEFT, border*5);
   //   fGSMain->Add(bSDisplay, 0, wxALL|wxALIGN_LEFT, border);
   
   theMiddleSizer->Add(fGSMain, 0, wxALL|wxALIGN_CENTER, 5);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SQPSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (theApplyButton != NULL)
      EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SQPSetupPanel::OnTextChange(wxCommandEvent& event)
{
   if (theApplyButton != NULL)
      EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckboxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SQPSetupPanel::OnCheckboxChange(wxCommandEvent& event)
{
   if (theApplyButton != NULL)
      EnableUpdate(true);
}

