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
#include "StringUtil.hpp"  // for ToDouble()
#include <sstream>

//#define DEBUG_ORBIT_PANEL 1
//#define DEBUG_ORBIT_PANEL_LOAD 1
//#define DEBUG_ORBIT_PANEL_CONVERT 1
//#define DEBUG_ORBIT_PANEL_COMBOBOX 1
//#define DEBUG_ORBIT_PANEL_SAVE 1
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
   
   mIsCoordSysChanged = false;
   mIsStateChanged = false;
   mIsStateModified = false;
   mIsStateTypeChanged = false;
   mIsEpochChanged = false;
   mIsEpochModified = false;
   canClose = true;
   dataChanged = false;
   
   anomaly = theSpacecraft->GetAnomaly();
   
   mMsgFormat =
      "The value of \"%s\" for field \"%s\" on object \""
      + theSpacecraft->GetName() +  "\" is not an allowed value. \n"
      "The allowed values are: [%s].";
   
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

   mInternalCoord = theGuiInterpreter->GetInternalCoordinateSystem();

   if (mInternalCoord == NULL)
   {
      MessageInterface::ShowMessage
         ("   mInternalCoord is NULL.\n");
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
      fromEpochFormat = epochFormat;
      
      std::string epochStr = theSpacecraft->GetStringParameter("Epoch"); 
      epochValue->SetValue(wxT(epochStr.c_str()));
      
      // load the coordinate system
      std::string coordSystemStr =
         theSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      mCoordSysComboBox->SetValue(coordSystemStr.c_str());
      mFromCoordStr = coordSystemStr;
      
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
         wxString stateTypeList[] =
         {
            wxT("Cartesian"),
            wxT("Keplerian"),
            wxT("ModifiedKeplerian"),
            wxT("SphericalAZFPA"),
            wxT("SphericalRADEC"),
            wxT("Equinoctial")
         };

         for (unsigned int i = 0; i<6; i++)
            stateTypeComboBox->Append(wxString(stateTypeList[i].c_str()));

         stType = theSpacecraft->GetDisplayCoordType();
      }
      else
      {
         wxString stateTypeList[] =
         {
            wxT("Cartesian"),
            wxT("SphericalAZFPA"),
            wxT("SphericalRADEC")
         };

         for (unsigned int i = 0; i<3; i++)
            stateTypeComboBox->Append(wxString(stateTypeList[i].c_str()));

         stType = "Cartesian";
      }
      
      stateTypeComboBox->SetValue(wxT(stType.c_str()));
      //mFromStateTypeStr = stType.c_str();
      mFromStateTypeStr = stType;

      // load the anomaly type - if state type is Keplerian or Modified Keplerian
      std::string anType = theSpacecraft->GetStringParameter("AnomalyType");

      #if DEBUG_ORBIT_PANEL_LOAD
         MessageInterface::ShowMessage("\nOrbitPanel::LoadData()...\n"
         "Anomaly type = %s \n", anType.c_str());   
      #endif

      // anomaly types - hard coded for now
      wxString anomalyList[] =
      {
         wxT("Mean Anomaly"),
         wxT("True Anomaly"),
         wxT("Eccentric Anomaly"),
         wxT("Hyperbolic Anomaly")
      };

      for (unsigned int i = 0; i<4; i++)
         anomalyComboBox->Append(wxString(anomalyList[i].c_str()));
      

      anomalyComboBox->SetValue(wxT(anType.c_str()));

      mFromAnomalyTypeStr = anType;
      
      #if DEBUG_ORBIT_PANEL_LOAD
          MessageInterface::ShowMessage("\nOrbitPanel::LoadData()...\n"
             "Anomaly info -> a: %f, e: %f, %s: %f\n", 
             anomaly.GetSMA(),anomaly.GetECC(),anomaly.GetType().c_str(),
             anomaly.GetValue());   
      #endif

      // Get Spacecraft initial state and display
      mCartState.Set(theSpacecraft->GetState().GetState());
      mTempCartState = mCartState;
      mOutState = mCartState;
      
      textCtrl1->SetValue(ToString(mOutState[0]));
      textCtrl2->SetValue(ToString(mOutState[1]));
      textCtrl3->SetValue(ToString(mOutState[2]));
      textCtrl4->SetValue(ToString(mOutState[3]));
      textCtrl5->SetValue(ToString(mOutState[4]));
      textCtrl6->SetValue(ToString(mOutState[5]));
      
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
      MessageInterface::ShowMessage(
         "OrbitPanel::SaveData() entered\n   mCartState=%s\n   "
         "mTempCartState=%s\n   mOutState=%s\n", mCartState.ToString().c_str(), 
         mTempCartState.ToString().c_str(), mOutState.ToString().c_str());
      MessageInterface::ShowMessage("mIsCoordSysChanged=%d\n", mIsCoordSysChanged);
      MessageInterface::ShowMessage("mIsStateTypeChanged=%d\n", mIsStateTypeChanged);
      MessageInterface::ShowMessage("mIsStateChanged=%d\n", mIsStateChanged);
      MessageInterface::ShowMessage("mIsEpochChanged=%d\n", mIsEpochChanged);
   #endif
   
   try
   {
      Rvector6 outState, cartState, displayState;
      canClose = true;
      
      Real rvalue, rvalue2;      
      wxString epochFormatStr = epochFormatComboBox->GetValue();
      wxString epochStr       = epochValue->GetValue();
      wxString coordSystemStr = mCoordSysComboBox->GetValue();
      wxString stateTypeStr   = stateTypeComboBox->GetValue();
      
      mElement1 = textCtrl1->GetValue();
      mElement2 = textCtrl2->GetValue();
      mElement3 = textCtrl3->GetValue();
      mElement4 = textCtrl4->GetValue();
      mElement5 = textCtrl5->GetValue();
      mElement6 = textCtrl6->GetValue();
      mAnomalyType = description6->GetLabel().c_str();
      
      GmatStringUtil::ToDouble(mElement1,&rvalue);
      GmatStringUtil::ToDouble(mElement2,&rvalue2);
      
      if ((rvalue >= 0.0) && (rvalue2 <= 1.0) && 
         ((strcmp(mAnomalyType.c_str(), "HA") == 0) || 
          (strcmp(mAnomalyType.c_str(), "EA") == 0)))
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Anomaly type and other elements are incompatible.");
         canClose = false;
      }
      else
      {
         //-----------------------------------------------------------
         // save epoch
         //-----------------------------------------------------------
         if (mIsEpochChanged)
         {
            // save epoch format and epoch
            try
            {
               theSpacecraft->SetDateFormat(epochFormatStr.c_str());
               theSpacecraft->SetEpoch(epochStr.c_str());
               mIsEpochChanged = false;
            }
            catch (BaseException &e)
            {
               MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
               canClose = false;
            }
         }
         
         //-----------------------------------------------------------
         // save orbital elements
         //-----------------------------------------------------------
         bool retval = true;
         
         if (strcmp(stateTypeStr.c_str(), "Cartesian") == 0)
         {
            retval = CheckCartesian(displayState);
         }
         else if ((strcmp(stateTypeStr.c_str(), "Keplerian") == 0) ||
                  (strcmp(stateTypeStr.c_str(), "ModifiedKeplerian") == 0))
         {
            if (strcmp(stateTypeStr.c_str(), "Keplerian") == 0)
            {
               retval = CheckKeplerian(displayState);
            }
            else if (strcmp(stateTypeStr.c_str(), "ModifiedKeplerian") == 0)
            {
               retval = CheckModKeplerian(displayState);               
            }        
         }
         else if ((strcmp(stateTypeStr.c_str(), "SphericalAZFPA") == 0) ||
                  (strcmp(stateTypeStr.c_str(), "SphericalRADEC") == 0))
         {
            retval = CheckSpherical(displayState, stateTypeStr);            
         }
         else if (strcmp(stateTypeStr.c_str(), "Equinoctial") == 0)
         {
            retval = CheckEquinoctial(displayState);
         }
         
         if (retval)
         {
            BuildState(displayState);
            theSpacecraft->SetState(mCartState);
            
            //--------------------------------------------------------
            // save state type name if changed
            //--------------------------------------------------------
            if (mIsStateTypeChanged)
            {
               mIsStateTypeChanged = false;
               
               #if DEBUG_ORBIT_PANEL_SAVE
               MessageInterface::ShowMessage
                  ("OrbitPanel::SaveData() Saving stateType=%s\n", 
                   stateTypeStr.c_str());
               #endif
               
               theSpacecraft->SetStringParameter("StateType", stateTypeStr.c_str());
               /// @todo: need to correct in spacecraft code because this should
               /// only save display state type
            }
            
            // save state type
            /// @todo: need to correct in spacecraft code because this should
            /// only save display state type
            
            //--------------------------------------------------------
            // save coordinate system name if changed
            //--------------------------------------------------------
            if (mIsCoordSysChanged)
            {
               mIsCoordSysChanged = false;
               theSpacecraft->SetStringParameter("CoordinateSystem", 
                                                 coordSystemStr.c_str());
            }
         }
         
         if (canClose)
            dataChanged = false;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OrbitPanel:SaveData() error occurred!\n%s\n", e.GetMessage().c_str());
         canClose = false;
         return;
   }
   
   #if DEBUG_ORBIT_PANEL_SAVE
      MessageInterface::ShowMessage
         ("OrbitPanel::SaveData() exiting\n   mCartState=%s\n   mTempCartState=%s\n   "
          "mOutState=%s\n", mCartState.ToString().c_str(), mTempCartState.ToString().c_str(),
          mOutState.ToString().c_str());
   MessageInterface::ShowMessage("===> mIsStateChanged=%d\n", mIsStateChanged);
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
   textCtrl1 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, 
                   wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
   label1 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label1"), 
                    wxDefaultPosition, wxDefaultSize, 0 );

   // Element 2
   description2 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor2    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl2 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
   label2 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label2"), 
                wxDefaultPosition, wxDefaultSize, 0 );
   
   // Element 3
   description3 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor3    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl3 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
   label3 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label3"), 
                wxDefaultPosition, wxDefaultSize, 0 );
    
   // Element 4
   description4 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor4    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl4 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
   label4 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label4"), 
                wxDefaultPosition, wxDefaultSize, 0 );

   // Element 5    
   description5 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor5    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl5 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
   label5 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label5"), 
                wxDefaultPosition, wxDefaultSize, 0 );
    
   // Element 6
   description6 = new wxStaticText( elementsPanel, ID_TEXT, 
                      wxT("Descriptor6    "), wxDefaultPosition, 
                      wxDefaultSize, 0 );
   textCtrl6 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""),
                   wxDefaultPosition, wxSize(150,-1), 0 );
   label6 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label6"), 
                wxDefaultPosition, wxDefaultSize, 0 );

   // Add to wx*Sizers 
   item3->Add( description1, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl1, 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( label1, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description2, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl2, 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( label2, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description3, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl3, 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( label3, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description4, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl4, 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( label4, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description5, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl5, 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( label5, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( description6, 0, wxALIGN_LEFT|wxALL, bsize );
   item3->Add( textCtrl6, 0, wxALIGN_CENTER|wxALL, bsize );
   item3->Add( label6, 0, wxALIGN_LEFT|wxALL, bsize );

   item0->Add( item3, 0, wxGROW|wxALL|wxALIGN_CENTER, bsize );

   elementsPanel->SetAutoLayout( TRUE );
   elementsPanel->SetSizer( item0 );

   item0->Fit( elementsPanel );
   item0->SetSizeHints( elementsPanel );
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
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
      std::string toEpochFormat = epochFormatComboBox->GetValue().c_str();    
      std::string epochStr = epochValue->GetValue().c_str();
      
      #if DEBUG_ORBIT_PANEL_COMBOBOX
      MessageInterface::ShowMessage
         ("OrbitPanel::OnComboBoxChange() fromEpochFormat=%s, "
          "toEpochFormat=%s\n   epochStr=%s, mIsEpochModified=%d\n",
          fromEpochFormat.c_str(), toEpochFormat.c_str(), epochStr.c_str(),
          mIsEpochModified);
      #endif
      
      try
      {
         // if modified by user, check if epoch is valid first
         if (mIsEpochModified)
         {
            TimeConverterUtil::ValidateTimeFormat(fromEpochFormat, epochStr);
            //theSpacecraft->SetDateFormat(fromEpochFormat);
            //theSpacecraft->SetEpoch(epochStr);
            
            // update format and epoch
            theSpacecraft->SetEpoch(epochStr);
            theSpacecraft->SetDateFormat(toEpochFormat);
            epochValue->SetValue(theSpacecraft->GetStringParameter("Epoch").c_str());
            
            mIsEpochModified = false;
            fromEpochFormat = toEpochFormat;
         }
         else
         {
            theSpacecraft->SetDateFormat(toEpochFormat);
            epochValue->SetValue(theSpacecraft->GetStringParameter("Epoch").c_str());
            fromEpochFormat = toEpochFormat;
         }
      }
      catch (BaseException &e)
      {
         epochFormatComboBox->SetValue(fromEpochFormat.c_str());
         theSpacecraft->SetDateFormat(fromEpochFormat);
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
         ("OrbitPanel::OnComboBoxChange() mFromCoordStr=%s, coordSysStr=%s\n"
          "   mFromStateTypeStr=%s, stateTypeStr=%s\n", mFromCoordStr.c_str(),
          coordSysStr.c_str(), mFromStateTypeStr.c_str(), stateTypeStr.c_str());
      #endif
      
      if (event.GetEventObject() == mCoordSysComboBox)
         mIsCoordSysChanged = true;
      
      if (event.GetEventObject() == stateTypeComboBox)
      {
         Rvector6 state;
         mIsStateTypeChanged = true;
         mAnomalyType = anomalyComboBox->GetValue();
         bool retval = false;
         
         // If state modified by user, validate elements first
         if (mIsStateModified)
         {            
            mElement1 = textCtrl1->GetValue();
            mElement2 = textCtrl2->GetValue();
            mElement3 = textCtrl3->GetValue();
            mElement4 = textCtrl4->GetValue();
            mElement5 = textCtrl5->GetValue();
            mElement6 = textCtrl6->GetValue();
            
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
      
      //Epoch is handled before coordinate system or state type change
      // so commented out (loj: 12/27/06)
      //if (mIsEpochChanged)
      //   UpdateEpoch();
      
      DisplayState();
      
      if (event.GetEventObject() == mCoordSysComboBox)
         mFromCoordStr = mCoordSysComboBox->GetValue().c_str();
      
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
      std::string anomalyType;
    
      wxString description, stateValue;
       
      wxString anomalySelected = anomalyComboBox->GetValue();

      if (anomalySelected == "Mean Anomaly") 
         anomalyType = "MA";
      else if (anomalySelected == "True Anomaly") 
         anomalyType = "TA";
      else if (anomalySelected == "Eccentric Anomaly")
         anomalyType = "EA";
      else if (anomalySelected == "Hyperbolic Anomaly")
         anomalyType = "HA";
      
      Real value = anomaly.Convert(mFromAnomalyTypeStr, anomalyType);
      
      int anomalyID = theSpacecraft->GetParameterID("AnomalyType");
      theSpacecraft->SetStringParameter(anomalyID, anomalyType);
      description.Printf("%s", anomalyType.c_str());
      description6->SetLabel(description);
      
      textCtrl6->SetValue(ToString(value));

      mFromAnomalyTypeStr = anomalyType;
      #if DEBUG_ORBIT_PANEL_COMBOBOX
         MessageInterface::ShowMessage( "anomaly id = %d\n",anomalyID );
         MessageInterface::ShowMessage( "anomaly type = %s\n",anomalyType.c_str() );
         MessageInterface::ShowMessage( "s/c anomaly type = %s\n",anomaly.GetType().c_str() );
      #endif
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
   
   if (obj == textCtrl1 || obj == textCtrl2 || obj == textCtrl3 ||
       obj == textCtrl4 || obj == textCtrl5 || obj == textCtrl6 )
   {
      if ( (textCtrl1->IsModified()) || (textCtrl2->IsModified()) || 
           (textCtrl3->IsModified()) || (textCtrl4->IsModified()) ||
           (textCtrl5->IsModified()) || (textCtrl6->IsModified()) )
      {
         mIsStateChanged = true;
         mIsStateModified = true;
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
  Integer baseLabel = theSpacecraft->GetParameterID("Element1"),
           baseUnit  = theSpacecraft->GetParameterID("Element1Units");

   std::string st = theSpacecraft->GetStringParameter("StateType");
   theSpacecraft->SetStringParameter("StateType", stateType);
   
   description1->SetLabel(theSpacecraft->GetParameterText(baseLabel).c_str());
   label1->SetLabel(theSpacecraft->GetStringParameter(baseUnit).c_str());

   description2->SetLabel(theSpacecraft->GetParameterText(baseLabel+1).c_str());
   label2->SetLabel(theSpacecraft->GetStringParameter(baseUnit+1).c_str());

   description3->SetLabel(theSpacecraft->GetParameterText(baseLabel+2).c_str());
   label3->SetLabel(theSpacecraft->GetStringParameter(baseUnit+2).c_str());

   description4->SetLabel(theSpacecraft->GetParameterText(baseLabel+3).c_str());
   label4->SetLabel(theSpacecraft->GetStringParameter(baseUnit+3).c_str());

   description5->SetLabel(theSpacecraft->GetParameterText(baseLabel+4).c_str());
   label5->SetLabel(theSpacecraft->GetStringParameter(baseUnit+4).c_str());

   description6->SetLabel(theSpacecraft->GetParameterText(baseLabel+5).c_str());
   label6->SetLabel(theSpacecraft->GetStringParameter(baseUnit+5).c_str());

   if ( (strcmp(stateType.c_str(), "Keplerian") == 0) || 
        (strcmp(stateType.c_str(), "ModifiedKeplerian") == 0) )
   {
      int anomalyID = theSpacecraft->GetParameterID("AnomalyType");
      wxString description = (theSpacecraft->GetStringParameter(anomalyID)).c_str();

      // set the labels for the anomaly
      anomalyStaticText->Show(true);
      anomalyComboBox->Show(true);
      
      if (strcmp(description.c_str(), "MA") == 0) 
         anomalyComboBox->SetSelection(0);
      else if (strcmp(description.c_str(), "TA") == 0) 
         anomalyComboBox->SetSelection(1);
      else if (strcmp(description.c_str(), "EA") == 0)
         anomalyComboBox->SetSelection(2);
      else if (strcmp(description.c_str(), "HA") == 0)
         anomalyComboBox->SetSelection(3);
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
// void UpdateEpoch()
//------------------------------------------------------------------------------
void OrbitPanel::UpdateEpoch()
{
   std::string newEpoch = epochValue->GetValue().c_str();
   std::string toEpochFormat = epochFormatComboBox->GetValue().c_str();
   
   #if DEBUG_ORBIT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("OrbitPanel::UpdateEpoch() newEpoch = %s\n", newEpoch.c_str() );
   MessageInterface::ShowMessage
      ("OrbitPanel::UpdateEpoch() fromEpochFormat=%s, toEpochFormat=%s\n",
       fromEpochFormat.c_str(), toEpochFormat.c_str());
   #endif
   
   try
   {
      theSpacecraft->SetStringParameter("DateFormat", toEpochFormat);
      theSpacecraft->SetEpoch(newEpoch);
      epochValue->SetValue(theSpacecraft->GetStringParameter("Epoch").c_str());
      mEpoch = theSpacecraft->GetEpoch();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetMessage());
   }
   
   // turn off modified flag
   epochValue->DiscardEdits();
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
   
   if (mIsStateModified)
   {
      // User has typed in new state data
      midState[0] = atof(textCtrl1->GetValue());
      midState[1] = atof(textCtrl2->GetValue());
      midState[2] = atof(textCtrl3->GetValue());
      midState[3] = atof(textCtrl4->GetValue());
      midState[4] = atof(textCtrl5->GetValue());
      midState[5] = atof(textCtrl6->GetValue());
   }
   else
   {
      // Load midState with Cartesian spacecraft state in internal coordinates
      midState = mCartState;
      isInternal = true;
   }
   
   BuildState(midState, isInternal);
   
   #if DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage(
         "OrbitPanel::DisplayState()--- after conversion, mOutState= %s\n",
         mOutState.ToString().c_str());
   #endif
      
   textCtrl1->SetValue(ToString(mOutState[0]));
   textCtrl2->SetValue(ToString(mOutState[1]));
   textCtrl3->SetValue(ToString(mOutState[2]));
   textCtrl4->SetValue(ToString(mOutState[3]));
   textCtrl5->SetValue(ToString(mOutState[4]));
   textCtrl6->SetValue(ToString(mOutState[5]));
   
   // labels for elements, anomaly and units
   SetLabelsUnits(stateTypeStr);
   
   // This flag is only reset in SaveData() (loj: 12/27/06)
   //mIsStateChanged = false;
   mIsStateModified = false;
   
   // turn off modified flag (loj: 12/27/06 added)
   textCtrl1->DiscardEdits();
   textCtrl2->DiscardEdits();
   textCtrl3->DiscardEdits();
   textCtrl4->DiscardEdits();
   textCtrl5->DiscardEdits();
   textCtrl6->DiscardEdits();
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
         "OrbitPanel::BuildState()--- The input state (%s %s coordinates)"
         " is [%s]\n",
         (isInternal ? "Internal" : mFromCoord->GetName().c_str()),
         (isInternal ? "Cartesian" : mFromStateTypeStr.c_str()),
         tempState.ToString().c_str());
   #endif

   Rvector6 midState;
   std::string stateTypeStr = stateTypeComboBox->GetValue().c_str();
   
   if (isInternal) {
      // Input state is Cartesian expressed in internal coordinates
      mCartState = inputState;
   }
   else
   {
      // Convert input state to the Cartesian representation
      stateConverter.SetMu(mFromCoord);
      midState = stateConverter.Convert(inputState, mFromStateTypeStr, 
         "Cartesian", anomaly);
      
      // Transform to internal coordinates
      mCoordConverter.Convert(A1Mjd(mEpoch), midState, mFromCoord, mCartState, 
         mInternalCoord);
   }
   
   #if DEBUG_ORBIT_PANEL_CONVERT
      MessageInterface::ShowMessage(
         "OrbitPanel::BuildState()--- The internal CS representation is [%s]\n",
         mCartState.ToString().c_str());
   #endif
   
   // Transform to the desired coordinate system
   mCoordConverter.Convert(A1Mjd(mEpoch), mCartState, mInternalCoord, midState, 
      mOutCoord);
   
   // and convert to the desired representation
   stateConverter.SetMu(mOutCoord);
   mOutState = stateConverter.Convert(midState, "Cartesian", stateTypeStr,
      anomaly);
}


//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString OrbitPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}


//------------------------------------------------------------------------------
// Rvector6 ConvertState(CoordinateSystem *cs)
//------------------------------------------------------------------------------
Rvector6 OrbitPanel::ConvertState(CoordinateSystem *cs, const Rvector6 &state, 
                                  const std::string &fromElementType, 
                                  const std::string &toElementType)
{
   Rvector6 newState;
   newState.Set(state[0],state[1],state[2],state[3],state[4],state[5]); 
   
   try
   {
      stateConverter.SetMu(cs);
      newState = stateConverter.Convert(state, fromElementType, toElementType, 
                                        anomaly);
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OrbitPanel:ConvertState() error occurred!\n%s\n", e.GetMessage().c_str());
   }
   return newState;
}


//------------------------------------------------------------------------------
// bool CheckCartesian(Rvector6 &state)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckCartesian(Rvector6 &state)
{
   bool retval = true;
   
   if (!CheckReal(state[0], mElement1, "X", "Real Number"))
      retval = false;
   if (!CheckReal(state[1], mElement2, "Y", "Real Number"))
      retval = false;
   if (!CheckReal(state[2], mElement3, "Z", "Real Number"))
      retval = false;
   if (!CheckReal(state[3], mElement4, "VX", "Real Number"))
      retval = false;
   if (!CheckReal(state[4], mElement5, "VY", "Real Number"))
      retval = false;
   if (!CheckReal(state[5], mElement6, "VZ", "Real Number"))
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
   bool retval = true;
   
   if (CheckReal(state[0], mElement1, "SMA", "Real Number"))
   {
      if (state[0] == 0.0)
      {
         CheckReal(state[0], mElement1, "SMA", "Real Number != 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (CheckReal(state[1], mElement2, "ECC", "Real Number"))
   {
      if ((state[1] < 0.0) || (state[1] >= 1.0))
      {
         CheckReal(state[1], mElement1, "ECC", "0.0 < = Real Number < 1.0", true);
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
   
   if (!CheckReal(state[2], mElement3, "INC", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[3], mElement4, "RAAN", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[4], mElement5, "AOP", "Real Number"))
      retval = false;
   
   // check Anomaly
   if (CheckReal(state[5], mElement6, mAnomalyType, "Real Number"))
   {
      anomaly.Set(state[0], state[1], state[5], mAnomalyType);
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
   bool retval = true;
   
   if (CheckReal(state[0], mElement1, "RadPer", "Real Number"))
   {
      if (state[0] == 0.0)
      {
         CheckReal(state[0], mElement1, "RadPer", "Real Number != 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (CheckReal(state[1], mElement2, "RadApo", "Real Number"))
   {
      if (state[1] < 0.0)
      {
         CheckReal(state[1], mElement2, "RadApo", "Real Number >= 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (!CheckReal(state[2], mElement3, "INC", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[3], mElement4, "RAAN", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[4], mElement5, "AOP", "Real Number"))
      retval = false;
   
   // check Anomaly
   if (CheckReal(state[5], mElement6, mAnomalyType, "Real Number"))
   {
      anomaly.Set(state[0], state[1], state[5], mAnomalyType);
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
   
   if (CheckReal(state[0], mElement1, "RMAG", "Real Number"))
   {
      if (state[0] <= 0.0)
      {
         CheckReal(state[0], mElement1, "RMAG", "Real Number > 0.0", true);
      }
   }
   else
   {
      retval = false;
   }
   
   if (!CheckReal(state[1], mElement2, "RA", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[2], mElement3, "DEC", "Real Number"))
      retval = false;

   if (CheckReal(state[3], mElement4, "VMAG", "Real Number"))
   {
      if (state[3] < 0.0)
      {
         CheckReal(state[4], mElement4, "VMAG", "Real Number >= 0.0", true);
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
   
   if (!CheckReal(state[4], mElement5, label5, "Real Number"))
      retval = false;
   
   if (!CheckReal(state[5], mElement6, label6, "Real Number"))
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
   
   if (CheckReal(state[0], mElement1, "SMA", "Real Number"))
   {
      if (state[0] == 0.0)
      {
         CheckReal(state[0], mElement1, "SMA", "Real Number != 0.0", true);
         retval = false;
         canClose = false;
      }
   }
   else
   {
      retval = false;
   }
   
   if (!CheckReal(state[1], mElement2, "h", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[2], mElement3, "k", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[3], mElement4, "p", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[4], mElement5, "q", "Real Number"))
      retval = false;
   
   if (!CheckReal(state[5], mElement6, "Mean Longitude", "Real Number"))
      retval = false;
   
   #if DEBUG_ORBIT_PANEL_CHECK_RANGE
   MessageInterface::ShowMessage("CheckEquinoctial() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckReal(Real &rvalue, const std::string &element,
//                const std::string &field, const std::string &expRange)
//------------------------------------------------------------------------------
bool OrbitPanel::CheckReal(Real &rvalue, const std::string &element,
                           const std::string &field, const std::string &expRange,
                           bool onlyMsg)
{
   //MessageInterface::ShowMessage
   //   ("===> CheckReal() element=%s, field=%s, expRange=%s\n", element.c_str(),
   //    field.c_str(), expRange.c_str());
   
   if (onlyMsg)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), element.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }

   // check for real value
   Real rval;
   if (GmatStringUtil::ToDouble(element, &rval))
   {
      rvalue = rval;
      return true;
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), element.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }
}
