//$Header$
//------------------------------------------------------------------------------
//                              PropagationConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Propagation Configuration window.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "PropagationConfigPanel.hpp"

//------------------------------------------------------------------------------
// PropagationConfigPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagationConfigPanel::PropagationConfigPanel(wxWindow *parent)
    :wxPanel(parent)
{
    CreateConfigWindow(this);
}

void PropagationConfigPanel::CreateConfigWindow(wxWindow *parent)
{
    item0 = new wxBoxSizer( wxVERTICAL );

    item1 = new wxBoxSizer( wxHORIZONTAL );

    item2 = new wxStaticText( parent, ID_TEXT, wxT("Propagation Name"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    item3 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(250,-1), 0 );
        
    item1->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );
        
    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    item4 = new wxBoxSizer( wxHORIZONTAL );

    item6 = new wxStaticBox( parent, -1, wxT("Numerical Integrator") );
    item5 = new wxStaticBoxSizer( item6, wxVERTICAL );

    item7 = new wxFlexGridSizer( 2, 0, 0 );

    item8 = new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs9[] = 
    {
        wxT("ComboItem")
    };
        
    item9 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs9, wxCB_DROPDOWN|wxCB_READONLY );
    item7->Add( item9, 0, wxALIGN_CENTRE|wxALL, 5 );

    item10 = new wxStaticText( parent, ID_TEXT, wxT("Setting 1"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item10, 0, wxALIGN_CENTRE|wxALL, 5 );

    item11 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item11, 0, wxALIGN_CENTRE|wxALL, 5 );

    item12 = new wxStaticText( parent, ID_TEXT, wxT("Setting 2"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item12, 0, wxALIGN_CENTRE|wxALL, 5 );

    item13 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item13, 0, wxALIGN_CENTRE|wxALL, 5 );

    item14 = new wxStaticText( parent, ID_TEXT, wxT("Setting 3"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item14, 0, wxALIGN_CENTRE|wxALL, 5 );

    item15 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item15, 0, wxALIGN_CENTRE|wxALL, 5 );

    item16 = new wxStaticText( parent, ID_TEXT, wxT("Setting 4"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item16, 0, wxALIGN_CENTRE|wxALL, 5 );

    item17 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item17, 0, wxALIGN_CENTRE|wxALL, 5 );

    item18 = new wxStaticText( parent, ID_TEXT, wxT("Setting 5"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item18, 0, wxALIGN_CENTRE|wxALL, 5 );

    item19 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item19, 0, wxALIGN_CENTRE|wxALL, 5 );

    item20 = new wxStaticText( parent, ID_TEXT, wxT("Setting 6"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item21 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item21, 0, wxALIGN_CENTRE|wxALL, 5 );

    item22 = new wxStaticText( parent, ID_TEXT, wxT("Setting 7"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item22, 0, wxALIGN_CENTRE|wxALL, 5 );

    item23 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item23, 0, wxALIGN_CENTRE|wxALL, 5 );

    item24 = new wxStaticText( parent, ID_TEXT, wxT("Setting 8"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item24, 0, wxALIGN_CENTRE|wxALL, 5 );

    item25 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item25, 0, wxALIGN_CENTRE|wxALL, 5 );

    item26 = new wxStaticText( parent, ID_TEXT, wxT("Setting 9"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item26, 0, wxALIGN_CENTRE|wxALL, 5 );

    item27 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item7->Add( item27, 0, wxALIGN_CENTRE|wxALL, 5 );

    item5->Add( item7, 0, wxALIGN_CENTRE|wxALL, 5 );

    item4->Add( item5, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item29 = new wxStaticBox( parent, -1, wxT("Force Model") );
    item28 = new wxStaticBoxSizer( item29, wxVERTICAL );

    item31 = new wxStaticBox( parent, -1, wxT("Primary Bodies") );
    item30 = new wxStaticBoxSizer( item31, wxVERTICAL );

    item32 = new wxBoxSizer( wxHORIZONTAL );

    wxString strs33[] = 
    {
        wxT("ComboItem")
    };
        
    item33 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs33, wxCB_DROPDOWN|wxCB_READONLY );
    item32->Add( item33, 0, wxALIGN_CENTRE|wxALL, 5 );

    item34 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item32->Add( item34, 0, wxALIGN_CENTRE|wxALL, 5 );

    item35 = new wxButton( parent, ID_BUTTON, wxT("Add Body"), wxDefaultPosition, wxDefaultSize, 0 );
    item32->Add( item35, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item32, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item37 = new wxStaticBox( parent, -1, wxT("Gravity Field") );
    wxStaticBoxSizer *item36 = new wxStaticBoxSizer( item37, wxHORIZONTAL );

    item38 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item38, 0, wxALIGN_CENTRE|wxALL, 5 );

    item39 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item36->Add( item39, 0, wxALIGN_CENTRE|wxALL, 5 );

    item40 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item40, 0, wxALIGN_CENTRE|wxALL, 5 );

    item41 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item36->Add( item41, 0, wxALIGN_CENTRE|wxALL, 5 );

    item42 = new wxStaticText( parent, ID_TEXT, wxT("Source"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item42, 0, wxALIGN_CENTRE|wxALL, 5 );

    item43 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item36->Add( item43, 0, wxALIGN_CENTRE|wxALL, 5 );

    item44 = new wxButton( parent, ID_BUTTON, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item44, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item46 = new wxStaticBox( parent, -1, wxT("Environment Model") );
    item45 = new wxStaticBoxSizer( item46, wxHORIZONTAL );

    item47 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item45->Add( item47, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxString strs48[] = 
    {
        wxT("ComboItem")
    };
        
    item48 = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(150,-1), 1, strs48, wxCB_DROPDOWN|wxCB_READONLY );
    item45->Add( item48, 0, wxALIGN_CENTRE|wxALL, 5 );

    item49 = new wxButton( parent, ID_BUTTON, wxT("Setup"), wxDefaultPosition, wxDefaultSize, 0 );
    item45->Add( item49, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item51 = new wxStaticBox( parent, -1, wxT("Magnetic Field") );
    wxStaticBoxSizer *item50 = new wxStaticBoxSizer( item51, wxHORIZONTAL );

    item52 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item52, 0, wxALIGN_CENTRE|wxALL, 5 );

    item53 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item50->Add( item53, 0, wxALIGN_CENTRE|wxALL, 5 );

    item54 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item54, 0, wxALIGN_CENTRE|wxALL, 5 );

    item55 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item50->Add( item55, 0, wxALIGN_CENTRE|wxALL, 5 );

    item56 = new wxStaticText( parent, ID_TEXT, wxT("Source"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item56, 0, wxALIGN_CENTRE|wxALL, 5 );

    item57 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item50->Add( item57, 0, wxALIGN_CENTRE|wxALL, 5 );

    item58 = new wxButton( parent, ID_BUTTON, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    item50->Add( item58, 0, wxALIGN_CENTRE|wxALL, 5 );

    item30->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item28->Add( item30, 0, wxALIGN_CENTRE|wxALL, 5 );

    item60 = new wxStaticBox( parent, -1, wxT("Point Masses") );
    item59 = new wxStaticBoxSizer( item60, wxVERTICAL );

    item61 = new wxFlexGridSizer( 2, 0, 2 );

    item62 = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(405,-1), 0 );
    item61->Add( item62, 0, wxALIGN_CENTRE|wxALL, 5 );

    item63 = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    item61->Add( item63, 0, wxALIGN_CENTRE|wxALL, 5 );

    item59->Add( item61, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item28->Add( item59, 0, wxALIGN_CENTRE|wxALL, 5 );

    item65 = new wxStaticBox( parent, -1, wxT("Solar Radiation Pressure") );
    item64 = new wxStaticBoxSizer( item65, wxHORIZONTAL );

    item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item66 = new wxCheckBox( parent, ID_CHECKBOX, wxT("Use"), wxDefaultPosition, wxDefaultSize, 0 );
    item64->Add( item66, 0, wxALIGN_CENTRE|wxALL, 5 );

    item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );

    item67 = new wxButton( parent, ID_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    item64->Add( item67, 0, wxALIGN_CENTRE|wxALL, 5 );

    item28->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item4->Add( item28, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    item68 = new wxBoxSizer( wxHORIZONTAL );

    item69 = new wxButton( parent, ID_BUTTON, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item68->Add( item69, 0, wxALIGN_CENTRE|wxALL, 5 );

    item70 = new wxButton( parent, ID_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    item68->Add( item70, 0, wxALIGN_CENTRE|wxALL, 5 );

    item71 = new wxButton( parent, ID_BUTTON, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item68->Add( item71, 0, wxALIGN_CENTRE|wxALL, 5 );

    item0->Add( item68, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    item0->Fit( parent );
    item0->SetSizeHints( parent );
}


