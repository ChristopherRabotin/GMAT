//------------------------------------------------------------------------------
//                           SolverCreatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/01/21
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Solver Create window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/notebook.h>

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatMainNotebook.hpp"
#include "DCSetupPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DCSetupPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON, DCSetupPanel::OnButton)
    EVT_TEXT(ID_TEXTCTRL, DCSetupPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// SolverEventPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
DCSetupPanel::DCSetupPanel(wxWindow *parent)
{
    Initialize();
    Setup(this);
    GetData();
}

void DCSetupPanel::Initialize()
{

}

void DCSetupPanel::Setup( wxWindow *parent)
{   
    // wxStaticText
    nameStaticText = new wxStaticText( parent, ID_TEXT, wxT("Solver Name"), wxDefaultPosition, wxDefaultSize, 0 );
    maxStaticText = new wxStaticText( parent, ID_TEXT, wxT("Max Iterations"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    maxTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    
    // wx*Sizer
    wxBoxSizer *boxsizerMain = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxsizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxsizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
    flexGridSizer1->AddGrowableCol( 1 );

    // wxButton
    okButton = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // Add to wx*Sizer
    flexGridSizer1->Add( nameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    flexGridSizer1->Add( nameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    flexGridSizer1->Add( maxStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    flexGridSizer1->Add( maxTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    
    boxsizer1->Add( flexGridSizer1, 0, wxALIGN_CENTER|wxALL, 5 );
    
    boxsizer2->Add( okButton, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( applyButton, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( cancelButton, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( helpButton, 0, wxALIGN_CENTER|wxALL, 5 );

    boxsizerMain->Add( boxsizer1, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizerMain->Add( boxsizer2, 0, wxALIGN_CENTER|wxALL, 5 );

    parent->SetAutoLayout( true );
    parent->SetSizer( boxsizerMain );
    boxsizerMain->Fit( parent );
    boxsizerMain->SetSizeHints( parent );
}

void DCSetupPanel::GetData()
{    
    //nameTextCtrl->Append();
    //maxTextCtrl->Append();
}

void DCSetupPanel::SetData()
{
    // = nameTextCtrl->GetValue();
    // = maxTextCtrl->GetValue();
}

void DCSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
    if ( event.GetEventObject() == nameTextCtrl )         
        applyButton->Enable(true);
    else if ( event.GetEventObject() == maxTextCtrl )      
        applyButton->Enable(true);
    else
        event.Skip();
}

void DCSetupPanel::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == okButton )      
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
        
    else
        event.Skip();
}
