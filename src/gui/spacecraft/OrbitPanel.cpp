//$Header$
//------------------------------------------------------------------------------
//                           OrbitPanel
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
 * which is a Cartesian MJ2000 equatorial state expressed in terns of the 
 * current GMAT internal CoordinateSystem.  When the panel is populated, that 
 * state is converted to the coordinate system and representation desired for
 * display, and then used to fill in the elements on the panel.  The OrbitPanel
 * maintains the state in the internal MJ2000 equatorial Cartesian coordinates,
 * in the mCartState member.  All conversiona and transformations return to this 
 * representation internally.
 */
//------------------------------------------------------------------------------
#include "OrbitPanel.hpp"
#include "MessageInterface.hpp"
#include "Anomaly.hpp"
#include <sstream>

//#define DEBUG_ORBIT_PANEL 1
//#define DEBUG_ORBIT_PANEL_LOAD 1
#define DEBUG_ORBIT_PANEL_CONVERT 1
//#define DEBUG_ORBIT_PANEL_COMBOBOX 1
#define DEBUG_ORBIT_PANEL_SAVE 1
//#define DEBUG_ORBIT_PANEL_CHECK_RANGE 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OrbitPanel, wxPanel)
   EVT_COMBOBOX(ID_COMBOBOX, OrbitPanel::OnComboBoxChange)
   EVT_TEXT(ID_TEXTCTRL, OrbitPanel::OnTextChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// OrbitPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft,
//            SolarSystem *solarsystem
//------------------------------------------------------------------------------
/**
 * Constructs OrbitPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
OrbitPanel::OrbitPanel(GmatPanel *scPanel, wxWindow *parent,
                       Spacecraft *spacecraft, SolarSystem *solarsystem)
   : wxPanel(parent)
{
   // initalize data members
   theScPanel = scPanel;
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   theSpacecraft = spacecraft;
   theSolarSystem = solarsystem;
   
   mIsEpochFormatChanged = false;
   mIsCoordSysChanged = false;
   mIsStateTypeChanged = false;
   mIsAnomalyTypeChanged = false;
   mIsStateChanged = false;
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
OrbitPanel::~OrbitPanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * @note Gets the values from theSpacecraft and puts them in the text fields
 */
//------------------------------------------------------------------------------
void OrbitPanel::LoadData()
{
   #if DEBUG_ORBIT_PANEL_LOAD
      MessageInterface::ShowMessage("In OrbitPanel::LoadData() \n");
   #endif
      
   mAnomaly = theSpacecraft->GetAnomaly();
   mTrueAnomaly = mAnomaly;
   
   mInternalCoord = theGuiInterpreter->GetInternalCoordinateSystem();

   if (mInternalCoord == NULL)
   {
      MessageInterface::ShowMessage("   mInternalCoord is NULL.\n");
   }
   else
   {
      #if DEBUG_ORBIT_PANEL_LOAD
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
      epochFormatComboBox->SetValue(wxT(epochFormat.c_str()));
      mFromEpochFormat = epochFormat;
      
      mEpochStr = theSpacecraft->GetStringParameter("Epoch"); 
      epochValue->SetValue(wxT(mEpochStr.c_str()));
      
      // load the coordinate system
      std::string coordSystemStr =
         theSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      mCoordSysComboBox->SetValue(coordSystemStr.c_str());
      mFromCoordSysStr = coordSystemStr;
      
      mOutCoord = (CoordinateSystem*)theGuiInterpreter->
         GetObject(coordSystemStr);
      
      mFromCoord = mOutCoord;
      
      if (mOutCoord == NULL)
      {
         MessageInterface::ShowMessage
            ("   The Spacecraft CoordinateSystem is NULL.\n");
      }
      else
      {
         #if DEBUG_ORBIT_PANEL_LOAD
            MessageInterface::ShowMessage("   mOutCoord=%s, addr=%d\n",
               mOutCoord->GetName().c_str(), mOutCoord);
         #endif
         
         // Set the CS's on the spacecraft
         theSpacecraft->SetInternalCoordSystem(mInternalCoord);
         theSpacecraft->SetRefObject(mOutCoord, Gmat::COORDINATE_SYSTEM);
         //theSpacecraft->Initialize(); //loj: 9/21/06 commented out
      }
      
      // get the origin for the output coordinate system
      std::string originName = mOutCoord->GetStringParameter("Origin");
      SpacePoint *origin = (SpacePoint*)theGuiInterpreter->GetObject(originName);

      #if DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   origin=%s, addr=%d\n", originName.c_str(), origin);
      #endif
      
      // load the state type - hard coded for now
      std::string stType;
      
      if (origin->IsOfType(Gmat::CELESTIAL_BODY))
      {
         const std::string *stateTypeList = stateConverter.GetStateTypeList();
         Integer typeCount = stateConverter.GetTypeCount();
         
         for (int i = 0; i<typeCount; i++)
            stateTypeComboBox->Append(wxString(stateTypeList[i].c_str()));
         
         stType = theSpacecraft->GetStringParameter("StateType");
      }
      else
      {
         wxString stateTypeList[] =
         {
            wxT("Cartesian"),
            wxT("SphericalAZFPA"),
            wxT("SphericalRADEC")
         };
         
         for (int i = 0; i<3; i++)
            stateTypeComboBox->Append(wxString(stateTypeList[i].c_str()));
         
         stType = "Cartesian";
      }
      
      stateTypeComboBox->SetValue(wxT(stType.c_str()));
      mFromStateTypeStr = stType;
      
      // load the anomaly type - if state type is Keplerian or Modified Keplerian
      std::string anType = theSpacecraft->GetStringParameter("AnomalyType");
      
      #if DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage("\nOrbitPanel::LoadData()...\n"
         "Anomaly type = %s \n", anType.c_str());   
      #endif
         
      // Get anomaly type list
      const std::string *anomalyList = Anomaly::GetLongTypeNameList();
      
      for (unsigned int i = 0; i<4; i++)
         anomalyComboBox->Append(wxString(anomalyList[i].c_str()));
      
      anomalyComboBox->SetValue(wxT(anType.c_str()));
      
      mFromAnomalyTypeStr = anType;
      
      #if DEBUG_ORBIT_PANEL_LOAD
          MessageInterface::ShowMessage("\nOrbitPanel::LoadData()...\n"
             "Anomaly info -> a: %f, e: %f, %s: %f\n", 
             mAnomaly.GetSMA(), mAnomaly.GetECC(), mAnomaly.GetTypeString().c_str(),
             mAnomaly.GetValue());   
      #endif
          
      // Get Spacecraft initial state and display
      mCartState.Set(theSpacecraft->GetState().GetState());
      mTempCartState = mCartState;
      mOutState = mCartState;
      
      for (int i=0; i<6; i++)
         textCtrl[i]->SetValue(ToString(mOutState[i]));
            
      mIsStateChanged = false;
      DisplayState();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OrbitPanel:LoadData() error occurred!\n%s\n", e.GetMessage().c_str());
   }
   
   dataChanged = false;
   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * @note Gets the values from the text fields and puts them in theSpacecraft
 */
//------------------------------------------------------------------------------
void OrbitPanel::SaveData()
{
   #if DEBUG_ORBIT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OrbitPanel::SaveData() entered\n   mCartState=%s\n   "
       "mTempCartState=%s\n   mOutState=%s\n", mCartState.ToString(16).c_str(), 
       mTempCartState.ToString(16).c_str(), mOutState.ToString(16).c_str());
   MessageInterface::ShowMessage("mIsCoordSysChanged=%d\n", mIsCoordSysChanged);
   MessageInterface::ShowMessage("mIsStateTypeChanged=%d\n", mIsStateTypeChanged);
   MessageInterface::ShowMessage("mIsAnomalyTypeChanged=%d\n", mIsAnomalyTypeChanged);
   MessageInterface::ShowMessage("mIsStateChanged=%d\n", mIsStateChanged);
   MessageInterface::ShowMessage("mIsEpochChanged=%d\n", mIsEpochChanged);
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------
   // check and save epoch
   //-----------------------------------------------------------
   if (mIsEpochFormatChanged || mIsEpochChanged)
   {
      std::string newEpoch = epochValue->GetValue().c_str();
      std::string epochFormat = epochFormatComboBox->GetValue().c_str();
      Real fromMjd = -999.999;
      Real taimjd = -999.999;
      std::string outStr;
      
      MessageInterface::ShowMessage
         ("   newEpoch=%s, epochFormat=%s\n", newEpoch.c_str(), epochFormat.c_str());
      
      if (mIsEpochChanged)
      {
         try
         {
            TimeConverterUtil::Convert(epochFormat, fromMjd, newEpoch,
                                       "TAIModJulian", taimjd, outStr);
            
            theSpacecraft->SetEpoch(epochFormat, newEpoch, taimjd);
            mEpochStr = outStr;
            mEpoch = taimjd;
            mIsEpochChanged = false;
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
            canClose = false;
         }
      }
   }
   
   
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   
   try
   {
      //-----------------------------------------------------------
      // check and save orbital elements
      //-----------------------------------------------------------
      if (mIsStateChanged)
      {
         Rvector6 state;
         bool retval = CheckState(state);
         canClose = retval;
         
         if (retval)
         {
            // Build state uses new state type
            BuildState(state);
            theSpacecraft->SetState(stateTypeStr, mCartState);
            theSpacecraft->SetAnomaly(mAnomalyType, mTrueAnomaly);
            
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
         theSpacecraft->SetStringParameter("CoordinateSystem", 
                                           coordSysStr.c_str());
      }
      
      if (canClose)
         dataChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
      canClose = false;
      return;
   }
   
   #if DEBUG_ORBIT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OrbitPanel::SaveData() exiting\n   mCartState=%s\n"
       "   mTempCartState=%s\n   mOutState=%s\n", mCartState.ToString(16).c_str(),
       mTempCartState.ToString(16).c_str(), mOutState.ToString(16).c_str());
   #endif
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 *
 * @note Creates the page for orbit information
 */
//------------------------------------------------------------------------------
void OrbitPanel::Create()
{
   #if DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage("In OrbitPanel::Create() \n");
   #endif

   Integer bsize = 2; // border size

   wxString emptyList[] = {};

   //-----------------------------------------------------------------
   //  create sizers 
   //-----------------------------------------------------------------
   // sizer for orbit tab
   wxBoxSizer *orbitSizer = new wxBoxSizer(wxHORIZONTAL);
      
   //flex grid sizer for the epoch format, coordinate system and state type
   wxFlexGridSizer *pageSizer = new wxFlexGridSizer(5, 2, bsize, bsize);
   
   // static box for the elements
   wxStaticBox *elementBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Elements"));
   wxStaticBoxSizer *elementSizer =
      new wxStaticBoxSizer(elementBox, wxVERTICAL);

   //-----------------------------------------------------------------
   // epoch 
   //-----------------------------------------------------------------
   // label for epoch format
   wxStaticText *epochFormatStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Epoch Format"), wxDefaultPosition, wxDefaultSize, 0 );
   
   // combo box for the epoch format
   epochFormatComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, emptyList, wxCB_DROPDOWN | wxCB_READONLY );

   // label for epoch
   wxStaticText *epochStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
   
   // textfield for the epoch value
   epochValue = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0 );

   //-----------------------------------------------------------------
   //  coordinate system 
   //-----------------------------------------------------------------
   // label for coordinate system
   wxStaticText *coordSysStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Coordinate System"), wxDefaultPosition, wxDefaultSize, 0 );

   //Get CordinateSystem ComboBox from the GuiItemManager.
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(150,-1));

   //-----------------------------------------------------------------
   //  state type 
   //-----------------------------------------------------------------
   // label for state type
   wxStaticText *stateTypeStaticText = new wxStaticText( this, ID_TEXT,
      wxT("State Type"), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the state
   stateTypeComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, emptyList, wxCB_DROPDOWN | wxCB_READONLY);

   //-----------------------------------------------------------------
   //  anomaly 
   //-----------------------------------------------------------------
   // label for anomaly type
   anomalyStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Anomaly Type "), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the anomaly type
   anomalyComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0, emptyList, wxCB_DROPDOWN | wxCB_READONLY );
   
   // add to page sizer
   pageSizer->Add( epochFormatStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( epochFormatComboBox, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( epochStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( epochValue, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( coordSysStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( mCoordSysComboBox, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( stateTypeStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( stateTypeComboBox, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( anomalyStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( anomalyComboBox, 0, wxALIGN_LEFT | wxALL, bsize );

   // panel that has the labels and text fields for the elements
   // adds default descriptors/labels
   AddElements(this);
   elementSizer->Add(elementsPanel, 0, wxALIGN_CENTER, bsize);
   
   orbitSizer->Add( pageSizer, 1, wxGROW|wxALIGN_CENTER, bsize );
   orbitSizer->Add( elementSizer, 1, wxGROW|wxALIGN_CENTER, bsize );
   
   this->SetSizer( orbitSizer );
}

//------------------------------------------------------------------------------
// void AddElements(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the default objects to put in the element static box
 */
//------------------------------------------------------------------------------
void OrbitPanel::AddElements(wxWindow *parent)
{
   Integer bsize = 2; // border size

   elementsPanel = new wxPanel(parent);
   wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

   wxFlexGridSizer *item3 = new wxFlexGridSizer( 6, 3, 0, 0 );
   item3->AddGrowableCol( 0 );
   item3->AddGrowableCol( 1 );
   item3->AddGrowableCol( 2 );

   // Element 1
   description1 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor1     "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl[0] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, 
                     wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
   unit1 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit1"), 
               wxDefaultPosition, wxDefaultSize, 0 );

   // Element 2
   description2 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor2    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl[1] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0 );
   unit2 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit2"), 
               wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 3
   description3 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor3    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl[2] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
   unit3 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit3"), 
                wxDefaultPosition, wxDefaultSize, 0 );
    
   // Element 4
   description4 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor4    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl[3] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
   unit4 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit4"), 
                wxDefaultPosition, wxDefaultSize, 0 );

   // Element 5    
   description5 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor5    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl[4] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                                 wxDefaultPosition, wxSize(150,-1), 0 );
   unit5 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit5"), 
                wxDefaultPosition, wxDefaultSize, 0 );
    
   // Element 6
   description6 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor6    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl[5] = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""),
                                wxDefaultPosition, wxSize(150,-1), 0 );
   unit6 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Unit6"), 
                wxDefaultPosition, wxDefaultSize, 0 );

   // Add to wx*Sizers 
   item3->Add( description1, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[0], 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit1, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description2, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[1], 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit2, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description3, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[2], 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit3, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description4, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[3], 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit4, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description5, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[4], 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( unit5, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description6, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl[5], 0, wxALIGN_CENTER|wxALL, bsize );
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
 * Converts state to ComboBox selection using utility. It does not call
 * Spacecraft object for conversion.
 */
//------------------------------------------------------------------------------ 
void OrbitPanel::OnComboBoxChange(wxCommandEvent& event)
{
   wxString coordSysStr  = mCoordSysComboBox->GetValue();
   wxString stateTypeStr = stateTypeComboBox->GetValue();
   
   //-----------------------------------------------------------------
   // epoch format change
   //-----------------------------------------------------------------
   if (event.GetEventObject() == epochFormatComboBox)
   {
      mIsEpochFormatChanged = true;
      std::string toEpochFormat = epochFormatComboBox->GetValue().c_str();    
      
      #if DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("OrbitPanel::OnComboBoxChange() mFromEpochFormat=%s, "
          "toEpochFormat=%s\n   mEpochStr=%s, mIsEpochModified=%d\n",
          mFromEpochFormat.c_str(), toEpochFormat.c_str(), mEpochStr.c_str(),
          mIsEpochModified);
      #endif
      
      try
      {
         // if modified by user, check if epoch is valid first
         if (mIsEpochModified)
         {
            Real fromMjd = -999.999;
            Real outMjd;
            std::string outStr;
            mEpochStr = epochValue->GetValue().c_str();
            TimeConverterUtil::Convert(mFromEpochFormat, fromMjd, mEpochStr,
                                       toEpochFormat, outMjd, outStr);
            
            epochValue->SetValue(outStr.c_str());
            mIsEpochModified = false;
            mFromEpochFormat = toEpochFormat;
         }
         else
         {
            theSpacecraft->SetDateFormat(toEpochFormat);
            epochValue->SetValue(theSpacecraft->GetStringParameter("Epoch").c_str());
            mFromEpochFormat = toEpochFormat;
         }
      }
      catch (BaseException &e)
      {
         epochFormatComboBox->SetValue(mFromEpochFormat.c_str());
         theSpacecraft->SetDateFormat(mFromEpochFormat);
         MessageInterface::PopupMessage
            (Gmat::ERROR_, e.GetMessage() +
             "\nPlease enter valid Epoch before changing the Epoch Format\n");
      }
   }
   //-----------------------------------------------------------------
   // coordinate system or state type change
   //-----------------------------------------------------------------
   else if (event.GetEventObject() == mCoordSysComboBox ||
            event.GetEventObject() == stateTypeComboBox)
   {      
      #if DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("OrbitPanel::OnComboBoxChange() mFromCoordSysStr=%s, coordSysStr=%s\n"
          "   mFromStateTypeStr=%s, stateTypeStr=%s\n", mFromCoordSysStr.c_str(),
          coordSysStr.c_str(), mFromStateTypeStr.c_str(), stateTypeStr.c_str());
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
                   "Please enter valid value before changing the State Type\n");
               return;
            }
         }
      }
      
      mOutCoord = (CoordinateSystem*)theGuiInterpreter->
         GetObject(mCoordSysComboBox->GetValue().c_str());
      
      DisplayState();
      
      if (event.GetEventObject() == mCoordSysComboBox)
         mFromCoordSysStr = mCoordSysComboBox->GetValue().c_str();
      
      if (event.GetEventObject() == stateTypeComboBox)      
         mFromStateTypeStr = stateTypeComboBox->GetValue().c_str();
      
      mFromCoord = mOutCoord;
      theSpacecraft->SetRefObject(mOutCoord, Gmat::COORDINATE_SYSTEM);
   }
   //-----------------------------------------------------------------
   // anomaly type change 
   //-----------------------------------------------------------------
   else if (event.GetEventObject() == anomalyComboBox)
   {
      mIsAnomalyTypeChanged = true;
      wxString description, stateValue;      
      mAnomalyType = anomalyComboBox->GetValue();
      
      #if DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("OrbitPanel::OnComboBoxChange() mFromAnomalyTypeStr=%s, "
          "mAnomalyType=%s\n", mFromAnomalyTypeStr.c_str(),
          mAnomalyType.c_str());
      #endif
      
      Real value = 12345.67890;
      bool validSma = true;
      bool validEcc = true;
      bool validVal = true;
      
      try
      {
         // Get Eccentricity and Anomaly value if modified
         if (IsStateModified())
         {
            Real sma, ecc;
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
            
            if (validSma && validEcc && validVal)
            {
               #if DEBUG_ORBIT_PANEL_COMBOBOX
               MessageInterface::ShowMessage
                  ("OrbitPanel::OnComboBoxChange() sma=%f, ecc=%f, value=%f\n",
                   sma, ecc, value);
               #endif
               
               Anomaly temp(sma, ecc, value, mFromAnomalyTypeStr);
               temp.Set(sma, ecc, value, mFromAnomalyTypeStr);
               value = temp.GetValue(mAnomalyType);
               
               mAnomaly.Set(sma, ecc, value, mAnomalyType);
               ResetStateFlags();
            }
            else
            {
               canClose = false;
               anomalyComboBox->
                  SetValue(Anomaly::GetLongTypeString(mFromAnomalyTypeStr).c_str());
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, +
                   "Please enter valid value before changing the Anomaly Type\n");
            }
         }
         else // state is not modified
         {
            // just convert to new anomaly type
            value = mAnomaly.GetValue(mAnomalyType);
         }
         
         // Update text field
         if (validSma && validEcc && validVal)
         {
            description.Printf("%s", Anomaly::GetTypeString(mAnomalyType).c_str());
            description6->SetLabel(description);
            textCtrl[5]->SetValue(ToString(value));
            
            mFromAnomalyTypeStr = mAnomalyType;
            
            // Convert anomaly to true anomaly
            if (mAnomaly.GetType() != Anomaly::TA)
               mTrueAnomaly = mAnomaly.ConvertToAnomaly(Anomaly::TA);
         }
      }
      catch (BaseException &e)
      {
         anomalyComboBox->SetValue(mFromAnomalyTypeStr.c_str());
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
         MessageInterface::PopupMessage
            (Gmat::ERROR_, +
             "Please enter valid value before changing the Anomaly Type\n");
      }
   }
   
   dataChanged = true;
   theScPanel->EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
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
         mIsStateChanged = true;
         dataChanged = true;
         theScPanel->EnableUpdate(true);
      }
   }
   else if (obj == epochValue && epochValue->IsModified())
   {
      mIsEpochChanged = true;
      mIsEpochModified = true;
      dataChanged = true;
      theScPanel->EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OrbitPanel::SetLabelsUnits(const std::string &stateType)
//------------------------------------------------------------------------------
/**
 * @param <stateType> input state type for display.
 *
 * @note Sets the labels and units for the state.
 */
//------------------------------------------------------------------------------
void OrbitPanel::SetLabelsUnits(const std::string &stateType)
{
   Integer baseLabel = theSpacecraft->GetParameterID("Element1");
   Integer baseUnit  = theSpacecraft->GetParameterID("Element1Units");
   
   std::string st = theSpacecraft->GetStringParameter("StateType");
   theSpacecraft->SetStringParameter("StateType", stateType);
   
   description1->SetLabel(theSpacecraft->GetParameterText(baseLabel).c_str());
   unit1->SetLabel(theSpacecraft->GetStringParameter(baseUnit).c_str());
   
   description2->SetLabel(theSpacecraft->GetParameterText(baseLabel+1).c_str());
   unit2->SetLabel(theSpacecraft->GetStringParameter(baseUnit+1).c_str());
   
   description3->SetLabel(theSpacecraft->GetParameterText(baseLabel+2).c_str());
   unit3->SetLabel(theSpacecraft->GetStringParameter(baseUnit+2).c_str());
   
   description4->SetLabel(theSpacecraft->GetParameterText(baseLabel+3).c_str());
   unit4->SetLabel(theSpacecraft->GetStringParameter(baseUnit+3).c_str());
   
   description5->SetLabel(theSpacecraft->GetParameterText(baseLabel+4).c_str());
   unit5->SetLabel(theSpacecraft->GetStringParameter(baseUnit+4).c_str());
   
   description6->SetLabel(theSpacecraft->GetParameterText(baseLabel+5).c_str());
   unit6->SetLabel(theSpacecraft->GetStringParameter(baseUnit+5).c_str());
   
   if ( (strcmp(stateType.c_str(), "Keplerian") == 0) || 
        (strcmp(stateType.c_str(), "ModifiedKeplerian") == 0) )
   {
      // Anomaly is no longer saved to spacecraft on combobox change
      //int anomalyID = theSpacecraft->GetParameterID("AnomalyType");
      //wxString description = (theSpacecraft->GetStringParameter(anomalyID)).c_str();
      // set the labels for the anomaly
      
      // Anomaly is no longer saved to spacecraft on combobox change
      description6->SetLabel(mAnomaly.GetTypeString().c_str());
      anomalyStaticText->Show(true);
      anomalyComboBox->Show(true);
      
      anomalyComboBox->SetSelection(mAnomaly.GetType());
   }
   else
   {
      anomalyStaticText->Show(false);
      anomalyComboBox->Show(false);
   }

   theSpacecraft->SetStringParameter("StateType", st);
}

//------------------------------------------------------------------------------
// void InitializeCoordinateSystem()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when epoch combobox is changed
 */
//------------------------------------------------------------------------------
void OrbitPanel::InitializeCoordinateSystem(CoordinateSystem *cs)
{
   cs->SetSolarSystem(theSolarSystem); //Assume you have the SolarSystem pointer
   
   SpacePoint *sp;
   std::string spName;

   // Set the Origin for the coordinate system
   spName = cs->GetStringParameter("Origin");
   sp = (SpacePoint*) theGuiInterpreter->GetObject(spName);
   
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
                              spName + "\" used for the coordinate system " +
                              cs->GetName() + " origin");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, spName);

   // Set the J2000Body for the coordinate system
   spName = cs->GetStringParameter("J2000Body");
   sp = (SpacePoint*) theGuiInterpreter->GetObject(spName);
   
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
void OrbitPanel::DisplayState()
{
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   std::string coordSysStr  = mCoordSysComboBox->GetValue().c_str();
   
   #if DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage(
         "OrbitPanel::DisplayState() coordSysStr = '%s', stateTypeStr='%s', "
         "mEpoch=%.11lf\n", coordSysStr.c_str(), stateTypeStr.c_str(), mEpoch);
   #endif
      
   Rvector6 midState;
   bool isInternal = false;
      
   if (IsStateModified())
   {
      // User has typed in new state data
      for (int i=0; i<6; i++)
         if (mIsStateModified[i])
            midState[i] = atof(textCtrl[i]->GetValue());
   }
   else
   {
      // Load midState with Cartesian spacecraft state in internal coordinates
      midState = mCartState;
      isInternal = true;
   }
   
   BuildState(midState, isInternal);
   
   #if DEBUG_ORBIT_PANEL_CONVERT
   MessageInterface::ShowMessage
      ("OrbitPanel::DisplayState()--- after conversion\n   mOutState= %s\n",
       mOutState.ToString(16).c_str());
   #endif
   
   for (int i=0; i<6; i++)
      textCtrl[i]->SetValue(ToString(mOutState[i]));
   
   // labels for elements, anomaly and units
   SetLabelsUnits(stateTypeStr);
   
   // This flag is only reset in SaveData() (loj: 12/27/06)
   //mIsStateChanged = false;
   ResetStateFlags(true);
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
   #if DEBUG_ORBIT_PANEL_CONVERT
      Rvector6 tempState = inputState;
      MessageInterface::ShowMessage(
         "OrbitPanel::BuildState() --- The input state (%s %s coordinates)"
         " is\n   [%s]\n",
         (isInternal ? "Internal" : mFromCoord->GetName().c_str()),
         (isInternal ? "Cartesian" : mFromStateTypeStr.c_str()),
         tempState.ToString(16).c_str());
   #endif
      
   Rvector6 midState;
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   
   if (stateTypeStr == "Keplerian" || stateTypeStr == "ModKeplerian")
      mAnomalyType = anomalyComboBox->GetValue();
   
   if (isInternal)
   {
      // Input state is Cartesian expressed in internal coordinates
      mCartState = inputState;
   }
   else
   {
      // Convert input state to the Cartesian representation
      stateConverter.SetMu(mFromCoord);
      midState = stateConverter.Convert(inputState, mFromStateTypeStr, 
         "Cartesian", mAnomaly);
      
      // Transform to internal coordinates
      mCoordConverter.Convert(A1Mjd(mEpoch), midState, mFromCoord, mCartState, 
         mInternalCoord);
   }
   
   #if DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage(
         "OrbitPanel::BuildState() --- The internal CS representation is\n   [%s]\n",
         mCartState.ToString(16).c_str());
   #endif
   
   // Transform to the desired coordinate system
   mCoordConverter.Convert(A1Mjd(mEpoch), mCartState, mInternalCoord, midState, 
      mOutCoord);
   
   // and convert to the desired representation
   stateConverter.SetMu(mOutCoord);
   mOutState = stateConverter.FromCartesian(midState, stateTypeStr, mAnomalyType);
   
   if (stateTypeStr == "Keplerian" || stateTypeStr == "ModKeplerian")
   {
      mAnomalyType = anomalyComboBox->GetValue();
      mOutState = stateConverter.FromCartesian(midState, stateTypeStr, mAnomalyType);
      
      mAnomaly.Set(mOutState[0], mOutState[1], mOutState[5], mAnomalyType);         
      mTrueAnomaly = mAnomaly.ConvertToAnomaly(Anomaly::TA);
   }
   else
   {
      mOutState = stateConverter.FromCartesian(midState, stateTypeStr, mAnomalyType);
   }
   
   #if DEBUG_ORBIT_PANEL_CONVERT
   MessageInterface::ShowMessage
      ("mAnomaly=%s\nmTrueAnomaly=%s\n", mAnomaly.ToString(16).c_str(),
       mTrueAnomaly.ToString(16).c_str());
   #endif
}


//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString OrbitPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


//------------------------------------------------------------------------------
// bool IsStateModified()
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
bool OrbitPanel::CheckState(Rvector6 &state)
{
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
   
   return retval;
   
}


//------------------------------------------------------------------------------
// bool CheckCartesian(Rvector6 &state)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckCartesian(Rvector6 &state)
{
   bool retval = true;
   
   if (!theScPanel->CheckReal(state[0], mElements[0], "X", "Real Number"))
      retval = false;
   if (!theScPanel->CheckReal(state[1], mElements[1], "Y", "Real Number"))
      retval = false;
   if (!theScPanel->CheckReal(state[2], mElements[2], "Z", "Real Number"))
      retval = false;
   if (!theScPanel->CheckReal(state[3], mElements[3], "VX", "Real Number"))
      retval = false;
   if (!theScPanel->CheckReal(state[4], mElements[4], "VY", "Real Number"))
      retval = false;
   if (!theScPanel->CheckReal(state[5], mElements[5], "VZ", "Real Number"))
      retval = false;
   
   #if DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckCartesian() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckKeplerian(Rvector6 &state)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckKeplerian(Rvector6 &state)
{
   mAnomalyType = anomalyComboBox->GetValue().c_str();
   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "SMA", "Real Number"))
   {
      if (state[0] == 0.0)
      {
         theScPanel->CheckReal(state[0], mElements[0], "SMA", "Real Number != 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[1], mElements[1], "ECC", "Real Number"))
   {
      if ((state[1] < 0.0) || (state[1] >= 1.0))
      {
         theScPanel->CheckReal(state[1], mElements[0], "ECC", "0.0 < = Real Number < 1.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   // check coupling restictions on SMA and ECC for circular and elliptical orbits
   if((state[0] < 0.0) && (state[1] <= 1.0))
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "(0.0 <= ECC <= 1.0) SMA should only be a positive Real Number", true);
      retval = false;
      canClose = false;
   }
   
   // for hyperbolic orbit
   else if((state[0] > 0.0) && (state[1] > 1.0))
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "(ECC > 1) SMA should only be a negative Real Number", true);
      retval = false;
      canClose = false;
   }
   
   if (!theScPanel->CheckReal(state[2], mElements[2], "INC", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[3], mElements[3], "RAAN", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[4], mElements[4], "AOP", "Real Number"))
      retval = false;
   
   // check Anomaly
   if (theScPanel->CheckReal(state[5], mElements[5], mAnomalyType, "Real Number"))
   {
      mAnomaly.Set(state[0], state[1], state[5], mAnomalyType);
   }
   else
   {
      retval = false;
   }
   
   #if DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckKeplerian() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckModKeplerian(Rvector6 &state)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckModKeplerian(Rvector6 &state)
{
   mAnomalyType = anomalyComboBox->GetValue().c_str();
   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "RadPer", "Real Number"))
   {
      if (state[0] == 0.0)
      {
         theScPanel->CheckReal(state[0], mElements[0], "RadPer", "Real Number != 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (theScPanel->CheckReal(state[1], mElements[1], "RadApo", "Real Number"))
   {
      if (state[1] < 0.0)
      {
         theScPanel->CheckReal(state[1], mElements[1], "RadApo", "Real Number >= 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (!theScPanel->CheckReal(state[2], mElements[2], "INC", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[3], mElements[3], "RAAN", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[4], mElements[4], "AOP", "Real Number"))
      retval = false;
   
   // check Anomaly
   if (theScPanel->CheckReal(state[5], mElements[5], mAnomalyType, "Real Number"))
   {
      mAnomaly.Set(state[0], state[1], state[5], mAnomalyType);
   }
   else
   {
      retval = false;
   }
   
   #if DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckModKeplerian() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckSpherical(Rvector6 &state, const wxString &stateType)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckSpherical(Rvector6 &state, const wxString &stateType)
{
   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "RMAG", "Real Number"))
   {
      if (state[0] <= 0.0)
      {
         theScPanel->CheckReal(state[0], mElements[0], "RMAG", "Real Number > 0.0", true);
      }
   }
   else
   {
      retval = false;
   }
   
   if (!theScPanel->CheckReal(state[1], mElements[1], "RA", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[2], mElements[2], "DEC", "Real Number"))
      retval = false;
   
   if (theScPanel->CheckReal(state[3], mElements[3], "VMAG", "Real Number"))
   {
      if (state[3] < 0.0)
      {
         theScPanel->CheckReal(state[4], mElements[3], "VMAG", "Real Number >= 0.0", true);
         retval = false;
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
   
   if (!theScPanel->CheckReal(state[4], mElements[4], label5, "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[5], mElements[5], label6, "Real Number"))
      retval = false;
   
   #if DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckSpherical() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckEquinoctial(Rvector6 &state)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckEquinoctial(Rvector6 &state)
{
   bool retval = true;
   
   if (theScPanel->CheckReal(state[0], mElements[0], "SMA", "Real Number"))
   {
      if (state[0] == 0.0)
      {
         theScPanel->CheckReal(state[0], mElements[0], "SMA", "Real Number != 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (!theScPanel->CheckReal(state[1], mElements[1], "h", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[2], mElements[2], "k", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[3], mElements[3], "p", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[4], mElements[4], "q", "Real Number"))
      retval = false;
   
   if (!theScPanel->CheckReal(state[5], mElements[5], "Mean Longitude", "Real Number"))
      retval = false;
   
   #if DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckEquinoctial() returning %d\n", retval);
   #endif
   
   return retval;
}


