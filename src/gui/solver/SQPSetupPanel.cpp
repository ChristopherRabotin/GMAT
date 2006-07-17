#include "GmatAppData.hpp"
#include "SQPSetupPanel.hpp"

#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiInterpreter.hpp"
#include "Solver.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SQPSetupPanel, GmatPanel)
END_EVENT_TABLE()

const wxString
SQPSetupPanel::DISPLAY_SCHEMES[4] =
{
   "Iter",
   "Off",
   "Notify",
   "Final"
}; 

SQPSetupPanel::SQPSetupPanel(wxWindow *parent, const wxString &name)
    : GmatPanel(parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    
    theSolver =
        theGuiInterpreter->GetSolver(std::string(name.c_str()));
        
//    theSQP = (SQP *)theSolver;

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

SQPSetupPanel::~SQPSetupPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void SQPSetupPanel::Create()
{
    Setup(this);
}

void SQPSetupPanel::LoadData()
{
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

void SQPSetupPanel::SaveData()
{   
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

void SQPSetupPanel::Setup( wxWindow *parent)
{   
	displayStaticText = new wxStaticText( parent, ID_TEXT, wxT("Display"));
	displayComboBox = new wxComboBox( parent, ID_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, 4, DISPLAY_SCHEMES);
	
	checkBox1 = new wxCheckBox( parent, ID_CHECKBOX, "Grad Obj");
	checkBox2 = new wxCheckBox( parent, ID_CHECKBOX, "Grad Constr");
	checkBox3 = new wxCheckBox( parent, ID_CHECKBOX, "Derivative Check");
	
	specStaticText1 = new wxStaticText( parent, ID_TEXT, wxT("Tol Fun"));
	specStaticText2 = new wxStaticText( parent, ID_TEXT, wxT("Tol Con"));
	specStaticText3 = new wxStaticText( parent, ID_TEXT, wxT("Tol X"));
	specStaticText4 = new wxStaticText( parent, ID_TEXT, wxT("Max Fun Evals"));
	specStaticText5 = new wxStaticText( parent, ID_TEXT, wxT("Max Iter"));
	specStaticText6 = new wxStaticText( parent, ID_TEXT, wxT("Diff Min Change"));
	specStaticText7 = new wxStaticText( parent, ID_TEXT, wxT("Diff Max Change"));
	
	specTextCtrl1 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
	specTextCtrl2 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
	specTextCtrl3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
	specTextCtrl4 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
	specTextCtrl5 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
	specTextCtrl6 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));
	specTextCtrl7 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(100,-1));

   wxFlexGridSizer *fGSMain = new wxFlexGridSizer(2);
   wxBoxSizer *bSCheckBoxes = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *bSDisplay = new wxBoxSizer(wxHORIZONTAL);
   wxGridSizer *gSSpecs = new wxGridSizer(2);

   Integer border = 6;
//   fGSMain->SetFlexibleDirection(wxVERTICAL);
   
   bSDisplay->Add(displayStaticText, 0, wxALL, border);
   bSDisplay->Add(displayComboBox, 0, wxALL, border);
   
   bSCheckBoxes->Add(checkBox1, 0, wxALL|wxALIGN_LEFT, border);
   bSCheckBoxes->Add(checkBox2, 0, wxALL|wxALIGN_LEFT, border);
   bSCheckBoxes->Add(checkBox3, 0, wxALL|wxALIGN_LEFT, border);
   
   gSSpecs->Add(specStaticText1, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl1, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(specStaticText2, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl2, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(specStaticText3, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl3, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(specStaticText4, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl4, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(specStaticText5, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl5, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(specStaticText6, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl6, 0, wxALL|wxALIGN_LEFT, border);
   gSSpecs->Add(specStaticText7, 0, wxALL|wxALIGN_RIGHT, border);
   gSSpecs->Add(specTextCtrl7, 0, wxALL|wxALIGN_LEFT, border);
   
   fGSMain->Add(bSCheckBoxes, 0, wxALL|wxALIGN_LEFT, border);
   fGSMain->Add(bSDisplay, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER, border);
   fGSMain->Add(gSSpecs, 0, wxALL|wxALIGN_RIGHT, border);
   
   theMiddleSizer->Add(fGSMain, 0, wxALL|wxALIGN_CENTER, 5);
}
