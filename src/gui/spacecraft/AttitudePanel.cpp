//$Header: 
//------------------------------------------------------------------------------
//                            AttitudePanel
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
// Created: 2006/03/01
/**
 * This class contains information needed to setup users spacecraft attitude
 * parameters.
 */
//------------------------------------------------------------------------------
#include "AttitudePanel.hpp"
#include "MessageInterface.hpp"
#include "GmatAppData.hpp"

//#define DEBUG_ATTITUDE_PANEL 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(AttitudePanel, wxPanel)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// AttitudePanel(wxWindow *parent, Spacecraft *spacecraft, wxButton *theApplyButton)
//------------------------------------------------------------------------------
/**
 * Constructs AttitudePanel object.
 */
//------------------------------------------------------------------------------
AttitudePanel::AttitudePanel(wxWindow *parent, Spacecraft *spacecraft,
                     wxButton *theApplyButton):wxPanel(parent)
{
	#if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::AttitudePanel() entered\n");
   #endif
   
    this->theSpacecraft = spacecraft;
    this->theApplyButton = theApplyButton;
    
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    theGuiManager = GuiItemManager::GetInstance();
    
    Create();
}

//------------------------------------------------------------------------------
// ~AttitudePanel()
//------------------------------------------------------------------------------
AttitudePanel::~AttitudePanel()
{
	#if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::~AttitudePanel() entered\n");
   #endif
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void AttitudePanel::Create()
{ 
	#if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() entered\n");
   #endif
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxStaticText objects.\n");
   #endif
   
	//wxStaticText
	config1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Mode"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
    config2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Coordinate System"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
    config3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Kinematic Attitude Type"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
    config4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Euler Angle Sequence"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
    state1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Initial Conditions"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
   state1StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, 
                       wxFONTSTYLE_NORMAL, true, _T(""), wxFONTENCODING_SYSTEM));
   state24StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Rate Initial Conditions"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
   state24StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, 
                         wxFONTSTYLE_NORMAL, true, _T(""), wxFONTENCODING_SYSTEM));
   stateTypeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude State Type"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
   stateTypeRate4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Rate State Type"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxString objects\n");
   #endif
                        
   //wxString
   wxString emptyArray[] =
   {
   };
   
   Integer defaultCount = 0;
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxComboBox objects.\n");
   #endif
   
   //wxComboBox
   config1ComboBox = new wxComboBox( this, ID_CB, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   config2ComboBox = new wxComboBox( this, ID_CB, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   config3ComboBox = new wxComboBox( this, ID_CB, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   config4ComboBox = new wxComboBox( this, ID_CB, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   stateTypeComboBox = new wxComboBox( this, ID_CB, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   stateTypeRateComboBox = new wxComboBox( this, ID_CB, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
                        
   Integer bsize = 6; // border size
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxBoxSizer objects.\n");
   #endif
   
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer5 = new wxBoxSizer( wxVERTICAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Adding objects to wxSizers.\n");
   #endif
   
   // Add to wx*Sizers
   flexGridSizer1->Add(config1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config1ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config2ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config3ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config4StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config4ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer4->Add(state1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer4->Add(stateTypeStaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer4->Add(stateTypeComboBox , 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);
   
   boxSizer5->Add(state24StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer5->Add(stateTypeRate4StaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer5->Add(stateTypeRateComboBox, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);
   
   boxSizer2->Add(flexGridSizer1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer3->Add(boxSizer4, 0,wxGROW| wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add( 20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   boxSizer3->Add(boxSizer5, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   boxSizer1->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   boxSizer1->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() exiting\n");
   #endif
}    

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AttitudePanel::LoadData()
{
	#if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::LoadData() entered\n");
   #endif
    return;
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AttitudePanel::SaveData()
{
	#if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::SaveData() entered\n");
   #endif
   
   if (!theApplyButton->IsEnabled())
       return;
}    
