//$Header$
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
   EVT_TEXT(ID_TEXTCTRL_ST, AttitudePanel::OnStateTypeTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_STR, AttitudePanel::OnStateTypeRateTextUpdate)
   EVT_COMBOBOX(ID_CB_CONFIG, AttitudePanel::OnConfigurationSelection)
   EVT_COMBOBOX(ID_CB_ST, AttitudePanel::OnStateTypeSelection)
   EVT_COMBOBOX(ID_CB_STR, AttitudePanel::OnStateTypeRateSelection)
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
    
   modeArray.clear();
   coordSysArray.clear();
   kinematicArray.clear();
   eulerAngleArray.clear();
   stateTypeArray.clear();
   stateTypeRateArray.clear();
    
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
                        wxDefaultPosition, wxSize(200,20), 0);
    config2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Coordinate System"),
                        wxDefaultPosition, wxSize(200,20), 0);
    config3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Kinematic Attitude Type"),
                        wxDefaultPosition, wxSize(200,20), 0);
    config4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Euler Angle Sequence"),
                        wxDefaultPosition, wxSize(200,20), 0);
    stateTypeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude State Type"),
                        wxDefaultPosition, wxSize(200,20), 0);
    stateTypeRate4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Rate State Type"),
                        wxDefaultPosition, wxSize(200,20), 0);
    st1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 1"),
                        wxDefaultPosition, wxSize(80,20), 0);
    st2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 2"),
                        wxDefaultPosition, wxSize(80,20), 0);
    st3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 3"),
                        wxDefaultPosition, wxSize(80,20), 0);
    st4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 4"),
                        wxDefaultPosition, wxSize(80,20), 0);
    col1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Column 1"),
                        wxDefaultPosition, wxSize(80,20), 0);
    col2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Column 2"),
                        wxDefaultPosition, wxSize(80,20), 0);
    col3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Column 3"),
                        wxDefaultPosition, wxSize(80,20), 0);
    str1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 1"),
                        wxDefaultPosition, wxSize(125,20), 0);
    str2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 2"),
                        wxDefaultPosition, wxSize(125,20), 0);
    str3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 3"),
                        wxDefaultPosition, wxSize(125,20), 0);
                                            
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxTextCtrl objects\n");
   #endif
   
   //wxTextCtrl
   st1TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st4TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st5TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st6TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st7TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st8TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st9TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st10TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );  
   str1TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(100,-1), 0 );
   str2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(100,-1), 0 );
   str3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_ST, wxT(""),
                      wxDefaultPosition, wxSize(100,-1), 0 );                
                                                                       
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxString objects\n");
   #endif
                        
   //StringArray Initialization
   stateTypeArray.push_back("Euler Angles");
   stateTypeArray.push_back("Quaternions");
   stateTypeArray.push_back("DCM");
   stateTypeRateArray.push_back("Euler Angles Rates");
   stateTypeRateArray.push_back("Angular Velocity");
   
   //wxString
   wxString emptyArray[] = {};
   
   wxString *stArray = new wxString[StateTypeCount];
   for (Integer i=0; i<StateTypeCount; i++)
      stArray[i] = stateTypeArray[i].c_str();
     
   wxString *strArray = new wxString[RateStateTypeCount];
   for (Integer i=0; i<RateStateTypeCount; i++)
      strArray[i] = stateTypeRateArray[i].c_str();
   
   Integer defaultCount = 0;
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxComboBox objects.\n");
   #endif
   
   //wxComboBox
   config1ComboBox = new wxComboBox( this, ID_CB_CONFIG, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   config2ComboBox = new wxComboBox( this, ID_CB_CONFIG, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   config3ComboBox = new wxComboBox( this, ID_CB_CONFIG, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   config4ComboBox = new wxComboBox( this, ID_CB_CONFIG, wxT(""),
                      wxDefaultPosition, wxDefaultSize, defaultCount,
                      emptyArray, wxCB_DROPDOWN|wxCB_READONLY );
   stateTypeComboBox = new wxComboBox( this, ID_CB_ST, wxT(stArray[0]),
                      wxDefaultPosition, wxSize(180,20), StateTypeCount,
                      stArray, wxCB_DROPDOWN|wxCB_READONLY );
   stateTypeRateComboBox = new wxComboBox( this, ID_CB_STR, wxT(strArray[0]),
                      wxDefaultPosition, wxSize(180,20), RateStateTypeCount,
                      strArray, wxCB_DROPDOWN|wxCB_READONLY );                  
                      
   Integer bsize = 6; // border size
                            
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() Creating wxBoxSizer objects.\n");
   #endif
   
#if __WXMAC__   
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer5 = new wxBoxSizer( wxVERTICAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 4, 0, 0 );
   wxFlexGridSizer *flexGridSizer3 = new wxFlexGridSizer( 2, 0, 0 );
   
   wxStaticText *state1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Initial Conditions"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);
   wxStaticText *state24StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Rate Initial Conditions"),
                        wxDefaultPosition, wxSize(200,20), wxST_NO_AUTORESIZE);   
   state1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
   state24StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
                       
   // Add to wx*Sizers
   flexGridSizer1->Add(config1StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config1ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config2StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config2ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config3StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config3ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config4StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config4ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add(col1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(col2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(col3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
      
   flexGridSizer2->Add(st1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st5TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st6TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(st2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st7TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st8TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(st3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st9TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st10TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(st4StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st4TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   flexGridSizer3->Add(str1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer4->Add(state1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer4->Add(stateTypeStaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer4->Add(stateTypeComboBox , 0, wxALIGN_LEFT|wxALL, bsize);  
   boxSizer4->Add(flexGridSizer2, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);  
   
   boxSizer5->Add(state24StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer5->Add(stateTypeRate4StaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer5->Add(stateTypeRateComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add(flexGridSizer3, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);  
   
   boxSizer2->Add(flexGridSizer1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer3->Add(boxSizer4, 0,wxGROW| wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add(boxSizer5, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   boxSizer1->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   boxSizer1->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
#else
   // wx*Sizers   
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxVERTICAL );
   
   wxStaticBox *staticBox1 = new wxStaticBox( this, -1, wxT("Attitude Initial Conditions") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
   wxStaticBox *staticBox2 = new wxStaticBox( this, -1, wxT("Attitude Rate Initial Conditions") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 4, 0, 0 );
   wxFlexGridSizer *flexGridSizer3 = new wxFlexGridSizer( 2, 0, 0 );

   // Add to wx*Sizers
   flexGridSizer1->Add(config1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config1ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config2ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config3ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config4StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(config4ComboBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add(col1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(col2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(col3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
      
   flexGridSizer2->Add(st1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st5TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st6TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(st2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st7TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st8TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(st3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st9TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st10TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   flexGridSizer2->Add(st4StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st4TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   flexGridSizer3->Add(str1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   staticBoxSizer1->Add(stateTypeStaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   staticBoxSizer1->Add(stateTypeComboBox , 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize); 
   staticBoxSizer1->Add(flexGridSizer2, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize); 
   
   staticBoxSizer2->Add(stateTypeRate4StaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   staticBoxSizer2->Add(stateTypeRateComboBox, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);
   staticBoxSizer2->Add(flexGridSizer3, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);  
   
   boxSizer2->Add(flexGridSizer1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer3->Add(staticBoxSizer1, 0,wxGROW| wxALIGN_CENTER|wxALL, bsize);
   boxSizer3->Add(staticBoxSizer2, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   boxSizer1->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   boxSizer1->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
#endif
   
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );
   
   #if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::Create() exiting\n");
   #endif
}    

//------------------------------------------------------------------------------
// void DisplayEulerAngles()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayEulerAngles()
{
	col1StaticText->Show(false);
   col2StaticText->Show(false);
   col3StaticText->Show(false);
   
   st1StaticText->Show(true);
   st2StaticText->Show(true);
   st3StaticText->Show(true);
   st4StaticText->Show(false);
   
   st1TextCtrl->Show(true);
   st2TextCtrl->Show(true);
   st3TextCtrl->Show(true);
   st4TextCtrl->Show(false);
   
   st5TextCtrl->Show(false);
   st6TextCtrl->Show(false);
   st7TextCtrl->Show(false);
   
   st8TextCtrl->Show(false);
   st9TextCtrl->Show(false);
   st10TextCtrl->Show(false);
   
   st1StaticText->SetLabel(wxT("Euler Angle 1"));
   st2StaticText->SetLabel(wxT("Euler Angle 2"));
   st3StaticText->SetLabel(wxT("Euler Angle 3"));
}

//------------------------------------------------------------------------------
// void DisplayQuaternions()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayQuaternions()
{
	col1StaticText->Show(false);
   col2StaticText->Show(false);
   col3StaticText->Show(false);
   
   st1StaticText->Show(true);
   st2StaticText->Show(true);
   st3StaticText->Show(true);
   st4StaticText->Show(true);
   
   st1TextCtrl->Show(true);
   st2TextCtrl->Show(true);
   st3TextCtrl->Show(true);
   st4TextCtrl->Show(true);
   
   st5TextCtrl->Show(false);
   st6TextCtrl->Show(false);
   st7TextCtrl->Show(false);
   
   st8TextCtrl->Show(false);
   st9TextCtrl->Show(false);
   st10TextCtrl->Show(false);
   
   st1StaticText->SetLabel(wxT("q1"));
   st2StaticText->SetLabel(wxT("q2"));
   st3StaticText->SetLabel(wxT("q3"));
   st4StaticText->SetLabel(wxT("q4"));
}

//------------------------------------------------------------------------------
// void DisplayDCM()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayDCM()
{
	col1StaticText->Show(true);
   col2StaticText->Show(true);
   col3StaticText->Show(true);
   
   st1StaticText->Show(true);
   st2StaticText->Show(true);
   st3StaticText->Show(true);
   st4StaticText->Show(false);
   
   st1TextCtrl->Show(true);
   st2TextCtrl->Show(true);
   st3TextCtrl->Show(true);
   st4TextCtrl->Show(false);
   
   st5TextCtrl->Show(true);
   st6TextCtrl->Show(true);
   st7TextCtrl->Show(true);
   
   st8TextCtrl->Show(true);
   st9TextCtrl->Show(true);
   st10TextCtrl->Show(true);
   
   st1StaticText->SetLabel(wxT("Row 1"));
   st2StaticText->SetLabel(wxT("Row 2"));
   st3StaticText->SetLabel(wxT("Row 3"));
}
   
//------------------------------------------------------------------------------
// void DisplayEulerAngleRates()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayEulerAngleRates()
{
	str1StaticText->SetLabel(wxT("Euler Angle Rate 1"));
   str2StaticText->SetLabel(wxT("Euler Angle Rate 2"));
   str3StaticText->SetLabel(wxT("Euler Angle Rate 3"));
}

//------------------------------------------------------------------------------
// void DisplayAngularVelocity()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayAngularVelocity()
{
	str1StaticText->SetLabel(wxT("Angular Velocity X"));
   str2StaticText->SetLabel(wxT("Angular Velocity Y"));
   str3StaticText->SetLabel(wxT("Angular Velocity Z"));
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AttitudePanel::LoadData()
{
	#if DEBUG_ATTITUDE_PANEL
   MessageInterface::ShowMessage("AttitudePanel::LoadData() entered\n");
   #endif
   
   //DisplayEulerAngles();
   DisplayDCM();
   DisplayEulerAngleRates();
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

//------------------------------------------------------------------------------
// void OnStateTypeTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeTextUpdate(wxCommandEvent &event)
{
}

//------------------------------------------------------------------------------
// void OnStateTypeRateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeRateTextUpdate(wxCommandEvent &event)
{
}

//------------------------------------------------------------------------------
// void OnConfigurationSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnConfigurationSelection(wxCommandEvent &event)
{
}

//------------------------------------------------------------------------------
// void OnStateTypeSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeSelection(wxCommandEvent &event)
{
	std::string stateTypeStr = stateTypeComboBox->GetStringSelection().c_str();
	
	if (stateTypeStr == stateTypeArray[EULER_ANGLES])
	   DisplayEulerAngles();
	else if (stateTypeStr == stateTypeArray[QUATERNIONS])
	   DisplayQuaternions();
	else if (stateTypeStr == stateTypeArray[DCM])
	   DisplayDCM();
}

//------------------------------------------------------------------------------
// void OnStateTypeRateSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeRateSelection(wxCommandEvent &event)
{
		std::string stateTypeRateStr = stateTypeRateComboBox->GetStringSelection().c_str();
	
	if (stateTypeRateStr == stateTypeRateArray[EULER_ANGLES_RATES])
	   DisplayEulerAngleRates();
	else if (stateTypeRateStr == stateTypeRateArray[ANGULAR_VELOCITY])
	   DisplayAngularVelocity();
}


