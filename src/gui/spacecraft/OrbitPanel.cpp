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

// #define DEBUG_ORBIT_PANEL 1

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(OrbitPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, OrbitPanel::OnTextChange)
   EVT_COMBOBOX(ID_CB_STATE, OrbitPanel::OnStateChange)
   EVT_COMBOBOX(ID_CB_EPOCH, OrbitPanel::OnEpochChange)
   EVT_COMBOBOX(ID_COMBOBOX, OrbitPanel::OnComboBoxChange)
   EVT_COMBOBOX(ID_CB_ANOMALY, OrbitPanel::OnAnomalyChange)
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

//   MessageInterface::ShowMessage
//         ("In OrbitPanel\n");

   theGuiManager = GuiItemManager::GetInstance();
   this->theSpacecraft = spacecraft;
   this->theSolarSystem = solarsystem;
   this->theApplyButton = theApplyButton;

   mIsCoordSysChanged = false;
   Create();
}

OrbitPanel::~OrbitPanel()
{
//    delete(theSpacecraft);
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
/**
 *
 * @note Creates the page for orbit information
 */
//------------------------------------------------------------------------------
void OrbitPanel::Create()
{
   Integer bsize = 3; // border size

   //------------- create sizers -------------
   // sizer for orbit tab
   wxBoxSizer *orbitSizer = new wxBoxSizer(wxVERTICAL);
   //static box for the state
   wxStaticBox *item9 = new wxStaticBox( this, ID_STATIC_ORBIT,
      wxT("State") );
   wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item9, wxVERTICAL );
   // gridsizer for inside the orbit state static box
   wxGridSizer *item10 = new wxGridSizer( 2, 0, 0 );
   // static box for the elements
   wxStaticBox *elementBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Elements"));
   wxStaticBoxSizer *elementSizer =
      new wxStaticBoxSizer(elementBox, wxVERTICAL);
   // sizer for anomaly
   wxBoxSizer *anomalySizer = new wxBoxSizer(wxHORIZONTAL);
   
   // label for epoch
   wxStaticText *item11 = new wxStaticText( this, ID_TEXT,
      wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
   
   // add epcoh label and blank to ?? sizer                         
   item10->Add( item11, 0, wxALIGN_CENTER, bsize );
   item10->Add( 20, 20, 0, wxALIGN_CENTER, bsize );

   wxString strs12[] =
   {
      wxT("TAIModJulian"),
      wxT("UTCModJulian"),
      wxT("TAIGregorian"),
      wxT("UTCGregorian")
   };

   // combo box for the date type
   dateComboBox = new wxComboBox( this, ID_CB_EPOCH, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 4, strs12, wxCB_DROPDOWN | wxCB_READONLY );
   item10->Add( dateComboBox, 0, wxALIGN_CENTER, bsize );

   // textfield for the epochvalue
   epochValue = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 0 );

   item10->Add( epochValue, 0, wxALIGN_CENTER, bsize );

   wxStaticText *coordSysStaticText = new wxStaticText( this, ID_TEXT,
      wxT("Coordinate System"), wxDefaultPosition, wxDefaultSize, 0 );
   item10->Add( coordSysStaticText, 0, wxALIGN_CENTER, bsize );

   wxStaticText *item14 = new wxStaticText( this, ID_TEXT,
      wxT("State Type"), wxDefaultPosition, wxDefaultSize, 0 );
   item10->Add( item14, 0, wxALIGN_CENTER, bsize );

   mCoordSysComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(120,-1));

   item10->Add(mCoordSysComboBox, 0, wxALIGN_CENTER|wxALL, bsize);

   wxString strs15[] =
   {
      wxT("Cartesian"),
      wxT("Keplerian"),
      wxT("ModifiedKeplerian"),
      wxT("SphericalAZFPA"),
      wxT("SphericalRADEC")
//         wxT("Equinotical")
   };

   // combo box for the state
   stateComboBox = new wxComboBox( this, ID_CB_STATE, wxT(""),
      wxDefaultPosition, wxSize(150,-1), 5, strs15, wxCB_DROPDOWN | wxCB_READONLY);

   item10->Add( stateComboBox, 0, wxALIGN_CENTER, bsize );
   item8->Add( item10, 0, wxALIGN_CENTER, bsize );

   // panel that has the labels and text fields for the elements
   // adds default descriptors/labels
   AddElements(this);
   elementSizer->Add(elementsPanel, 0, wxALIGN_CENTER, bsize);

   item8->Add( elementSizer, 0, wxALIGN_CENTER, bsize );

   // label for anomaly type
   anomalyLabel = new wxStaticText( this, ID_TEXT,
      wxT("Anomaly Type "), wxDefaultPosition, wxDefaultSize, 0 );

   // combo box for the anomaly type
   wxString typeList[] =
   {
      wxT("Mean Anomaly"),
      wxT("True Anomaly"),
      wxT("Eccentric Anomaly")
   };

   anomalyCB = new wxComboBox( this, ID_CB_ANOMALY, wxT(""),
      wxDefaultPosition, wxSize(125,-1), 3, typeList, wxCB_DROPDOWN | wxCB_READONLY );

   anomalyLabel->Show(false);
   anomalyCB->Show(false);
   
   anomalySizer->Add(anomalyLabel, 0, wxALIGN_CENTER, bsize );
   anomalySizer->Add(anomalyCB, 0, wxALIGN_CENTER, bsize );

   item8->Add(anomalySizer, 0, wxALIGN_CENTER, bsize );

   orbitSizer->Add( item8, 1, wxGROW|wxALIGN_CENTER, bsize );

   theSpacecraft->SetDisplay(true);

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
// void LoadData()
//------------------------------------------------------------------------------
/**
 * @note Gets the values from theSpacecraft and puts them in the text fields
 */
//------------------------------------------------------------------------------
void OrbitPanel::LoadData()
{
   // load data from the core engine
   
   // CoordinateSystem (loj: 5/13/05 Used actual CoordinateSystem
   mCoordSysComboBox->SetStringSelection
      (theSpacecraft->GetStringParameter("CoordinateSystem").c_str());
   
   // State type
   std::string refFrame = theSpacecraft->GetDisplayCoordType();
   stateComboBox->SetValue(wxT(refFrame.c_str()));
   // for the labels
   //OnStateChange();  
  
   wxString description;
    
   int stateId = theSpacecraft->GetParameterID("Element1");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description1->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element2");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description2->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element3");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description3->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element4");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description4->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element5");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description5->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element6");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description6->SetLabel(description);
    
   #if DEBUG_ORBIT_PANEL
   MessageInterface::ShowMessage("OrbitPanel:LoadData() description =%s\n",
      description.c_str());
   #endif

   // hard code label change for now, should actually
   // come from the spacecraft object
   if (refFrame == "Cartesian")
   {
      label1->SetLabel("Km");
      label2->SetLabel("Km");
      label3->SetLabel("Km");
      label4->SetLabel("Km/s");
      label5->SetLabel("Km/s");
      label6->SetLabel("Km/s");
      
      anomalyLabel->Show(false);
      anomalyCB->Show(false);
   }
   else if ((refFrame == "Keplerian") || (refFrame == "ModifiedKeplerian"))
   {
      label1->SetLabel("Km");
      if (refFrame == "Keplerian")
         label2->SetLabel("");
      else
         label2->SetLabel("Km");
      label3->SetLabel("deg");
      label4->SetLabel("deg");
      label5->SetLabel("deg");
      label6->SetLabel("deg");
      
      anomalyLabel->Show(true);
      anomalyCB->Show(true);
      
      if (strcmp(description.c_str(), "MA") == 0) 
         anomalyCB->SetSelection(0);
      else if (strcmp(description.c_str(), "TA") == 0) 
         anomalyCB->SetSelection(1);
      else if (strcmp(description.c_str(), "EA") == 0)
         anomalyCB->SetSelection(2);
   }
   else if ((refFrame == "SphericalAZFPA") || (refFrame == "SphericalRADEC"))
   {
      label1->SetLabel("Km");
      label2->SetLabel("deg");
      label3->SetLabel("deg");
      label4->SetLabel("Km/s");
      label5->SetLabel("deg");
      label6->SetLabel("deg");
      
      anomalyLabel->Show(false);
      anomalyCB->Show(false);
   }

    //loj: get element type first
    //loj: if element type is Cartesian, the combobox should show Cartesian
    //loj: if Keplerian, the combobox should show Keplerian, etc
    //loj: copy actual element type and elements to diaplay member data.
    //loj: when combobox changes, use display data to convert and display
    //loj: do not readback from the elements field unless user enters the new value
    
    // get elements
   std::string epochStr = theSpacecraft->GetDisplayEpoch();
//    MessageInterface::ShowMessage("\nLoaded epoch as %s", epochStr.c_str());

   std::string dateFormat = theSpacecraft->GetDisplayDateFormat();
   dateComboBox->SetValue(wxT(dateFormat.c_str()));

   Real *displayState = theSpacecraft->GetDisplayState();
   Real element1 = displayState[0];
   Real element2 = displayState[1];
   Real element3 = displayState[2];
   Real element4 = displayState[3];
   Real element5 = displayState[4];
   Real element6 = displayState[5];

   epochValue->SetValue(epochStr.c_str());
     
   wxString el1;
   el1.Printf("%.9f", element1);
   textCtrl1->SetValue(el1);
    
   wxString el2;
   el2.Printf("%.9f", element2);
   textCtrl2->SetValue(el2);

   wxString el3;
   el3.Printf("%.9f", element3);
   textCtrl3->SetValue(el3);

   wxString el4;
   el4.Printf("%.9f", element4);
   textCtrl4->SetValue(el4);

   wxString el5;
   el5.Printf("%.9f", element5);
   textCtrl5->SetValue(el5);

   wxString el6;
   el6.Printf("%.9f", element6);
   textCtrl6->SetValue(el6);

   //loj: 5/13/05 disable it for now
   mCoordSysComboBox->Disable();

}

//------------------------------------------------------------------------------
// void OnComboChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mCoordSysComboBox)
   {
      mIsCoordSysChanged = true;
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnTextChange(wxCommandEvent &event)
{
    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnStateChange()
//------------------------------------------------------------------------------
/**
 * @note Changes the element descriptors and labels based on the state combo box
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnStateChange(wxCommandEvent &event)
{
   wxString el1 = textCtrl1->GetValue();
   wxString el2 = textCtrl2->GetValue();
   wxString el3 = textCtrl3->GetValue();
   wxString el4 = textCtrl4->GetValue();
   wxString el5 = textCtrl5->GetValue();
   wxString el6 = textCtrl6->GetValue(); 

   Real displayState[6];
   displayState[0] = atof(el1);
   displayState[1] = atof(el2);
   displayState[2] = atof(el3);
   displayState[3] = atof(el4);
   displayState[4] = atof(el5);
   displayState[5] = atof(el6);
    
   theSpacecraft->SetDisplayState(displayState);
    
   wxString refFrame = stateComboBox->GetStringSelection();
//    theSpacecraft->ConvertRepresentation(refFrame.c_str());
   theSpacecraft->SetDisplayCoordType(refFrame.c_str());

   wxString description;
    
   int stateId = theSpacecraft->GetParameterID("Element1");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description1->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element2");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description2->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element3");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description3->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element4");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description4->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element5");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description5->SetLabel(description);

   stateId = theSpacecraft->GetParameterID("Element6");
   description.Printf("%s", theSpacecraft->GetParameterText(stateId).c_str());
   description6->SetLabel(description);
   
   #if DEBUG_ORBIT_PANEL
   MessageInterface::ShowMessage("OrbitPanel:OnStateChange() description =%s\n",
      description.c_str());
   #endif

   Real *returnState = theSpacecraft->GetDisplayState();

   wxString stateValue;

   stateValue.Printf("%.9f", returnState[0]);

   textCtrl1->SetValue(stateValue);

   stateValue.Printf("%.9f", returnState[1]);
   textCtrl2->SetValue(stateValue);

   stateValue.Printf("%.9f", returnState[2]);
   textCtrl3->SetValue(stateValue);

   stateValue.Printf("%.9f", returnState[3]);
   textCtrl4->SetValue(stateValue);

   stateValue.Printf("%.9f", returnState[4]);
   textCtrl5->SetValue(stateValue);

   stateValue.Printf("%.9f", returnState[5]);
   textCtrl6->SetValue(stateValue);


   // hard code label change for now, should actually
   // come from the spacecraft object
   if (refFrame == "Cartesian")
   {
      label1->SetLabel("Km");
      label2->SetLabel("Km");
      label3->SetLabel("Km");
      label4->SetLabel("Km/s");
      label5->SetLabel("Km/s");
      label6->SetLabel("Km/s");
      
      anomalyLabel->Show(false);
      anomalyCB->Show(false);
   }
   else if ((refFrame == "Keplerian") || (refFrame == "ModifiedKeplerian"))
   {
      label1->SetLabel("Km");
      if (refFrame == "Keplerian")
         label2->SetLabel("");
      else
         label2->SetLabel("Km");
      label3->SetLabel("deg");
      label4->SetLabel("deg");
      label5->SetLabel("deg");
      label6->SetLabel("deg");
      
      anomalyLabel->Show(true);
      anomalyCB->Show(true);
      
      if (strcmp(description.c_str(), "MA") == 0) 
         anomalyCB->SetSelection(0);
      else if (strcmp(description.c_str(), "TA") == 0) 
         anomalyCB->SetSelection(1);
      else if (strcmp(description.c_str(), "EA") == 0)
         anomalyCB->SetSelection(2);
   }
   else if ((refFrame == "SphericalAZFPA") || (refFrame == "SphericalRADEC"))
   {
      label1->SetLabel("Km");
      label2->SetLabel("deg");
      label3->SetLabel("deg");
      label4->SetLabel("Km/s");
      label5->SetLabel("deg");
      label6->SetLabel("deg");
      
      anomalyLabel->Show(false);
      anomalyCB->Show(false);
   }

   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnEpochChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when epoch combobox is changed
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnEpochChange(wxCommandEvent &event)
{
    SaveData();   
//    MessageInterface::ShowMessage("Inside epochchange()\n");
    theApplyButton->Enable();
 
//    Real epoch1 = theSpacecraft->GetDisplayEpoch(); 
//    MessageInterface::ShowMessage("epoch value %f\n", epoch1);

    wxString dateFormat = dateComboBox->GetStringSelection();
    
//    MessageInterface::ShowMessage("date format is %s\n", dateFormat.c_str());
//    theSpacecraft->SetDisplayDateFormat("UTC");
    
    theSpacecraft->SetDisplayDateFormat(dateFormat.c_str());

    // get elements
//    Real epoch = theSpacecraft->GetDisplayEpoch();  
//    wxString epochStr;
//    epochStr.Printf("%18.9f", epoch);
//    epochValue->SetValue(epochStr);

   std::string epochStr = theSpacecraft->GetDisplayEpoch();
//   MessageInterface::ShowMessage("\nnew value of epoch is %s\n", epochStr.c_str());
   epochValue->SetValue(epochStr.c_str());
   theSpacecraft->SetDisplayEpoch(epochStr.c_str());
}

//------------------------------------------------------------------------------
// void OnAnomalyChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when anomaly combobox is changed
 */
//------------------------------------------------------------------------------
void OrbitPanel::OnAnomalyChange(wxCommandEvent &event)
{
   std::string anomalyType;

   wxString description, stateValue;
   
   int anomalySelected = anomalyCB->GetSelection();

   if (anomalySelected == 0) 
      anomalyType = "MA";
   else if (anomalySelected == 1) 
      anomalyType = "TA";
   else if (anomalySelected == 2)
      anomalyType = "EA";
         
   int anomalyID = theSpacecraft->GetParameterID("AnomalyType");

   #if DEBUG_ORBIT_PANEL
   MessageInterface::ShowMessage("OrbitPanel:OnAnomalyChange() anomaly selected = %d\n",
      anomalySelected);
   MessageInterface::ShowMessage("OrbitPanel:OnAnomalyChange() anomaly ID =%d\n",
      anomalyID);
   MessageInterface::ShowMessage("OrbitPanel:OnAnomalyChange() anomaly type = %s\n",
      anomalyType.c_str());
   #endif

   description.Printf("%s", anomalyType.c_str());
   description6->SetLabel(description);
   
   theSpacecraft->SetStringParameter(anomalyID, anomalyType);
   
   Real anomaly = theSpacecraft->GetRealParameter(anomalyType);
   stateValue.Printf("%.9f", anomaly);
   textCtrl6->SetValue(stateValue);    

   theApplyButton->Enable();
}


void OrbitPanel::SaveData()
{
    // save state type
    wxString stateStr = stateComboBox->GetStringSelection();
    
//    int refBodyId = theSpacecraft->GetParameterID("ReferenceBody");
//    wxString refBodyStr = referenceBodyComboBox->GetStringSelection();
//    theSpacecraft->SetStringParameter(refBodyId,
//                   std::string(refBodyStr.c_str()));
    
    // refFrame id = 8
//    theSpacecraft->SetStringParameter(8, std::string (stateStr.c_str()));
    wxString epochStr = epochValue->GetValue();
//    MessageInterface::ShowMessage("Going to save epoch as %s", epochStr.c_str());
//    theSpacecraft->SetRealParameter(0, atof(epochStr));
    theSpacecraft->SetDisplayEpoch(epochStr.c_str());

    
    wxString dateFormatStr = dateComboBox->GetStringSelection();
//    theSpacecraft->SetStringParameter(11, std::string (dateFormatStr.c_str()));
    theSpacecraft->SetDisplayDateFormat(dateFormatStr.c_str());

//    MessageInterface::ShowMessage("theSpacecraft->GetDisplayEpoch: %s", 
//                                  theSpacecraft->GetDisplayEpoch().c_str());

    wxString el1 = textCtrl1->GetValue();
    wxString el2 = textCtrl2->GetValue();
    wxString el3 = textCtrl3->GetValue();
    wxString el4 = textCtrl4->GetValue();
    wxString el5 = textCtrl5->GetValue();
    wxString el6 = textCtrl6->GetValue(); 
    

//    theSpacecraft->SetRealParameter(1, atof(el1));
//    theSpacecraft->SetRealParameter(2, (double)atof(el2));
//    theSpacecraft->SetRealParameter(3, atof(el3));
//    theSpacecraft->SetRealParameter(4, atof(el4));
//    theSpacecraft->SetRealParameter(5, atof(el5));
//    theSpacecraft->SetRealParameter(6, atof(el6));
    Real displayState[6];
    displayState[0] = atof(el1);
    displayState[1] = atof(el2);
    displayState[2] = atof(el3);
    displayState[3] = atof(el4);
    displayState[4] = atof(el5);
    displayState[5] = atof(el6);
    
   // Check to make sure that the Keplerian values 
   // are acceptable for the spacecraft
   if (stateStr.IsSameAs("Keplerian"))
   {
      if(displayState[1] < 0.0)
      {
         displayState[1] *= -1.0;
      }
      else if((displayState[0] > 0.0) && (displayState[1] > 1.0))
      {
         displayState[0] *= -1.0;
      }
      else if((displayState[0] < 0.0) && (displayState[1] < 1.0))
      {
         displayState[0] *= -1.0;
      }          
   }    
    
    theSpacecraft->SetDisplayCoordType(std::string (stateStr.c_str()));
    theSpacecraft->SetDisplayState(displayState);
    theSpacecraft->SaveDisplay();
}

