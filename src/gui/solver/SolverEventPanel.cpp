//------------------------------------------------------------------------------
//                           SolverEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/12/16
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
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
#include "SolverEventPanel.hpp"

// base includes
#include "gmatdefs.hpp"
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
SolverEventPanel::SolverEventPanel(wxWindow *parent)
{
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
    item4 = new wxButton( parent, ID_BUTTON, wxT("Create Script"), wxDefaultPosition, wxSize(100,-1), 0 );
    item12 = new wxButton( parent, ID_BUTTON, wxT("Apply Corrections"), wxDefaultPosition, wxSize(117,-1), 0 );
    item14 = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item15 = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    item16 = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item17 = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    
    // wxStaticText
    item2 = new wxStaticText( parent, ID_TEXT, wxT("Event Name"), wxDefaultPosition, wxDefaultSize, 0 );
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
    item7 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strArray1, wxCB_DROPDOWN );
    item9 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strArray2, wxCB_DROPDOWN );
    item11 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strArray3, wxCB_DROPDOWN );
    
    // wx*Sizer
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *item13 = new wxBoxSizer( wxHORIZONTAL );
    wxFlexGridSizer *item5 = new wxFlexGridSizer( 4, 0, 0 );
    
    // Add wx*Sizer
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item1->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );    

    item5->Add( item6, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item5->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

    item13->Add( item14, 0, wxALIGN_CENTRE|wxALL, 5 );
    item13->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );
    item13->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );
    item13->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( item5, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );

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
    item15->Enable(true);
}

void SolverEventPanel::OnComboSelection(wxCommandEvent& event)
{
    item15->Enable(true);
}

void SolverEventPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == item4 )         
        ;
    else if ( event.GetEventObject() == item12 )      
        ;
    else if ( event.GetEventObject() == item14 )
        ;
    else if ( event.GetEventObject() == item15 )
        ;
    else if ( event.GetEventObject() == item16 )
        ;
    else if ( event.GetEventObject() == item17 )
        ;
    else
        event.Skip();
}
