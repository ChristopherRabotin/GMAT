//$Header$
//------------------------------------------------------------------------------
//                           SolverEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
//
/**
 * This class contains the Solver Event window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "gmatwxdefs.hpp"
#include "GmatMainNotebook.hpp"
#include "SolverEventPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SolverEventPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, SolverEventPanel::OnButton)
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
SolverEventPanel::SolverEventPanel(wxWindow *parent, const wxString &name)
    : wxPanel(parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter(); 
    Command *theCommand = theGuiInterpreter->GetCommand(name.c_str());
    
    Initialize();
    Setup(this);
    GetData();
}

void SolverEventPanel::Initialize()
{
    return;
}

void SolverEventPanel::Setup( wxWindow *parent)
{    
    // wxButton
    scriptButton = new wxButton( parent, ID_BUTTON, wxT("Create Script"), wxDefaultPosition, wxSize(100,-1), 0 );
    correctionsButton = new wxButton( parent, ID_BUTTON, wxT("Apply Corrections"), wxDefaultPosition, wxSize(117,-1), 0 );
    okButton = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxStaticText
    item6 = new wxStaticText( parent, ID_TEXT, wxT("Solver Name"), wxDefaultPosition, wxDefaultSize, 0 );
    item8 = new wxStaticText( parent, ID_TEXT, wxT("Upon Convergence"), wxDefaultPosition, wxDefaultSize, 0 );
    item10 = new wxStaticText( parent, ID_TEXT, wxT("Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    
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
    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *item13 = new wxBoxSizer( wxHORIZONTAL );
    wxFlexGridSizer *item5 = new wxFlexGridSizer( 5, 0, 0 );
    
    // Add wx*Sizer
    item1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( scriptButton, 0, wxALIGN_CENTRE|wxALL, 5 );    
    
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

    item13->Add( okButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item13->Add( applyButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item13->Add( cancelButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    item13->Add( helpButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );
   
    applyButton->Enable(false);
    
    // waw: Future Implementation
    helpButton->Enable(false);
    scriptButton->Enable(false);
    correctionsButton->Enable(false);

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    
    item0->Fit( parent );
    item0->SetSizeHints( parent );
}

void SolverEventPanel::GetData()
{    
    return;
}

void SolverEventPanel::SetData()
{
    return;
}

void SolverEventPanel::OnTextUpdate(wxCommandEvent& event)
{
    applyButton->Enable(true);
}

void SolverEventPanel::OnComboSelection(wxCommandEvent& event)
{
    applyButton->Enable(true);
}

void SolverEventPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == scriptButton )         
        ;
    else if ( event.GetEventObject() == correctionsButton )      
        ;
    else if ( event.GetEventObject() == okButton )
    {
        SetData();
        
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();
    }
    else if ( event.GetEventObject() == applyButton )  
    {
        SetData();
        
        applyButton->Enable(false);
    }
    else if ( event.GetEventObject() == cancelButton )  
    {
        GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
        gmatMainNotebook->ClosePage();
    }
    else if ( event.GetEventObject() == helpButton )
        ;
    else
        event.Skip();
}

