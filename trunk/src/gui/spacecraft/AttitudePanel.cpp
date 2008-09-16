//$Id$
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
// Author: Waka Waktola (and then later, heavily modified by Wendy C. Shoan)
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
#include "GmatStaticBoxSizer.hpp"
#include "StringUtil.hpp"
#include "RealUtilities.hpp"

//#define DEBUG_ATTITUDE_PANEL 1

//------------------------------
// static data
//-----------------------------
const std::string AttitudePanel::STATE_TEXT[StateTypeCount] = 
{
   "EulerAngles",
   "Quaternion",
   "DirectionCosineMatrix",
};

const std::string AttitudePanel::STATE_RATE_TEXT[StateRateTypeCount] = 
{
   "EulerAngleRates",
   "AngularVelocity",
};

// initial selections in combo boxes
const Integer AttitudePanel::STARTUP_STATE_TYPE_SELECTION      = EULER_ANGLES;
const Integer AttitudePanel::STARTUP_RATE_STATE_TYPE_SELECTION = EULER_ANGLE_RATES;

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(AttitudePanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL_STATE,      AttitudePanel::OnStateTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_STATE_RATE, AttitudePanel::OnStateRateTextUpdate)
   EVT_COMBOBOX(ID_CB_STATE,        AttitudePanel::OnStateTypeSelection)
   EVT_COMBOBOX(ID_CB_STATE_RATE,   AttitudePanel::OnStateTypeRateSelection)
   EVT_COMBOBOX(ID_CB_SEQ,          AttitudePanel::OnEulerSequenceSelection)
   EVT_COMBOBOX(ID_CB_COORDSYS,     AttitudePanel::OnCoordinateSystemSelection)
   EVT_COMBOBOX(ID_CB_MODEL,        AttitudePanel::OnAttitudeModelSelection)
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
                             Spacecraft *spacecraft) :
   wxPanel     (parent), 
   theAttitude (NULL),
   attCS       (NULL),
   toCS        (NULL),
   fromCS      (NULL),
   canClose    (true)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::AttitudePanel() entered\n");
   #endif

   theScPanel        = scPanel;
   theSpacecraft     = spacecraft;
   
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager     = GuiItemManager::GetInstance();
   
   modelArray.clear();
   eulerSeqArray.clear();
   stateTypeArray.clear();
   stateRateTypeArray.clear();
   //coordSysArray.clear();
   //kinematicArray.clear();
   
   unsigned int defSeq[3] = {3, 1, 2};
   seq.push_back(defSeq[0]);
   seq.push_back(defSeq[1]);
   seq.push_back(defSeq[2]);
   
   attitudeModel       = "";
   attCoordSystem      = "";
   eulerSequence       = "312";
   attStateType        = "";
   attRateStateType    = "";
   //attitudeType        = "";  // currently not used
   
   stateTypeModified     = false;
   rateStateTypeModified = false;
   stateModified         = false;
   stateRateModified     = false;
   csModified            = false;
   seqModified           = false;
   modelModified         = false;
   
   ResetStateFlags("Both");   

   dataChanged = false;
   canClose    = true;
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
   
   q = Rvector(4);
   
   // gt list of models and put them into the combo box
   modelArray = theGuiInterpreter->GetListOfFactoryItems(Gmat::ATTITUDE);
   unsigned int modelSz = modelArray.size();
   attitudeModelArray = new wxString[modelSz];
   for (x = 0; x < modelSz; ++x)
      attitudeModelArray[x] = wxT(modelArray[x].c_str());

   config1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Model"),
                        wxDefaultPosition, wxDefaultSize, 0);
   config1ComboBox = 
      new wxComboBox( this, ID_CB_MODEL, wxT(attitudeModelArray[0]), 
         wxDefaultPosition, wxDefaultSize, modelSz, attitudeModelArray, 
         wxCB_DROPDOWN|wxCB_READONLY );

   // Coordinate System
   config2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Coordinate System"),
         wxDefaultPosition, wxDefaultSize, 0);
   config2ComboBox =  theGuiManager->GetCoordSysComboBox(this, ID_CB_COORDSYS, 
      wxDefaultSize);

   //Euler Angle Sequence
   eulerSeqArray           = Attitude::GetEulerSequenceStrings();
   unsigned int eulerSeqSz = eulerSeqArray.size();
   
   eulerSequenceArray = new wxString[eulerSeqSz];  // Euler sequence types
   unsigned int i;
   Integer ii;
   for (i=0; i<eulerSeqSz; i++)
      eulerSequenceArray[i] = eulerSeqArray[i].c_str();

   config4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Euler Angle Sequence"),
                        wxDefaultPosition, wxDefaultSize, 0);
   config4ComboBox = new wxComboBox( this, ID_CB_SEQ, wxT(eulerSequenceArray[0]),
                      wxDefaultPosition, wxDefaultSize, 12,
                      eulerSequenceArray, wxCB_DROPDOWN|wxCB_READONLY );

   // State Type
   stateTypeStaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude State Type"),
                        wxDefaultPosition, wxDefaultSize, 0);

   for (ii = 0; ii < StateTypeCount; ii++)
      stateTypeArray.push_back(STATE_TEXT[ii]);
   
   stateArray = new wxString[StateTypeCount];
   for (ii=0; ii<StateTypeCount; ii++)
      stateArray[ii] = stateTypeArray[ii].c_str();

   stateTypeComboBox = 
      new wxComboBox( this, ID_CB_STATE, wxT(stateArray[STARTUP_STATE_TYPE_SELECTION]),
         wxDefaultPosition, wxSize(180,20), StateTypeCount, stateArray, 
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
   //col1StaticText =
   //   new wxStaticText( this, ID_TEXT, wxT(""),              // was "Column 1"
   //                     wxDefaultPosition, wxSize(80,20), 0);
   //col2StaticText =
   //   new wxStaticText( this, ID_TEXT, wxT(""),              // was "Column 2"
   //                     wxDefaultPosition, wxSize(80,20), 0);
   //col3StaticText =
   //   new wxStaticText( this, ID_TEXT, wxT(""),              // was "Column 3"
   //                     wxDefaultPosition, wxSize(80,20), 0);
   st1TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st4TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st5TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st6TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st7TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st8TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st9TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );
   st10TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE, wxT(""),
                      wxDefaultPosition, wxSize(60,-1), 0 );  

   // Rate State Type
   stateTypeRate4StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Attitude Rate State Type"),
                        wxDefaultPosition, wxDefaultSize, 0);

   for (ii = 0; ii < StateRateTypeCount; ii++)
      stateRateTypeArray.push_back(STATE_RATE_TEXT[ii]);

   stateRateArray = new wxString[StateRateTypeCount];
   for (ii=0; ii< StateRateTypeCount; ii++)
      stateRateArray[ii] = stateRateTypeArray[ii].c_str();

   stateRateTypeComboBox =
      new wxComboBox( this, ID_CB_STATE_RATE, 
         wxT(stateRateArray[STARTUP_RATE_STATE_TYPE_SELECTION]), wxDefaultPosition, 
         wxSize(180,20), StateRateTypeCount, stateRateArray, 
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
      new wxTextCtrl( this, ID_TEXTCTRL_STATE_RATE, wxT(""), wxDefaultPosition, 
         wxSize(100,-1), 0 );
   str2TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE_RATE, wxT(""), wxDefaultPosition, 
         wxSize(100,-1), 0 );
   str3TextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL_STATE_RATE, wxT(""), wxDefaultPosition, 
         wxSize(100,-1), 0 );                

   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("AttitudePanel::Create() Creating wxTextCtrl objects\n");
   #endif

   //attUnits1 = new wxStaticText(this,ID_TEXT,wxT("deg"));
   //attUnits2 = new wxStaticText(this,ID_TEXT,wxT("deg"));
   //attUnits3 = new wxStaticText(this,ID_TEXT,wxT("deg"));

   rateUnits1 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));
   rateUnits2 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));
   rateUnits3 = new wxStaticText( this, ID_TEXT, wxT("deg/sec"));

   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("AttitudePanel::Create() Creating wxString objects\n");
   #endif
      
      
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage(
         "AttitudePanel::Create() Creating wxBoxSizer objects.\n");
   #endif
   
   Integer bsize = 2; // border size
   // wx*Sizers   
   GmatStaticBoxSizer *boxSizer1 = new GmatStaticBoxSizer( wxHORIZONTAL, this, "" );
   GmatStaticBoxSizer *boxSizer2 = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
   GmatStaticBoxSizer *boxSizer3 = new GmatStaticBoxSizer( wxVERTICAL, this, "" );
   GmatStaticBoxSizer *boxSizer4 = new GmatStaticBoxSizer( wxVERTICAL, this, "Attitude Initial Conditions" );
   GmatStaticBoxSizer *boxSizer5 = new GmatStaticBoxSizer( wxVERTICAL, this, "Attitude Rate Initial Conditions" );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 4, 0, 0 );
   wxFlexGridSizer *flexGridSizer3 = new wxFlexGridSizer( 3, 0, 0 );        
                                  
   // Add to wx*Sizers
   flexGridSizer1->Add(config1StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config1ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );        
   flexGridSizer1->Add(config2StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config2ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );         
   flexGridSizer1->Add(config4StaticText, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );
   flexGridSizer1->Add(config4ComboBox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize );         
   
   //flexGridSizer2->Add(20, 20, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   //flexGridSizer2->Add(col1StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   //flexGridSizer2->Add(col2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   //flexGridSizer2->Add(col3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );         
      
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
   flexGridSizer3->Add(rateUnits1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str2TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(rateUnits2, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3StaticText, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer3->Add(str3TextCtrl, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );        
   flexGridSizer3->Add(rateUnits3, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   boxSizer4->Add(stateTypeStaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer4->Add(stateTypeComboBox , 0, wxALIGN_LEFT|wxALL, bsize);  
   boxSizer4->Add(flexGridSizer2, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);            
   
   boxSizer5->Add(stateTypeRate4StaticText , 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   boxSizer5->Add(stateRateTypeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
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
         
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::Create() exiting\n");
   #endif
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
   bool newAttitude = false;
   // check to see if the spacecrat has an attitude object
   theAttitude = (Attitude*) theSpacecraft->GetRefObject(Gmat::ATTITUDE, "");
   if (theAttitude == NULL)   // no attitude yet
   {
      #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage
         ("   Attitude is NULL, so try to create %s.\n", attitudeModelArray[0].c_str());
      #endif
      
      theAttitude = (Attitude *)theGuiInterpreter->
         CreateObject((attitudeModelArray[0]).c_str(), ""); // Use no name
      newAttitude = true;
   }
   if (theAttitude == NULL)
   {
      std::string ex = "ERROR- unable to find or create an attitude object for ";
      ex += theSpacecraft->GetName() + "\n";
      throw GmatBaseException(ex);
   }
   
   try
   {
      #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("   Now retrieve data from the attitude\n");
      #endif
      
      epoch = theAttitude->GetEpoch();
      
      attStateType     = 
         theAttitude->GetStringParameter("AttitudeDisplayStateType");
      attRateStateType = 
         theAttitude->GetStringParameter("AttitudeRateDisplayStateType");
      attitudeModel    = theAttitude->GetAttitudeModelName();
      config1ComboBox->SetValue(wxT(attitudeModel.c_str()));
      
      eulerSequence  = theAttitude->GetStringParameter("EulerAngleSequence");
      seq            = Attitude::ExtractEulerSequence(eulerSequence);
      config4ComboBox->SetValue(wxT(eulerSequence.c_str()));
   
      attCoordSystem = theAttitude->GetStringParameter("AttitudeCoordinateSystem");
      config2ComboBox->SetValue(wxT(attCoordSystem.c_str()));
      if (!attCS) attCS  = (CoordinateSystem*)theGuiInterpreter->
                     GetConfiguredObject(attCoordSystem);
      //if (newAttitude) attCS = NULL;
      //else             attCS = (CoordinateSystem*) theAttitude->
      //                 GetRefObject(Gmat::COORDINATE_SYSTEM, 
      //                 attCoordSystem);
      
      if (attStateType == STATE_TEXT[EULER_ANGLES])
      {
         Rvector eaVal = theAttitude->GetRvectorParameter(STATE_TEXT[EULER_ANGLES]);
         for (x = 0; x < 3; ++x)
         {
            *eulerAngles[x] = theGuiManager->ToWxString(eaVal[x]);
            ea[x]           = eaVal[x];
         }
         DisplayEulerAngles();
      }
      else if (attStateType == "Quaternion")
      {
         Rvector qVal = theAttitude->GetRvectorParameter("Quaternion");
         for (x = 0; x < 4; ++x)
         {
            *quaternion[x] = theGuiManager->ToWxString(qVal[x]);
            q[x]           = qVal[x];
         }
         DisplayQuaternion();
      }
      else // "DirectionCosineMatrix
      {
         Rmatrix matVal = theAttitude->GetRmatrixParameter("DirectionCosineMatrix");
         for (x = 0; x < 3; ++x)
            for (y = 0; y < 3; ++y)
            {
               *cosineMatrix[x*3+y] = theGuiManager->ToWxString(matVal(x,y));
               mat(x,y)             = matVal(x,y);
            }
         DisplayDCM();
      }
   
      if (attRateStateType == "EulerAngleRates") 
      {
         Rvector earVal = theAttitude->GetRvectorParameter("EulerAngleRates");
         for (x = 0; x < 3; ++x)
         {
            *eulerAngleRates[x] = theGuiManager->ToWxString(earVal[x]);
            ear[x]              = earVal[x];
         }
         DisplayEulerAngleRates();
      }
      else // AngularVelocity
      {
         Rvector avVal = theAttitude->GetRvectorParameter("AngularVelocity");
         for (x = 0; x < 3; ++x)
         {
            *angVel[x] = theGuiManager->ToWxString(avVal[x]);
            av[x]      = avVal[x];
         }
         DisplayAngularVelocity();
      }
      
      dataChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void AttitudePanel::SaveData()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::SaveData() entered\n");
   #endif
   
   if (!ValidateState("Both"))
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value(s) before saving the Attitude data\n");
         canClose = false;
     return;
   }
   canClose = true;
   dataChanged = false;
   
   // if the user selected a different attitude model, we will need to create it
   bool isNewAttitude = false;
   Attitude *useAttitude = NULL;
   if (modelModified)  
   {
      try
      {
         #ifdef DEBUG_ATTITUDE_PANEL
            MessageInterface::ShowMessage(
            "   about to create a new attitude of type %s\n", 
            attitudeModel.c_str());
         #endif
         useAttitude = (Attitude *)theGuiInterpreter->
                       CreateObject(attitudeModel, ""); // Use no name
      }
      catch (BaseException &ex)
      {
         MessageInterface::ShowMessage("ERROR: %s\n", ex.GetFullMessage().c_str());
      }
      isNewAttitude = true;
      modelModified = false;
   }
   else useAttitude = theAttitude;
   
   #ifdef DEBUG_ATTITUDE_PANEL
      if (!useAttitude)
        MessageInterface::ShowMessage("   Attitude pointer is NULL\n");
   #endif

   
   if (seqModified || isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   Setting new sequence: %s\n", 
         eulerSequence.c_str());
      #endif
      useAttitude->SetStringParameter("EulerAngleSequence", eulerSequence);
      
      // set attitude state and rate as well, to match what the user sees on the screen
      if (attStateType == stateTypeArray[EULER_ANGLES])
         useAttitude->SetRvectorParameter("EulerAngles", ea);
      else if (attStateType == stateTypeArray[QUATERNION])
         useAttitude->SetRvectorParameter("Quaternion", q);
      else 
         useAttitude->SetRmatrixParameter("DirectionCosineMatrix", mat);
         
      if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
         useAttitude->SetRvectorParameter("EulerAngleRates", ear);
      else 
         useAttitude->SetRvectorParameter("AngularVelocity", av);
      seqModified = false;
   }
   
   if (csModified || isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   Setting new coordinate system: %s\n",
         attCoordSystem.c_str());
      #endif
      useAttitude->SetStringParameter("AttitudeCoordinateSystem",attCoordSystem);
      useAttitude->SetRefObject(attCS, Gmat::COORDINATE_SYSTEM, attCoordSystem);
      csModified = false;
   }
   
   if (stateTypeModified || isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   Setting new state type to ...\n",
         attStateType.c_str());
      #endif
      useAttitude->SetStringParameter("AttitudeDisplayStateType", attStateType);
   }
      
   if (stateModified || isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   Setting new state ...\n");
      #endif
      if (attStateType == stateTypeArray[EULER_ANGLES])
         useAttitude->SetRvectorParameter("EulerAngles", ea);
      else if (attStateType == stateTypeArray[QUATERNION])
         useAttitude->SetRvectorParameter("Quaternion", q);
      else 
         useAttitude->SetRmatrixParameter("DirectionCosineMatrix", mat);
      stateModified = false;
   }
   
   if (rateStateTypeModified || isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   Setting new rate state type to ...\n",
         attRateStateType.c_str());
      #endif
      useAttitude->SetStringParameter("AttitudeRateDisplayStateType", attRateStateType);
   }

   if (stateRateModified || isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("   Setting new state rate ...\n");
      #endif
      if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
         useAttitude->SetRvectorParameter("EulerAngleRates", ear);
      else 
         useAttitude->SetRvectorParameter("AngularVelocity", av);
      stateRateModified = false;
   }
   
   if (isNewAttitude)
   {
      #ifdef DEBUG_ATTITUDE_PANEL
         MessageInterface::ShowMessage("Setting new attitude model of type %s on spacecraft\n",
         attitudeModel.c_str());
      #endif
      theSpacecraft->SetRefObject(useAttitude, Gmat::ATTITUDE, "");
   }
   ResetStateFlags("Both", true);
}

//------------------------------------------------------------------------------
// bool IsStateModified(const std::string which = "Both")
//------------------------------------------------------------------------------
bool AttitudePanel::IsStateModified(const std::string which)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::IsStateModified() entered\n");
   #endif
   if ((which == "State")  || (which == "Both"))
   {
      if (attStateType == stateTypeArray[EULER_ANGLES])
         for (Integer ii = 0; ii<3; ii++)
            if (eaModified[ii]) return true;
      else if (attStateType == stateTypeArray[QUATERNION])
         for (Integer ii = 0; ii < 4; ii++)
            if (qModified[ii]) return true;
      else if (attStateType == stateTypeArray[DCM])
         for (Integer ii = 0; ii < 9; ii++)
            if (matModified[ii]) return true;
      return false;
   }
   if ((which == "Rate") || (which == "Both"))
   {
      if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
         for (Integer ii = 0; ii<3; ii++)
            if (earModified[ii]) return true;
      if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY])
         for (Integer ii = 0; ii<3; ii++)
            if (avModified[ii]) return true;
      return false;
   }
   return false;
}


//------------------------------------------------------------------------------
// void ResetStateFlags(const std::string which = "Both",
//                      bool discardEdits = false)
//------------------------------------------------------------------------------
void AttitudePanel::ResetStateFlags(const std::string which,
                                    bool discardEdits)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::ResetStateFlags() entered\n");
   #endif
   if ((which == "State") || (which == "Both"))
   {
      for (Integer ii = 0; ii < 9; ii++)
         matModified[ii]   = false;
      for (Integer ii = 0; ii < 4; ii++)
         qModified[ii]   = false;
      for (Integer ii = 0; ii < 3; ii++)
         earModified[ii]  = false;
      if (discardEdits)
      {
         st1TextCtrl->DiscardEdits();
         st2TextCtrl->DiscardEdits();
         st3TextCtrl->DiscardEdits();
         st4TextCtrl->DiscardEdits();
         st5TextCtrl->DiscardEdits();
         st6TextCtrl->DiscardEdits();
         st7TextCtrl->DiscardEdits();
         st8TextCtrl->DiscardEdits();
         st9TextCtrl->DiscardEdits();
         st10TextCtrl->DiscardEdits();
      }
   }
   if ((which == "Rate") || (which == "Both"))
   {
      for (Integer ii = 0; ii < 3; ii++)
      {
         eaModified[ii]   = false;
         avModified[ii]   = false;
      }
      if (discardEdits)
      {
         str1TextCtrl->DiscardEdits();
         str2TextCtrl->DiscardEdits();
         str3TextCtrl->DiscardEdits();
      }
   }

}

//------------------------------------------------------------------------------
// bool ValidateState(const std::string which = "Both")
//------------------------------------------------------------------------------
bool AttitudePanel::ValidateState(const std::string which)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::ValidateState() entered\n");
   #endif
   bool retval = true;
   std::string strVal;
   Real        tmpVal;
   if ((which == "State") || (which == "Both"))
   {
      if (attStateType == stateTypeArray[EULER_ANGLES])
      {
         if (eaModified[0])
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle 1", "Real Number"))
               retval = false;
            else  ea[0] = tmpVal;
         }
         if (eaModified[1]) 
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle 2", "Real Number"))
               retval = false;
            else  ea[1] = tmpVal;
         }
         if (eaModified[2])
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle 3", "Real Number"))
               retval = false;
            else  ea[2] = tmpVal;
         }
      }
      else if (attStateType == stateTypeArray[QUATERNION])
      {
         if (qModified[0])
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q1", "Real Number"))
               retval = false;
            else  q[0] = tmpVal;
         }
         if (qModified[1])
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q2", "Real Number"))
               retval = false;
            else  q[1] = tmpVal;
         }
         if (qModified[2])
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q3", "Real Number"))
               retval = false;
            else  q[2] = tmpVal;
         }
         if (qModified[3])
         {
            strVal = st4TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "q4", "Real Number"))
               retval = false;
            else  q[3] = tmpVal;
         }
      }
      else if (attStateType == stateTypeArray[DCM])
      {
         if (matModified[0])
         {
            strVal = st1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 1,1", "Real Number"))
               retval = false;
            else  mat(0,0) = tmpVal;
         }
         if (matModified[1])
         {
            strVal = st5TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 1,2", "Real Number"))
               retval = false;
            else  mat(0,1) = tmpVal;
         }
         if (matModified[2])
         {
            strVal = st8TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 1,3", "Real Number"))
               retval = false;
            else  mat(0,2) = tmpVal;
         }
         if (matModified[3])
         {
            strVal = st2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 2,1", "Real Number"))
               retval = false;
            else  mat(1,0) = tmpVal;
         }
         if (matModified[4])
         {
            strVal = st6TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 2,2", "Real Number"))
               retval = false;
            else  mat(1,1) = tmpVal;
         }
         if (matModified[5])
         {
            strVal = st9TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 2,3", "Real Number"))
               retval = false;
            else  mat(1,2) = tmpVal;
         }
         if (matModified[6])
         {
            strVal = st3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 3,1", "Real Number"))
               retval = false;
            else  mat(2,0) = tmpVal;
         }
         if (matModified[7])
         {
            strVal = st7TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 3,2", "Real Number"))
               retval = false;
            else  mat(2,1) = tmpVal;
         }
         if (matModified[8])
         {
            strVal = st10TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "DCM 3,3", "Real Number"))
               retval = false;
            else  mat(2,2) = tmpVal;
         }
      }

   }
   if ((which == "Rate") || (which == "Both"))
   {
      if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
      {
         if (earModified[0])
         {
            strVal = str1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle Rate 1", "Real Number"))
               retval = false;
            else  ear[0] = tmpVal;
         }
         if (earModified[1])
         {
            strVal = str2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle Rate 2", "Real Number"))
               retval = false;
            else  ear[1] = tmpVal;
         }
         if (earModified[2])
         {
            strVal = str3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Euler Angle Rate 3", "Real Number"))
               retval = false;
            else  ear[2] = tmpVal;
         }
      }
      else if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY])
      {
         if (avModified[0])
         {
            strVal = str1TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Angular Velocity X", "Real Number"))
               retval = false;
            else  av[0] = tmpVal;
         }
         if (avModified[1])
         {
            strVal = str2TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Angular Velocity Y", "Real Number"))
               retval = false;
            else  av[1] = tmpVal;
         }
         if (avModified[2])
         {
            strVal = str3TextCtrl->GetValue();
            if (!theScPanel->CheckReal(tmpVal, strVal, "Angular Velocity Z", "Real Number"))
               retval = false;
            else  av[2] = tmpVal;
         }
      }
   }
   if (!retval) canClose = false;
   else         canClose = true;
   return retval;
}

//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString AttitudePanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


//------------------------------------------------------------------------------
// void OnStateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTextUpdate(wxCommandEvent &event)
{
   //if (!canClose) return; // ??
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateTextUpdate() entered\n");
   #endif
   
   if (attStateType == STATE_TEXT[EULER_ANGLES])
   {
      if (st1TextCtrl->IsModified())  eaModified[0]  = true;
      if (st2TextCtrl->IsModified())  eaModified[1]  = true;
      if (st3TextCtrl->IsModified())  eaModified[2]  = true;
   }
   else if (attStateType == STATE_TEXT[QUATERNION])
   {
      if (st1TextCtrl->IsModified())  qModified[0]   = true;
      if (st2TextCtrl->IsModified())  qModified[1]   = true;
      if (st3TextCtrl->IsModified())  qModified[2]   = true;
      if (st4TextCtrl->IsModified())  qModified[3]   = true;
   }
   else // DCM
   {
      if (st1TextCtrl->IsModified())  matModified[0] = true;
      if (st2TextCtrl->IsModified())  matModified[1] = true;
      if (st3TextCtrl->IsModified())  matModified[2] = true;
      // not st4TextCtrl
      if (st5TextCtrl->IsModified())  matModified[3] = true;
      if (st6TextCtrl->IsModified())  matModified[4] = true;
      if (st7TextCtrl->IsModified())  matModified[5] = true;
      if (st8TextCtrl->IsModified())  matModified[6] = true;
      if (st9TextCtrl->IsModified())  matModified[7] = true;
      if (st10TextCtrl->IsModified()) matModified[8] = true;
   }
    
   if (IsStateModified("State"))
   {
      stateModified = true;
      dataChanged   = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnStateRateTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateRateTextUpdate(wxCommandEvent &event)
{
   //if (!canClose) return;  // ??
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateRateTextUpdate() entered\n");
   #endif
   
   if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
   {
      if (str1TextCtrl->IsModified())  earModified[0] = true;
      if (str2TextCtrl->IsModified())  earModified[1] = true;
      if (str3TextCtrl->IsModified())  earModified[2] = true;
   }
   else // ANGULAR_VELOCITY
   {
      if (str1TextCtrl->IsModified())  avModified[0]  = true;
      if (str2TextCtrl->IsModified())  avModified[1]  = true;
      if (str3TextCtrl->IsModified())  avModified[2]  = true;
   }
    
   if (IsStateModified("Rate"))
   {
      stateRateModified = true;
      dataChanged   = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnCoordinateSystemSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnCoordinateSystemSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnCoordinateSystemSelection() entered\n");
   #endif
   std::string newCS = config2ComboBox->GetValue().c_str();
   if (newCS == attCoordSystem) return;
   // first, validate the state
   if (!ValidateState("Both"))
   {
      config2ComboBox->SetValue(wxT(attCoordSystem.c_str()));
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value(s) before changing the Reference Coordinate System\n");
         return;
   }
   if (!attCS) attCS  = (CoordinateSystem*)theGuiInterpreter->
                        GetConfiguredObject(attCoordSystem);
   fromCS = attCS;
   toCS   = (CoordinateSystem*)theGuiInterpreter->
             GetConfiguredObject(newCS);
             
   // convert things here *****
   
   //csModified     = true; // uncomment when completed .... (see below)
   //dataChanged    = true;
   //attCoordSystem = newCS;
   //attCS          = toCS;
   
   // until know how to convert to new reference coordinate system .........
   config2ComboBox->SetValue(wxT(attCoordSystem.c_str()));
   MessageInterface::PopupMessage(Gmat::WARNING_, +
      "Conversion of Attitude to a new Reference Coordinate System not yet implemented\n");
   return;
}

//------------------------------------------------------------------------------
// void OnAttitudeModelSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnAttitudeModelSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnAttitudeModelSelection() entered\n");
   #endif
   // if the user changes the attitude model, we will need to create a new one
    std::string newModel = config1ComboBox->GetValue().c_str();
    if (newModel != attitudeModel)
    {
      modelModified = true;
      dataChanged   = true;
      attitudeModel = newModel;
      theScPanel->EnableUpdate(true);
    }
    // will need to do something with this on SaveData
}

//------------------------------------------------------------------------------
// void OnEulerSequenceSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnEulerSequenceSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnEulerSequenceSelection() entered\n");
   #endif
   std::string newSeq = config4ComboBox->GetValue().c_str();
   if (newSeq != eulerSequence)
   {
      seqModified   = true;
      dataChanged   = true;
      eulerSequence = newSeq;
      theScPanel->EnableUpdate(true);
      seq = Attitude::ExtractEulerSequence(eulerSequence);
   }
}


//------------------------------------------------------------------------------
// void OnStateTypeSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateTypeSelection() entered\n");
   #endif
   std::string newStateType = stateTypeComboBox->GetStringSelection().c_str();
   if (newStateType == attStateType) return;
   if (!ValidateState("State"))
   {
      stateTypeComboBox->SetValue(wxT(attStateType.c_str()));
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value before changing the Attitude State Type\n");
     return;
   }
   
   if (newStateType == stateTypeArray[EULER_ANGLES])
      DisplayEulerAngles();
   else if (newStateType == stateTypeArray[QUATERNION])
      DisplayQuaternion();
   else if (newStateType == stateTypeArray[DCM])
      DisplayDCM();
      
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("   Now setting attitude state type to %s\n",
      newStateType.c_str());
   #endif
   attStateType      = newStateType;
   dataChanged       = true;
   stateTypeModified = true;
   theScPanel->EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnStateTypeRateSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void AttitudePanel::OnStateTypeRateSelection(wxCommandEvent &event)
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::OnStateTypeRateSelection() entered\n");
   #endif
   std::string newStateRateType = 
      stateRateTypeComboBox->GetStringSelection().c_str();
      if (newStateRateType == attRateStateType) return;
      
   if (!ValidateState("Both"))
   {
      stateRateTypeComboBox->SetValue(wxT(attRateStateType.c_str()));
      MessageInterface::PopupMessage(Gmat::ERROR_, +
         "Please enter valid value before changing the Attitude Rate State Type\n");
     return;
   }
  
   if (newStateRateType == stateRateTypeArray[EULER_ANGLE_RATES])
      DisplayEulerAngleRates();
   else if (newStateRateType == stateRateTypeArray[ANGULAR_VELOCITY])
      DisplayAngularVelocity();
      
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("   Now setting attitude rate state type to %s\n",
      newStateRateType.c_str());
   #endif
   attRateStateType      = newStateRateType;
   dataChanged           = true;
   rateStateTypeModified = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void DisplayEulerAngles()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayEulerAngles()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayEulerAngles() entered\n");
   #endif
   UpdateEulerAngles();
   stateTypeComboBox->
      SetValue(wxT("Euler Angles"));
   attStateType = STATE_TEXT[EULER_ANGLES];
   //col1StaticText->Show(false);
   //col2StaticText->Show(false);
   //col3StaticText->Show(false);
   
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

   st1TextCtrl->SetValue(*eulerAngles[0]);
   st2TextCtrl->SetValue(*eulerAngles[1]);
   st3TextCtrl->SetValue(*eulerAngles[2]);
   
   ResetStateFlags("State", true);
}

//------------------------------------------------------------------------------
// void DisplayQuaternion()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayQuaternion()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayQuaternion() entered\n");
   #endif
   UpdateQuaternion();
   stateTypeComboBox->
      SetValue(wxT("Quaternion"));
   attStateType = "Quaternion";
   //col1StaticText->Show(false);
   //col2StaticText->Show(false);
   //col3StaticText->Show(false);
   
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

   st1TextCtrl->SetValue(*quaternion[0]);
   st2TextCtrl->SetValue(*quaternion[1]);
   st3TextCtrl->SetValue(*quaternion[2]);
   st4TextCtrl->SetValue(*quaternion[3]);

   ResetStateFlags("State", true);
}

//------------------------------------------------------------------------------
// void DisplayDCM()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayDCM()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayDCM() entered\n");
   #endif
   UpdateCosineMatrix();
   stateTypeComboBox->
      SetValue(wxT("DirectionCosineMatrix"));
   attStateType = "DirectionCosineMatrix";
   //col1StaticText->Show(true);
   //col2StaticText->Show(true);
   //col3StaticText->Show(true);
   
   st1StaticText->Show(true);
   st2StaticText->Show(true);
   st3StaticText->Show(true);
   st1StaticText->Show(false);
   st2StaticText->Show(false);
   st3StaticText->Show(false);
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
   
   //st1StaticText->SetLabel(wxT("Row 1"));
   //st2StaticText->SetLabel(wxT("Row 2"));
   //st3StaticText->SetLabel(wxT("Row 3"));
   st1StaticText->SetLabel(wxT("     "));
   st2StaticText->SetLabel(wxT("     "));
   st3StaticText->SetLabel(wxT("     "));

   st1TextCtrl->SetValue(*cosineMatrix[0]);
   st2TextCtrl->SetValue(*cosineMatrix[3]);
   st3TextCtrl->SetValue(*cosineMatrix[6]);
   st5TextCtrl->SetValue(*cosineMatrix[1]);
   st6TextCtrl->SetValue(*cosineMatrix[4]);
   st7TextCtrl->SetValue(*cosineMatrix[7]);
   st8TextCtrl->SetValue(*cosineMatrix[2]);
   st9TextCtrl->SetValue(*cosineMatrix[5]);
   st10TextCtrl->SetValue(*cosineMatrix[8]);
   
   ResetStateFlags("State", true);
}


//------------------------------------------------------------------------------
// void DisplayEulerAngleRates()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayEulerAngleRates()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayEulerAngleRates() entered\n");
   #endif
   UpdateEulerAngleRates();
   stateRateTypeComboBox->
      SetValue(wxT("EulerAngleRates"));
   attRateStateType = "EulerAngleRates";
   str1StaticText->SetLabel(wxT("Euler Angle Rate 1"));
   str2StaticText->SetLabel(wxT("Euler Angle Rate 2"));
   str3StaticText->SetLabel(wxT("Euler Angle Rate 3"));

   str1TextCtrl->SetValue(*eulerAngleRates[0]);
   str2TextCtrl->SetValue(*eulerAngleRates[1]);
   str3TextCtrl->SetValue(*eulerAngleRates[2]);

   ResetStateFlags("Rate", true);

}

//------------------------------------------------------------------------------
// void DisplayAngularVelocity()
//------------------------------------------------------------------------------
void AttitudePanel::DisplayAngularVelocity()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::DisplayAngularVelocity() entered\n");
   #endif
   UpdateAngularVelocity();
   stateRateTypeComboBox->
      SetValue(wxT("AngularVelocity"));
   attRateStateType = "AngularVelocity";
   str1StaticText->SetLabel(wxT("Angular Velocity X"));
   str2StaticText->SetLabel(wxT("Angular Velocity Y"));
   str3StaticText->SetLabel(wxT("Angular Velocity Z"));

   str1TextCtrl->SetValue(*angVel[0]);
   str2TextCtrl->SetValue(*angVel[1]);
   str3TextCtrl->SetValue(*angVel[2]);

   ResetStateFlags("Rate", true);
}


//------------------------------------------------------------------------------
// void UpdateCosineMatrix()
//------------------------------------------------------------------------------
void AttitudePanel::UpdateCosineMatrix()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateCosineMatrix() entered\n");
   #endif
   if (attStateType == stateTypeArray[DCM]) return;
   if (attStateType == stateTypeArray[QUATERNION])
   {
      mat = Attitude::ToCosineMatrix(q);
   }
   else if (attStateType == stateTypeArray[EULER_ANGLES])
   {
      mat = Attitude::ToCosineMatrix(ea * GmatMathUtil::RAD_PER_DEG, 
                      (Integer) seq[0], (Integer) seq[1], (Integer) seq[2]);
   }
   // update string versions of mat values (cosineMatrix)
   unsigned int x, y;
   for (x = 0; x < 3; ++x)
      for (y = 0; y < 3; ++y)
         *cosineMatrix[x*3+y] = theGuiManager->ToWxString(mat(x,y));
}

//------------------------------------------------------------------------------
// void UpdateQuaternion()
//------------------------------------------------------------------------------
void AttitudePanel::UpdateQuaternion()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateQuaternion() entered\n");
   #endif
   if (attStateType == stateTypeArray[QUATERNION]) return;
   if (attStateType == stateTypeArray[DCM])
   {
      q = Attitude::ToQuaternion(mat);
   }
   else if (attStateType == stateTypeArray[EULER_ANGLES])
   {
      q = Attitude::ToQuaternion(ea * GmatMathUtil::RAD_PER_DEG, 
                    (Integer) seq[0], (Integer) seq[1], (Integer) seq[2]);
   }
   // update string versions of q values 
   for (unsigned int x = 0; x < 4; ++x)
      *quaternion[x] = theGuiManager->ToWxString(q[x]);
}

//------------------------------------------------------------------------------
// void UpdateEulerAngles()
//------------------------------------------------------------------------------
void AttitudePanel::UpdateEulerAngles()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateEulerAngles() entered\n");
   #endif
   if (attStateType == stateTypeArray[EULER_ANGLES]) return;
   if (attStateType == stateTypeArray[DCM])
   {
      ea = Attitude::ToEulerAngles(mat, 
                     (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                     * GmatMathUtil::DEG_PER_RAD;
   }
   else if (attStateType == stateTypeArray[QUATERNION])
   {
      ea = Attitude::ToEulerAngles(q, 
                    (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                    * GmatMathUtil::DEG_PER_RAD;
   }
   // update string versions of ea values 
   for (unsigned int x = 0; x < 3; ++x)
      *eulerAngles[x] = theGuiManager->ToWxString(ea[x]);
}

//------------------------------------------------------------------------------
// void UpdateAngularVelocity()
//------------------------------------------------------------------------------
void AttitudePanel::UpdateAngularVelocity()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateAngularVelocity() entered\n");
   #endif
   if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY]) return;
   if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES])
   {
      UpdateEulerAngles();
      av = Attitude::ToAngularVelocity(ear * GmatMathUtil::RAD_PER_DEG, 
                     ea * GmatMathUtil::RAD_PER_DEG, 
                     (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                     * GmatMathUtil::DEG_PER_RAD;
   }
   // update string versions of av values 
   for (unsigned int x = 0; x < 3; ++x)
      *angVel[x] = theGuiManager->ToWxString(av[x]);
}

//------------------------------------------------------------------------------
// void UpdateEulerAngleRates()
//------------------------------------------------------------------------------
void AttitudePanel::UpdateEulerAngleRates()
{
   #ifdef DEBUG_ATTITUDE_PANEL
      MessageInterface::ShowMessage("AttitudePanel::UpdateEulerAngleRates() entered\n");
   #endif
   if (attRateStateType == stateRateTypeArray[EULER_ANGLE_RATES]) return;
   if (attRateStateType == stateRateTypeArray[ANGULAR_VELOCITY])
   {
      UpdateEulerAngles();
      ear = Attitude::ToEulerAngleRates(av * GmatMathUtil::RAD_PER_DEG, 
                      ea * GmatMathUtil::RAD_PER_DEG, 
                      (Integer) seq[0], (Integer) seq[1], (Integer) seq[2])
                      * GmatMathUtil::DEG_PER_RAD;
   }
   // update string versions of av values 
      for (unsigned int x = 0; x < 3; ++x)
         *eulerAngleRates[x] = theGuiManager->ToWxString(ear[x]);
}
