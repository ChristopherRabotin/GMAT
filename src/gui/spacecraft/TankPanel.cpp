//$Header:
//------------------------------------------------------------------------------
//                            TankPanel
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
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "TankPanel.hpp"
#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(TankPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, TankPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TankPanel(wxWindow *parent, Spacecraft *spacecraft, wxButton *theApplyButton)
//------------------------------------------------------------------------------
/**
 * Constructs TankPanel object.
 */
//------------------------------------------------------------------------------
TankPanel::TankPanel(wxWindow *parent, Spacecraft *spacecraft,
                     wxButton *theApplyButton):wxPanel(parent)
{
    this->theSpacecraft = spacecraft;
    this->theFuelTank = NULL;
    this->theApplyButton = theApplyButton;
    
    tankCount = 0;
    
    Create();
}

//------------------------------------------------------------------------------
// ~TankPanel()
//------------------------------------------------------------------------------
TankPanel::~TankPanel()
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
void TankPanel::Create()
{
   // wxString
   wxString emptyList[] = {};
   
   // wxButton
   addButton = new wxButton( this, ID_BUTTON, wxT("Add Tank"),
                             wxDefaultPosition, wxDefaultSize, 0 );
   removeButton = new wxButton( this, ID_BUTTON, wxT("Remove Tank"),
                             wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxTextCtrl
   temperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   refTemperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   fuelMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   fuelDensityTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   pressureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   volumeTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    
   //wxStaticText
   temperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Temperature"),
                            wxDefaultPosition,wxDefaultSize, 0);
   refTemperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Reference Temperature"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelMassStaticText = new wxStaticText( this, ID_TEXT, wxT("Furel Mass"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelDensityStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Density"),
                            wxDefaultPosition,wxDefaultSize, 0);
   pressureStaticText = new wxStaticText( this, ID_TEXT, wxT("Pressure"),
                            wxDefaultPosition,wxDefaultSize, 0);
   volumeStaticText = new wxStaticText( this, ID_TEXT, wxT("Volume"),
                            wxDefaultPosition,wxDefaultSize, 0);  
   unit1StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit2StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit3StaticText = new wxStaticText( this, ID_TEXT, wxT("kg"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit4StaticText = new wxStaticText( this, ID_TEXT, wxT("kg/m^3"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit5StaticText = new wxStaticText( this, ID_TEXT, wxT("kPa"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit6StaticText = new wxStaticText( this, ID_TEXT, wxT("m^3"),
                            wxDefaultPosition,wxDefaultSize, 0); 
                            
   // wxListBox
   tankListBox = new wxListBox(this, ID_LISTBOX, wxDefaultPosition,
                    wxSize(150,250), tankCount, emptyList, wxLB_SINGLE);
                            
   Integer bsize = 3; // border size
   
    // wx*Sizers      
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
   
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Tank List") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Tank Parameters") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 0 );

   // Add to wx*Sizers 
   flexGridSizer1->Add(temperatureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(temperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit1StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(refTemperatureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(refTemperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit2StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelMassStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelMassTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit3StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelDensityStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelDensityTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit4StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(pressureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(pressureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit5StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(volumeStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(volumeTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit6StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer2->Add(addButton, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer2->Add(removeButton, 0, wxALIGN_CENTER|wxALL, bsize );
    
   boxSizer3->Add(flexGridSizer1, 0, wxALIGN_CENTER|wxALL, bsize );
   boxSizer3->Add(boxSizer2, 0, wxALIGN_CENTER|wxALL, bsize );
      
   staticBoxSizer1->Add( tankListBox, 0, wxALIGN_CENTER|wxALL, bsize ); 
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
void TankPanel::LoadData()
{  
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankPanel::SaveData()
{
    if (!theApplyButton->IsEnabled())
       return;
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void TankPanel::OnTextChange()
{
    theApplyButton->Enable();
}
