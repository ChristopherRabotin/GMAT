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
 */
//------------------------------------------------------------------------------
#include "OrbitPanel.hpp"
#include "MessageInterface.hpp"
#include "GmatAppData.hpp"

#include <sstream>

//#define DEBUG_ORBIT_PANEL 1

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
// OrbitPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs OrbitPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
OrbitPanel::OrbitPanel(wxWindow *parent,
                        Spacecraft *spacecraft,
                        SolarSystem *solarsystem,
                        wxButton *theApplyButton)
                       :wxPanel(parent)
{
   // initalize data members
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();

   this->theSpacecraft = spacecraft;
   this->theSolarSystem = solarsystem;
   this->theApplyButton = theApplyButton;

   mIsCoordSysChanged = false;
   mIsTextChanged = false;
   mIsStateChanged = false;

   Create();
}

OrbitPanel::~OrbitPanel()
{
   //loj: 6/7/05 Added
   theGuiManager->UnregisterComboBox("CoordinateSystem", mCoordSysComboBox);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnComboChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnComboBoxChange(wxCommandEvent& event)
{
   Rvector6 inState, outState, tempState;
   
   wxString element;
   std::stringstream buffer;
   buffer.precision(18);

   // get values from text fields
   wxString el1 = textCtrl1->GetValue();
   wxString el2 = textCtrl2->GetValue();
   wxString el3 = textCtrl3->GetValue();
   wxString el4 = textCtrl4->GetValue();
   wxString el5 = textCtrl5->GetValue();
   wxString el6 = textCtrl6->GetValue(); 

   inState[0] = atof(el1);
   inState[1] = atof(el2);
   inState[2] = atof(el3);
   inState[3] = atof(el4);
   inState[4] = atof(el5);
   inState[5] = atof(el6);

   // -------------------- epoch format change --------------------
   if (event.GetEventObject() == epochFormatComboBox)
   {
      wxString toEpochFormat = epochFormatComboBox->GetStringSelection();    
      wxString epochStr = epochValue->GetValue();
    
      // convert to new epoch format
      std::string newEpoch = timeConverter.Convert(epochStr.c_str(), fromEpochFormat,
                                                   toEpochFormat.c_str());
    
      epochValue->SetValue(newEpoch.c_str());
    
      fromEpochFormat = toEpochFormat.c_str();
   }

   // -------------------- coordinate system change --------------------
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      try
      {
         #if DEBUG_ORBIT_PANEL
            MessageInterface::ShowMessage("---------- C/S change ----------\n");
         #endif

         // epoch
         Real epoch = theSpacecraft->GetRealParameter("Epoch");
    
         // output Coordinate System
         wxString outCoordSystem = mCoordSysComboBox->GetStringSelection();
         CoordinateSystem* outCoord = (CoordinateSystem*) 
            theGuiInterpreter->GetCoordinateSystem(outCoordSystem.c_str());

         if ( (strcmp(fromStateType.c_str(), "Cartesian") == 0) && 
              (strcmp(outCoordSystem.c_str(), "EarthMJ2000Eq") == 0) && 
              (!mIsTextChanged) )
         {
            // use unchanged cartesian values for output
            outState = mCartState;
         }
         else if ( (strcmp(fromStateType.c_str(), "Cartesian") == 0) && 
                   (strcmp(outCoordSystem.c_str(), "EarthMJ2000Eq") != 0) &&
                   (!mIsTextChanged) )
         {
            // input Coordinate System
            CoordinateSystem* inCoord = (CoordinateSystem*) 
               theGuiInterpreter->GetCoordinateSystem("EarthMJ2000Eq");

            // initialize coordinate systems
            InitializeCoordinateSystem(inCoord);
            InitializeCoordinateSystem(outCoord);
   
            // convert to output coordinate system
            mCoordConverter.Convert(A1Mjd(epoch), mCartState, inCoord, outState,
                                    outCoord);
         }
         else if ( (strcmp(fromStateType.c_str(), "Cartesian") != 0) && 
                   (strcmp(outCoordSystem.c_str(), "EarthMJ2000Eq") != 0) &&
                   (!mIsTextChanged) )
         {
            // input Coordinate System
            CoordinateSystem* inCoord = (CoordinateSystem*) 
               theGuiInterpreter->GetCoordinateSystem("EarthMJ2000Eq");

            // initialize coordinate systems
            InitializeCoordinateSystem(inCoord);
            InitializeCoordinateSystem(outCoord);
   
            // convert to output coordinate system
            mCoordConverter.Convert(A1Mjd(epoch), mCartState, inCoord, outState,
                                    outCoord);

            // convert from cartesian to selected state type
            tempState = stateConverter.Convert(outState, "Cartesian",
                                               fromStateType, anomaly);
            outState = tempState;
         }
         else
         {
            // convert to cartesian
            tempState = stateConverter.Convert(inState, fromStateType,
                                               "Cartesian", anomaly);

            // input Coordinate System
            CoordinateSystem* inCoord = (CoordinateSystem*) 
               theGuiInterpreter->GetCoordinateSystem(fromCoordSys.c_str());

            // initialize coordinate systems
            InitializeCoordinateSystem(inCoord);
            InitializeCoordinateSystem(outCoord);

            // convert to output coordinate system
            mCoordConverter.Convert(A1Mjd(epoch), tempState, inCoord, outState,
                                    outCoord);

            // convert from cartesian to selected state type
            tempState = stateConverter.Convert(outState, "Cartesian",
                                               fromStateType, anomaly);
            outState = tempState;
         }

         #if DEBUG_ORBIT_PANEL
//            MessageInterface::ShowMessage
//               ( "from coordinate system = %s\nto coordinate system = %s\n", 
//                inCoord->GetName().c_str(),outCoord->GetName().c_str() );
            MessageInterface::ShowMessage
               ( "state type = %s\n" ,fromStateType.c_str() );
            MessageInterface::ShowMessage
               ( "anomaly type = %s\n",anomaly.GetType().c_str() );
            MessageInterface::ShowMessage
               ( "--- after C/S convert --- \nstate = %s\n",
               outState.ToString().c_str());
            MessageInterface::ShowMessage("---------- C/S change ----------\n\n");
         #endif

         mIsCoordSysChanged = true;
         fromCoordSys = outCoordSystem.c_str();
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage
            ("OrbitPanel:OnComboBoxChange() error occurred!\n%s\n", e.GetMessage().c_str());
      }

   }
   
   // -------------------- state type change --------------------
   if (event.GetEventObject() == stateTypeComboBox)
   {
      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage("---------- state change ----------\n");
      #endif

      wxString stateType = stateTypeComboBox->GetStringSelection();
      std::string toStateType = stateType.c_str();
    
      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage
            ("from state type = %s\n to state type = %s\n" , 
               fromStateType.c_str(),toStateType.c_str());
      #endif

      if ( (strcmp(toStateType.c_str(), "Cartesian") == 0) && 
           (strcmp(fromCoordSys.c_str(), "EarthMJ2000Eq") == 0)&& 
           (!mIsTextChanged) )
      {
         #if DEBUG_ORBIT_PANEL
            MessageInterface::ShowMessage( "Cartesian state, no text changed\n" );
         #endif

         // use unchanged cartesian values for output
         outState = mCartState;
      }
      else if ( (strcmp(toStateType.c_str(), "Cartesian") != 0) && 
                (strcmp(fromCoordSys.c_str(), "EarthMJ2000Eq") == 0)&& 
                (!mIsTextChanged) )
      {
         #if DEBUG_ORBIT_PANEL
            MessageInterface::ShowMessage( "Not Cartesian and no text changed\n" );
         #endif

         // convert to new state type
         outState = stateConverter.Convert(mCartState,"Cartesian",toStateType,
                                           anomaly);
      }
      else
      {
         #if DEBUG_ORBIT_PANEL
            MessageInterface::ShowMessage( "Text change\n" );
         #endif

         // convert to new state type
         outState = stateConverter.Convert(inState,fromStateType,toStateType, 
                                           anomaly);
      }

      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage
            ("--- after state convert--- \nout state = %s\ncartesian state = %s\n",
               outState.ToString().c_str(), mCartState.ToString().c_str());
      #endif
    
      // labels for elements, anomaly and units
      SetLabelsUnits(toStateType);
    
      mIsStateChanged = true;
      fromStateType = stateType.c_str(); 

      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage("---------- state change ----------\n\n");
      #endif
   }

   // -------------------- anomaly type change --------------------
   if (event.GetEventObject() == anomalyComboBox)
   {
      std::string anomalyType;
    
      wxString description, stateValue;
       
      int anomalySelected = anomalyComboBox->GetSelection();
    
      if (anomalySelected == 0) 
         anomalyType = "MA";
      else if (anomalySelected == 1) 
         anomalyType = "TA";
      else if (anomalySelected == 2)
         anomalyType = "EA";
             
      int anomalyID = theSpacecraft->GetParameterID("AnomalyType");
    
      description.Printf("%s", anomalyType.c_str());
      description6->SetLabel(description);
       
      theSpacecraft->SetStringParameter(anomalyID, anomalyType);

      // this->anomaly = theSpacecraft->anomaly;
      this->anomaly = theSpacecraft->GetAnomaly();   // Joey added it
       
      Real an = theSpacecraft->GetRealParameter(anomalyType);
      buffer << an;
      element.Printf ("%s",buffer.str().c_str());
      textCtrl6->SetValue (element);
      buffer.str(std::string());
      
      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage( "anomaly id = %d\n",anomalyID );
         MessageInterface::ShowMessage( "anomaly type = %s\n",anomalyType.c_str() );
         MessageInterface::ShowMessage( "s/c anomaly type = %s\n",anomaly.GetType().c_str() );
      #endif
   }
   
   if ( (event.GetEventObject() == mCoordSysComboBox) || 
        (event.GetEventObject() == stateTypeComboBox) )
   {
      wxString element;
      std::stringstream buffer;
      buffer.precision(18);
   
      buffer << outState[0];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl1->SetValue (element);
      buffer.str(std::string()); 

      buffer << outState[1];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl2->SetValue (element);
      buffer.str(std::string()); 

      buffer << outState[2];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl3->SetValue (element);
      buffer.str(std::string()); 
   
      buffer << outState[3];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl4->SetValue (element);
      buffer.str(std::string()); 

      buffer << outState[4];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl5->SetValue (element);
      buffer.str(std::string()); 

      buffer << outState[5];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl6->SetValue (element);
      buffer.str(std::string());
   }
      
   theApplyButton->Enable();
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
   if ( (textCtrl1->IsModified()) || (textCtrl2->IsModified()) || 
        (textCtrl3->IsModified()) || (textCtrl4->IsModified()) ||
        (textCtrl5->IsModified()) || (textCtrl6->IsModified()) )
   {     
      mIsTextChanged = true;
   }
   else
   {     
      mIsTextChanged = false;
   }
   
   theApplyButton->Enable();
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
   // labels for elements, anomaly and units
   if ( strcmp(stateType.c_str(), "Cartesian") == 0 )
   {
      // set the labels for the elements
      description1->SetLabel("X");
      description2->SetLabel("Y");
      description3->SetLabel("Z");
      description4->SetLabel("VX");
      description5->SetLabel("VY");
      description6->SetLabel("VZ");

      // set the labels for the units
      label1->SetLabel("Km");
      label2->SetLabel("Km");
      label3->SetLabel("Km");
      label4->SetLabel("Km/s");
      label5->SetLabel("Km/s");
      label6->SetLabel("Km/s");
        
      anomalyStaticText->Show(false);
      anomalyComboBox->Show(false);
   }
   else if ( (strcmp(stateType.c_str(), "Keplerian") == 0) || 
             (strcmp(stateType.c_str(), "ModifiedKeplerian") == 0) )
   {
      // set the labels for the elements
      if ( strcmp(stateType.c_str(), "Keplerian") == 0 )
      {
         description1->SetLabel("SMA");
         description2->SetLabel("ECC");
      }
      else
      {
         description1->SetLabel("RadPer");
         description2->SetLabel("RadApo");
      }
      description3->SetLabel("INC");
      description4->SetLabel("RAAN");
      description5->SetLabel("AOP");

      int anomalyID = theSpacecraft->GetParameterID("AnomalyType");
      wxString description = (theSpacecraft->GetStringParameter(anomalyID)).c_str();
      description6->SetLabel(description);

      // set the labels for the units
      label1->SetLabel("Km");
      if (strcmp(stateType.c_str(), "Keplerian") == 0)
         label2->SetLabel("");
      else
         label2->SetLabel("Km");
      label3->SetLabel("deg");
      label4->SetLabel("deg");
      label5->SetLabel("deg");
      label6->SetLabel("deg");
      
      // set the labels for the anomaly
      anomalyStaticText->Show(true);
      anomalyComboBox->Show(true);
      
      if (strcmp(description.c_str(), "MA") == 0) 
         anomalyComboBox->SetSelection(0);
      else if (strcmp(description.c_str(), "TA") == 0) 
         anomalyComboBox->SetSelection(1);
      else if (strcmp(description.c_str(), "EA") == 0)
         anomalyComboBox->SetSelection(2);
   }
   else if ( (strcmp(stateType.c_str(), "SphericalAZFPA") == 0) || 
             (strcmp(stateType.c_str(), "SphericalRADEC") == 0) )
   {
      // set the labels for the elements
      description1->SetLabel("RMAG");
      description2->SetLabel("RA");
      description3->SetLabel("DEC");
      description4->SetLabel("VMAG");
      if (strcmp(stateType.c_str(), "SphericalAZFPA") == 0)
      {
         description5->SetLabel("AZI");
         description6->SetLabel("FPA");
      }
      else
      {
         description5->SetLabel("RAV");
         description6->SetLabel("DECV");
      }

      // set the labels for the units
      label1->SetLabel("Km");
      label2->SetLabel("deg");
      label3->SetLabel("deg");
      label4->SetLabel("Km/s");
      label5->SetLabel("deg");
      label6->SetLabel("deg");
     
      anomalyStaticText->Show(false);
      anomalyComboBox->Show(false);
   }
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
    elementsPanel = new wxPanel(parent);
    wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 6, 3, 0, 0 );
    item3->AddGrowableCol( 0 );
    item3->AddGrowableCol( 1 );
    item3->AddGrowableCol( 2 );

    description1 = new wxStaticText( elementsPanel, ID_TEXT, 
                    wxT("Descriptor1     "), wxDefaultPosition, 
                    wxDefaultSize, 0 );
    item3->Add( description1, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl1 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, 
                    wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl1, 0, wxALIGN_CENTER|wxALL, 5 );
    label1 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label1"), 
                    wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label1, 0, wxALIGN_CENTER|wxALL, 5 );

    description2 = new wxStaticText( elementsPanel, ID_TEXT, 
                    wxT("Descriptor2    "), wxDefaultPosition, 
                    wxDefaultSize, 0 );
    item3->Add( description2, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl2 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                    wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl2, 0, wxALIGN_CENTER|wxALL, 5 );
    label2 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label2"), 
                    wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label2, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description3 = new wxStaticText( elementsPanel, ID_TEXT, 
                    wxT("Descriptor3    "), wxDefaultPosition, 
                    wxDefaultSize, 0 );
    item3->Add( description3, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl3 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl3, 0, wxALIGN_CENTER|wxALL, 5 );
    label3 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label3"), 
                   wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label3, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description4 = new wxStaticText( elementsPanel, ID_TEXT, 
                   wxT("Descriptor4    "), wxDefaultPosition, 
                   wxDefaultSize, 0 );
    item3->Add( description4, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl4 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl4, 0, wxALIGN_CENTER|wxALL, 5 );
    label4 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label4"), 
                   wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label4, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description5 = new wxStaticText( elementsPanel, ID_TEXT, 
                   wxT("Descriptor5    "), wxDefaultPosition, 
                   wxDefaultSize, 0 );
    item3->Add( description5, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl5 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl5, 0, wxALIGN_CENTER|wxALL, 5 );
    label5 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label5"), 
                   wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label5, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description6 = new wxStaticText( elementsPanel, ID_TEXT, 
                  wxT("Descriptor6    "), wxDefaultPosition, 
                  wxDefaultSize, 0 );
    item3->Add( description6, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl6 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""),
                  wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl6, 0, wxALIGN_CENTER|wxALL, 5 );
    label6 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label6"), 
                  wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item3, 0, wxGROW|wxALL|wxALIGN_CENTER, 5 );

    elementsPanel->SetAutoLayout( TRUE );
    elementsPanel->SetSizer( item0 );

    item0->Fit( elementsPanel );
    item0->SetSizeHints( elementsPanel );
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
   sp = (SpacePoint*) theGuiInterpreter->GetConfiguredItem(spName);
   
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
                              spName + "\" used for the coordinate system " +
                              cs->GetName() + " origin");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, spName);

   // Set the J2000Body for the coordinate system
   spName = cs->GetStringParameter("J2000Body");
   sp = (SpacePoint*) theGuiInterpreter->GetConfiguredItem(spName);
   
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         spName + "\" used for the coordinate system " +
         cs->GetName() + " J2000 body");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, spName);
   
   cs->Initialize();
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

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

   Integer bsize = 3; // border size

   //------------- create sizers -------------
   // sizer for orbit tab
//   wxBoxSizer *orbitSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *orbitSizer = new wxBoxSizer(wxHORIZONTAL);
      
   //flex grid sizer for the epoch format, coordinate system and state type
      wxFlexGridSizer *pageSizer = new wxFlexGridSizer(5, 2, bsize, bsize);
   
   // static box for the elements
   wxStaticBox *elementBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Elements"));
   wxStaticBoxSizer *elementSizer =
      new wxStaticBoxSizer(elementBox, wxVERTICAL);
   //-----------------------------------------

   //----------------- epoch -----------------
   // label for epoch
   wxStaticText *epochStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
   
   // combo box for the epoch format
   wxString strs12[] =
   {
      wxT("TAIModJulian"),
      wxT("UTCModJulian"),
      wxT("TAIGregorian"),
      wxT("UTCGregorian")
   };

   epochFormatComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 4, strs12, wxCB_DROPDOWN | wxCB_READONLY );

   // textfield for the epoch value
   epochValue = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0 );
   //-----------------------------------------

   //----------- coordinate system -----------
   // label for coordinate system
   wxStaticText *coordSysStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Coordinate System"), wxDefaultPosition, wxDefaultSize, 0 );

   //Get CordinateSystem ComboBox from the GuiItemManager.
   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   //-----------------------------------------

   //-------------- state type ---------------
   // label for state type
   wxStaticText *stateTypeStaticText = new wxStaticText( this, ID_TEXT,
      wxT("State Type"), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the state
   wxString strs15[] =
   {
      wxT("Cartesian"),
      wxT("Keplerian"),
      wxT("ModifiedKeplerian"),
      wxT("SphericalAZFPA"),
      wxT("SphericalRADEC")
//         wxT("Equinotical") not implemeted as of 6/01/05 LTR
   };

   stateTypeComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 5, strs15, wxCB_DROPDOWN | wxCB_READONLY);
   //-----------------------------------------

   //---------------- anomaly ----------------
   // label for anomaly type
   anomalyStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Anomaly Type "), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the anomaly type
   wxString typeList[] =
   {
      wxT("Mean Anomaly"),
      wxT("True Anomaly"),
      wxT("Eccentric Anomaly")
   };

   anomalyComboBox = new wxComboBox( this, ID_COMBOBOX, wxT(""),
      wxDefaultPosition, wxSize(125,-1), 3, typeList, wxCB_DROPDOWN | wxCB_READONLY );
   //-----------------------------------------
   
   // add to page sizer
   pageSizer->Add( epochStaticText, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( 20, 20, 0, wxALIGN_LEFT | wxALL, bsize );
   pageSizer->Add( epochFormatComboBox, 0, wxALIGN_LEFT | wxALL, bsize );
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

   theSpacecraft->SetDisplay(true);

   this->SetSizer( orbitSizer );
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
   #if DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage("In OrbitPanel::LoadData() \n");
   #endif

   // load data from the core engine
   try
   {
      Rvector6 outState, displayState;

      // load the epoch format
      std::string epochFormat = theSpacecraft->GetDisplayDateFormat();
      epochFormatComboBox->SetValue(wxT(epochFormat.c_str()));
      fromEpochFormat = epochFormat.c_str();
   
      // load the epoch
      std::string epochStr = theSpacecraft->GetDisplayEpoch();
      epochValue->SetValue(epochStr.c_str());
   
      // load the coordiante system
      std::string coordSystem = theSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      mCoordSysComboBox->SetValue(coordSystem.c_str());
      fromCoordSys = coordSystem.c_str();
   
      // load the state type  
      std::string stType = theSpacecraft->GetDisplayCoordType();
      stateTypeComboBox->SetValue(wxT(stType.c_str()));
      fromStateType = stType.c_str();

      // load orbital elements  
      PropState ps = theSpacecraft->GetState(); // should be cartesian state
      for (int i=0; i<6; i++)
      {
         mCartState[i] = ps[i];
      }

      if (strcmp(coordSystem.c_str(), "EarthMJ2000Eq") == 0)
      {
         if (strcmp(stType.c_str(), "Cartesian") == 0)
         {
            displayState = mCartState;
         }
         else
         {
            displayState = stateConverter.Convert(mCartState, "Cartesian",
                                                  stType, anomaly);
         }        
      }
      else
      {
         Real epoch = theSpacecraft->GetRealParameter("Epoch");
         
         // Earth Mean J2000 Equatorial Coordinate System
         CoordinateSystem* inCoord = (CoordinateSystem*) 
            theGuiInterpreter->GetCoordinateSystem("EarthMJ2000Eq");

         // output coordinate system
         CoordinateSystem* outCoord = (CoordinateSystem*) 
            theGuiInterpreter->GetCoordinateSystem(coordSystem.c_str());

         // convert to output coordinate system
         mCoordConverter.Convert(A1Mjd(epoch), mCartState, inCoord, outState,
                                 outCoord);

         // if the state type is Cartesian, then display state equals Cartesian
         // else convert to the display state
         if (strcmp(stType.c_str(), "Cartesian") == 0)
         {
            displayState = outState;
         }
         else
         {
            // DJC added, 9/22/05
            stateConverter.SetMu(outCoord);
            displayState = stateConverter.Convert(outState, "Cartesian",
                                               stType, anomaly);
         }
      }

      // place values into text fields
      wxString element;
      std::stringstream buffer;
      buffer.precision(18);
   
      buffer << displayState[0];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl1->SetValue (element);
      buffer.str(std::string()); 

      buffer << displayState[1];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl2->SetValue (element);
      buffer.str(std::string()); 

      buffer << displayState[2];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl3->SetValue (element);
      buffer.str(std::string()); 
   
      buffer << displayState[3];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl4->SetValue (element);
      buffer.str(std::string()); 

      buffer << displayState[4];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl5->SetValue (element);
      buffer.str(std::string()); 

      buffer << displayState[5];
      element.Printf ("%s",buffer.str().c_str());
      textCtrl6->SetValue (element);
      buffer.str(std::string()); 

      // labels for elements, anomaly and units
      SetLabelsUnits(stType);     

      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage( "epoch format      = %s\n",epochFormat.c_str() );
         MessageInterface::ShowMessage( "epoch             = %s\n",epochStr.c_str() );
         MessageInterface::ShowMessage( "coordinate system = %s\n",coordSystem.c_str() );
         MessageInterface::ShowMessage( "state type        = %s\n",stType.c_str() );
         MessageInterface::ShowMessage( "mCartState        = %s\n",mCartState.ToString().c_str() );
         MessageInterface::ShowMessage( "anomaly type      = %s\n",anomaly.GetType().c_str() );
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("OrbitPanel:LoadData() error occurred!\n%s\n", e.GetMessage().c_str());
   }
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
   #if DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage( "In OrbitPanel::SaveData() \n" );
   #endif
   
   Rvector6 outState, cartState, displayState;
   Real propagateState[6];
   
   // save epoch format
   wxString epochFormatStr = epochFormatComboBox->GetStringSelection();
   theSpacecraft->SetDisplayDateFormat(epochFormatStr.c_str());

   // save epoch
   wxString epochStr = epochValue->GetValue();
   theSpacecraft->SetDisplayEpoch(epochStr.c_str());

   // save coordinate system
   wxString coordSystemStr = mCoordSysComboBox->GetStringSelection();
   theSpacecraft->SetRefObjectName(Gmat::COORDINATE_SYSTEM, coordSystemStr.c_str());
    
   // save state type
   wxString stateTypeStr = stateTypeComboBox->GetStringSelection();
   theSpacecraft->SetDisplayCoordType(std::string (stateTypeStr.c_str()));
   /// @todo: need to correct in spacecraft code because this should only save display state type

   // save orbital elements    
   wxString el1 = textCtrl1->GetValue();
   wxString el2 = textCtrl2->GetValue();
   wxString el3 = textCtrl3->GetValue();
   wxString el4 = textCtrl4->GetValue();
   wxString el5 = textCtrl5->GetValue();
   wxString el6 = textCtrl6->GetValue(); 
    
   displayState[0] = atof(el1);
   displayState[1] = atof(el2);
   displayState[2] = atof(el3);
   displayState[3] = atof(el4);
   displayState[4] = atof(el5);
   displayState[5] = atof(el6);
    
   // Check to make sure that the Keplerian values 
   // are acceptable for the spacecraft
   if (strcmp(stateTypeStr.c_str(), "Keplerian") == 0)
   {
      if(displayState[1] < 0.0)
      {
         displayState[1] *= -1.0;
         MessageInterface::PopupMessage
         (Gmat::WARNING_, "ECC < 0, so multipled ECC by -1\n");
      }
      else if((displayState[0] > 0.0) && (displayState[1] > 1.0))
      {
         displayState[0] *= -1.0;
         MessageInterface::PopupMessage
         (Gmat::WARNING_, "SMA > 0 and ECC > 1, so multipled SMA by -1\n");
      }
      else if((displayState[0] < 0.0) && (displayState[1] < 1.0))
      {
         displayState[0] *= -1.0;
         MessageInterface::PopupMessage
         (Gmat::WARNING_, "SMA < 0 and ECC < 1, so multipled SMA by -1\n");
      }          
   }    

   // Check to see if state needs to be converted to Cartesian before save
   if (mIsTextChanged)
   {
      mIsTextChanged = false; //loj: added
      
      #if DEBUG_ORBIT_PANEL
         MessageInterface::ShowMessage( "text changed\n" );
      #endif

      // Make sure the saved state is cartesian
      if (strcmp(stateTypeStr.c_str(), "Cartesian") == 0)
      {
         // already cartesian state
         cartState = displayState;
      }
      else
      {
         // convert to cartesian state
         cartState = stateConverter.Convert(displayState,stateTypeStr.c_str(),
                                                 "Cartesian", anomaly);
      }
      
      // Make sure the saved state is earth mean J2000 equatorial
      if (strcmp(coordSystemStr.c_str(), "EarthMJ2000Eq") == 0)
      {
         // already earth mean J2000 equatorial
         outState = cartState;
      }         
      else
      {
         Real epoch = theSpacecraft->GetRealParameter("Epoch");

         // input Coordinate System
         CoordinateSystem* inCoord = (CoordinateSystem*) 
            theGuiInterpreter->GetCoordinateSystem(coordSystemStr.c_str());

         // Earth Mean J2000 Equatorial Coordinate System
         CoordinateSystem* outCoord = (CoordinateSystem*) 
            theGuiInterpreter->GetCoordinateSystem("EarthMJ2000Eq");

         // convert to Earth Mean J2000 Equatorial
         mCoordConverter.Convert(A1Mjd(epoch), cartState, inCoord, outState,
                                 outCoord);
      }         
      
      // save outState
      mCartState = outState;       
      MessageInterface::ShowMessage("===> SaveData() after convert mCartState=%s\n",
                                    mCartState.ToString().c_str());
      for (int i=0; i<6; i++)
      {
         propagateState[i] = outState[i];
      }
      
      theSpacecraft->SetState("Cartesian", propagateState);
      //theSpacecraft->SetDisplayState(displayState);
      theSpacecraft->SetDisplayState(mCartState); //loj: need to set mCartState here
   }
   else
   {
      outState = mCartState;
   }
       
//   // theSpacecraft->SetEpoch();   *** Joey commented it out for now ***
//   // theSpacecraft->SetState(stateTypeStr.c_str(), propagateState);
//   // theSpacecraft->SaveDisplay();

   #if DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage( "epoch format      = %s\n",epochFormatStr.c_str() );
      MessageInterface::ShowMessage( "epoch             = %s\n",epochStr.c_str() );
      MessageInterface::ShowMessage( "coordinate system = %s\n",coordSystemStr.c_str() );
      MessageInterface::ShowMessage( "state type        = %s\n",stateTypeStr.c_str() );
      MessageInterface::ShowMessage( "anomaly type      = %s\n",anomaly.GetType().c_str() );
      MessageInterface::ShowMessage( "mCartState        = %s\n",mCartState.ToString().c_str() );
      for (int i=0; i<6; i++)
      {
         MessageInterface::ShowMessage( "propagateState[%d] = %.16f\n", i, propagateState[i] );
      }
   #endif
}

