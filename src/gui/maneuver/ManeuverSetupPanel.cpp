//$Header Exp $
//------------------------------------------------------------------------------
//                              ManeuverSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2003/12/01
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#include "ManeuverSetupPanel.hpp"
#include "GmatAppData.hpp"
#include "gmatdefs.hpp" //put this one after GUI includes
#include "GuiInterpreter.hpp"
#include "Command.hpp"
#include "Burn.hpp"
#include "RealUtilities.hpp"

#include <stdlib.h>

//using namespace Gmat;

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ManeuverSetupPanel, wxPanel)
    EVT_COMBOBOX(ID_CB_VECTOR, ManeuverSetupPanel::OnVectorChange)
    EVT_COMBOBOX(ID_CB_COORDINATE, ManeuverSetupPanel::OnVectorChange)
    EVT_BUTTON(ID_BUTTON_OK, ManeuverSetupPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, ManeuverSetupPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, ManeuverSetupPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, ManeuverSetupPanel::OnScript)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ManeuverSetupPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs ManeuverSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the maneuver dialog box
 */
//------------------------------------------------------------------------------
ManeuverSetupPanel::ManeuverSetupPanel(wxWindow *parent)
    :wxPanel(parent)
{
    CreateManeuver(this);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void CreateImpulsive(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the panel for the Impulsive notebook page
 * @note Use impulsivePanel as the parent of all the objects
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::CreateManeuver(wxWindow *parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();

    theManeuver = theGuiInterpreter->CreateCommand("Maneuver");
    
    if (theManeuver->GetTypeName() == "Maneuver")  // to be safe, check type
    {
    
        theBurn = theGuiInterpreter->CreateBurn("ImpulsiveBurn", "Burn1");
//        theBurn = theGuiInterpreter->GetObject(Gmat::BURN);

        if (theBurn != NULL)
        {
            wxFlexGridSizer *impulsiveSizer = new wxFlexGridSizer(1, 0, 0 );
            impulsiveSizer->AddGrowableCol(0);
            impulsiveSizer->AddGrowableCol(1);
            impulsiveSizer->AddGrowableRow(0);
            impulsiveSizer->AddGrowableRow(1);

            // create label and text field for maneuver name
            wxBoxSizer *nameSizer = new wxBoxSizer(wxHORIZONTAL);

            wxStaticText *nameLabel = new wxStaticText(parent, ID_TEXT,
                wxT("Maneuver name:"), wxDefaultPosition, wxDefaultSize, 0);
            nameField = new wxTextCtrl(parent, ID_TEXTCTRL, wxT("Burn1"), 
                wxDefaultPosition, wxSize(150,-1), 0);
            nameSizer->Add(nameLabel, 0, wxALIGN_CENTER | wxALL, 5);
            nameSizer->Add(nameField, 0, wxALIGN_CENTER | wxALL, 5);

            // create script button
            wxGridSizer *topSizer = new wxGridSizer(2, 2, 0, 0);
            scriptButton = new wxButton(parent, ID_BUTTON_SCRIPT, "Create Script", 
                wxDefaultPosition, wxDefaultSize, 0);
    
            // create type of burn label and combo box
            wxBoxSizer *burnSizer = new wxBoxSizer(wxHORIZONTAL);
            wxStaticText *item44 = new wxStaticText(parent, ID_TEXT,
                wxT("Type of burn:"), wxDefaultPosition, wxDefaultSize, 0);
            // combo box for burn types 
            wxString burnChoices[] =
            {
                wxT("Impulsive"),
                wxT("Finite")
            };
            burnCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), 2, burnChoices, wxCB_DROPDOWN);
            burnSizer->Add(item44, 0, wxALIGN_CENTER | wxALL, 5);
            burnSizer->Add(burnCB, 0, wxALIGN_CENTER | wxALL, 5);

            // create spacecraft label and combo box
            wxBoxSizer *spacecraftSizer = new wxBoxSizer(wxHORIZONTAL);
            wxStaticText *item41 = new wxStaticText(parent, ID_TEXT,
                wxT("Spacecraft:"), wxDefaultPosition, wxDefaultSize, 0);
            // list of spacecrafts
            StringArray &listSat = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
            int listSize = listSat.size();
            if (listSize > 0)  // check to see if any spacecrafts exist
            {
                wxString choices[listSize];
                for (int i=0; i<listSize; i++)
                    choices[i] = wxString(listSat[i].c_str());
                // combo box for avaliable spacecrafts 
                satCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                    wxSize(150,-1), listSize, choices, wxCB_DROPDOWN);
            }
            else
            {
                // combo box for avaliable spacecrafts 
                wxString strs9[] =
                {
                    wxT("No Spacecrafts Available"), 
                }; 
                satCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                    wxSize(150,-1), 1, strs9, wxCB_DROPDOWN);
            }               
            spacecraftSizer->Add(item41, 0, wxALIGN_CENTER | wxALL, 5);
            spacecraftSizer->Add(satCB, 0, wxALIGN_CENTER | wxALL, 5);
    
            // add items to top sizer
            topSizer->Add(nameSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
            topSizer->Add(scriptButton, 0, wxALIGN_RIGHT | wxALL, 5);
            topSizer->Add(burnSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
            topSizer->Add(spacecraftSizer, 0, wxGROW | wxALIGN_RIGHT | wxALL, 5);

            // static box for the Input's
            wxStaticBox *item2 = new wxStaticBox(parent, ID_STATIC_COORD, wxT(""));
            wxStaticBoxSizer *middleSizer = new wxStaticBoxSizer(item2, wxVERTICAL);

            // gridsizer for inside the Input static box
            wxGridSizer *item3 = new wxGridSizer(2, 0, 0);

            // label for coordinate frames combo box
            wxStaticText *item4 = new wxStaticText(parent, ID_TEXT,
                wxT("Coordinate frames:"), wxDefaultPosition, wxDefaultSize, 0);
            item3->Add(item4, 0, wxALIGN_CENTER | wxALL, 5);
            // list of coordinate frames
            Integer id = theBurn->GetParameterID("CoordinateFrame");
            StringArray frames = theBurn->GetStringArrayParameter(id);
            listSize = frames.size();
            // check to see if any coordinate frames exist
            if (listSize > 0)
            {
                wxString choices[listSize];
                for (int i=0; i<listSize; i++)
                    choices[i] = wxString(frames[i].c_str());
                // combo box for avaliable coordinate frames 
                coordCB = new wxComboBox(parent, ID_CB_COORDINATE, wxT(""), 
                    wxDefaultPosition, wxSize(150,-1), listSize, choices, wxCB_DROPDOWN);
            }
            else
            {
                // combo box for coordinate systems 
                wxString strs6[] =
                {
                    wxT("VBN"), 
                    wxT("LHLV"), 
                    wxT("S/c - center Sun Pointing"), 
                    wxT("Inertial"), 
                    wxT("Other")
                };    
                coordCB = new wxComboBox(parent, ID_CB_COORDINATE, wxT(""),
                    wxDefaultPosition, wxSize(150,-1), 5, strs6, wxCB_DROPDOWN);
            }
            
            wxStaticText *item5 = new wxStaticText(parent, ID_TEXT,
                wxT("Vector format:"), wxDefaultPosition, wxDefaultSize, 0);
            item3->Add(item5, 0, wxALIGN_CENTER | wxALL, 5);
            // combo box for Vector format
            wxString strs7[] =
            {
               wxT("Cartesian Vector"), 
               wxT("Spherical Vector") 
            };
            vectorCB = new wxComboBox(parent, ID_CB_VECTOR, wxT(""),
                wxDefaultPosition, wxSize(120,-1), 2, strs7, wxCB_DROPDOWN);

            item3->Add(coordCB, 0, wxALIGN_CENTER | wxALL, 5);
            item3->Add(vectorCB, 0, wxALIGN_CENTER | wxALL, 5);

            middleSizer->Add(item3, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

            // static box for the vector
            vectorBox = new wxStaticBox(parent, ID_STATIC_VECTOR, wxT("Vector"));
            vectorSizer = new wxStaticBoxSizer(vectorBox, wxVERTICAL);

            // panel that has the labels and text fields for the vectors
            // set it to null
            vectorPanel = (wxPanel *)NULL; 

            // adds default descriptors/labels 
            AddVector(parent);
            vectorSizer->Add(vectorPanel, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);

            middleSizer->Add( vectorSizer, 0, wxGROW| wxALIGN_CENTER_VERTICAL| wxALL, 5);

            // create label and text field for maneuver name
            wxBoxSizer *bodySizer = new wxBoxSizer(wxHORIZONTAL);
            wxStaticText *item6 = new wxStaticText(parent, ID_TEXT, wxT("Central body:"),
                wxDefaultPosition, wxDefaultSize, 0);
            wxStaticText *item7 = new wxStaticText(parent, ID_TEXT, wxT("Earth"),
                wxDefaultPosition, wxDefaultSize, wxCAPTION);
            bodySizer->Add( item6, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);
            bodySizer->Add( item7, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

            middleSizer->Add(bodySizer, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

            // create buttons
            wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
            okButton = new wxButton(parent, ID_BUTTON_OK, "OK", 
                wxDefaultPosition, wxDefaultSize, 0);
            applyButton = new wxButton(parent, ID_BUTTON_APPLY, "Apply", 
                wxDefaultPosition, wxDefaultSize, 0);
            cancelButton = new wxButton(parent, ID_BUTTON_CANCEL, "Cancel", 
                wxDefaultPosition, wxDefaultSize, 0);
            helpButton = new wxButton(parent, ID_BUTTON_HELP, "Help", 
                wxDefaultPosition, wxDefaultSize, 0);
            // adds the buttons to button sizer    
            buttonSizer->Add(okButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
            buttonSizer->Add(applyButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
            buttonSizer->Add(cancelButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
            buttonSizer->Add(helpButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);

            // add items to panel sizer
            impulsiveSizer->Add(topSizer, 0, wxGROW | wxALL, 5);
            impulsiveSizer->Add(middleSizer, 0, wxGROW | wxALL, 5);
            impulsiveSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

            impulsiveSizer->Fit(parent);

            // tells the enclosing window to adjust to the size of the sizer
            impulsiveSizer->SetSizeHints(parent);
            parent->SetSizer(impulsiveSizer);
            parent->SetAutoLayout( TRUE );

            // gets the values from theManeuver
            UpdateValues();
        }
        else
        {
            // show error message
        }
    }  // theManeuver->GetTypeName()
    else
    {
       // show error message
    }
}

//------------------------------------------------------------------------------
// void CreateFinite(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the panel for the Finite notebook page
 * @note Use finitePanel as the parent of all the objects
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::CreateFinite(wxWindow *parent)
{
    finitePanel = new wxPanel(parent);
}

//------------------------------------------------------------------------------
// void AddElements(AddVector *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the default objects to put in the vector static box
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::AddVector(wxWindow *parent)
{
    vectorPanel = new wxPanel(parent);
    wxGridSizer *item0 = new wxGridSizer( 1, 0, 0 );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 6, 3, 0, 0 );
    item3->AddGrowableCol( 0 );
    item3->AddGrowableCol( 1 );
    item3->AddGrowableCol( 2 );

    description1 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Descriptor1     "), 
                   wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description1, 0, wxALIGN_CENTER | wxALL, 5 );
    textCtrl1 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                wxDefaultPosition, wxSize(100,-1), 0 );
    item3->Add( textCtrl1, 0, wxALIGN_CENTER | wxALL, 5 );
    label1 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label1"), 
             wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label1, 0, wxALIGN_CENTER | wxALL, 5 );

    description2 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Descriptor2    "), 
                   wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description2, 0, wxALIGN_CENTER | wxALL, 5 );
    textCtrl2 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                wxDefaultPosition, wxSize(100,-1), 0 );
    item3->Add( textCtrl2, 0, wxALIGN_CENTER|wxALL, 5 );
    label2 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label2"), 
             wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label2, 0, wxALIGN_CENTER | wxALL, 5 );
    
    description3 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Descriptor3    "), 
                   wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( description3, 0, wxALIGN_CENTER | wxALL, 5 );
    textCtrl3 = new wxTextCtrl( vectorPanel, ID_TEXTCTRL, wxT(""), 
                wxDefaultPosition, wxSize(100,-1), 0 );
    item3->Add( textCtrl3, 0, wxALIGN_CENTER | wxALL, 5 );
    label3 = new wxStaticText( vectorPanel, ID_TEXT, wxT("Label3"), 
             wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( label3, 0, wxALIGN_CENTER | wxALL, 5 );
 
    item0->Add( item3, 0, wxGROW|wxALL|wxALIGN_CENTER, 5 );

    vectorPanel->SetAutoLayout( TRUE );
    vectorPanel->SetSizer( item0 );

    item0->Fit( vectorPanel );
    item0->SetSizeHints( vectorPanel );
}

//------------------------------------------------------------------------------
// void CartesianVector()
//------------------------------------------------------------------------------
/**
 * @note Sets the descriptors and labels for cartesian vectors
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::CartesianVector()
{
    Integer id1 = theBurn->GetParameterID("Element1Label");
    std::string element1 = theBurn->GetStringParameter(id1);
//    std::string element1 = theBurn->GetParameterText(id);
    wxString e1 = wxString(element1.c_str());

    Integer id2 = theBurn->GetParameterID("Element2Label");
    std::string element2 = theBurn->GetStringParameter(id2);
    wxString e2 = wxString(element2.c_str());

    Integer id3 = theBurn->GetParameterID("Element3Label");
    std::string element3 = theBurn->GetStringParameter(id3);
    wxString e3 = wxString(element3.c_str());

//    if (coordCB->GetSelection() == 0)
//    {
        description1->SetLabel(e1);
        description2->SetLabel(e2);
        description3->SetLabel(e3);
//        description1->SetLabel("T-Velocity:");
//        description2->SetLabel("T-Normal:");
//        description3->SetLabel("T-Binormal:");
    
        label1->SetLabel("km/s");
        label2->SetLabel("km/s");
        label3->SetLabel("km/s");
/*    }
    else if (coordCB->GetSelection() == 1)
    {
        description1->SetLabel("T-Radial:");
        description2->SetLabel("T-Tangential:");
        description3->SetLabel("T-Normal:");
    
        label1->SetLabel("km/s");
        label2->SetLabel("km/s");
        label3->SetLabel("km/s");
    }
    else if ((coordCB->GetSelection() == 2) || (coordCB->GetSelection() == 3) ||
        (coordCB->GetSelection() == 4))
    {
        description1->SetLabel("X-Component:");
        description2->SetLabel("Y-Component:");
        description3->SetLabel("Z-Component:");
    
        label1->SetLabel("km/s");
        label2->SetLabel("km/s");
        label3->SetLabel("km/s");
    }
*/
}

//------------------------------------------------------------------------------
// void SphericalVector()
//------------------------------------------------------------------------------
/**
 * @note Sets the descriptors and labels for spherical vectors
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::SphericalVector()
{
    description1->SetLabel("Right Ascension:");
    description2->SetLabel("Declination:");
    description3->SetLabel("Velocity Mag.:");
    
    label1->SetLabel("deg");
    label2->SetLabel("deg");
    label3->SetLabel("km/s");
}

//------------------------------------------------------------------------------
// void UpdateValues()
//------------------------------------------------------------------------------
/**
 * @note Gets the values from theManeuver and puts them in the text fields
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::UpdateValues()
{
    // Coordinate Frame
/*    std::string coordFrame = theBurn->GetStringParameter(0);
    if (coordFrame == "VBN") 
        coordCB->SetSelection(0);
    else if (coordFrame == "LHLV") 
        coordCB->SetSelection(1);
    else if (coordFrame == "S/c - center Sun Pointing") 
        coordCB->SetSelection(2);
    else if (coordFrame == "Inertial") 
        coordCB->SetSelection(3);
    else
        coordCB->SetSelection(4);
*/
    // Vector Format
    std::string coordSystem = theBurn->GetStringParameter(1);
    if (coordSystem == "Cartesian") 
        vectorCB->SetSelection(0);
    else
        vectorCB->SetSelection(1);

    // Vector 
    Real element1 = theBurn->GetRealParameter(2);
    Real element2 = theBurn->GetRealParameter(3);
    Real element3 = theBurn->GetRealParameter(4);

    wxString el1;
    el1.Printf("%f", element1);
    textCtrl1->SetValue(el1);

    wxString el2;
    el2.Printf("%f", element2);
    textCtrl2->SetValue(el2);

    wxString el3;
    el3.Printf("%f", element3);
    textCtrl3->SetValue(el3);

    // default values for now
    coordCB->SetSelection(0);
    burnCB->SetSelection(0);
    satCB->SetSelection(0);
    OnVectorChange();
}

//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnOK()
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
void ManeuverSetupPanel::OnCancel()
{
    // need to figure out the best way to close
    // the notebook page

    this->Destroy();
}

//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * @note Saves the data to theManeuver
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnApply()
{
    Integer el;
    
    wxString nameStr = nameField->GetValue();
    
    // save element1
    wxString el1 = textCtrl1->GetValue();
    el = theBurn->GetParameterID("Element1");
    theBurn->SetRealParameter(el, atof(el1));
//    theBurn->SetRealParameter(el, 1.0);

    // save element2
    wxString el2 = textCtrl1->GetValue();
    el = theBurn->GetParameterID("Element2");
    theBurn->SetRealParameter(el, atof(el2));

    // save element3
    wxString el3 = textCtrl1->GetValue();
    el = theBurn->GetParameterID("Element3");
    theBurn->SetRealParameter(el, atof(el3));

}

//------------------------------------------------------------------------------
// void OnStateChange()
//------------------------------------------------------------------------------
/**
 * @note Changes the vector descriptors and labels based on the state combo box
 */
//------------------------------------------------------------------------------
/*void ManeuverSetupPanel::OnFormatChange()
{
    if (vectorCB->GetSelection() == 0)
       CartesianVector();
    else
       SphericalVector();
}
*/
//------------------------------------------------------------------------------
// void OnStateChange()
//------------------------------------------------------------------------------
/**
 * @note Changes the vector descriptors and labels based on the state combo box
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnVectorChange()
{
    Integer id;
    wxString coordStr, vectorStr, el;
    std::string element;
    
    coordStr = coordCB->GetStringSelection();
    id = theBurn->GetParameterID("CoordinateFrame");

    vectorStr = coordCB->GetStringSelection();
    
    if (coordStr == "Inertial")
        theBurn->SetStringParameter(id, "Inertial");
    else if (coordStr == "VNB")
        theBurn->SetStringParameter(id, "VNB");
    
    // set label for element1 
    id = theBurn->GetParameterID("Element1Label");
    element = theBurn->GetStringParameter(id);
    el = wxString(element.c_str());
    description1->SetLabel(el);
    label1->SetLabel("km/s");
        
    // set label for element2
    id = theBurn->GetParameterID("Element2Label");
    element = theBurn->GetStringParameter(id);
    el = wxString(element.c_str());
    description2->SetLabel(el);
    label2->SetLabel("km/s");
       
    // set label for element3
    id = theBurn->GetParameterID("Element3Label");
    element = theBurn->GetStringParameter(id);
    el = wxString(element.c_str());
    description3->SetLabel(el);
    label3->SetLabel("km/s");

}

void ManeuverSetupPanel::OnScript()
{
}

