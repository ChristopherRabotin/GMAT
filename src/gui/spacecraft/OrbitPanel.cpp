//$Id$
//------------------------------------------------------------------------------
//                           OrbitPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Allison Greene
// Created: 2004/04/08
/**
 * This class contains information needed to setup users spacecraft orbit
 * through GUI
 * 
 * The basic design for state handling on this panel is that the spacecraft is
 * the repository for the state information.  When the panel is displayed, a 
 * clone of the spacecraft is made and the state information for that spacecraft
 * clone is accessed to populate the data on the panel.  This population is done
 * by accessing the spacecraft state from the spacecraft's internal PropState,
 * which is a Cartesian MJ2000 equatorial state expressed in terms of the 
 * current GMAT internal CoordinateSystem.  When the panel is populated, that 
 * state is converted to the coordinate system and representation desired for
 * display, and then used to fill in the elements on the panel.  The OrbitPanel
 * maintains the state in the internal MJ2000 equatorial Cartesian coordinates,
 * in the mCartState member.  All conversions and transformations return to this
 * representation internally.
 *
 */
//------------------------------------------------------------------------------
#include "OrbitPanel.hpp"
#include "MessageInterface.hpp"
#include "StateConversionUtil.hpp"
#include "RealUtilities.hpp"
#include "GmatGlobal.hpp"
#include <sstream>
#include <wx/config.h>

//#define DEBUG_ORBIT_PANEL
//#define DEBUG_ORBIT_PANEL_LOAD
//#define DEBUG_ORBIT_PANEL_CONVERT
//#define DEBUG_ORBIT_PANEL_CHECKSTATE
//#define DEBUG_ORBIT_PANEL_COMBOBOX
//#define DEBUG_ORBIT_PANEL_STATE_TYPE
//#define DEBUG_ORBIT_PANEL_STATE_CHANGE
//#define DEBUG_ORBIT_PANEL_TEXT_CHANGE
//#define DEBUG_ORBIT_PANEL_REFRESH
//#define DEBUG_ORBIT_PANEL_SAVE
//#define DEBUG_ORBIT_PANEL_CHECK_RANGE

//------------------------------
// event tables for wxWidgets
//------------------------------
BEGIN_EVENT_TABLE( OrbitPanel, wxPanel)
   EVT_COMBOBOX(ID_COMBOBOX, OrbitPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, OrbitPanel::OnTextChange)
   EVT_BUTTON(ID_BUTTON, OrbitPanel::OnButton)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// OrbitPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft,
//            SolarSystem *solarsystem)
//------------------------------------------------------------------------------
/**
 * Constructs an OrbitPanel object.
 *
 * @param <scPanel>     the parent SpacecraftPanel
 * @param <parent>      window parent
 * @param <spacecraft>  the associated spacecraft
 * @param <solarSystem> pointer to the solar system
 *
 */
//------------------------------------------------------------------------------
OrbitPanel::OrbitPanel(GmatPanel *scPanel, wxWindow *parent,
                       Spacecraft *spacecraft, SolarSystem *solarsystem)
   : wxPanel(parent)
{
   // initialize data members
   theScPanel = scPanel;
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   theSpacecraft = spacecraft;
   theSolarSystem = solarsystem;
   
   mIsCoordSysChanged = false;
   mIsStateTypeChanged = false;
   mIsAnomalyTypeChanged = false;
   mIsStateChanged = false;
   mShowFullStateTypeList = true;
   ResetStateFlags();   
   mIsEpochChanged = false;
   mIsEpochModified = false;
   canClose = true;
   dataChanged = false;
   
   Create();
}


//------------------------------------------------------------------------------
// ~OrbitPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the OrbitPanel object.
 */
//------------------------------------------------------------------------------
OrbitPanel::~OrbitPanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
   theGuiManager->RemoveFromResourceUpdateListeners(theScPanel);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Gets the values from theSpacecraft and puts them in the corresponding
 * GUI components
 */
//------------------------------------------------------------------------------
void OrbitPanel::LoadData()
{
   #ifdef DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage("OrbitPanel::LoadData() entered\n");
   #endif
      
   mAnomaly     = theSpacecraft->GetAnomaly();
   mTrueAnomaly = mAnomaly;
   
   mInternalCoord = theGuiInterpreter->GetInternalCoordinateSystem();
   
   if (mInternalCoord == NULL)
   {
      MessageInterface::ShowMessage("   mInternalCoord is NULL.\n");
   }
   else
   {
      #ifdef DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage(
            "   mInternalCoord=%s, addr=%d\n",
            mInternalCoord->GetName().c_str(), mInternalCoord);
      #endif
   }
   
   // load data from the core engine
   try
   {
      Rvector6 outState, displayState;
      
      // Load the epoch formats
      StringArray reps = TimeConverterUtil::GetValidTimeRepresentations();
      for (unsigned int i = 0; i < reps.size(); i++)
         epochFormatComboBox->Append(reps[i].c_str());
      
      // load the epoch
      std::string epochFormat = theSpacecraft->GetStringParameter("DateFormat");      
      mEpoch = theSpacecraft->GetRealParameter("A1Epoch");
      
      #ifdef DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage
         ("   epochFormat=%s, mEpoch=%f\n", epochFormat.c_str(), mEpoch);
      #endif
      
      epochFormatComboBox->SetValue(wxT(epochFormat.c_str()));
      mFromEpochFormat = epochFormat;
      
      mEpochStr = theSpacecraft->GetStringParameter("Epoch"); 
      epochValue->SetValue(wxT(mEpochStr.c_str()));
      
      // Save to TAIModJulian string to avoid keep reading the field
      // and convert to proper format when ComboBox is changed.
      if (epochFormat == "TAIModJulian")
      {
         mTaiMjdStr = mEpochStr;
      }
      else
      {
         Real fromMjd = -999.999;
         Real outMjd;
         std::string outStr;
         TimeConverterUtil::Convert(mFromEpochFormat, fromMjd, mEpochStr,
                                    "TAIModJulian", outMjd, outStr);
         mTaiMjdStr = outStr;
         
         #ifdef DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage("   mTaiMjdStr=%s\n", mTaiMjdStr.c_str());
         #endif
      }
      
      // load the coordinate system
      std::string coordSystemStr =
         theSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      
      mCoordSysComboBox->SetValue(coordSystemStr.c_str());
      mFromCoordSysStr = coordSystemStr;
      
      mOutCoord = (CoordinateSystem*)theSpacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, "");
      #ifdef DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage
         ("   coordSystemStr=%s, mOutCoord=%p\n", coordSystemStr.c_str(), mOutCoord);
      #endif
      
      if (mOutCoord == NULL)
      {
         mOutCoord = (CoordinateSystem*)theGuiInterpreter->
            GetConfiguredObject(coordSystemStr);
         
         #ifdef DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   Get configured %s %p\n", coordSystemStr.c_str(), mOutCoord);
         #endif
         
      }
      
      mFromCoord = mOutCoord;
      
      if (mOutCoord == NULL)
      {
         MessageInterface::ShowMessage
            ("   The Spacecraft CoordinateSystem is NULL.\n");
      }
      else
      {
         #ifdef DEBUG_ORBIT_PANEL_LOAD
            MessageInterface::ShowMessage("   mOutCoord=%s, addr=%d\n",
               mOutCoord->GetName().c_str(), mOutCoord);
         #endif
         
         // Set the CSs on the spacecraft
         theSpacecraft->SetInternalCoordSystem(mInternalCoord);
         theSpacecraft->SetRefObjectName(Gmat::COORDINATE_SYSTEM, mOutCoord->GetName());
         theSpacecraft->SetRefObject(mOutCoord, Gmat::COORDINATE_SYSTEM);
      }
      
      // get the origin for the output coordinate system
      std::string originName = mOutCoord->GetStringParameter("Origin");
      
      #ifdef DEBUG_ORBIT_PANEL_LOAD
         SpacePoint *origin = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(originName);
         MessageInterface::ShowMessage
            ("   origin=%s, addr=%d\n", originName.c_str(), origin);
      #endif
         
      mFromStateTypeStr = theSpacecraft->GetStringParameter("DisplayStateType");
      
      #ifdef DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   mFromStateTypeStr=%s\n", mFromStateTypeStr.c_str());
      #endif

      BuildValidStateTypes();

      BuildValidCoordinateSystemList(mFromStateTypeStr);
         
      Integer typeCount = 0;
      
      // load the anomaly type - if state type is Keplerian or Modified Keplerian
      mAnomalyType = theSpacecraft->GetStringParameter("AnomalyType");
      
      #ifdef DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage("   mAnomalyType (from SC) = %s \n", mAnomalyType.c_str());
      #endif
      
      // Get anomaly type list from the base code (Anomaly)
      const std::string *anomalyTypeList = StateConversionUtil::GetLongTypeNameList();
      typeCount = StateConversionUtil::AnomalyTypeCount;
      for (int i = 0; i<typeCount; i++)
      {
         mAnomalyTypeNames.push_back(anomalyTypeList[i]);
         anomalyComboBox->Append(wxString(anomalyTypeList[i].c_str()));
      }
      
      if ( (mFromStateTypeStr == mStateTypeNames[StateConversionUtil::KEPLERIAN]) ||
           (mFromStateTypeStr == mStateTypeNames[StateConversionUtil::MOD_KEPLERIAN]) )
         //if (mFromStateTypeStr == "Keplerian" || mFromStateTypeStr == "ModKeplerian")
      {
         anomalyComboBox->SetValue(wxT(mAnomalyType.c_str()));
         mFromAnomalyTypeStr = mAnomalyType;
      }
      
      #ifdef DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage
         ("   mAnomalyType=\n   [%s]\n", mAnomalyType.c_str());
      #endif
      
      // Get Spacecraft initial state
      mCartState.Set(theSpacecraft->GetState().GetState());
      mTempCartState = mCartState;
      mOutState = mCartState;
      
      #ifdef DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage
         ("   mCartState=\n   [%s]\n", mCartState.ToString(16).c_str());
      #endif

      DisplayState();
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OrbitPanel:LoadData() error occurred!\n%s\n", e.GetFullMessage().c_str());
   }
   
   #ifdef DEBUG_ORBIT_PANEL_LOAD
   MessageInterface::ShowMessage("OrbitPanel::LoadData() leaving\n");
   #endif
   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Gets the updated values from the GUI components and saves them to theSpacecraft
 */
//------------------------------------------------------------------------------
void OrbitPanel::SaveData()
{
   #ifdef DEBUG_ORBIT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OrbitPanel::SaveData() entered\n   mCartState=%s\n   "
       "mTempCartState=%s\n   mOutState=%s\n", mCartState.ToString(16).c_str(), 
       mTempCartState.ToString(16).c_str(), mOutState.ToString(16).c_str());
   MessageInterface::ShowMessage("   mIsCoordSysChanged=%d\n", mIsCoordSysChanged);
   MessageInterface::ShowMessage("   mIsStateTypeChanged=%d\n", mIsStateTypeChanged);
   MessageInterface::ShowMessage("   mIsAnomalyTypeChanged=%d\n", mIsAnomalyTypeChanged);
   MessageInterface::ShowMessage("   mIsStateChanged=%d\n", mIsStateChanged);
   MessageInterface::ShowMessage("   mIsEpochChanged=%d\n", mIsEpochChanged);
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------
   // check and save epoch
   //-----------------------------------------------------------
   if (mIsEpochChanged)
   {
      std::string newEpoch = epochValue->GetValue().c_str();
      std::string epochFormat = epochFormatComboBox->GetValue().c_str();
      Real fromMjd = -999.999;
      Real a1mjd = -999.999;
      std::string outStr;
      
      #ifdef DEBUG_ORBIT_PANEL_SAVE
      MessageInterface::ShowMessage
         ("   newEpoch=%s, epochFormat=%s\n", newEpoch.c_str(), epochFormat.c_str());
      #endif
      
      if (mIsEpochChanged)
      {
         try
         {
            bool timeOK = theScPanel->CheckTimeFormatAndValue(epochFormat, newEpoch,
                  "Epoch", true);
            // time sent to the spacecraft should be in A1   WCS 2010.05.22
            if (timeOK)
            {
               TimeConverterUtil::Convert(epochFormat, fromMjd, newEpoch,
                                          "A1ModJulian", a1mjd, outStr);

               theSpacecraft->SetEpoch(epochFormat, newEpoch, a1mjd);
               mEpochStr = outStr;
               mEpoch = a1mjd;
               mIsEpochChanged = false;
            }
            else
            {
               #ifdef DEBUG_ORBIT_PANEL_SAVE
                  MessageInterface::ShowMessage("OrbitPanel::SaveData() setting canClose to false inside (epoch) try\n");
               #endif
               canClose = false;
            }
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
            #ifdef DEBUG_ORBIT_PANEL_SAVE
            MessageInterface::ShowMessage("OrbitPanel::SaveData() setting canClose to false after epoch change part\n");
            #endif
            canClose = false;
         }
      }
   }
   
   
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   // save the current text values to re-display, to avoid floating point differences
   Rvector6 savedState;
   for (int ii = 0; ii < 6; ii++)
      savedState[ii] = atof(textCtrl[ii]->GetValue());
   
   try
   {
      //-----------------------------------------------------------
      // check and save orbital elements
      //-----------------------------------------------------------
      if (mIsStateChanged)
      {
         Rvector6 state;
         bool retval = CheckState(state);
         #ifdef DEBUG_ORBIT_PANEL_SAVE
         MessageInterface::ShowMessage("OrbitPanel::SaveData() setting canClose to %s in state change part\n", retval? "true" : "false");
         #endif
         canClose = retval;
         
         if (retval)
         {
            // Build state uses new state type
            BuildState(state);
            theSpacecraft->SetState(stateTypeStr, mCartState);
            ComputeTrueAnomaly(mCartState, stateTypeStr);
            theSpacecraft->SetAnomaly(mAnomalyType, mTrueAnomaly);
            
            // Since BuildState() recomputes internal state and compute back to
            // current state type, numbers will be different, so re display
            // mOutState. (savedState; mOutState is modified in BuildState - wcs 2011.12.20)
            for (int i=0; i<6; i++)
               textCtrl[i]->SetValue(ToString(savedState[i]));
            
            ResetStateFlags(true);
            
            mIsStateChanged = false;
            mIsStateTypeChanged = false;
            mIsAnomalyTypeChanged = false;
         }
      }
      
      //--------------------------------------------------------
      // save state type if changed
      //--------------------------------------------------------
      if (mIsStateTypeChanged || mIsAnomalyTypeChanged)
      {
         theSpacecraft->SetState(stateTypeStr, mCartState);
         theSpacecraft->SetAnomaly(mAnomalyType, mTrueAnomaly);
         
         mIsStateTypeChanged = false;
         mIsAnomalyTypeChanged = false;        
      }
      
      //--------------------------------------------------------
      // save coordinate system name if changed
      //--------------------------------------------------------
      if (mIsCoordSysChanged)
      {
         wxString coordSysStr  = mCoordSysComboBox->GetValue();
         mIsCoordSysChanged = false;
         theSpacecraft->
            SetRefObjectName(Gmat::COORDINATE_SYSTEM, coordSysStr.c_str());
      }
      
      if (canClose)
         dataChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      #ifdef DEBUG_ORBIT_PANEL_SAVE
      MessageInterface::ShowMessage("OrbitPanel::SaveData() setting canClose to false in final catch clause\n");
      #endif
      canClose = false;
      return;
   }
      
   #ifdef DEBUG_ORBIT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OrbitPanel::SaveData() exiting\n   mCartState=%s\n"
       "   mTempCartState=%s\n   mOutState=%s\n", mCartState.ToString(16).c_str(),
       mTempCartState.ToString(16).c_str(), mOutState.ToString(16).c_str());
   MessageInterface::ShowMessage("****** canClose = %d\n", canClose);
   #endif
}

//------------------------------------------------------------------------------
//bool RefreshComponents()
//------------------------------------------------------------------------------
/**
 * Refreshes the components that need refreshing when the panel/tab is activated
 * (currently only the CoordinateSystem ComboBox).
 *
 * @return true if components were successfully refreshed; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::RefreshComponents()
{
   #ifdef DEBUG_ORBIT_PANEL_REFRESH
   MessageInterface::ShowMessage
      ("OrbitPanel::RefreshComponents called ...\n");
   #endif
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   BuildValidCoordinateSystemList(stateTypeStr);
   return true;
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the GUI components (text fields, combo boxes, etc.) for the
 * spacecraft information.
 */
//------------------------------------------------------------------------------
void OrbitPanel::Create()
{
   #ifdef DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage("In OrbitPanel::Create() \n");
   #endif

   Integer bsize = 2; // border size

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Orbit"));

   //-----------------------------------------------------------------
   //  create sizers 
   //-----------------------------------------------------------------
   // sizer for orbit tab
   wxBoxSizer *orbitSizer = new wxBoxSizer(wxHORIZONTAL);
   
   //flex grid sizer for the epoch format, coordinate system and state type
   //wxFlexGridSizer *epochSizer = new wxFlexGridSizer(5, 2, bsize, bsize);
   wxFlexGridSizer *epochSizer = new wxFlexGridSizer(2);
   
   // static box for the orbit def and elements
   wxStaticBoxSizer *orbitDefSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "");
   wxStaticBoxSizer *elementSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Elements");
   
   //-----------------------------------------------------------------
   // epoch 
   //-----------------------------------------------------------------
   // label for epoch format
   wxStaticText *epochFormatStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Epoch "GUI_ACCEL_KEY"Format"), wxDefaultPosition, wxDefaultSize, 0 );
   
   int epochWidth = 170;
   #ifdef __WXMAC__
      epochWidth = 178;
   #endif
   // combo box for the epoch format
   epochFormatComboBox = new wxComboBox
      ( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
        emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   epochFormatComboBox->SetToolTip(pConfig->Read(_T("EpochFormatHint")));
   
   // label for epoch
   wxStaticText *epochStaticText = new wxStaticText( this, ID_TEXT,
      wxT(GUI_ACCEL_KEY"Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
   
   // textfield for the epoch value
   epochValue = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(epochWidth,-1), 0 );
   epochValue->SetToolTip(pConfig->Read(_T("EpochHint")));

   //-----------------------------------------------------------------
   //  coordinate system 
   //-----------------------------------------------------------------
   // label for coordinate system
   wxStaticText *coordSysStaticText = new wxStaticText( this, ID_TEXT,
      wxT(GUI_ACCEL_KEY"Coordinate System"), wxDefaultPosition, wxDefaultSize, 0 );

   //Get CordinateSystem ComboBox from the GuiItemManager.
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(epochWidth,-1));
   // register for updates, in order to refresh
   theGuiManager->AddToResourceUpdateListeners(theScPanel);

   mCoordSysComboBox->SetToolTip(pConfig->Read(_T("CoordinateSystemHint")));

   //-----------------------------------------------------------------
   //  state type 
   //-----------------------------------------------------------------
   // label for state type
   wxStaticText *stateTypeStaticText = new wxStaticText( this, ID_TEXT,
      wxT(GUI_ACCEL_KEY"State Type"), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the state
   stateTypeComboBox = new wxComboBox
      ( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(epochWidth,-1),
        emptyList, wxCB_DROPDOWN | wxCB_READONLY);
   stateTypeComboBox->SetToolTip(pConfig->Read(_T("StateTypeHint")));
   
   //-----------------------------------------------------------------
   //  anomaly 
   //-----------------------------------------------------------------
   // label for anomaly type
   anomalyStaticText = new wxStaticText( this, ID_TEXT,
      wxT(GUI_ACCEL_KEY"Anomaly Type "), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the anomaly type
   anomalyComboBox = new wxComboBox
      ( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(150,-1),
        emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   anomalyComboBox->SetToolTip(pConfig->Read(_T("AnomalyHint")));
   
   // add to epoch sizer
   epochSizer->AddGrowableCol( 1 );
   epochSizer->Add( epochFormatStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   epochSizer->Add( epochFormatComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   
   epochSizer->Add( epochStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   epochSizer->Add( epochValue, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   
   epochSizer->Add( coordSysStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   epochSizer->Add( mCoordSysComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   
   epochSizer->Add( stateTypeStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   epochSizer->Add( stateTypeComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   
   epochSizer->Add( anomalyStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   epochSizer->Add( anomalyComboBox, 0, wxGROW|wxALIGN_LEFT | wxALL, bsize );
   
   // wcs 2010.01.27 remove the Anomaly combo box for now, per S. Hughes
   anomalyStaticText->Show(false);
   anomalyComboBox->Show(false);

   // panel that has the labels and text fields for the elements
   // adds default descriptors/labels
   AddElements(this);
   orbitDefSizer->Add( epochSizer, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   elementSizer->Add( elementsPanel, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize);

   //add orbit designer button
   orbitDesignerButton = new wxButton
      (this, ID_BUTTON, wxT("Orbit Designer"), wxDefaultPosition, wxDefaultSize, 0);
   elementSizer->Add(orbitDesignerButton, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, bsize );
   // tgg 2012-12-14 Remove At-risk Orbit Designer feature, GMT-3383
   orbitDesignerButton->Show(false);

   orbitSizer->Add( orbitDefSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   orbitSizer->Add( elementSizer, 1, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   
   this->SetSizer( orbitSizer );

   gg           = GmatGlobal::Instance();
   errMsgFormat = theSpacecraft->GetErrorMessageFormat();

}

//------------------------------------------------------------------------------
// void AddElements(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Creates the GUI components to hold the spacecraft elements
 *
 * @param <parent> input parent.
 */
//------------------------------------------------------------------------------
void OrbitPanel::AddElements(wxWindow *parent)
{
   Integer bsize = 2; // border size

   elementsPanel = new wxPanel(parent);
   wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );
   wxFlexGridSizer *item3 = new wxFlexGridSizer( 6, 3, 0, 0 );
   
   // Element 1
   description1 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   textCtrl[0] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, 
                     wxT(""), wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unit1 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit1"), 
               wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 2
   description2 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   textCtrl[1] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unit2 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit2"), 
               wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 3
   description3 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   textCtrl[2] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unit3 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit3"), 
                wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 4
   description4 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   textCtrl[3] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unit4 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit4"), 
                wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 5    
   description5 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   textCtrl[4] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unit5 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit5"), 
                wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 6
   description6 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   textCtrl[5] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""),
                                wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   unit6 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit6"), 
                wxDefaultPosition, wxDefaultSize, 0 );
   
   // Add to wx*Sizers 
   item3->Add( description1, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[0], 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit1, 0, wxALIGN_LEFT|wxALL, bsize );
   
   item3->Add( description2, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[1], 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit2, 0, wxALIGN_LEFT|wxALL, bsize );
   
   item3->Add( description3, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[2], 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit3, 0, wxALIGN_LEFT|wxALL, bsize );
   
   item3->Add( description4, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[3], 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit4, 0, wxALIGN_LEFT|wxALL, bsize );
   
   item3->Add( description5, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[4], 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit5, 0, wxALIGN_LEFT|wxALL, bsize );
   
   item3->Add( description6, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[5], 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit6, 0, wxALIGN_LEFT|wxALL, bsize );

   item0->Add( item3, 0, wxGROW|wxALL|wxALIGN_CENTER, bsize );

   elementsPanel->SetAutoLayout( TRUE );
   elementsPanel->SetSizer( item0 );

   item0->Fit( elementsPanel );
   item0->SetSizeHints( elementsPanel );
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Converts state to ComboBox selection using utility.
 *
 * @param <event> the wxCommandEvent to handle
 */
//------------------------------------------------------------------------------ 
void OrbitPanel::OnComboBoxChange(wxCommandEvent& event)
{
   std::string coordSysStr  = mCoordSysComboBox->GetValue().c_str();
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   
   #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("OrbitPanel::OnComboBoxChange() coordSysStr=%s, stateTypeStr=%s\n",
       coordSysStr.c_str(), stateTypeStr.c_str());
   #endif
   
   //-----------------------------------------------------------------
   // epoch format change
   //-----------------------------------------------------------------
   if (event.GetEventObject() == epochFormatComboBox)
   {
      mIsEpochChanged = true;
      std::string toEpochFormat = epochFormatComboBox->GetValue().c_str();    
      
      #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("   OnComboBoxChange() mFromEpochFormat=%s, "
          "toEpochFormat=%s\n   mEpochStr=%s, mIsEpochModified=%d\n",
          mFromEpochFormat.c_str(), toEpochFormat.c_str(), mEpochStr.c_str(),
          mIsEpochModified);
      #endif
      
      try
      {
         Real fromMjd = -999.999;
         Real outMjd;
         std::string outStr;
         
         // if modified by user, check if epoch is valid first
         if (mIsEpochModified)
         {
            mEpochStr = epochValue->GetValue().c_str();
            
            // Save to TAIModJulian string to avoid keep reading the field
            // and convert to proper format when ComboBox is changed.
            if (mFromEpochFormat == "TAIModJulian")
            {
               mTaiMjdStr = mEpochStr;
            }
            else
            {
               TimeConverterUtil::Convert(mFromEpochFormat, fromMjd, mEpochStr,
                                          "TAIModJulian", outMjd, outStr);
               mTaiMjdStr = outStr;
            }
            
            // Convert to desired format with new date
            TimeConverterUtil::Convert(mFromEpochFormat, fromMjd, mEpochStr,
                                       toEpochFormat, outMjd, outStr);
            
            epochValue->SetValue(outStr.c_str());
            mIsEpochModified = false;
            mFromEpochFormat = toEpochFormat;
         }
         else
         {
            // Convert to desired format using TAIModJulian date
            TimeConverterUtil::Convert("TAIModJulian", fromMjd, mTaiMjdStr,
                                       toEpochFormat, outMjd, outStr);
            
            epochValue->SetValue(outStr.c_str());
            mFromEpochFormat = toEpochFormat;
         }
      }
      catch (BaseException &e)
      {
         epochFormatComboBox->SetValue(mFromEpochFormat.c_str());
         theSpacecraft->SetDateFormat(mFromEpochFormat);
         MessageInterface::PopupMessage
            (Gmat::ERROR_, e.GetFullMessage() +
             "\nPlease enter valid Epoch before changing the Epoch Format\n");
      }
   }
   //-----------------------------------------------------------------
   // coordinate system or state type change
   //-----------------------------------------------------------------
   else if (event.GetEventObject() == mCoordSysComboBox ||
            event.GetEventObject() == stateTypeComboBox)
   {      
      #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("   OnComboBoxChange() mFromCoordSysStr=%s, mFromStateTypeStr=%s\n",
          mFromCoordSysStr.c_str(), mFromStateTypeStr.c_str());
      MessageInterface::ShowMessage("------ epochChanged = %s, epochModified = %s, stateModified = %s\n",
           (mIsEpochChanged? "true" : "false"), (mIsEpochModified? "true" : "false"),
           (IsStateModified()? "true" : "false"));
      #endif
      
      if (event.GetEventObject() == mCoordSysComboBox)
         mIsCoordSysChanged = true;
      
      if (event.GetEventObject() == stateTypeComboBox)
      {
         Rvector6 state;
         mIsStateTypeChanged = true;
         bool retval = false;
         
         // If state modified by user, validate elements first
         if (IsStateModified())
         {
            retval = CheckState(state);
            
            if (!retval)
            {
               stateTypeComboBox->SetValue(mFromStateTypeStr.c_str());
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, +
                   "Please enter valid state before changing the State Type\n");  // *****
               return;
            }
         }
         // make sure to compute the true anomaly if the state is converted form Cartesian
         if (mFromStateTypeStr == "Cartesian")
         {
            try
            {
               Real mu      = GetOriginData(theSpacecraft->GetOrigin(), "mu");
               Real flat    = GetOriginData(theSpacecraft->GetOrigin(), "flattening");
               Real radius  = GetOriginData(theSpacecraft->GetOrigin(), "radius");
               Rvector6 st  = StateConversionUtil::Convert(mCartState, mFromStateTypeStr,
                              "Keplerian", mu, flat, radius, "TA");
               mTrueAnomaly = st[5];
               mAnomaly = mTrueAnomaly;
               mAnomalyType = mAnomalyTypeNames[StateConversionUtil::TA];
               mFromAnomalyTypeStr = mAnomalyType;

               #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
               MessageInterface::ShowMessage
                  ("   Computed TrueAnomaly =\n   [%12.10f]\n", mTrueAnomaly);
               #endif
            }
            catch (BaseException &be)
            {
               #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
               MessageInterface::ShowMessage
                  ("ERROR computing True Anomaly - message: %s\n", (be.GetFullMessage()).c_str());
               #endif
               //throw; // GMAT crashes if I put this in here ... ???
            }
         }


         BuildValidCoordinateSystemList(stateTypeStr);
      }
      
      CoordinateSystem *prevCoord = mOutCoord;
      
      mOutCoord = (CoordinateSystem*)theGuiInterpreter->
         GetConfiguredObject(mCoordSysComboBox->GetValue().c_str());
      
      BuildValidStateTypes();
      
      try
      {
         DisplayState();
         
         if (event.GetEventObject() == mCoordSysComboBox)
            mFromCoordSysStr = mCoordSysComboBox->GetValue().c_str();
         
         if (event.GetEventObject() == stateTypeComboBox)
            mFromStateTypeStr = stateTypeComboBox->GetValue().c_str();
         
         mFromCoord = mOutCoord;
         theSpacecraft->SetRefObjectName(Gmat::COORDINATE_SYSTEM, mOutCoord->GetName());
         theSpacecraft->SetRefObject(mOutCoord, Gmat::COORDINATE_SYSTEM);
      }
      catch (BaseException &)
      {
         mCoordSysComboBox->SetValue(mFromCoordSysStr.c_str());
         stateTypeComboBox->SetValue(mFromStateTypeStr.c_str());
         mOutCoord = prevCoord;
         BuildValidStateTypes();
         return;
      }
   }
   //-----------------------------------------------------------------
   // anomaly type change 
   //-----------------------------------------------------------------
   else if (event.GetEventObject() == anomalyComboBox)
   {
      mIsAnomalyTypeChanged = true;
      wxString description, stateValue;      
      mAnomalyType = anomalyComboBox->GetValue();
      // ****** NOTE: For now, since only "TA" is allowed, GetValue will return "".  Reset to "TA" ******
      mAnomalyType = mAnomalyTypeNames[StateConversionUtil::TA];
      
      #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("   OnComboBoxChange() mFromAnomalyTypeStr=%s, "
          "mAnomalyType=%s\n", mFromAnomalyTypeStr.c_str(),
          mAnomalyType.c_str());
      #endif
      
      Rvector6 state;
      bool retval = false;
      
      // If state modified by user, validate elements first
      if (IsStateModified())
      {
         retval = CheckState(state);
         
         if (!retval)
         {
            anomalyComboBox->SetValue(mFromAnomalyTypeStr.c_str());
            MessageInterface::PopupMessage
               (Gmat::ERROR_, +
                "Please enter valid value before changing the Anomaly Type\n");
            return;
         }
      }
      
      try
      {
         DisplayState();
         mFromAnomalyTypeStr = mAnomalyType;
      }
      catch (BaseException &)
      {
         anomalyComboBox->SetValue(mFromAnomalyTypeStr.c_str());
         return;
      }
   }
   
   dataChanged = true;
   theScPanel->EnableUpdate(true);
   
   #ifdef DEBUG_ORBIT_PANEL_COMBOBOX
   MessageInterface::ShowMessage
      ("OrbitPanel::OnComboBoxChange() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Handles text change event.
 *
 * @param <event> the wxCommandEvent to handle
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnTextChange(wxCommandEvent& event)
{
   wxObject *obj = event.GetEventObject();
   bool isStateText = false;
   
   for (int i=0; i<6; i++)
      if (obj == textCtrl[i])
         isStateText = true;
   
   if (isStateText)
   {
      for (int i=0; i<6; i++)
         if (textCtrl[i]->IsModified())
            mIsStateModified[i] = true;

      if (IsStateModified())
      {
         #ifdef DEBUG_ORBIT_PANEL_TEXT_CHANGE
            MessageInterface::ShowMessage("--- The state has been modified\n");
         #endif
         mIsStateChanged = true;
         dataChanged = true;
         theScPanel->EnableUpdate(true);
      }
   }
   else if (obj == epochValue && epochValue->IsModified())
   {
      #ifdef DEBUG_ORBIT_PANEL_TEXT_CHANGE
         MessageInterface::ShowMessage("--- The epoch has been changed to: %s\n",
               (epochValue->GetValue()).c_str());
      #endif
      mIsEpochChanged = true;
      mIsEpochModified = true;
      dataChanged = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/*
 * Handles button event.
 *
 * @param <event> the wxCommandEvent to handle
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnButton(wxCommandEvent& event)
{

   OrbitDesignerDialog orbitDlg(this, theSpacecraft);
   orbitDlg.ShowModal();

   if (!orbitDlg.updateOrbit)
      return;
   stateTypeComboBox->SetValue(wxT("Keplerian"));
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();


   Rvector6 state;
   mIsStateTypeChanged = true;
   bool retval = false;

   retval = CheckState(state);

   CoordinateSystem *prevCoord = mOutCoord;

   if (retval)
   {
      BuildValidCoordinateSystemList(stateTypeStr);

      mOutCoord = (CoordinateSystem*)theGuiInterpreter->
      GetConfiguredObject(mCoordSysComboBox->GetValue().c_str());
   }

   BuildValidStateTypes();

   try
   {
      DisplayState();

      if (event.GetEventObject() == mCoordSysComboBox)
         mFromCoordSysStr = mCoordSysComboBox->GetValue().c_str();

      if (event.GetEventObject() == stateTypeComboBox)
         mFromStateTypeStr = stateTypeComboBox->GetValue().c_str();

      mFromCoord = mOutCoord;
      theSpacecraft->SetRefObjectName(Gmat::COORDINATE_SYSTEM, mOutCoord->GetName());
      theSpacecraft->SetRefObject(mOutCoord, Gmat::COORDINATE_SYSTEM);
   }
   catch (BaseException &)
   {
      mCoordSysComboBox->SetValue(mFromCoordSysStr.c_str());
      stateTypeComboBox->SetValue(mFromStateTypeStr.c_str());
      mOutCoord = prevCoord;
      BuildValidStateTypes();
      return;
   }

   wxArrayString keplerianElements;
   keplerianElements = orbitDlg.GetElementsString();

   for(int i=0; i<6; i++)
      textCtrl[i]->SetValue(keplerianElements[i]);

   Rvector6 kepState = orbitDlg.GetElementsDouble();
   Real mu      = GetOriginData(theSpacecraft->GetOrigin(), "mu");
   Real flat    = GetOriginData(theSpacecraft->GetOrigin(), "flattening");
   Real radius  = GetOriginData(theSpacecraft->GetOrigin(), "radius");
   mCartState   = StateConversionUtil::Convert(kepState, "Keplerian",
                  "Cartesian", mu, flat, radius, "TA");
   mOutState    = mCartState;

   dataChanged = true;
   theScPanel->EnableUpdate(true);

   if (orbitDlg.isEpochChanged)
   {
     mIsEpochChanged = true;
     mIsEpochFormatChanged = true;

      std::string toEpochFormat = orbitDlg.GetEpochFormat();
     epochFormatComboBox->SetValue(toEpochFormat.c_str());

      try
      {
         Real fromMjd = -999.999;
         Real outMjd;
         std::string outStr;

         // if modified by user, check if epoch is valid first
         if (mIsEpochModified)
         {
          mEpochStr = orbitDlg.GetEpoch();

            // Save to TAIModJulian string to avoid keep reading the field
            // and convert to proper format when ComboBox is changed.
            if (mFromEpochFormat == "TAIModJulian")
            {
               mTaiMjdStr = mEpochStr;
            }
            else
            {
               TimeConverterUtil::Convert(mFromEpochFormat, fromMjd, mEpochStr,
                                          "TAIModJulian", outMjd, outStr);
               mTaiMjdStr = outStr;
            }

            // Convert to desired format with new date
            TimeConverterUtil::Convert(mFromEpochFormat, fromMjd, mEpochStr,
                                       toEpochFormat, outMjd, outStr);

            epochValue->SetValue(outStr.c_str());
            mIsEpochModified = false;
            mFromEpochFormat = toEpochFormat;
         }
         else
         {
            // Convert to desired format using TAIModJulian date
            TimeConverterUtil::Convert("TAIModJulian", fromMjd, mTaiMjdStr,
                                       toEpochFormat, outMjd, outStr);

            epochValue->SetValue(outStr.c_str());
            mFromEpochFormat = toEpochFormat;
         }
      }
      catch (BaseException &e)
      {
         epochFormatComboBox->SetValue(mFromEpochFormat.c_str());
         theSpacecraft->SetDateFormat(mFromEpochFormat);
         MessageInterface::PopupMessage
            (Gmat::ERROR_, e.GetFullMessage() +
             "\nPlease enter valid Epoch before changing the Epoch Format\n");
      }

     epochValue->SetValue(orbitDlg.GetEpoch().c_str());
   }

}



//------------------------------------------------------------------------------
// void OrbitPanel::SetLabelsUnits(const std::string &stateType)
//------------------------------------------------------------------------------
/**
 * Sets the labels and units for the state.
 *
 * @param <stateType> input state type for display.
 */
//------------------------------------------------------------------------------
void OrbitPanel::SetLabelsUnits(const std::string &stateType)
{
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Orbit"));

   Integer baseLabel = theSpacecraft->GetParameterID("Element1");
   Integer baseUnit  = theSpacecraft->GetParameterID("Element1Units");
   
   std::string st = theSpacecraft->GetStringParameter("DisplayStateType");
   theSpacecraft->UpdateElementLabels(stateType);
   
   description1->SetLabel(theSpacecraft->GetParameterText(baseLabel).c_str());
   textCtrl[0]->SetToolTip(pConfig->Read(_T(("Elements"+theSpacecraft->GetParameterText(baseLabel)+"Hint").c_str())));
   unit1->SetLabel(theSpacecraft->GetStringParameter(baseUnit).c_str());
   
   description2->SetLabel(theSpacecraft->GetParameterText(baseLabel+1).c_str());
   textCtrl[1]->SetToolTip(pConfig->Read(_T(("Elements"+theSpacecraft->GetParameterText(baseLabel+1)+"Hint").c_str())));
   unit2->SetLabel(theSpacecraft->GetStringParameter(baseUnit+1).c_str());
   
   description3->SetLabel(theSpacecraft->GetParameterText(baseLabel+2).c_str());
   textCtrl[2]->SetToolTip(pConfig->Read(_T(("Elements"+theSpacecraft->GetParameterText(baseLabel+2)+"Hint").c_str())));
   unit3->SetLabel(theSpacecraft->GetStringParameter(baseUnit+2).c_str());
   
   description4->SetLabel(theSpacecraft->GetParameterText(baseLabel+3).c_str());
   textCtrl[3]->SetToolTip(pConfig->Read(_T(("Elements"+theSpacecraft->GetParameterText(baseLabel+3)+"Hint").c_str())));
   unit4->SetLabel(theSpacecraft->GetStringParameter(baseUnit+3).c_str());
   
   description5->SetLabel(theSpacecraft->GetParameterText(baseLabel+4).c_str());
   textCtrl[4]->SetToolTip(pConfig->Read(_T(("Elements"+theSpacecraft->GetParameterText(baseLabel+4)+"Hint").c_str())));
   unit5->SetLabel(theSpacecraft->GetStringParameter(baseUnit+4).c_str());
   
   description6->SetLabel(theSpacecraft->GetParameterText(baseLabel+5).c_str());
   textCtrl[5]->SetToolTip(pConfig->Read(_T(("Elements"+theSpacecraft->GetParameterText(baseLabel+5)+"Hint").c_str())));
   unit6->SetLabel(theSpacecraft->GetStringParameter(baseUnit+5).c_str());
   
   if ( (stateType == mStateTypeNames[StateConversionUtil::KEPLERIAN]) ||
        (stateType == mStateTypeNames[StateConversionUtil::MOD_KEPLERIAN]) )
   {
      wxString label = StateConversionUtil::GetAnomalyShortText(mAnomalyType).c_str();
      description6->SetLabel(label);
//      anomalyStaticText->Show(true); // commented out for now - wcs - 2010.01.27 - per S. Hughes
//      anomalyComboBox->Show(true);
      anomalyComboBox->SetSelection(StateConversionUtil::GetAnomalyType(mAnomalyType));
   }
   else
   {
      anomalyStaticText->Show(false);
      anomalyComboBox->Show(false);
   }
   
   theSpacecraft->UpdateElementLabels(st);
}

//------------------------------------------------------------------------------
// void InitializeCoordinateSystem()
//------------------------------------------------------------------------------
/**
 * Initializes the input coordinate system.
 *
 * @param <cs> coordinate system to initialize
 */
//------------------------------------------------------------------------------
void OrbitPanel::InitializeCoordinateSystem(CoordinateSystem *cs)
{
   cs->SetSolarSystem(theSolarSystem); //Assume you have the SolarSystem pointer
   
   SpacePoint *sp;
   std::string spName;

   // Set the Origin for the coordinate system
   spName = cs->GetStringParameter("Origin");
   sp = (SpacePoint*) theGuiInterpreter->GetConfiguredObject(spName);
   
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
                              spName + "\" used for the coordinate system " +
                              cs->GetName() + " origin");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, spName);

   // Set the J2000Body for the coordinate system
   spName = cs->GetStringParameter("J2000Body");
   sp = (SpacePoint*) theGuiInterpreter->GetConfiguredObject(spName);
   
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         spName + "\" used for the coordinate system " +
         cs->GetName() + " J2000 body");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, spName);
   
   cs->Initialize();
}


//------------------------------------------------------------------------------
// void DisplayState()
//------------------------------------------------------------------------------
/**
 * Displays the state in the selected state representation and coordinate system.
 */
//------------------------------------------------------------------------------
void OrbitPanel::DisplayState()
{
   std::string coordSysStr  = mCoordSysComboBox->GetValue().c_str();
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   
   #ifdef DEBUG_ORBIT_PANEL
   MessageInterface::ShowMessage
      ("OrbitPanel::DisplayState() coordSysStr = '%s', stateTypeStr='%s', "
       "mEpoch=%.11lf\n", coordSysStr.c_str(), stateTypeStr.c_str(), mEpoch);
   #endif
   
   Rvector6 midState;
   bool isInternal = false;
   
   if ((IsStateModified()) || mIsEpochChanged)
   {      
      // User has typed in new state data
      for (int i=0; i<6; i++)
      {
         if (mIsStateModified[i])
            midState[i] = atof(textCtrl[i]->GetValue());
         else
            midState[i] = mOutState[i];
      }
      
      #ifdef DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage
         ("   state value modified\n   [%s]\n", midState.ToString(16).c_str());
      #endif
      
      // compute current anomaly if Keplerian or ModKeplerian here
      // BuildState() does not compute mAnomaly
      if ( ((stateTypeStr == mStateTypeNames[StateConversionUtil::KEPLERIAN]) ||
            (stateTypeStr == mStateTypeNames[StateConversionUtil::MOD_KEPLERIAN])) &&
           (mIsStateModified[0] || mIsStateModified[1] || mIsStateModified[5]) )
      {
         #ifdef DEBUG_ORBIT_PANEL_STATE_CHANGE
            MessageInterface::ShowMessage(
                  "In DisplayState, stateType = %s, elements [0,1,5] modified = %s  %s  %s\n",
                  stateTypeStr.c_str(), (mIsStateModified[0]? "true" : "false"),
                  (mIsStateModified[1]? "true" : "false"), (mIsStateModified[5]? "true" : "false"));
            MessageInterface::ShowMessage("About to create new Anomaly with anomaly type = %s\n",
                  mFromAnomalyTypeStr.c_str());
         #endif
         mAnomalyType = mFromAnomalyTypeStr;  // is this right??
         // ****** NOTE: For now, since only "TA" is allowed, GetValue will return "".  Reset to "TA" ******
         mAnomalyType = mAnomalyTypeNames[StateConversionUtil::TA];

         #ifdef DEBUG_ORBIT_PANEL_STATE_CHANGE
         MessageInterface::ShowMessage("In DisplayState, about to print out anomaly type ...\n");
            MessageInterface::ShowMessage(
                  "In DisplayState, after operator = , mAnomalyType = %s\n",
                  (mAnomalyType.c_str()));
         #endif
      }
   }
   else
   {
      // Load midState with Cartesian spacecraft state in internal coordinates
      midState = mCartState;
      isInternal = true;
      
      #ifdef DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage
         ("   using interval state\n   [%s]\n", midState.ToString(16).c_str());
      #endif
   }
   
   //-----------------------------------------------------------
   // check and save epoch
   //-----------------------------------------------------------
   if (mIsEpochChanged)
   {
      std::string newEpoch = epochValue->GetValue().c_str();
      std::string epochFormat = epochFormatComboBox->GetValue().c_str();
      Real fromMjd = -999.999;
      Real a1mjd = -999.999;
      std::string outStr;

      try
      {
         TimeConverterUtil::Convert(epochFormat, fromMjd, newEpoch,
                                    "A1ModJulian", a1mjd, outStr);

         theSpacecraft->SetEpoch(epochFormat, newEpoch, a1mjd);
         mEpochStr = outStr;
         mEpoch = a1mjd;
         mIsEpochChanged = false;
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         canClose = false;
      }
   }

   try
   {
      BuildState(midState, isInternal);

      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage
         ("   DisplayState()--- after conversion, mOutState=\n   [%s]\n",
          mOutState.ToString(16).c_str());
      #endif
   
      for (int i=0; i<6; i++)
         textCtrl[i]->SetValue(ToString(mOutState[i]));

      // labels for elements, anomaly and units
      SetLabelsUnits(stateTypeStr);
      ResetStateFlags(true);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      throw;
   }

   #ifdef DEBUG_ORBIT_PANEL_CONVERT
   MessageInterface::ShowMessage
      ("OrbitPanel::DisplayState() leaving\n");
   #endif
   
}


//------------------------------------------------------------------------------
// void BuildValidStateTypes()
//------------------------------------------------------------------------------
/**
 * Builds a valid list of state types to be displayed in the combo box, based
 * on the current coordinate system selection.  Not all state representations
 * are valid when the origin of the coordinate system is a celestial body.
 */
//------------------------------------------------------------------------------
void OrbitPanel::BuildValidStateTypes()
{
   bool rebuild = false;
   if (mStateTypeNames.empty())
       rebuild = true;
   
   // get the origin for the output coordinate system
   std::string originName = mOutCoord->GetStringParameter("Origin");
   SpacePoint *origin = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(originName);
   
   #ifdef DEBUG_ORBIT_PANEL_STATE_TYPE
   MessageInterface::ShowMessage
      ("   BuildValidStateTypes() origin=%s, addr=%d, mShowFullStateTypeList=%d\n",
       originName.c_str(), origin, mShowFullStateTypeList);
   #endif
   
   if (origin->IsOfType(Gmat::CELESTIAL_BODY) && !mShowFullStateTypeList)
      rebuild = true;
   else if (!origin->IsOfType(Gmat::CELESTIAL_BODY) && mShowFullStateTypeList)
      rebuild = true;
   
   if (!rebuild)
      return;
   
   #ifdef DEBUG_ORBIT_PANEL_STATE_TYPE
   MessageInterface::ShowMessage("   Building new state type list...\n");
   #endif
   
   Integer typeCount = 0;
   mStateTypeNames.clear();
   stateTypeComboBox->Clear();
   
   // get state type list from the base code (StateConverter)
   const std::string *stateTypeList = StateConversionUtil::GetStateTypeList();
   typeCount = StateConversionUtil::GetTypeCount();
   for (int i = 0; i<typeCount; i++)
      mStateTypeNames.push_back(stateTypeList[i]);
   
   // fill state type combobox
   if (origin->IsOfType(Gmat::CELESTIAL_BODY))
   {         
      for (int i = 0; i<typeCount; i++)
         stateTypeComboBox->Append(wxString(stateTypeList[i].c_str()));
      
      mShowFullStateTypeList = true;
      stateTypeComboBox->SetValue(wxT(mFromStateTypeStr.c_str()));
   }
   else
   {
      // if origin is other calculated points (LibrationPoint, Barycenter)
      // there is no mu associated with it, so we don't want to
      // show Keplerian or ModKeplerian types
      for (Integer ii = 0; ii < typeCount; ii++)
      {
         if (!(StateConversionUtil::RequiresCelestialBodyOrigin(stateTypeList[ii])))
            stateTypeComboBox->Append(wxString(stateTypeList[ii].c_str()));
      }
      
      mShowFullStateTypeList = false;
      
      if ( (mFromStateTypeStr == mStateTypeNames[StateConversionUtil::KEPLERIAN]) ||
           (mFromStateTypeStr == mStateTypeNames[StateConversionUtil::MOD_KEPLERIAN]))
      {
         // default to Cartesian
         stateTypeComboBox->
            SetValue(wxT(mStateTypeNames[StateConversionUtil::CARTESIAN]).c_str());
      }
      else
      {
         stateTypeComboBox->SetValue(wxT(mFromStateTypeStr.c_str()));
      }
   }
   
   #ifdef DEBUG_ORBIT_PANEL_STATE_TYPE
   MessageInterface::ShowMessage
      ("   BuildValidStateTypes() Setting state type to %s\n",
       mFromStateTypeStr.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void BuildValidCoordinateSystemList(const std::string &forStateType)
//------------------------------------------------------------------------------
/**
 * Builds a valid list of coordinate systems to be displayed in the combo box, based
 * on the input state type selection.  When the input state type requires a
 * coordinate system with a celestial body origin, only those available coordinate
 * systems with a celestial body origin are displayed; otherwise, all are displayed.
 *
 * @param <forStateType> state type to check to determine valid coordinate systems.
 */
//------------------------------------------------------------------------------
void OrbitPanel::BuildValidCoordinateSystemList(const std::string &forStateType)
{
   bool reqCBOnly = StateConversionUtil::RequiresCelestialBodyOrigin(forStateType);
   bool reqFixedCSOnly = StateConversionUtil::RequiresFixedCoordinateSystem(forStateType);
   #ifdef DEBUG_ORBIT_PANEL_REFRESH
      MessageInterface::ShowMessage("Entering BuildValidCoordinateSystemList with stateType = %s\n",
            forStateType.c_str());
      MessageInterface::ShowMessage("   reqCBOnly      = %s\n   reqFixedCsOnly = %s\n",
                                    (reqCBOnly? "true" : "false"), (reqFixedCSOnly? "true" : "false"));
   #endif
      
   // get the names of the coordinate systems (the list may have been updated)
   coordSystemNames.clear();
   wxArrayString csNames = theGuiManager->GetCoordSysList();
   for (Integer ii = 0; ii < (Integer) csNames.GetCount(); ii++)
   {
      #ifdef DEBUG_ORBIT_PANEL_REFRESH
         MessageInterface::ShowMessage("   adding %s to list of coordSystemNames\n", (csNames.Item(ii)).c_str());
      #endif
      coordSystemNames.push_back((csNames.Item(ii)).c_str());
   }

   CoordinateSystem *tmpCS = NULL;
   SpacePoint       *origin = NULL;
   std::string      originName;
   std::string      currentCS = mCoordSysComboBox->GetValue().c_str();
   std::string      newCS     = currentCS;
   mCoordSysComboBox->Clear();

   #ifdef DEBUG_ORBIT_PANEL_REFRESH
   MessageInterface::ShowMessage("   currentCS = '%s'\n", currentCS.c_str());
   #endif
   
   if (reqFixedCSOnly)
   {
      // If current selection is BodyFixed CS, show it; otherwise show the
      // first BodyFixed CS from the list
      mCoordSysComboBox->Append(theGuiManager->GetCoordSystemWithAxesOf("BodyFixedAxes"));
      if (mCoordSysComboBox->FindString(currentCS.c_str(), true) == wxNOT_FOUND)
         newCS = mCoordSysComboBox->GetString(0);
      else
         newCS = currentCS;
      mCoordSysComboBox->SetValue(newCS.c_str());
   }
   else if (reqCBOnly)
   {
      for (Integer ii = 0; ii < (Integer) coordSystemNames.size(); ii++)
      {
         if (ii == 0)                                   newCS = coordSystemNames.at(ii);
         else if (currentCS == coordSystemNames.at(ii)) newCS = currentCS;
         tmpCS      = (CoordinateSystem*) theGuiInterpreter->GetConfiguredObject(coordSystemNames.at(ii));
         originName = tmpCS->GetStringParameter("Origin");
         origin     = (SpacePoint*) theGuiInterpreter->GetConfiguredObject(originName);
         if ((origin->IsOfType("CelestialBody")) && !(tmpCS->UsesSpacecraft(theSpacecraft->GetName()))) // add it to the list
            mCoordSysComboBox->Append(wxString(coordSystemNames[ii].c_str()));
      }
      mCoordSysComboBox->SetValue(newCS.c_str());
   }
   else
   {
      for (Integer ii = 0; ii < (Integer) coordSystemNames.size(); ii++)
      {
         tmpCS      = (CoordinateSystem*) theGuiInterpreter->GetConfiguredObject(coordSystemNames.at(ii));
         if (!tmpCS->UsesSpacecraft(theSpacecraft->GetName()))
            mCoordSysComboBox->Append(wxString(coordSystemNames[ii].c_str()));
      }
      mCoordSysComboBox->SetValue(currentCS.c_str());
   }

   #ifdef DEBUG_ORBIT_PANEL_REFRESH
      MessageInterface::ShowMessage("Leaving BuildValidCoordinateSystemList with stateType = %s\n",
            forStateType.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void BuildState(const Rvector6 &inputState, bool isInternal = false)
//------------------------------------------------------------------------------
/**
 * This method takes the input state and converts it to match the state settings
 * on the GUI panel.
 * 
 * @param <inputState>  The state that gets converted.
 * @param <isInternal>  true if the input state is a Cartesian state in internal 
 *                      coordinates. 
 */
//------------------------------------------------------------------------------ 
void OrbitPanel::BuildState(const Rvector6 &inputState, bool isInternal)
{
   #ifdef DEBUG_ORBIT_PANEL_CONVERT
      Rvector6 tempState = inputState;
      MessageInterface::ShowMessage(
         "   BuildState() --- The input state (%s %s coordinates)"
         " is\n   [%s]\n",
         (isInternal ? "Internal" : mFromCoord->GetName().c_str()),
         (isInternal ? "Cartesian" : mFromStateTypeStr.c_str()),
         tempState.ToString(16).c_str());
      MessageInterface::ShowMessage(" ... and mAnomaly type is %s\n",
            (mAnomalyType.c_str()));
   #endif
      
   Rvector6 midState;
   
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   
   #ifdef DEBUG_ORBIT_PANEL_CONVERT
   MessageInterface::ShowMessage
      ("   stateTypeStr=%s, mAnomalyType=%s\n", stateTypeStr.c_str(),
       mAnomalyType.c_str());
   #endif
   
   try
   {
      if (isInternal)
      {
         #ifdef DEBUG_ORBIT_PANEL_CONVERT
         MessageInterface::ShowMessage
            ("   isInternal is true, setting mCartState to the input state.\n");
         #endif
         // Input state is Cartesian expressed in internal coordinates
         mCartState = inputState;
      }
      else
      {
         #ifdef DEBUG_ORBIT_PANEL_CONVERT
//         MessageInterface::ShowMessage
//            ("   mAnomaly = [%s]\n", mAnomaly.ToString(16).c_str());
         MessageInterface::ShowMessage("before Coordinate Conversion, mEpoch = %12.10f\n", mEpoch);
         MessageInterface::ShowMessage("  mFromCoord = %s, mInternalCoord = %s\n",
               mFromCoord->GetName().c_str(), mInternalCoord->GetName().c_str());
         MessageInterface::ShowMessage("  and mAnomalyType = %s\n", mAnomalyType.c_str());
         #endif
         
         // Convert input state to the Cartesian representation
         Real mu      = GetOriginData(mFromCoord, "mu");
         Real flat    = GetOriginData(mFromCoord, "flattening");
         Real radius  = GetOriginData(mFromCoord, "radius");
         midState     = StateConversionUtil::Convert(inputState,
                        mFromStateTypeStr,"Cartesian", mu, flat, radius, mAnomalyType);
         
         // Transform to internal coordinates
         mCoordConverter.Convert(A1Mjd(mEpoch), midState, mFromCoord, mCartState, 
                                 mInternalCoord);
      }
      
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage(
          "   BuildState() --- The internal CS representation is\n   [%s]\n",
          mCartState.ToString(16).c_str());
      #endif
      
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage("before Coordinate Conversion, mEpoch = %12.10f\n", mEpoch);
      MessageInterface::ShowMessage("  mInternalCoord = %s, mOutCoord = %s\n",
            mInternalCoord->GetName().c_str(), mOutCoord->GetName().c_str());
      #endif
      // Transform to the desired coordinate system
      mCoordConverter.Convert(A1Mjd(mEpoch), mCartState, mInternalCoord, midState, 
                              mOutCoord);
      
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage
         ("   after mCoordConverter.Convert() midState=\n   [%s]\n",
          midState.ToString().c_str());
      #endif
      
      // and convert to the desired representation
      Real mu      = GetOriginData(mOutCoord, "mu");
      Real flat    = GetOriginData(mOutCoord, "flattening");
      Real radius  = GetOriginData(mOutCoord, "radius");
      mOutState    = StateConversionUtil::Convert(midState, "Cartesian",
                     stateTypeStr, mu, flat, radius, mAnomalyType);
      
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage
         ("   after FromCartesian() mOutState=\n   [%s]\n", mOutState.ToString().c_str());
      MessageInterface::ShowMessage("   OrbitPanel::BuildState() leaving\n");
      #endif      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("**** ERROR in BuildState()\n");
      // wcs - 2012.02.07 commented out to avoid seeing it Popup twice
//      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      throw;
   }
   
}


//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
/**
 * Converts a real number to a wxString.
 *
 * @param <rval> real number to convert
 *
 * @return wxString representation of the input real number
 */
//------------------------------------------------------------------------------
wxString OrbitPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


//------------------------------------------------------------------------------
// bool IsStateModified()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the state has been modified.
 *
 * @return flag indicating whether or not state has been modified
 */
//------------------------------------------------------------------------------
bool OrbitPanel::IsStateModified()
{
   for (int i=0; i<6; i++)
      if (mIsStateModified[i])
         return true;

   return false;
}


//------------------------------------------------------------------------------
// void ResetStateFlags(bool discardEdits = false)
//------------------------------------------------------------------------------
/**
 * Resets the state modified flags.  If discardEdits is true, it will also cause
 * the corresponding text fields to discard any edits the user may have made.
 *
 * @param <discardEdits>  flag indicating whether or not to discard the user's
 *                        text edits, if any
 */
//------------------------------------------------------------------------------
void OrbitPanel::ResetStateFlags(bool discardEdits)
{
   for (int i=0; i<6; i++)
      mIsStateModified[i] = false;

   if (discardEdits)
      for (int i=0; i<6; i++)
         textCtrl[i]->DiscardEdits();
}


//------------------------------------------------------------------------------
// bool CheckState(Rvector6 &state)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed state for validity.
 *
 * @param <state> output state
 *
 * @return true if state is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckState(Rvector6 &state)
{
   #ifdef DEBUG_ORBIT_PANEL_CHECKSTATE
      MessageInterface::ShowMessage(
            "OrbitPanel::CheckState, state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("   current state type = %s\n", mFromStateTypeStr.c_str());
   #endif
   for (int i=0; i<6; i++)
      mElements[i] = textCtrl[i]->GetValue();
   
   wxString stateTypeStr  = stateTypeComboBox->GetValue().c_str();
   bool retval = false;
   
   if (mFromStateTypeStr == "Cartesian")
      retval = CheckCartesian(state);
   else if (mFromStateTypeStr == "Keplerian")
      retval = CheckKeplerian(state);
   else if (mFromStateTypeStr == "ModifiedKeplerian")
      retval = CheckModKeplerian(state);
   else if (mFromStateTypeStr == "SphericalAZFPA" ||
            mFromStateTypeStr == "SphericalRADEC")
      retval = CheckSpherical(state, stateTypeStr);
   else if (mFromStateTypeStr == "Equinoctial")
      retval = CheckEquinoctial(state);
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, + "*** Internal Error ***\nUnknown State Type: " +
          stateTypeStr);
   }
   
   if (!retval)
      return retval;
   
   state = mOutState;
   
   // Copy only modified fields
   for (int i=0; i<6; i++)
   {
      if (mIsStateModified[i])
         state[i] = atof(mElements[i].c_str());
   }
   #ifdef DEBUG_ORBIT_PANEL_CHECKSTATE
      MessageInterface::ShowMessage(
            "OrbitPanel::CheckState, LEAVING, state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("   and current state type = %s\n", mFromStateTypeStr.c_str());
   #endif
   
   return retval;
   
}


//------------------------------------------------------------------------------
// bool CheckCartesian(Rvector6 &state)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed Cartesian state for validity.
 *
 * @param <state> output state
 *
 * @return true if state is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckCartesian(Rvector6 &state)
{
   bool retval = true;
   std::string cartesianNames[6];
   cartesianNames[0] = "X";
   cartesianNames[1] = "Y";
   cartesianNames[2] = "Z";
   cartesianNames[3] = "VX";
   cartesianNames[4] = "VY";
   cartesianNames[5] = "VZ";

   for (unsigned int ii = 0; ii < 6; ii++)
   {
      if (!theScPanel->CheckReal(state[ii], mElements[ii], cartesianNames[ii], "Real Number"))
         retval = false;
      else
      {
         try
         {
            StateConversionUtil::ValidateValue(cartesianNames[ii], state[ii], errMsgFormat, gg->GetDataPrecision());
         }
         catch (BaseException &ue)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, ue.GetFullMessage() + "\n");
            retval   = false;
            canClose = false;
         }
      }
   }

   
   
   #ifdef DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckCartesian() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckKeplerian(Rvector6 &state)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed Keplerian state for validity.
 *
 * @param <state> output state
 *
 * @return true if state is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckKeplerian(Rvector6 &state)
{
   mAnomalyType = anomalyComboBox->GetValue().c_str();
   // ****** NOTE: For now, since only "TA" is allowed, GetValue will return "".  Reset to "TA" ******
   mAnomalyType = mAnomalyTypeNames[StateConversionUtil::TA];

   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "SMA", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("SMA", state[0], errMsgFormat, gg->GetDataPrecision(), "ECC", state[1]);
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[1], mElements[1], "ECC", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("ECC", state[1], errMsgFormat, gg->GetDataPrecision(), "SMA", state[0]);
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   
   if (theScPanel->CheckReal(state[2], mElements[2], "INC", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("INC", state[2], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[3], mElements[3], "RAAN", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("RAAN", state[3], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[4], mElements[4], "AOP", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("AOP", state[4], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   // check Anomaly
   if (theScPanel->CheckReal(state[5], mElements[5], mAnomalyType, "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("TA", state[5], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
      mAnomaly = state[5];
   }
   else
   {
      retval = false;
   }
   
   #ifdef DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckKeplerian() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckModKeplerian(Rvector6 &state)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed Modified Keplerian state for validity.
 *
 * @param <state> output state
 *
 * @return true if state is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckModKeplerian(Rvector6 &state)
{
   mAnomalyType = anomalyComboBox->GetValue().c_str();
   // ****** NOTE: For now, since only "TA" is allowed, GetValue will return "".  Reset to "TA" ******
   mAnomalyType = mAnomalyTypeNames[StateConversionUtil::TA];

   bool retval = true;
   
   #ifdef DEBUG_ORBIT_PANEL_CHECKSTATE
      MessageInterface::ShowMessage(
            "OrbitPanel::CheckModKeplerian, state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("   current anomaly type = %s\n", mAnomalyType.c_str());
   #endif

   if (theScPanel->CheckReal(state[0], mElements[0], "RadPer", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("RadPer", state[0], errMsgFormat, gg->GetDataPrecision(), "RadApo", state[1]);
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[1], mElements[1], "RadApo", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("RadApo", state[1], errMsgFormat, gg->GetDataPrecision(), "RadPer", state[0]);
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[2], mElements[2], "INC", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("INC", state[2], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[3], mElements[3], "RAAN", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("RAAN", state[3], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[4], mElements[4], "AOP", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("AOP", state[4], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   // check Anomaly
   if (theScPanel->CheckReal(state[5], mElements[5], mAnomalyType, "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("TA", state[5], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
      #ifdef DEBUG_ORBIT_PANEL_CHECKSTATE
         MessageInterface::ShowMessage(
               "OrbitPanel::CheckModKeplerian, about to set anomaly with %12.10f  %12.10f  %12.10f  %s\n",
               state[0], state[1], state[5], mAnomalyType.c_str());
      #endif
         mAnomaly = state[5];
   }
   else
   {
      retval = false;
   }
   
   #ifdef DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckModKeplerian() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckSpherical(Rvector6 &state, const wxString &stateType)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed Spherical state for validity.
 *
 * @param <state> output state
 *
 * @return true if state is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckSpherical(Rvector6 &state, const wxString &stateType)
{
   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "RMAG", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("RMAG", state[0], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[1], mElements[1], "RA", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("RA", state[1], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[2], mElements[2], "DEC", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("DEC", state[2], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[3], mElements[3], "VMAG", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("VMAG", state[3], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }

   std::string label5, label6;
   
   if (strcmp(stateType.c_str(), "SphericalAZFPA") == 0)
   {
      label5 = "AZI";
      label6 = "FPA";
   }
   else if (strcmp(stateType.c_str(), "SphericalRADEC") == 0)
   {
      label5 = "RAV";
      label6 = "DECV";
   }
   
   if (theScPanel->CheckReal(state[4], mElements[4], label5, "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue(label5, state[4], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[5], mElements[5], label6, "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue(label6, state[5], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   #ifdef DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckSpherical() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckEquinoctial(Rvector6 &state)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed Equinoctial state for validity.
 *
 * @param <state> output state
 *
 * @return true if state is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckEquinoctial(Rvector6 &state)
{
   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "SMA", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("SMA", state[0], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[1], mElements[1], "EquinoctialH", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("EquinoctialH", state[1], errMsgFormat, gg->GetDataPrecision(), "EquinoctialK",state[2]);
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
         retval =  false;
   }
   
   if (theScPanel->CheckReal(state[2], mElements[2], "EquinoctialK", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("EquinoctialK", state[2], errMsgFormat, gg->GetDataPrecision(), "EquinoctialH",state[1]);
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[3], mElements[3], "EquinoctialP", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("EquinoctialP", state[3], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[4], mElements[4], "EquinoctialQ", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("EquinoctialQ", state[4], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[5], mElements[5], "Mean Longitude", "Real Number"))
   {
      try
      {
         StateConversionUtil::ValidateValue("EquinoctialQ", state[5], errMsgFormat, gg->GetDataPrecision());
      }
      catch (BaseException &ue)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, ue.GetFullMessage() + "\n");
         retval   = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   #ifdef DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckEquinoctial() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckAnomaly(Rvector6 &state)
//------------------------------------------------------------------------------
/**
 * Checks the currently displayed anomaly for validity.
 *
 * @param <state> output state
 *
 * @return true if anomaly is valid; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::CheckAnomaly(Rvector6 &state)
{
   bool validSma = true;
   bool validEcc = true;
   bool validVal = true;
   Real sma, ecc, value;
   std::string str;
   
   sma = mOutState[0];
   if (mIsStateModified[0])
   {
      str = textCtrl[0]->GetValue().c_str();
      validSma = theScPanel->CheckReal(sma, str, "SMA", "Real Number");
   }
   
   ecc = mOutState[1];
   if (mIsStateModified[1])
   {
      str = textCtrl[1]->GetValue().c_str();
      validEcc = theScPanel->CheckReal(ecc, str, "ECC", "Real Number");
   }
   
   value = mOutState[5];
   if (mIsStateModified[5])
   {
      str = textCtrl[5]->GetValue().c_str();
      validVal = theScPanel->CheckReal(value, str, mFromAnomalyTypeStr,
                                       "Real Number");
   }
   
   return validSma && validEcc && validVal;
}


//------------------------------------------------------------------------------
// bool ComputeTrueAnomaly(const std::string &stateTypeStr)
//------------------------------------------------------------------------------
/*
 * Computes true anomaly (mTrueAnomaly) from the current anomaly (mAnomaly).
 *
 * @param <stateTypeStr> the state type
 *
 * @return true if true anomaly successfully computed; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitPanel::ComputeTrueAnomaly(Rvector6 &state, const std::string &stateTypeStr)
{
   #ifdef DEBUG_ORBIT_PANEL_CONVERT
   MessageInterface::ShowMessage
      ("   OrbitPanel::ComputeTrueAnomaly() stateTypeStr=%s\n", stateTypeStr.c_str());
   #endif

   if (mAnomalyType == mAnomalyTypeNames[StateConversionUtil::TA])
      return true;

   Rvector6 midState, outState;
   
   if (stateTypeStr == mStateTypeNames[StateConversionUtil::CARTESIAN])
   {
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage("before Coordinate Conversion in ComputeTrueAnomaly, mEpoch = %12.10f\n", mEpoch);
      #endif
      // Transform to the desired coordinate system
      mCoordConverter.Convert(A1Mjd(mEpoch), mCartState, mInternalCoord, midState, 
                              mOutCoord);
      
      // and convert to the desired representation
      Real mu      = GetOriginData(mOutCoord, "mu");
      Real flat    = GetOriginData(mOutCoord, "flattening");
      Real radius  = GetOriginData(mOutCoord, "radius");
      outState     = StateConversionUtil::Convert(midState, "Cartesian",
                     stateTypeStr, mu, flat, radius, "True Anomaly");
   }
   else if ((stateTypeStr == mStateTypeNames[StateConversionUtil::KEPLERIAN]) ||
            (stateTypeStr == mStateTypeNames[StateConversionUtil::MOD_KEPLERIAN]))
   {
      Real sma = mOutState[0];
      Real ecc = mOutState[1];
      Real anomaly = mOutState[5];
      
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage
         ("   before computing TA, SMA=%f, ECC=%f, %s:%f\n", sma, ecc,
          mAnomalyType.c_str(), anomaly);
      #endif
      
      // if state type is MOD_KEPLERIAN, compute sma and ecc
      if (stateTypeStr == mStateTypeNames[StateConversionUtil::MOD_KEPLERIAN])
      {
         Real mu     = GetOriginData(theSpacecraft->GetOrigin(), "mu");
         Real flat   = GetOriginData(theSpacecraft->GetOrigin(), "flattening");
         Real radius = GetOriginData(theSpacecraft->GetOrigin(), "radius");
         Rvector kepl = StateConversionUtil::Convert
                        (state, "Cartesian", mStateTypeNames[StateConversionUtil::KEPLERIAN],
                        mu, flat, radius, mAnomalyType);
         sma = kepl[0];
         ecc = kepl[1];
         anomaly = kepl[5];         
         
         #ifdef DEBUG_ORBIT_PANEL_CONVERT
         MessageInterface::ShowMessage
            ("   after convert to Keplerian, SMA=%f, ECC=%f, %s:%f\n", sma, ecc,
             mAnomalyType.c_str(), anomaly);
         #endif
      }
      
      mAnomaly = anomaly;
      mTrueAnomaly = StateConversionUtil::ConvertToTrueAnomaly(mAnomalyType, anomaly, ecc);
      
      #ifdef DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage
         ("   mAnomaly     = [%12.10f]\n   mTrueAnomaly = [%12.10f]\n",
          mAnomaly, mTrueAnomaly);
      #endif
   }
   
   return true;
}

//------------------------------------------------------------------------------
// Real GetOriginData(GmatBase *fromObject, const std::string &whichData = "mu");
//------------------------------------------------------------------------------
/*
 * Gets the origin mu from the object.
 *
 * @param <fromObject>
 *
 * @return fromObject's mu value
 */
//------------------------------------------------------------------------------
Real OrbitPanel::GetOriginData(GmatBase *fromObject, const std::string &whichData)
{
   Real result = 0.0;

   if (fromObject != NULL)
   {
      if (fromObject->IsOfType("CoordinateSystem"))
      {
         // Get the coordinate system's origin
         SpacePoint *origin = ((CoordinateSystem*) fromObject)->GetOrigin();
         // Check if it is Celestial Body then get the mu;
         // Otherwise, it sets mu to zero
         if (origin->IsOfType("CelestialBody"))
         {
            if (whichData == "mu")
               result = ((CelestialBody*) origin)->GetGravitationalConstant();
            else if (whichData == "flattening")
               result = ((CelestialBody*) origin)->GetFlattening();
            else if (whichData == "radius")
               result = ((CelestialBody*) origin)->GetEquatorialRadius();
         }
         else
            result = 0.0;
      }
      else if (fromObject->IsOfType("CelestialBody"))
      {
         if (whichData == "mu")
            result = ((CelestialBody*) fromObject)->GetGravitationalConstant();
         else if (whichData == "flattening")
            result = ((CelestialBody*) fromObject)->GetFlattening();
         else if (whichData == "radius")
            result = ((CelestialBody*) fromObject)->GetEquatorialRadius();
      }
      else
      {
         result = 0.0;
      }

   }
   return result;
}
