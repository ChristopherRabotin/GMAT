//$Header$
//------------------------------------------------------------------------------
//                           SolverCreatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/21
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "DCSetupPanel.hpp"

#include <wx/variant.h>

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiInterpreter.hpp"
#include "Solver.hpp"
#include "DifferentialCorrector.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DCSetupPanel, GmatPanel)
//    EVT_BUTTON(ID_BUTTON, DCSetupPanel::OnButton)
    EVT_TEXT(ID_TEXTCTRL, DCSetupPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// DCSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
DCSetupPanel::DCSetupPanel(wxWindow *parent, const wxString &name)
    : GmatPanel(parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    
    theSolver =
        theGuiInterpreter->GetSolver(std::string(name.c_str()));
        
    theDC = (DifferentialCorrector *)theSolver;

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

DCSetupPanel::~DCSetupPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void DCSetupPanel::Create()
{
    maxIterationID = theDC->GetParameterID("MaximumIterations");
    maxIteration = theDC->GetIntegerParameter(maxIterationID);
        
    Setup(this);
}

void DCSetupPanel::LoadData()
{
    maxTextCtrl->AppendText(wxVariant((long)maxIteration));

    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

void DCSetupPanel::SaveData()
{   
    theDC->SetIntegerParameter(maxIterationID, maxIteration);

    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

void DCSetupPanel::Setup( wxWindow *parent)
{   
    // wxStaticText
    maxStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Iterations"), 
                     wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    maxTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), 
                  wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wx*Sizer
    wxBoxSizer *boxsizerMain = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxsizer1 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
    flexGridSizer1->AddGrowableCol( 1 );
 
    // Add to wx*Sizer
    flexGridSizer1->Add( maxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    flexGridSizer1->Add( maxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    
    boxsizer1->Add( flexGridSizer1, 0, wxALIGN_CENTER|wxALL, 5 );
    
    boxsizerMain->Add( boxsizer1, 0, wxALIGN_CENTER|wxALL, 5 );

    theMiddleSizer->Add(boxsizerMain, 0, wxGROW, 5);

}

void DCSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == maxTextCtrl )   
    {   
        wxString value = maxTextCtrl->GetValue();
        
        maxIteration = atoi(value);
        theApplyButton->Enable(true);
    }
    else
        event.Skip();
}
