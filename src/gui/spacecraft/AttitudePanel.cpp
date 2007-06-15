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
// Author: Waka Waktola (and heavily modified by Wendy C. Shoan)
// Created: 2006/03/01 (2007.06.12)
/**
 * This class contains information needed to setup users spacecraft attitude
 * parameters.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "AttitudePanel.hpp"
#include "AttitudeFactory.hpp"
#include "MessageInterface.hpp"
#include "GmatAppData.hpp"
#include "GmatBaseException.hpp"

// initial selections in combo boxes
#define STARTUP_STATE_TYPE_SELECTION            EULER_ANGLES
#define STARTUP_RATE_STATE_TYPE_SELECTION       EULER_ANGLE_RATES

//#define DEBUG_ATTITUDE_PANEL 1


//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(AttitudePanel, wxPanel)
//   EVT_COMBOBOX(ID_CB_ST, AttitudePanel::OnStateTypeTextUpdate)
//   EVT_COMBOBOX(ID_CB_STR, AttitudePanel::OnStateTypeRateTextUpdate)
   EVT_COMBOBOX(ID_CB_COORDSYS, AttitudePanel::OnConfigurationSelection)
   EVT_COMBOBOX(ID_CB_MODE, AttitudePanel::OnAttitudeModelSelection)
   EVT_COMBOBOX(ID_CB_ST, AttitudePanel::OnStateTypeSelection)
   EVT_COMBOBOX(ID_CB_STR, AttitudePanel::OnStateTypeRateSelection)
   EVT_COMBOBOX(ID_CB_EAS, AttitudePanel::OnEulerSequenceSelection)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// AttitudePanel(GmatPanel *scPanel, xWindow *parent, Spacecraft *spacecraft)
//------------------------------------------------------------------------------
/**
 * Constructs AttitudePanel object.
 */
//------------------------------------------------------------------------------
AttitudePanel::AttitudePanel(GmatPanel *scPanel, wxWindow *parent,
                             Spacecraft *spacecraft)
   : wxPanel(parent), dontUpdate(false)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::AttitudePanel() entered\n");
   #endif

   theScPanel = scPanel;
   theSpacecraft = spacecraft;
   
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   modelArray.clear();
   coordSysArray.clear();
   kinematicArray.clear();
   eulerAngleArray.clear();
   stateTypeArray.clear();
   stateTypeRateArray.clear();
   
   unsigned int defSeq[3] = {3, 1, 2};
   seq.push_back(defSeq[0]);
   seq.push_back(defSeq[1]);
   seq.push_back(defSeq[2]);
   
   attStateType        = "";
   attRateStateType    = "";
   attitudeModel       = "";
   attitudeType        = "";  // currently not used
   attCoordSystem      = "";
   eulerSequence       = "312";

   dataChanged = false;
   Create();
}

//------------------------------------------------------------------------------
// ~AttitudePanel()
//------------------------------------------------------------------------------
AttitudePanel::~AttitudePanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", config2ComboBox);

   #ifdef DEBUG_ATTITUDE_PANEL
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
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::Create() entered\n");
   #endif
   
   // arrays to hold temporary values
   unsigned int x;
   for (x = 0; x < 3; ++x)
   {
      eulerAngles[x] = new wxString();
      eulerAngleRates[x] = new wxString();
      quaternion[x] = new wxString();
      cosineMatrix[x] = new wxString();
      angVel[x] = new wxString();
   }
   quaternion[3] = new wxString();
   cosineMatrix[3] = new wxString();
   for (x = 4; x < 9; ++x)
   {
      cosineMatrix[x] = new wxString();
   }
      
   // gt list of models and put them into the combo box
   modelArray = theGuiInterpreter->GetListOfFactoryItems(Gmat::ATTITUDE);
   unsigned int modelSz = modelArray.size();
   attitudeModelArray = new wxString[modelSz];
   for (x = 0; x < modelSz; ++x)
      attitudeModelArray[x] = wxT(modelArray[x].c_str());
   //attitudeModelArray[0] = "CoordinateSystemFixed";
   //attitudeModelArray[1] = "Spinner";

   config1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Model"),
                        wxDefaultPosition, wxDefaultSize, 0);
   config1ComboBox = 
      new wxComboBox( this, ID_CB_MODE, wxT(attitudeModelArray[0]), 
         wxDefaultPosition, wxDefaultSize, modelSz, attitudeModelArray, 
         wxCB_DROPDOWN|wxCB_READONLY );

   // Coordinate System
   config2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Coordinate System"),
         wxDefaultPosition, wxDefaultSize, 0);
   config2ComboBox =  theGuiManager->GetCoordSysComboBox(this, ID_CB_COORDSYS, 
      wxDefaultSize);

   //Euler Angle Sequence
   StringArray eulerSequenceStringArray = Attitude::GetEulerSequenceStrings();
   Integer eulerSeqSz = eulerSequenceStringArray.size();
   
   wxString *estArray = new wxString[eulerSeqSz];  // Euler sequence types
   for (Integer i=0; i<eulerSeqSz; i++)
      estArray[i] = eulerSequenceStringArray[i].c_str();

   config4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Euler Angle Sequence"),
                        wxDefaultPosition, wxDefaultSize, 0);
   config4ComboBox = new wxComboBox( this, ID_CB_EAS, wxT(estArray[0]),
                      wxDefaultPosition, wxDefaultSize, 12,
                      estArray, wxCB_DROPDOWN|wxCB_READONLY );

   // State Type
   stateTypeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude State Type"),
                        wxDefaultPosition, wxDefaultSize, 0);

   stateTypeArray.push_back("EulerAngles");
   stateTypeArray.push_back("Quaternion");
   stateTypeArray.push_back("DirectionCosineMatrix");

   wxString *stArray = new wxString[StateTypeCount];
   for (Integer i=0; i<StateTypeCount; i++)
      stArray[i] = stateTypeArray[i].c_str();

   stateTypeComboBox = 
      new wxComboBox( this, ID_CB_ST, wxT(stArray[STARTUP_STATE_TYPE_SELECTION]),
         wxDefaultPosition, wxSize(180,20), StateTypeCount, stArray, 
         wxCB_DROPDOWN|wxCB_READONLY );

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

   // Rate State Type
   stateTypeRate4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Rate State Type"),
                        wxDefaultPosition, wxDefaultSize, 0);

   stateTypeRateArray.push_back("EulerAngleRates");
   stateTypeRateArray.push_back("AngularVelocity");
   
   wxString *strArray = new wxString[RateStateTypeCount];
   for (Integer i=0; i<RateStateTypeCount; i++)
      strArray[i] = stateTypeRateArray[i].c_str();

   stateTypeRateComboBox =
      new wxComboBox( this, ID_CB_STR, 
         wxT(strArray[STARTUP_RATE_STATE_TYPE_SELECTION]), wxDefaultPosition, 
         wxSize(180,20), RateStateTypeCount, strArray, 
         wxCB_DROPDOWN|wxCB_READONLY );                  

   str1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 1"), wxDefaultPosition, 
         wxSize(125,20), 0);
   str2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 2"), wxDefaultPosition, 
         wxSize(125,20), 0);
   str3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("String 3"), wxDefaultPosition, 
         wxSize(125,20), 0);

   str1TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STR, wxT(""), wxDefaultPosition, 
         wxSize(100,-1), 0 );
   str2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STR, wxT(""), wxDefaultPosition, 
         wxSize(100,-1), 0 );
   str3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STR, wxT(""), wxDefaultPosition, 
         wxSize(100,-1), 0 );                

   rateUnits1 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));
   rateUnits2 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));
   rateUnits3 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));

   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("AttitudePanel::Create() Creating wxTextCtrl objects\n");
   #endif
   
   //wxTextCtrl
                  

    attUnits1 = new wxStaticText(this,ID_TEXT,wxT("deg"));
    attUnits2 = new wxStaticText(this,ID_TEXT,wxT("deg"));
    attUnits3 = new wxStaticText(this,ID_TEXT,wxT("deg"));



   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("AttitudePanel::Create() Creating wxString objects\n");
   #endif

                      
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage(
         "AttitudePanel::Create() Creating wxBoxSizer objects.\n");
   #endif
   
#if __WXMAC__   
   Integer bsize = 6; // border size
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
   //flexGridSizer1->Add(config3StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   //flexGridSizer1->Add(config3ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );        
   flexGridSizer1->Add(config4StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config4ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );         
   
   flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add(col1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(col2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(col3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );         
      
   flexGridSizer2->Add(st1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st1TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st5TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st8TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );          
   
   flexGridSizer2->Add(st2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st6TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st9TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );        
   
   flexGridSizer2->Add(st3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer2->Add(st7TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
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
        
   
   this->SetAutoLayout( true );  
   this->SetSizer( boxSizer1 );
   boxSizer1->Fit( this );
   boxSizer1->SetSizeHints( this );        
   
#else  
   Integer bsize = 3;
   Integer text_bsize = 1;
  
   wxBoxSizer *bSMain = new wxBoxSizer( wxVERTICAL );  
   
   wxBoxSizer *bSInitCond = new wxBoxSizer( wxHORIZONTAL );
   
   wxGridSizer *gSConfig = new wxGridSizer( 2 );  // four rows by two columns
   
   wxStaticBox *staticBoxAttInitCond = new wxStaticBox( this, -1, wxT("Attitude Initial Conditions") );
   wxStaticBoxSizer *sBSAttInitCond = new wxStaticBoxSizer( staticBoxAttInitCond, wxVERTICAL );
   wxStaticBox *staticBoxRateInitCond = new wxStaticBox( this, -1, wxT("Attitude Rate Initial Conditions") );
   wxStaticBoxSizer *sBSRateInitCond = new wxStaticBoxSizer( staticBoxRateInitCond, wxVERTICAL );
   
   wxGridSizer *gSAttState = new wxGridSizer( 4 );  // four by four
/* Alternate layout in the works
   wxFlexGridSizer *gSAttState = new wxFlexGridSizer( 2 );  // two by two
   wxBoxSizer *bSColumnLabels = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *bSRowLabels = new wxBoxSizer( wxVERTICAL );
   wxFlexGridSizer *fGSAttEntriesAndUnits = new wxFlexGridSizer( 4 );  // four by four
   */
   wxFlexGridSizer *gSRateState = new wxFlexGridSizer( 3 );  // three rows by three columns
   /*
   bSColumnLabels->Add(col1StaticText, 0, wxALL, text_bsize);
   bSColumnLabels->Add(col2StaticText, 0, wxALL, text_bsize);
   bSColumnLabels->Add(col3StaticText, 0, wxALL, text_bsize);

   bSRowLabels->Add( st1StaticText, 0, wxLEFT, text_bsize);
   bSRowLabels->Add( st2StaticText, 0, wxLEFT, text_bsize);
   bSRowLabels->Add( st3StaticText, 0, wxLEFT, text_bsize);
   bSRowLabels->Add( st4StaticText, 0, wxLEFT, text_bsize);
   
   fGSAttEntriesAndUnits->Add(st1TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st5TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st8TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(attUnits1, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st2TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st6TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st9TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(attUnits2, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st3TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st7TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st10TextCtrl, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(attUnits3, 0, wxALL, text_bsize);
   fGSAttEntriesAndUnits->Add(st4TextCtrl, 0, wxALL, text_bsize);

   gSAttState->AddSpacer(20);
   gSAttState->Add(bSColumnLabels, 0, wxALL, text_bsize);
   gSAttState->Add(bSRowLabels, 0, wxALL, text_bsize);
   gSAttState->Add(fGSAttEntriesAndUnits, 0, wxALL, text_bsize);
   */
   
   gSAttState->AddSpacer(20);  // blank space
   gSAttState->Add(col1StaticText, 0, wxALL, text_bsize);
   gSAttState->Add(col2StaticText, 0, wxALL, text_bsize);
   gSAttState->Add(col3StaticText, 0, wxALL, text_bsize);
   gSAttState->Add( st1StaticText, 0, wxLEFT, text_bsize);
   gSAttState->Add(   st1TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add(   st5TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add(   st8TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add( st2StaticText, 0, wxLEFT, text_bsize);
   gSAttState->Add(   st2TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add(   st6TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add(   st9TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add( st3StaticText, 0, wxLEFT, text_bsize);
   gSAttState->Add(   st3TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add(   st7TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add(  st10TextCtrl, 0, wxALL, text_bsize);
   gSAttState->Add( st4StaticText, 0, wxLEFT, text_bsize);
   gSAttState->Add(   st4TextCtrl, 0, wxALL, text_bsize);
   
   gSRateState->Add(str1StaticText, 0, wxALL, bsize);
   gSRateState->Add(str1TextCtrl, 0, wxALL, bsize);
   //gSRateState->Add(rateUnits1, 0, wxALL, bsize);
   gSRateState->Add(str2StaticText, 0, wxALL, bsize);
   gSRateState->Add(str2TextCtrl, 0, wxALL, bsize);
   //gSRateState->Add(rateUnits2, 0, wxALL, bsize);
   gSRateState->Add(str3StaticText, 0, wxALL, bsize);
   gSRateState->Add(str3TextCtrl, 0, wxALL, bsize);
   //gSRateState->Add(rateUnits3, 0, wxALL, bsize);
   
   gSConfig->Add(config1StaticText, 0, wxALL, bsize);
   gSConfig->Add(  config1ComboBox, 0, wxEXPAND|wxALL, bsize);
   gSConfig->Add(config2StaticText, 0, wxALL, bsize);
   gSConfig->Add(  config2ComboBox, 0, wxEXPAND|wxALL, bsize);
//   gSConfig->Add(config3StaticText, 0, wxALL, bsize);
//   gSConfig->Add(  config3ComboBox, 0, wxEXPAND|wxALL, bsize);
   gSConfig->Add(config4StaticText, 0, wxALL, bsize);
   gSConfig->Add(  config4ComboBox, 0, wxEXPAND|wxALL, bsize);
   
   sBSAttInitCond->Add(stateTypeStaticText, 0, wxTOP|wxLEFT|wxRIGHT, bsize);
   sBSAttInitCond->Add(stateTypeComboBox, 0, wxALL, bsize);
   sBSAttInitCond->Add(gSAttState, 0, wxALL, bsize);

   sBSRateInitCond->Add(stateTypeRate4StaticText, 0, wxTOP|wxLEFT|wxRIGHT, bsize);
   sBSRateInitCond->Add(stateTypeRateComboBox, 0, wxALL, bsize);
   sBSRateInitCond->Add(gSRateState, 0, wxALL, bsize);

   bSInitCond->Add(sBSAttInitCond, 0, wxALL, bsize);
   bSInitCond->Add(sBSRateInitCond, 0, wxALL, bsize);
   
   bSMain->Add(gSConfig, 0, wxALIGN_CENTER|wxALL, bsize);
   bSMain->Add(bSInitCond, 0, wxALIGN_CENTER|wxALL, bsize);
   
   this->SetAutoLayout( true );  
   this->SetSizer( bSMain );
   bSMain->Fit( this );
   bSMain->SetSizeHints( this );
   
#endif
   
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::Create() exiting\n");
   #endif
}    

//------------------------------------------------------------------------------
// void DisplayEulerAngles()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayEulerAngles()
{
   stateTypeComboBox->
      SetValue(wxT("EulerAngles"));
   attStateType = "EulerAngles";
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

   dontUpdate=true;
   st1TextCtrl->SetValue(*eulerAngles[0]);
   st2TextCtrl->SetValue(*eulerAngles[1]);
   st3TextCtrl->SetValue(*eulerAngles[2]);
   dontUpdate=false;
}

//------------------------------------------------------------------------------
// void DisplayQuaternion()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayQuaternion()
{
   stateTypeComboBox->
      SetValue(wxT("Quaternion"));
   attStateType = "Quaternion";
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

   dontUpdate=true;
   st1TextCtrl->SetValue(*quaternion[0]);
   st2TextCtrl->SetValue(*quaternion[1]);
   st3TextCtrl->SetValue(*quaternion[2]);
   st4TextCtrl->SetValue(*quaternion[3]);
   dontUpdate=false;
}

//------------------------------------------------------------------------------
// void DisplayDCM()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayDCM()
{
   stateTypeComboBox->
      SetValue(wxT("DirectionCosineMatrix"));
   attStateType = "DirectionCosineMatrix";
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

   dontUpdate=true;
   /*
   st1TextCtrl->SetValue(*cosineMatrix[0]);
   st2TextCtrl->SetValue(*cosineMatrix[1]);
   st3TextCtrl->SetValue(*cosineMatrix[2]);
   st5TextCtrl->SetValue(*cosineMatrix[3]);
   st6TextCtrl->SetValue(*cosineMatrix[4]);
   st7TextCtrl->SetValue(*cosineMatrix[5]);
   st8TextCtrl->SetValue(*cosineMatrix[6]);
   st9TextCtrl->SetValue(*cosineMatrix[7]);
   st10TextCtrl->SetValue(*cosineMatrix[8]);
   */
   st1TextCtrl->SetValue(*cosineMatrix[0]);
   st2TextCtrl->SetValue(*cosineMatrix[3]);
   st3TextCtrl->SetValue(*cosineMatrix[6]);
   st5TextCtrl->SetValue(*cosineMatrix[1]);
   st6TextCtrl->SetValue(*cosineMatrix[4]);
   st7TextCtrl->SetValue(*cosineMatrix[7]);
   st8TextCtrl->SetValue(*cosineMatrix[2]);
   st9TextCtrl->SetValue(*cosineMatrix[5]);
   st10TextCtrl->SetValue(*cosineMatrix[8]);
   
   dontUpdate=false;
}


//------------------------------------------------------------------------------
// void DisplayEulerAngleRates()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayEulerAngleRates()
{
   stateTypeRateComboBox->
      SetValue(wxT("EulerAngleRates"));
   attRateStateType = "EulerAngleRates";
   str1StaticText->SetLabel(wxT("Euler Angle Rate 1"));
   str2StaticText->SetLabel(wxT("Euler Angle Rate 2"));
   str3StaticText->SetLabel(wxT("Euler Angle Rate 3"));

   dontUpdate=true;
   str1TextCtrl->SetValue(*eulerAngleRates[0]);
   str2TextCtrl->SetValue(*eulerAngleRates[1]);
   str3TextCtrl->SetValue(*eulerAngleRates[2]);
   dontUpdate=false;
}

//------------------------------------------------------------------------------
// void DisplayAngularVelocity()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayAngularVelocity()
{
   stateTypeRateComboBox->
      SetValue(wxT("AngularVelocity"));
   attRateStateType = "AngularVelocity";
   str1StaticText->SetLabel(wxT("Angular Velocity X"));
   str2StaticText->SetLabel(wxT("Angular Velocity Y"));
   str3StaticText->SetLabel(wxT("Angular Velocity Z"));

   dontUpdate=true;
   str1TextCtrl->SetValue(*angVel[0]);
   str2TextCtrl->SetValue(*angVel[1]);
   str3TextCtrl->SetValue(*angVel[2]);
   dontUpdate=false;
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void AttitudePanel::LoadData()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::LoadData() entered\n");
   #endif
   
   unsigned int x, y;
   // check to see if the spacecrat has an attitude object
   theAttitude = (Attitude*) theSpacecraft->GetRefObject(Gmat::ATTITUDE, "");
   if (theAttitude == NULL)   // no attitude yet
   {
      theAttitude = (Attitude *)theGuiInterpreter->
         CreateObject((attitudeModelArray[0]).c_str(), ""); // Use no name
   }
   if (theAttitude == NULL)
   {
      std::string ex = "ERROR- unable to find or create an attitude object for ";
      ex += theSpacecraft->GetName() + "\n";
      throw GmatBaseException(ex);
   }
   
   attStateType     = 
          theAttitude->GetStringParameter("AttitudeDisplayStateType");
   attRateStateType = 
          theAttitude->GetStringParameter("AttitudeRateDisplayStateType");
   attitudeModel    = theAttitude->GetAttitudeModelName();
   config1ComboBox->SetValue(wxT(attitudeModel.c_str()));
   
   eulerSequence  = theAttitude->GetStringParameter("EulerAngleSequence");
   config4ComboBox->SetValue(wxT(eulerSequence.c_str()));
   
   attCoordSystem = theAttitude->GetStringParameter("AttitudeCoordinateSystem");
   config2ComboBox->SetValue(wxT(attCoordSystem.c_str()));
   
   if (attStateType == "EulerAngles")
   {
      Rvector ea = theAttitude->GetRvectorParameter("EulerAngles");
      for (x = 0; x < 3; ++x)
         *eulerAngles[x] = theGuiManager->ToWxString(ea[x]);
      DisplayEulerAngles();
   }
   else if (attStateType == "Quaternion")
   {
      Rvector q = theAttitude->GetRvectorParameter("Quaternion");
      for (x = 0; x < 4; ++x)
         *quaternion[x] = theGuiManager->ToWxString(q[x]);
      DisplayQuaternion();
   }
   else // "DirectionCosineMatrix
   {
      Rmatrix mat = theAttitude->GetRmatrixParameter("DirectionCosineMatrix");
      for (x = 0; x < 3; ++x)
         for (y = 0; y < 3; ++y)
            *cosineMatrix[x*3+y] = theGuiManager->ToWxString(mat(x,y));
      DisplayDCM();
   }
   
   if (attRateStateType == "EulerAngleRates") 
   {
      Rvector ear = theAttitude->GetRvectorParameter("EulerAngleRates");
      for (x = 0; x < 3; ++x)
         *eulerAngleRates[x] = theGuiManager->ToWxString(ear[x]);
      DisplayEulerAngleRates();
   }
   else // AngularVelocity
   {
      Rvector av = theAttitude->GetRvectorParameter("AngularVelocity");
      for (x = 0; x < 3; ++x)
         *angVel[x] = theGuiManager->ToWxString(av[x]);
      DisplayAngularVelocity();
   }

   dataChanged = false;
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AttitudePanel::SaveData()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::SaveData() entered\n");
      MessageInterface::ShowMessage("Attitude creation via the GUI is not implemented yet\n");
   #endif
      
   wxString model = config1ComboBox->GetValue();
   //wxString typeVal = config3ComboBox->GetValue();
   
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("Attitude creation: model = %s\n", model.c_str());
      //MessageInterface::ShowMessage("Attitude creation: type = %s\n", typeVal.c_str());
   #endif
   AttitudeFactory af;
   
   //Attitude* a = af.CreateAttitude(model.c_str(), "");
   //Attitude* a = af.CreateAttitude(typeVal.c_str(), "");

   //if (a == NULL)
   //{
   //   MessageInterface::ShowMessage("Attitude object made is null\n");
   //   return;
   //}
   
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("Attitude creation: a != NULL\n");
   #endif

      //loj: 9/22/06 Commented out for now
//    try
//    {
//       theSpacecraft->SetRefObject(a, Gmat::ATTITUDE, "");
//    }
//    catch (BaseException &be)
//    {
//    #ifdef DEBUG_ATTITUDE_PANEL
//       MessageInterface::ShowMessage(
//       "ERROR setting attitude object on spacecraft object!!!\n");
//    #endif
//    }
   
   std::string selectedEulerSeq = config4ComboBox->GetValue().c_str();
   
   theAttitude->SetStringParameter("EulerAngleSequence", selectedEulerSeq);
           
   std::string stateTypeStr = stateTypeComboBox->GetStringSelection().c_str();
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("stateTypeStr = %s\n", stateTypeStr.c_str());
   #endif
   
   if (stateTypeStr == stateTypeArray[EULER_ANGLES])
   {
      Rvector3 eulerAnglesRvector;
      Real dEA;
      for (unsigned int x = 0; x < 3; ++x)
      {
         if (eulerAngles[x]->ToDouble(&dEA))
         {
            eulerAnglesRvector[x] = dEA;
         }
      }
      theAttitude->SetRvectorParameter(
                   theAttitude->GetParameterID("EulerAngles"), eulerAnglesRvector);
   }
   else if (stateTypeStr == stateTypeArray[QUATERNION])
   {
      Rvector  quaternionRvector(4);
      Real dQ;
      for (unsigned int x = 0; x < 4; ++x)
      {
         if (quaternion[x]->ToDouble(&dQ))
         {
            #ifdef DEBUG_ATTITUDE_PANEL
                MessageInterface::ShowMessage("q[%u] = %f\n", x, dQ);
            #endif               
            quaternionRvector[x] = dQ;
         }
         else 
         {
                MessageInterface::ShowMessage("failed with: \"%s\"\n", quaternion[x]->c_str());
         }
                
      }
      theAttitude->SetRvectorParameter(
                   theAttitude->GetParameterID("Quaternion"), quaternionRvector);
   }
   else if (stateTypeStr == stateTypeArray[DCM])
   {
          Rmatrix33 cosRmatrix;
          Real dCos;
          unsigned int x,y;
          for (x = 0; x < 3; ++x)
      {
         for (y = 0; y < 3; ++y)
         {
            if (cosineMatrix[y+x*3]->ToDouble(&dCos))
            {
               cosRmatrix.SetElement(x, y, dCos);
            }
        }
      }
          theAttitude->SetRmatrixParameter(
                       theAttitude->GetParameterID("DirectionCosineMatrix"), cosRmatrix);
   }

   std::string stateTypeRateStr = stateTypeRateComboBox->GetStringSelection().c_str();
   if (stateTypeRateStr == stateTypeRateArray[EULER_ANGLE_RATES])
   {
      Rvector3 eulerAngleRatesRvector;
      Real dEAR;
      for (unsigned int x = 0; x < 3; ++x)
      {
         if (eulerAngleRates[x]->ToDouble(&dEAR))
         {
            eulerAngleRatesRvector[x] = dEAR;
         }
      }
      theAttitude->SetRvectorParameter(
                   theAttitude->GetParameterID("EulerAngleRates"), eulerAngleRatesRvector);
   }
   else if (stateTypeRateStr == stateTypeRateArray[ANGULAR_VELOCITY])
   {
          Rvector3 angularVelocityRvector;
          Real dAV;
      for (unsigned int x = 0; x < 3; ++x)
      {
         if (angVel[x]->ToDouble(&dAV))
         {
            angularVelocityRvector[x] = dAV;
         }
      }
      theAttitude->SetRvectorParameter(
                   theAttitude->GetParameterID("AngularVelocity"), angularVelocityRvector);
   }

   theSpacecraft->SetRefObject(theAttitude, Gmat::ATTITUDE, "");
   dataChanged = false;
   
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::SaveData() ending\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnStateTypeTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeTextUpdate(wxCommandEvent &event)
{
   if (dontUpdate) return;
   
   std::string stateTypeStr = stateTypeComboBox->GetStringSelection().c_str();
   if (stateTypeStr == stateTypeArray[EULER_ANGLES])
   {
      if (st1TextCtrl)
         eulerAngles[0]->Printf("%s", st1TextCtrl->GetValue().c_str());
      else if (st2TextCtrl)
         eulerAngles[1]->Printf("%s", st2TextCtrl->GetValue().c_str());
      else if (st3TextCtrl)
         eulerAngles[2]->Printf("%s", st3TextCtrl->GetValue().c_str());
      
      CalculateFromEulerAngles(); // use this new information to
      // calculate the quaternion,
      // the cosine matrix, and whatever
      // else is not immediately shown
      
      std::string stateTypeRateStr = stateTypeRateComboBox->GetStringSelection().c_str();
      if (stateTypeRateStr == stateTypeRateArray[EULER_ANGLE_RATES])
      {
         CalculateFromEulerAngleRates();
      }
      else if (stateTypeRateStr == stateTypeRateArray[ANGULAR_VELOCITY]) {
         CalculateFromAngularVelocity();
      }
   }
   else if (stateTypeStr == stateTypeArray[QUATERNION])
   {
      if (st1TextCtrl)
         quaternion[0]->Printf("%s", st1TextCtrl->GetValue().c_str());
      else if (st2TextCtrl)
         quaternion[1]->Printf("%s", st2TextCtrl->GetValue().c_str());
      else if (st3TextCtrl)
         quaternion[2]->Printf("%s", st3TextCtrl->GetValue().c_str());
      else if (st4TextCtrl)
         quaternion[3]->Printf("%s", st4TextCtrl->GetValue().c_str());
      
      CalculateFromQuaternion(); // use this new information to
      // calculate the Euler angles and
      // the cosine matrix
   }
   else if (stateTypeStr == stateTypeArray[DCM])
   {
      if (st1TextCtrl)
         cosineMatrix[0]->Printf("%s", st1TextCtrl->GetValue().c_str());
      else if (st2TextCtrl)
         cosineMatrix[1]->Printf("%s", st2TextCtrl->GetValue().c_str());
      else if (st3TextCtrl)
         cosineMatrix[2]->Printf("%s", st3TextCtrl->GetValue().c_str());
      else if (st5TextCtrl)
         cosineMatrix[3]->Printf("%s", st5TextCtrl->GetValue().c_str());
      else if (st6TextCtrl)
         cosineMatrix[4]->Printf("%s", st6TextCtrl->GetValue().c_str());
      else if (st7TextCtrl)
         cosineMatrix[5]->Printf("%s", st7TextCtrl->GetValue().c_str());
      else if (st8TextCtrl)
         cosineMatrix[6]->Printf("%s", st8TextCtrl->GetValue().c_str());
      else if (st9TextCtrl)
         cosineMatrix[7]->Printf("%s", st9TextCtrl->GetValue().c_str());
      else if (st10TextCtrl)
         cosineMatrix[8]->Printf("%s", st10TextCtrl->GetValue().c_str());
      
      CalculateFromCosineMatrix(); // use this new information to
      // calculate the quaternion and
      // the Euler angles
   }
   
   dataChanged = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnStateTypeRateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeRateTextUpdate(wxCommandEvent &event)
{
   if (dontUpdate) return;
   
   std::string stateTypeRateStr = stateTypeRateComboBox->GetStringSelection().c_str();
   if (stateTypeRateStr == stateTypeRateArray[EULER_ANGLE_RATES])
   {
      if (str1TextCtrl)
         eulerAngleRates[0]->Printf("%s", str1TextCtrl->GetValue().c_str());
      else if (str2TextCtrl)
         eulerAngleRates[1]->Printf("%s", str2TextCtrl->GetValue().c_str());
      else if (str3TextCtrl)
         eulerAngleRates[2]->Printf("%s", str3TextCtrl->GetValue().c_str());
                        
      CalculateFromEulerAngleRates();  // calculate the equivalent angular velocities
   }
   else if (stateTypeRateStr == stateTypeRateArray[ANGULAR_VELOCITY])
   {
      if (str1TextCtrl)
         angVel[0]->Printf("%s", str1TextCtrl->GetValue().c_str());
      else if (str2TextCtrl)
         angVel[1]->Printf("%s", str2TextCtrl->GetValue().c_str());
      else if (str3TextCtrl)
         angVel[2]->Printf("%s", str3TextCtrl->GetValue().c_str());
                        
      CalculateFromAngularVelocity();  // calculate the equivalent angular velocities
   }
   
   dataChanged = true;
   theScPanel->EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnConfigurationSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnConfigurationSelection(wxCommandEvent &event)
{
}

//------------------------------------------------------------------------------
// void OnAttitudeModelSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnAttitudeModelSelection(wxCommandEvent &event)
{
   // if the user changes the attitude model, we will need to create a new one
   //currentModel = stateTypeComboBox->GetValue().c_str();
;
   
}

//------------------------------------------------------------------------------
// void OnEulerSequenceSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnEulerSequenceSelection(wxCommandEvent &event)
{
   // if the user changes the euler sequence, we must recompute the 
   // euler angles and euler angle rates, if they are shown
}


//------------------------------------------------------------------------------
// void OnStateTypeSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeSelection(wxCommandEvent &event)
{
   unsigned int x, y;
   std::string stateTypeStr = stateTypeComboBox->GetStringSelection().c_str();
   if (stateTypeStr == stateTypeArray[EULER_ANGLES])
   {
      Rvector ea = theAttitude->GetRvectorParameter("EulerAngles");
      for (x = 0; x < 3; ++x)
         *eulerAngles[x] = theGuiManager->ToWxString(ea[x]);
      DisplayEulerAngles();
   }
   else if (stateTypeStr == stateTypeArray[QUATERNION])
   {
      Rvector q = theAttitude->GetRvectorParameter("Quaternion");
      for (x = 0; x < 4; ++x)
         *quaternion[x] = theGuiManager->ToWxString(q[x]);
      DisplayQuaternion();
   }
   else if (stateTypeStr == stateTypeArray[DCM])
   {
      Rmatrix mat = theAttitude->GetRmatrixParameter("DirectionCosineMatrix");
      for (x = 0; x < 3; ++x)
         for (y = 0; y < 3; ++y)
            *cosineMatrix[x*3+y] = theGuiManager->ToWxString(mat(x,y));
      DisplayDCM();
   }
}


//------------------------------------------------------------------------------
// void OnStateTypeRateSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeRateSelection(wxCommandEvent &event)
{
   unsigned int x;
   std::string stateTypeRateStr = stateTypeRateComboBox->GetStringSelection().c_str();
  
   if (stateTypeRateStr == stateTypeRateArray[EULER_ANGLE_RATES])
   {
      Rvector ear = theAttitude->GetRvectorParameter("EulerAngleRates");
      for (x = 0; x < 3; ++x)
         *eulerAngleRates[x] = theGuiManager->ToWxString(ear[x]);
      DisplayEulerAngleRates();
   }
   else if (stateTypeRateStr == stateTypeRateArray[ANGULAR_VELOCITY])
   {
      Rvector av = theAttitude->GetRvectorParameter("AngularVelocity");
      for (x = 0; x < 3; ++x)
         *angVel[x] = theGuiManager->ToWxString(av[x]);
      DisplayAngularVelocity();
   }
}


//------------------------------------------------------------------------------
// void CalculateFromEulerAngles()
//------------------------------------------------------------------------------
void AttitudePanel::CalculateFromEulerAngles()
{
   Rvector3 ea;
   unsigned int x,y;
   for (x = 0; x < 3; ++x)
   {
      Real dEA;
                
      if (eulerAngles[x]->ToDouble(&dEA))
      {
         ea[x] = dEA;
      }
      else
      {
         return;  // unusable input
      }
   }
        
   Integer esA, esB, esC;  // Euler sequence
   // for now, default to first
   esA = 1;
   esB = 2;
   esC = 3;
        
   Rmatrix33 cm = Attitude::ToCosineMatrix(ea, esA, esB, esC);

   for (x = 0; x < 3; ++x) {
      Rvector c = cm.GetColumn(x);
      for (y = 0; y < 3; ++y) {
         //cosineMatrix[y+x*3]->Printf("%.1f", c[y]);
         *cosineMatrix[y+x*3] = theGuiManager->ToWxString(c[y]);
      }
   }

   Rvector q = Attitude::ToQuaternion(ea, esA, esB, esC);
                
   for (x = 0; x < 4; ++x) {
      //quaternion[x]->Printf("%.1f", q[x]);
      *quaternion[x] = theGuiManager->ToWxString(q[x]);
   }
}


//------------------------------------------------------------------------------
// void CalculateFromQuaternion()
//------------------------------------------------------------------------------
void AttitudePanel::CalculateFromQuaternion()
{
   Rvector q(4);
   unsigned int x,y;
   for (x = 0; x < 4; ++x) {
      Real dQ_x;
                
      if (quaternion[x]->ToDouble(&dQ_x)) {  // will return false on bad input
         q[x] = dQ_x;
      }
      else
      {
         return;  // unusable input
      }
   }

   Rmatrix33 cm = Attitude::ToCosineMatrix(q);

   for (x = 0; x < 3; ++x) {
      Rvector c = cm.GetColumn(x);
      for (y = 0; y < 3; ++y) {
         //cosineMatrix[y+x*3]->Printf("%.1f", c[y]);
         *cosineMatrix[y+x*3] = theGuiManager->ToWxString(c[y]);
      }
   }
        
   Integer esA, esB, esC;  // Euler sequence
   // until there's a convinient method to retrieve integers,
   // default to the first sequence;
   esA = 1;
   esB = 2;
   esC = 3;
        
   Rvector3 ea = Attitude::ToEulerAngles(q, esA, esB, esC);
        
   for (x = 0; x < 3; ++x)
   {
      //eulerAngles[x]->Printf("%.1f", ea[x]);
      *eulerAngles[x] = theGuiManager->ToWxString(ea[x]);
   }
}


//------------------------------------------------------------------------------
// void AttitudePanel::CalculateFromCosineMatrix()
//------------------------------------------------------------------------------
void AttitudePanel::CalculateFromCosineMatrix()
{
   Rmatrix33 cm;
   unsigned int c,r;
   for (c = 0; c < 3; ++c) {  // columns
      for (r = 0; r < 3; ++r) {  // rows
         Real dC;
                
         if (cosineMatrix[r+c*3]->ToDouble(&dC)) {  // will return false on bad input
            cm.SetElement(r,c,dC);
         }
         else
         {
            return;  // unusable input
         }
      }
   }

   Rvector q = Attitude::ToQuaternion(cm);

   for (c = 0; c < 4; ++c) {
      //quaternion[c]->Printf("%.1f", q[c]);
      *quaternion[c] = theGuiManager->ToWxString(q[c]);
   }
   
   Integer esA, esB, esC;  // Euler sequence
   // until there's a convinient method to retrieve integers,
   // default to the first sequence;
   esA = 1;
   esB = 2;
   esC = 3;
        
   Rvector3 ea = Attitude::ToEulerAngles(cm, esA, esB, esC);
        
   for (c = 0; c < 3; ++c)
   {
      //eulerAngles[c]->Printf("%.1f", ea[c]);
      *eulerAngles[c] = theGuiManager->ToWxString(ea[c]);
   }
}

//------------------------------------------------------------------------------
// void AttitudePanel::CalculateFromEulerAngleRates()
//------------------------------------------------------------------------------
void AttitudePanel::CalculateFromEulerAngleRates()
{
   Rvector3 ea;
   unsigned int x;
   for (x = 0; x < 3; ++x)
   {
      Real dEA;
                
      if (eulerAngles[x]->ToDouble(&dEA))
      {
         ea[x] = dEA;
      }
      else
      {
         return;  // unusable input
      }
   }
        
   Integer esA, esB, esC;  // Euler sequence
   // for now, default to first
   esA = 1;
   esB = 2;
   esC = 3;

   Rvector3 ear;
   for (x = 0; x < 3; ++x)
   {
      Real dEAR;
                
      if (eulerAngleRates[x]->ToDouble(&dEAR))
      {
         ear[x] = dEAR;
      }
      else
      {
         return;  // unusable input
      }
   }
        
   Rvector3 av = Attitude::ToAngularVelocity(ear, ea, esA, esB, esC);
        
   for (x = 0; x < 3; ++x)
   {
      //angVel[x]->Printf("%.1f", av[x]);
      *angVel[x] = theGuiManager->ToWxString(av[x]);
   }
}


//------------------------------------------------------------------------------
// void AttitudePanel::CalculateFromAngularVelocity()
//------------------------------------------------------------------------------
void AttitudePanel::CalculateFromAngularVelocity()
{
   Rvector3 ea;
   unsigned int x;
   for (x = 0; x < 3; ++x)
   {
      Real dEA;
                
      if (eulerAngles[x]->ToDouble(&dEA))
      {
         ea[x] = dEA;
      }
      else
      {
         return;  // unusable input
      }
   }
        
   Integer esA, esB, esC;  // Euler sequence
   // for now, default to first
   esA = 1;
   esB = 2;
   esC = 3;
        
   Rvector3 av;
   for (x = 0; x < 3; ++x)
   {
      Real dAV;
                
      if (angVel[x]->ToDouble(&dAV))
      {
         av[x] = dAV;
      }
      else
      {
         return;  // unusable input
      }
   }
        
   Rvector3 ear = Attitude::ToEulerAngleRates(av, ea, esA, esB, esC);
        
   for (x = 0; x < 3; ++x)
   {
      //eulerAngleRates[x]->Printf("%.1f", ear[x]);
      *eulerAngleRates[x] = theGuiManager->ToWxString(ear[x]);
   }
}

//------------------------------------------------------------------------------
// bool IsStateModified()
//------------------------------------------------------------------------------
bool AttitudePanel::IsStateModified()
{
   //for (int i=0; i<6; i++)
   //   if (mIsStateModified[i])
   //      return true;

   return false;
}



//------------------------------------------------------------------------------
// void ResetStateFlags(bool discardEdits = false)
//------------------------------------------------------------------------------
void AttitudePanel::ResetStateFlags(bool discardEdits)
{
   //for (int i=0; i<6; i++)
   //   mIsStateModified[i] = false;

   //if (discardEdits)
   //   for (int i=0; i<6; i++)
    //     textCtrl[i]->DiscardEdits();
}

