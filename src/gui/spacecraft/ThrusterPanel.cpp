//$Header:
//------------------------------------------------------------------------------
//                            ThrusterPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Waka Waktola
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "ThrusterPanel.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ThrusterPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, ThrusterPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft, wxButton *theApplyButton)
//------------------------------------------------------------------------------
/**
 * Constructs ThrusterPanel object.
 */
//------------------------------------------------------------------------------
ThrusterPanel::ThrusterPanel(wxWindow *parent, Spacecraft *spacecraft, 
                             wxButton *theApplyButton):wxPanel(parent)
{
    thrusterCount = 0;
    tankCount = 0;
    coordsysCount = 0;
    
    this->theSpacecraft = spacecraft;
    this->theThruster = NULL;
    this->theApplyButton = theApplyButton;
    Create();
}

//------------------------------------------------------------------------------
// ~ThrusterPanel()
//------------------------------------------------------------------------------
ThrusterPanel::~ThrusterPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterPanel::Create()
{
    // Integer
    Integer bsize = 3; // border size
    
    
    // wxString
    wxString emptyList[] = {};
    
    // wxButton
    addButton = new wxButton( this, ID_BUTTON, wxT("Add Thruster"),
                              wxDefaultPosition, wxDefaultSize, 0 );
    removeButton = new wxButton( this, ID_BUTTON, wxT("Remove Thruster"),
                              wxDefaultPosition, wxDefaultSize, 0 );
    cCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit C Coef."),
                              wxDefaultPosition, wxDefaultSize, 0 );
    kCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit K Coef."),
                              wxDefaultPosition, wxDefaultSize, 0 );  
                                
    // wxComboBox 
    tankComboBox = new wxComboBox( this, ID_COMBO, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), tankCount,
                      emptyList, wxCB_DROPDOWN|wxCB_READONLY );;
    coordsysComboBox = new wxComboBox( this, ID_COMBO, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), coordsysCount,
                      emptyList, wxCB_DROPDOWN|wxCB_READONLY );;
   
    // wxTextCtrl
    XTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    YTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    ZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
                            
    //wxStaticText
    tankStaticText = new wxStaticText( this, ID_TEXT, wxT("Tank"),
                            wxDefaultPosition,wxDefaultSize, 0);
    coordsysStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coordinate System"), wxDefaultPosition,
                            wxDefaultSize, 0);
    XStaticText = new wxStaticText( this, ID_TEXT, wxT("X Direction"),
                            wxDefaultPosition,wxDefaultSize, 0);
    YStaticText = new wxStaticText( this, ID_TEXT, wxT("Y Direction"),
                            wxDefaultPosition,wxDefaultSize, 0);  
    ZStaticText = new wxStaticText( this, ID_TEXT, wxT("Z Direction"),
                            wxDefaultPosition,wxDefaultSize, 0); 
                            
    // wxListBox
    thrusterListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,200), thrusterCount, emptyList, wxLB_SINGLE);
    
    // wx*Sizers                                            
    wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   
    wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Thruster List") );
    wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
    wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Thruster Parameters") );
    wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );

    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
    wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 0 );
    
    // Add to wx*Sizers 
    flexGridSizer1->Add(tankStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(tankComboBox, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(coordsysStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(coordsysComboBox, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(XStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(XTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(YStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(YTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(ZStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(ZTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add(cCoefButton, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(kCoefButton, 0, wxALIGN_CENTRE|wxALL, bsize );
    
    boxSizer2->Add(addButton, 0, wxALIGN_CENTER|wxALL, bsize );
    boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
    
    boxSizer3->Add(flexGridSizer1, 0, wxALIGN_CENTER|wxALL, bsize );
    
    boxSizer4->Add(thrusterListBox, 0, wxALIGN_CENTER|wxALL, bsize );
    boxSizer4->Add(boxSizer2, 0, wxALIGN_CENTER|wxALL, bsize );
        
    staticBoxSizer1->Add( boxSizer4, 0, wxALIGN_CENTER|wxALL, bsize ); 
    staticBoxSizer2->Add( boxSizer3, 0, wxALIGN_CENTER|wxALL, bsize );
   
    flexGridSizer2->Add( staticBoxSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer2->Add( staticBoxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);
   
    boxSizer1->Add( flexGridSizer2, 0, wxGROW, bsize);
    
    this->SetAutoLayout( true );  
    this->SetSizer( boxSizer1 );
    boxSizer1->Fit( this );
    boxSizer1->SetSizeHints( this );
    
    // Temp
    addButton->Enable(false);
    removeButton->Enable(false);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterPanel::LoadData()
{  
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return; 
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void ThrusterPanel::OnTextChange()
{
    theApplyButton->Enable();
}
