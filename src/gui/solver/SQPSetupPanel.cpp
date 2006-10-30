#include "GmatAppData.hpp"
#include "SQPSetupPanel.hpp"

#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Solver.hpp"
#include "MessageInterface.hpp"

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
          
      theSolver->SetStringParameter("TolFun", tolFunTextCtrl->GetValue().c_str());
      theSolver->SetStringParameter("TolCon", tolConTextCtrl->GetValue().c_str());
      theSolver->SetStringParameter("TolX", tolXTextCtrl->GetValue().c_str());
      theSolver->SetStringParameter("MaxFunEvals", maxFunEvalsTextCtrl->GetValue().c_str());
      theSolver->SetStringParameter("MaxIter", maxIterTextCtrl->GetValue().c_str());
      theSolver->SetStringParameter("DiffMinChange", diffMinChangeTextCtrl->GetValue().c_str());
      theSolver->SetStringParameter("DiffMaxChange", diffMaxChangeTextCtrl->GetValue().c_str());

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
   displayStaticText = new wxStaticText( parent, ID_TEXT, wxT("Display"));
   displayComboBox = new wxComboBox( parent, ID_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, 
                                     4, DISPLAY_SCHEMES, wxCB_READONLY);
        
   gradObjCB = new wxCheckBox( parent, ID_CHECKBOX, "Grad Obj");
   gradConstrCB = new wxCheckBox( parent, ID_CHECKBOX, "Grad Constr");
   derivativeCheckCB = new wxCheckBox( parent, ID_CHECKBOX, "Derivative Check");
   diagnosticsCB = new wxCheckBox( parent, ID_CHECKBOX, "Diagnostics");
        
   tolFunStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tol Fun"));
   tolConStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tol Con"));
   tolXStaticText = new wxStaticText( parent, ID_TEXT, wxT("Tol X"));
   maxFunEvalsStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Fun Evals"));
   maxIterStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Iter"));
   diffMinChangeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Diff Min Change"));
   diffMaxChangeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Diff Max Change"));
        
   tolFunTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
   tolConTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
   tolXTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
   maxFunEvalsTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
   maxIterTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
   diffMinChangeTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
   diffMaxChangeTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));

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


