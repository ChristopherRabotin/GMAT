//$Header$
//------------------------------------------------------------------------------
//                            SpacecraftPanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
// Modified: 2003/12/15 by Allison Greene for event handling
/**
 * This class contains information needed to setup users spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------
#include "SpacecraftPanel.hpp"
#include "GmatAppData.hpp"

#include "gmatdefs.hpp" //put this one after GUI includes
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"
#include "RealUtilities.hpp"

#include <stdlib.h>

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(SpacecraftPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, SpacecraftPanel::OnTextChange)
   EVT_COMBOBOX(ID_CB_STATE, SpacecraftPanel::OnStateChange)
   EVT_COMBOBOX(ID_CB_EPOCH, SpacecraftPanel::OnEpochChange)
   EVT_BUTTON(ID_BUTTON_OK, SpacecraftPanel::OnOk) 
   EVT_BUTTON(ID_BUTTON_APPLY, SpacecraftPanel::OnApply) 
   EVT_BUTTON(ID_BUTTON_CANCEL, SpacecraftPanel::OnCancel) 
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// SpacecraftPanel(wxWindow *parent, const wxString &scName)
//------------------------------------------------------------------------------
/**
 * Constructs SpacecraftPanel object.
 *
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the spacecraft GUI
 */
//------------------------------------------------------------------------------
SpacecraftPanel::SpacecraftPanel(wxWindow *parent, const wxString &scName)
    :wxPanel(parent)
{
    CreateNotebook(this, scName);
}

//-------------------------------
// private methods
//-------------------------------
//------------------------------------------------------------------------------
// void CreateNotebook(wxWindow *parent, const wxString &scName)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::CreateNotebook(wxWindow *parent, const wxString &scName)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();

    theSpacecraft = theGuiInterpreter->GetSpacecraft(std::string(scName.c_str()));
    
    if (theSpacecraft != NULL)
    {
        // use a grid sizer so that it expands both horizonatally and
        // vertically
        wxGridSizer *item = new wxGridSizer( 1, 0, 0 );

        mainNotebook = new wxNotebook( parent, ID_NOTEBOOK, wxDefaultPosition, wxSize(350,300), 0 );
        sizer = new wxNotebookSizer( mainNotebook );
 
        // set orbitPanel to null   
        orbitPanel = (wxPanel *)NULL;
        CreateOrbit( mainNotebook );
        mainNotebook->AddPage( orbitPanel, wxT("Orbit") );

        attitude = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( attitude, wxT("Attitude") );

        properties = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( properties, wxT("Ballistic/Mass") );

        actuators = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( actuators, wxT("Actuators") );

        sensors = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( sensors, wxT("Sensors") );

        tanks = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( tanks, wxT("Tanks") );

        visual = new wxPanel( mainNotebook, -1 );
        mainNotebook->AddPage( visual, wxT("Visualization") );

        item->Add( sizer, 0, wxGROW|wxALL, 5 );
        
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item );
        item->Fit( parent );
        item->SetSizeHints( parent );
    }
    else
    {
        // show error message
    }
}

//------------------------------------------------------------------------------
// void CreateOrbit(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the panel for the Orbit notebook page
 * @note Use orbitPanel as the parent of all the objects
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::CreateOrbit(wxWindow *parent)
{
    orbitPanel = new wxPanel(parent);

    wxFlexGridSizer *orbitSizer = new wxFlexGridSizer(1, 0, 0 );
    orbitSizer->AddGrowableCol( 0 );
    orbitSizer->AddGrowableCol( 1 );
    orbitSizer->AddGrowableRow( 0 );
    orbitSizer->AddGrowableRow( 1 );

    // static box for the Coordinate System
    wxStaticBox *item2 = new wxStaticBox( orbitPanel, ID_STATIC_COORD, 
                         wxT("Coordinate System") );
    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

    // gridsizer for inside the Coordinate System static box
    wxGridSizer *item3 = new wxGridSizer( 2, 0, 0 );

    wxStaticText *item4 = new wxStaticText( orbitPanel, ID_TEXT, 
                          wxT("Reference Body"), wxDefaultPosition, 
                          wxDefaultSize, 0 );
    item4->Disable();
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( orbitPanel, ID_TEXT, wxT("Reference Frame"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Disable();
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs6[] =
    {
        wxT("ComboItem")
    };
    wxComboBox *item6 = new wxComboBox( orbitPanel, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs6, wxCB_DROPDOWN );
    item6->Disable();
    item3->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs7[] =
    {
        wxT("ComboItem")
    };
    wxComboBox *item7 = new wxComboBox( orbitPanel, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs7, wxCB_DROPDOWN );
    item7->Disable();
    item3->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    //static box for the orbit state
    wxStaticBox *item9 = new wxStaticBox( orbitPanel, ID_STATIC_ORBIT, 
                         wxT("Orbit State") );
    wxStaticBoxSizer *item8 = new wxStaticBoxSizer( item9, wxVERTICAL );

    // gridsizer for inside the orbit state static box
    wxGridSizer *item10 = new wxGridSizer( 2, 0, 0 );

    wxStaticText *item11 = new wxStaticText( orbitPanel, ID_TEXT, wxT("Epoch"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item10->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs12[] =
    {
        wxT("UTC1"),
        wxT("Julian Date"),
        wxT("TAI Julian"),
        wxT("Gregorian")
    };
    
    // combo box for the date type
    dateCB = new wxComboBox( orbitPanel, ID_CB_EPOCH, wxT(""), 
             wxDefaultPosition, wxSize(100,-1), 4, strs12, 
             wxCB_DROPDOWN );
    item10->Add( dateCB, 0, wxALIGN_CENTER|wxALL, 5 );

    // textfield for the epochvalue
    epochValue = new wxTextCtrl( orbitPanel, ID_TEXTCTRL, wxT(""), 
                 wxDefaultPosition, wxSize(150,-1), 0 );
   
    item10->Add( epochValue, 0, wxALIGN_CENTER|wxALL, 5 );
    item10->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item14 = new wxStaticText( orbitPanel, ID_TEXT, wxT("State Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item10->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs15[] =
    {
        wxT("Cartesian"),
        wxT("Keplerian"),
        wxT("Modified Keplerian"),
        wxT("Spherical"),
        wxT("Equinotical")
    };
    
    // combo box for the state
    stateCB = new wxComboBox( orbitPanel, ID_CB_STATE, wxT(""), 
              wxDefaultPosition, wxSize(100,-1), 5, strs15, wxCB_DROPDOWN );
    item10->Add( stateCB, 0, wxALIGN_CENTER|wxALL, 5 );

    item8->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    // static box for the elements
    elementBox = new wxStaticBox(orbitPanel, ID_STATIC_ELEMENT, wxT("Elements"));
    elementSizer = new wxStaticBoxSizer(elementBox, wxVERTICAL);

    // panel that has the labels and text fields for the elements
    // set it to null
    elementsPanel = (wxPanel *)NULL; 
    // adds default descriptors/labels 
    AddElements(orbitPanel);
    elementSizer->Add(elementsPanel, 0, wxGROW|wxALIGN_CENTER|wxALL, 5);

    item8->Add( elementSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    // adds the buttons
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    okButton = new wxButton(orbitPanel, ID_BUTTON_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);
    applyButton = new wxButton(orbitPanel, ID_BUTTON_APPLY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
    cancelButton = new wxButton(orbitPanel, ID_BUTTON_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
    helpButton = new wxButton(orbitPanel, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);

    buttonSizer->Add(okButton, 0, wxALL|wxALIGN_CENTER, 5);
    buttonSizer->Add(applyButton, 0, wxALL|wxALIGN_CENTER, 5);
    buttonSizer->Add(cancelButton, 0, wxALL|wxALIGN_CENTER, 5);
    buttonSizer->Add(helpButton, 0, wxALL|wxALIGN_CENTER, 5);

    orbitSizer->Add( item1, 0, wxGROW|wxALL, 5 );
    orbitSizer->Add( item8, 0, wxGROW|wxALL, 5 );
    orbitSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    orbitPanel->SetAutoLayout( TRUE );
    orbitPanel->SetSizer( orbitSizer );
    orbitSizer->Fit( orbitPanel );
    orbitSizer->SetSizeHints( orbitPanel );
    
    // gets the values from theSpacecraft
    UpdateValues();
    //disable apply button until a field is changed
    applyButton->Disable();
    helpButton->Disable();
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
void SpacecraftPanel::AddElements(wxWindow *parent)
{
    elementsPanel = new wxPanel(parent);
    wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 6, 3, 0, 0 );
    item3->AddGrowableCol( 0 );
    item3->AddGrowableCol( 1 );
    item3->AddGrowableCol( 2 );

    description1 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Descriptor1     "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description1, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl1 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl1, 0, wxALIGN_CENTER|wxALL, 5 );
    label1 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label1"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label1, 0, wxALIGN_CENTER|wxALL, 5 );

    description2 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Descriptor2    "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description2, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl2 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl2, 0, wxALIGN_CENTER|wxALL, 5 );
    label2 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label2"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label2, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description3 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Descriptor3    "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description3, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl3 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl3, 0, wxALIGN_CENTER|wxALL, 5 );
    label3 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label3"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label3, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description4 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Descriptor4    "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description4, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl4 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl4, 0, wxALIGN_CENTER|wxALL, 5 );
    label4 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label4"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label4, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description5 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Descriptor5    "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description5, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl5 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl5, 0, wxALIGN_CENTER|wxALL, 5 );
    label5 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label5"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label5, 0, wxALIGN_CENTER|wxALL, 5 );
    
    description6 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Descriptor6    "), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description6, 0, wxALIGN_CENTER|wxALL, 5 );
    textCtrl6 = new wxTextCtrl( elementsPanel, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item3->Add( textCtrl6, 0, wxALIGN_CENTER|wxALL, 5 );
    label6 = new wxStaticText( elementsPanel, ID_TEXT, wxT("Label6"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item3, 0, wxGROW|wxALL|wxALIGN_CENTER, 5 );

    elementsPanel->SetAutoLayout( TRUE );
    elementsPanel->SetSizer( item0 );

    item0->Fit( elementsPanel );
    item0->SetSizeHints( elementsPanel );

}


//------------------------------------------------------------------------------
// void OnKepElements()
//------------------------------------------------------------------------------
/**
 * @note Sets the descriptors and labels for kepelerian elements
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnKepElements()
{
    description1->SetLabel("Semi-Major Axis");
    description2->SetLabel("Eccentricity");
    description3->SetLabel("Inclination");
    description4->SetLabel("R.A. of Ascending Node");
    description5->SetLabel("Arguement of Perigee");
    description6->SetLabel("True Anomaly");
    
    label1->SetLabel("Km");
    label2->SetLabel("");
    label3->SetLabel("deg");
    label4->SetLabel("deg");
    label5->SetLabel("deg");
    label6->SetLabel("deg");
    
    // convert values to Kepelarian values
    
    // get current cartesian values
    Real el1 = atof(textCtrl1->GetValue());
    Real el2 = atof(textCtrl2->GetValue());
    Real el3 = atof(textCtrl3->GetValue());
    Real el4 = atof(textCtrl4->GetValue());
    Real el5 = atof(textCtrl5->GetValue());
    Real el6 = atof(textCtrl6->GetValue()); 
    
    // create cartesian object
    Cartesian *cartesian = new Cartesian(el1, el2, el3, el4, el5, el6);
    // convert into keplerian values, all units are in radians
    Keplerian keplerian = ToKeplerian(*cartesian, GmatPhysicalConst::mu);

    // create a keplerian object
    Keplerian *kepObj = new Keplerian(keplerian);

    // get the new values
    Real kepEl1 = kepObj->GetSemimajorAxis();
    Real kepEl2 = kepObj->GetEccentricity();
    Radians kepEl3 = kepObj->GetInclination();
    Radians kepEl4 = kepObj->GetRAAscendingNode();
    Radians kepEl5 = kepObj->GetArgumentOfPeriapsis();
    Radians kepEl6 = kepObj->GetTrueAnomaly();

    wxString element1;
    element1.Printf("%.12f", kepEl1);
    textCtrl1->SetValue(element1);
    
    wxString element2;
    element2.Printf("%.12f", kepEl2);
    textCtrl2->SetValue(element2);
    
    wxString element3;
    element3.Printf("%.12f", kepEl3 * GmatMathUtil::DEG_PER_RAD);
    textCtrl3->SetValue(element3);
    
    wxString element4;
    element4.Printf("%.12f", kepEl4 * GmatMathUtil::DEG_PER_RAD);
    textCtrl4->SetValue(element4);
    
    wxString element5;
    element5.Printf("%.12f", kepEl5 * GmatMathUtil::DEG_PER_RAD);
    textCtrl5->SetValue(element5);
    
    wxString element6;
    element6.Printf("%.12f", kepEl6 * GmatMathUtil::DEG_PER_RAD);
    textCtrl6->SetValue(element6);
  
}

//------------------------------------------------------------------------------
// void OnCartElements()
//------------------------------------------------------------------------------
/**
 * @note Sets the descriptors and labels for cartesian elements
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnCartElements()
{
    description1->SetLabel("X");
    description2->SetLabel("Y");
    description3->SetLabel("Z");
    description4->SetLabel("Vx");
    description5->SetLabel("Vy");
    description6->SetLabel("Vz");
    
    label1->SetLabel("Km");
    label2->SetLabel("Km");
    label3->SetLabel("Km");
    label4->SetLabel("Km/s");
    label5->SetLabel("Km/s");
    label6->SetLabel("Km/s");

    // convert values to Cartesian values
    
    // get current cartesian values
    Real el1 = atof(textCtrl1->GetValue());
    Real el2 = atof(textCtrl2->GetValue());
    Real el3 = atof(textCtrl3->GetValue()) * GmatMathUtil::RAD_PER_DEG;
    Real el4 = atof(textCtrl4->GetValue()) * GmatMathUtil::RAD_PER_DEG;
    Real el5 = atof(textCtrl5->GetValue()) * GmatMathUtil::RAD_PER_DEG;
    Real ta  = atof(textCtrl6->GetValue()) * GmatMathUtil::RAD_PER_DEG;
    Real el6 = TrueToMeanAnomaly(ta, el2); // Keplerian() expects mean anomaly
    
    // create keplerian object
    Keplerian *keplerian = new Keplerian(el1, el2, el3, el4, el5, el6);
    // convert into cartesian values
    Cartesian cartesian = ToCartesian(*keplerian, GmatPhysicalConst::mu);

    // create a cartesian object
    Cartesian *cartObj = new Cartesian(cartesian);

    // get the new values
    Real cartEl1 = cartObj->GetPosition(0);
    Real cartEl2 = cartObj->GetPosition(1);
    Real cartEl3 = cartObj->GetPosition(2);
    Real cartEl4 = cartObj->GetVelocity(0);
    Real cartEl5 = cartObj->GetVelocity(1);
    Real cartEl6 = cartObj->GetVelocity(2);

    wxString element1;
    element1.Printf("%.12f", cartEl1);
    textCtrl1->SetValue(element1);
    
    wxString element2;
    element2.Printf("%.12f", cartEl2);
    textCtrl2->SetValue(element2);
    
    wxString element3;
    element3.Printf("%.12f", cartEl3);
    textCtrl3->SetValue(element3);
    
    wxString element4;
    element4.Printf("%.12f", cartEl4);
    textCtrl4->SetValue(element4);
    
    wxString element5;
    element5.Printf("%.12f", cartEl5);
    textCtrl5->SetValue(element5);
    
    wxString element6;
    element6.Printf("%.12f", cartEl6);
    textCtrl6->SetValue(element6);
}

//------------------------------------------------------------------------------
// void UpdateValues()
//------------------------------------------------------------------------------
/**
 * @note Gets the values from theSpacecraft and puts them in the text fields
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::UpdateValues()
{
    //loj: since Spacecraft class is not complete,
    //loj: use theSpacecraft->GetRealParameter(0) for epoch
    //loj: use theSpacecraft->GetRealParameter(1) for state[0], etc

    // default values for now
    // do this first, otherwise on state change will
    // change the element value
    stateCB->SetSelection(0);
    OnStateChange();
    dateCB->SetSelection(2);

    //loj: get element type first
    //loj: if element type is Cartesian, the combobox should show Cartesian
    //loj: if Keplerian, the combobox should show Keplerian, etc
    //loj: copy actual element type and elements to diaplay member data.
    //loj: when combobox changes, use display data to convert and display
    //loj: do not readback from the elements field unless user enters the new value
    
    // get elements
    Real epoch = theSpacecraft->GetRealParameter(0);
    Real element1 = theSpacecraft->GetRealParameter(1);
    Real element2 = theSpacecraft->GetRealParameter(2);
    Real element3 = theSpacecraft->GetRealParameter(3);
    Real element4 = theSpacecraft->GetRealParameter(4);
    Real element5 = theSpacecraft->GetRealParameter(5);
    Real element6 = theSpacecraft->GetRealParameter(6);

    wxString epochStr;
    epochStr.Printf("%f", epoch);
    epochValue->SetValue(epochStr);
    
    wxString el1;
    el1.Printf("%.12f", element1);
    textCtrl1->SetValue(el1);
    
    wxString el2;
    el2.Printf("%.12f", element2);
    textCtrl2->SetValue(el2);

    wxString el3;
    el3.Printf("%.12f", element3);
    textCtrl3->SetValue(el3);

    wxString el4;
    el4.Printf("%.12f", element4);
    textCtrl4->SetValue(el4);

    wxString el5;
    el5.Printf("%.12f", element5);
    textCtrl5->SetValue(el5);

    wxString el6;
    el6.Printf("%.12f", element6);
    textCtrl6->SetValue(el6);  
    
}

//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnOk()
{
    OnApply();
    
    // need to figure out the best way to close
    // the notebook page
    this->Destroy();
}

//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * @note Does not save entered data and destroys the notebook
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnCancel()
{
    // need to figure out the best way to close
    // the notebook page
    this->Destroy();
//    this->Close();
    
    /* // make item most current, then close it
 GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
 mainNotebook->CreatePage(item);
 mainNotebook->ClosePage();*/
}

//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * @note Saves the data to theSpacecraft
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnApply()
{
    //loj: used display member data to save to Spacecraft, instead of reading the field again
    
    wxString epochStr = epochValue->GetValue();
    wxString el1 = textCtrl1->GetValue();
    wxString el2 = textCtrl2->GetValue();
    wxString el3 = textCtrl3->GetValue();
    wxString el4 = textCtrl4->GetValue();
    wxString el5 = textCtrl5->GetValue();
    wxString el6 = textCtrl6->GetValue(); 
    
    theSpacecraft->SetRealParameter(0, atof(epochStr));
    theSpacecraft->SetRealParameter(1, atof(el1));
    theSpacecraft->SetRealParameter(2, atof(el2));
    theSpacecraft->SetRealParameter(3, atof(el3));
    theSpacecraft->SetRealParameter(4, atof(el4));
    theSpacecraft->SetRealParameter(5, atof(el5));
    theSpacecraft->SetRealParameter(6, atof(el6));
    
    applyButton->Disable();
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnTextChange()
{
    applyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnStateChange()
//------------------------------------------------------------------------------
/**
 * @note Changes the element descriptors and labels based on the state combo box
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnStateChange()
{
    applyButton->Enable();
    if (stateCB->GetSelection() == 0)
    {
       OnCartElements();
    }
    else if (stateCB->GetSelection() == 1)
    {
       OnKepElements();
    }
    else  //do nothing
    {
        ;
    }
}

//------------------------------------------------------------------------------
// void OnEpochChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when epoch combobox is changed
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnEpochChange()
{
    applyButton->Enable();
}

