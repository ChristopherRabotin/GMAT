//$Header: 
//------------------------------------------------------------------------------
//                              CoordSystemConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#include "CoordSystemConfigPanel.hpp"

// gui includes
#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CoordSystemConfigPanel, GmatPanel)
    EVT_TEXT(ID_TEXTCTRL, CoordSystemConfigPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBO, CoordSystemConfigPanel::OnComboUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CoordSystemConfigPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CoordSystemConfigPanel::CoordSystemConfigPanel(wxWindow *parent)
    : GmatPanel(parent)
{   
   Create();
   Show();
}

CoordSystemConfigPanel::~CoordSystemConfigPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void CoordSystemConfigPanel::Create()
{
    Setup(this);    
}

void CoordSystemConfigPanel::Setup( wxWindow *parent)
{
    // wxStaticText
    nameStaticText = new wxStaticText( parent, ID_TEXT, wxT("Coordinate System Name"), wxDefaultPosition, wxDefaultSize, 0 );
    originStaticText = new wxStaticText( parent, ID_TEXT, wxT("Origin"), wxDefaultPosition, wxDefaultSize, 0 );
    typeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    primaryStaticText = new wxStaticText( parent, ID_TEXT, wxT("Primary"), wxDefaultPosition, wxDefaultSize, 0 );
    formatStaticText = new wxStaticText( parent, ID_TEXT, wxT("Epoch Format"), wxDefaultPosition, wxDefaultSize, 0 );
    secondaryStaticText = new wxStaticText( parent, ID_TEXT, wxT("Secondary"), wxDefaultPosition, wxDefaultSize, 0 );
    epochStaticText = new wxStaticText( parent, ID_TEXT, wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    nameTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(160,-1), 0 );

    // wxString
    wxString strs5[] = 
    {
        wxT("")
    };   
    wxString strs10[] = 
    {
        wxT("")
    };
    wxString strs12[] = 
    {
        wxT("")
    };
    wxString strs14[] = 
    {
        wxT("")
    };
    wxString strs17[] = 
    {
        wxT("")
    };
    wxString strs19[] = 
    {
        wxT("ComboItem")
    };
    
    // wxComboBox
    originComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(160,-1), 1, strs5, wxCB_DROPDOWN );   
    typeComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs10, wxCB_DROPDOWN );
    primaryComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs12, wxCB_DROPDOWN );
    formatComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs14, wxCB_DROPDOWN );
    secondaryComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs17, wxCB_DROPDOWN );
    epochComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs19, wxCB_DROPDOWN );
    
    // wx*Sizers
    wxStaticBox *staticbox1 = new wxStaticBox( parent, -1, wxT("Axes") );
    wxStaticBoxSizer *staticboxsizer1 = new wxStaticBoxSizer( staticbox1, wxHORIZONTAL );
    wxFlexGridSizer *flexgridsizer1 = new wxFlexGridSizer( 1, 0, 0 );
    wxBoxSizer *boxsizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxsizer2 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxsizer3 = new wxBoxSizer( wxVERTICAL );
    
    flexgridsizer1->Add( nameStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    flexgridsizer1->Add( nameTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
    flexgridsizer1->Add( originStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    flexgridsizer1->Add( originComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
       
    boxsizer2->Add( typeStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( typeComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( primaryStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( primaryComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer2->Add( formatStaticText, 0, wxALIGN_CENTER|wxALL, 5 );  
    boxsizer2->Add( formatComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

    staticboxsizer1->Add( boxsizer2, 0, wxALIGN_CENTER|wxALL, 5 );

    boxsizer3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer3->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer3->Add( secondaryStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer3->Add( secondaryComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer3->Add( epochStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
    boxsizer3->Add( epochComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

    staticboxsizer1->Add( boxsizer3, 0, wxALIGN_CENTER|wxALL, 5 );
    
    flexgridsizer1->Add( staticboxsizer1, 0, wxALIGN_CENTER|wxALL, 5 );

    boxsizer1->Add( flexgridsizer1, 0, wxALIGN_CENTER|wxALL, 5 );
   
    theMiddleSizer->Add(flexGridSizer1, 0, wxGROW, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::LoadData()
{
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::SaveData()
{
}

//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnTextUpdate(wxCommandEvent& event)
{  
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnComboUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnComboUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
}
