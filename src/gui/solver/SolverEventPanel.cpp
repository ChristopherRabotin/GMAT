//$Header$
//------------------------------------------------------------------------------
//                           SolverEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Event window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "SolverEventPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SolverEventPanel, GmatPanel)
    EVT_TEXT(ID_TEXTCTRL, SolverEventPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBO, SolverEventPanel::OnComboSelection)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SolverEventPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
SolverEventPanel::SolverEventPanel(wxWindow *parent, GmatCommand *cmd)
    : GmatPanel(parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter(); 
    theCommand = cmd;
    
    if (theCommand != NULL)
    {
        Create();
        Show();
    }
    else
    {
        // show error message
    }
}

SolverEventPanel::~SolverEventPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void SolverEventPanel::Create()
{
    Initialize();
    Setup(this);
}

void SolverEventPanel::LoadData()
{
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
    
    return;
}

void SolverEventPanel::SaveData()
{
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
    
    return;
}

void SolverEventPanel::Initialize()
{
    return;
}

void SolverEventPanel::Setup( wxWindow *parent)
{    
    // wxButton
    correctionsButton = new wxButton( parent, ID_BUTTON, 
         wxT("Apply Corrections"), wxDefaultPosition, wxSize(117,-1), 0 );
   
    // wxStaticText
    item6 = new wxStaticText( parent, ID_TEXT, wxT("Solver Name"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    item8 = new wxStaticText( parent, ID_TEXT, wxT("Upon Convergence"), 
         wxDefaultPosition, wxDefaultSize, 0 );
    item10 = new wxStaticText( parent, ID_TEXT, wxT("Mode"), wxDefaultPosition,
         wxDefaultSize, 0 );
    
    // wxString
    wxString strArray1[] = 
    {
        wxT("")
    };
    wxString strArray2[] = 
    {
        wxT("")
    };
    wxString strArray3[] = 
    {
        wxT("")
    };
    
    // wxComboBox
    item7 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(200,-1), 1, strArray1, wxCB_DROPDOWN );
    item9 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(200,-1), 1, strArray2, wxCB_DROPDOWN );
    item11 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(200,-1), 1, strArray3, wxCB_DROPDOWN );
    
    // wx*Sizer
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *item5 = new wxFlexGridSizer( 5, 0, 0 );
      
    //First row
    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    //Second row
    item5->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( correctionsButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );

    correctionsButton->Enable(false);
    
    theMiddleSizer->Add(item0, 0, wxGROW, 5);

}

void SolverEventPanel::OnTextUpdate(wxCommandEvent& event)
{
    theApplyButton->Enable(true);
}

void SolverEventPanel::OnComboSelection(wxCommandEvent& event)
{
    theApplyButton->Enable(true);
}
