//------------------------------------------------------------------------------
//                              PropagatorSetup
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Propagator Setup window.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "PropagatorSetupPanel.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagatorSetup, wxPanel)
    EVT_GRID_CELL_RIGHT_CLICK(PropagatorSetup::OnRightClick)
    EVT_CLOSE(PropagatorSetup::OnCloseWindow)
    EVT_BUTTON(-1, PropagatorSetup::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagatorSetup()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagatorSetup::PropagatorSetup( wxWindow *parent )
                :wxPanel(parent)
{
	CreateSetupWindow(this);
}

void PropagatorSetup::CreateSetupWindow( wxWindow *parent)
{
    item0 = new wxBoxSizer( wxVERTICAL );

    item1 = new wxBoxSizer( wxHORIZONTAL );

    item2 = new wxStaticText( parent, ID_TEXT, wxT("Event Name"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item4 = new wxButton( parent, ID_BUTTON, wxT("Create Script"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item6 = new wxStaticBox( parent, -1, wxT("Propagators and Spacecraft") );
    item5 = new wxStaticBoxSizer( item6, wxVERTICAL );

    item7 = new wxStaticText( parent, ID_TEXT, wxT("Synchronization Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs8 = (wxString*) NULL;
    item8 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 0, strs8, wxCB_DROPDOWN|wxCB_READONLY );
    item5->Add( item8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item9 = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(200,160), wxWANTS_CHARS );
    item9->CreateGrid( 10, 10, wxGrid::wxGridSelectCells );
    item5->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item11 = new wxStaticBox( parent, -1, wxT("Stopping Conditions") );
    item10 = new wxStaticBoxSizer( item11, wxVERTICAL );

    item12 = new wxGrid( parent, ID_GRID, wxDefaultPosition, wxSize(200,160), wxWANTS_CHARS );
    item12->CreateGrid( 10, 10, wxGrid::wxGridSelectCells );
    item10->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item14 = new wxStaticBox( parent, -1, wxT("Stopping Condition Details") );
    item13 = new wxStaticBoxSizer( item14, wxVERTICAL );

    item15 = new wxFlexGridSizer( 4, 0, 1 );

    item16 = new wxStaticText( parent, ID_TEXT, wxT("Spacecraft"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );

    item17 = new wxStaticText( parent, ID_TEXT, wxT("Property"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

    item15->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item18 = new wxStaticText( parent, ID_TEXT, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item18, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs19[] =
    {
        wxT("MMS1")
    };
    item19  = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs19, wxCB_DROPDOWN|wxCB_READONLY );
    item15->Add( item19, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs20[] =
    {
        wxT("Periapsis")
    };
    item20 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs20, wxCB_DROPDOWN|wxCB_READONLY );
    item15->Add( item20, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs21[] =
    {
        wxT("=")
    };
    item21 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs21, wxCB_DROPDOWN|wxCB_READONLY );
    item15->Add( item21, 0, wxALIGN_CENTRE|wxALL, 5 );

    item22 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item15->Add( item22, 0, wxALIGN_CENTRE|wxALL, 5 );

    item23 = new wxStaticText( parent, ID_TEXT, wxT("Repeat"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item23, 0, wxALIGN_CENTRE|wxALL, 5 );

    item24 = new wxStaticText( parent, ID_TEXT, wxT("Tolerance"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item24, 0, wxALIGN_CENTRE|wxALL, 5 );

    item25 = new wxStaticText( parent, ID_TEXT, wxT("Central Body"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item25, 0, wxALIGN_CENTRE|wxALL, 5 );

    item26 = new wxStaticText( parent, ID_TEXT, wxT("Coordinate System"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item26, 0, wxALIGN_CENTRE|wxALL, 5 );

    item27 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item15->Add( item27, 0, wxALIGN_CENTRE|wxALL, 5 );

    item28 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item15->Add( item28, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs29[] =
    {
        wxT("Earth")
    };
    item29 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs29, wxCB_DROPDOWN|wxCB_READONLY );
    item15->Add( item29, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs30[] =
    {
        wxT("")
    };
    item30 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs30, wxCB_DROPDOWN|wxCB_READONLY );
    item15->Add( item30, 0, wxALIGN_CENTRE|wxALL, 5 );

    item13->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );

    item31 = new wxBoxSizer( wxHORIZONTAL );

    item32 = new wxStaticText( parent, ID_TEXT, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    item31->Add( item32, 0, wxALIGN_CENTRE|wxALL, 5 );

    item33 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT("Stop at Periapis of orbit for MMS1"), wxDefaultPosition, wxSize(300,-1), 0 );
    item31->Add( item33, 0, wxALIGN_CENTRE|wxALL, 5 );

    item34 = new wxButton( parent, ID_BUTTON, wxT("User Defined"), wxDefaultPosition, wxDefaultSize, 0 );
    item31->Add( item34, 0, wxALIGN_CENTRE|wxALL, 5 );

    item13->Add( item31, 0, wxALIGN_CENTRE|wxALL, 5 );

    item10->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    item35 = new wxBoxSizer( wxHORIZONTAL );

    item36 = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item36, 0, wxALIGN_CENTRE|wxALL, 5 );

    item37 = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item37, 0, wxALIGN_CENTRE|wxALL, 5 );

    item38 = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item38, 0, wxALIGN_CENTRE|wxALL, 5 );

    item39 = new wxButton( parent, ID_BUTTON, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item39, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    item0->Fit( parent );
    item0->SetSizeHints( parent );
}

void PropagatorSetup::OnRightClick()
{
	return;
}

void PropagatorSetup::OnCloseWindow()
{
	Destroy();
	Close(TRUE);
}

void PropagatorSetup::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == item4 )  // Create Script
    {
		// waw - Build 1 development
		
        //delete m_btnFocused;
        //m_btnFocused = NULL;

       // m_btnDelete->Disable();
    }
    else if ( event.GetEventObject() == item34 )  // User Defined
    {
		// waw - Build 1 development
		
        //wxGetTextFromUser(_T("Dummy prompt"),
                         // _T("Modal dialog called from dialog"),
                         // _T(""), this);
    }
	else if ( event.GetEventObject() == item36 )  // OK
    {
        OnCloseWindow();
    }
	else if ( event.GetEventObject() == item37 )  // Apply
    {
        // waw - Build 1 development
    }
	else if ( event.GetEventObject() == item38 )  // Cancel
    {
        OnCloseWindow();
    }
	else if ( event.GetEventObject() == item39 )  // Help
    {
       // waw - Future development...
    }
    else
    {
        event.Skip();
    }
}


