//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
/**
 * This class contains the Impulsive Burn Setup window.
 */
//------------------------------------------------------------------------------

#include "ImpulsiveBurnSetupPanel.hpp"
//#include "GmatAppData.hpp"
//#include "gmatdefs.hpp" //put this one after GUI includes
//#include "GuiInterpreter.hpp"
//#include "Burn.hpp"
//#include "RealUtilities.hpp"
//#include "MessageInterface.hpp"

//#include <stdlib.h>

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ImpulsiveBurnSetupPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, ImpulsiveBurnSetupPanel::OnTextChange)
   EVT_COMBOBOX(ID_CB_VECTOR, ImpulsiveBurnSetupPanel::OnVectorChange)
   EVT_COMBOBOX(ID_CB_COORD, ImpulsiveBurnSetupPanel::OnVectorChange)
   EVT_BUTTON(ID_BUTTON_OK, ImpulsiveBurnSetupPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, ImpulsiveBurnSetupPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, ImpulsiveBurnSetupPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, ImpulsiveBurnSetupPanel::OnScript)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ImpulsiveBurnSetupPanel(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * Constructs ImpulsiveBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the impulsive burn GUI
 */
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::ImpulsiveBurnSetupPanel(wxWindow *parent, const wxString &burnName)
    :wxPanel(parent)
{
    CreateBurn(this, burnName);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void CreateBurn(wxWindow *parent, const wxString &burnName)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <burnName> input burn name.
 *
 * @note Creates the panel for the impulsive burn data
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::CreateBurn(wxWindow *parent, const wxString &burnName)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();

    theBurn = theGuiInterpreter->GetBurn(std::string(burnName.c_str()));

    if (theBurn != NULL)
    {
        // create layout sizer
        wxFlexGridSizer *impulsiveSizer = new wxFlexGridSizer(1, 0, 0 );
        impulsiveSizer->AddGrowableCol(0);
        impulsiveSizer->AddGrowableCol(1);
        impulsiveSizer->AddGrowableRow(0);
        impulsiveSizer->AddGrowableRow(1);

        // create script button
        wxGridSizer *topSizer = new wxGridSizer(1, 2, 0, 0);
        scriptButton = new wxButton(parent, ID_BUTTON_SCRIPT, "Create Script", 
            wxDefaultPosition, wxDefaultSize, 0);
    
        // button not functional, yet 
        scriptButton->Disable();
        
        // create spacers
        wxBoxSizer *spacerSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText *spacer1 = new wxStaticText(parent, ID_TEXT,
            wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        wxStaticText *spacer2 = new wxStaticText(parent, ID_TEXT,
            wxT(""), wxDefaultPosition, wxDefaultSize, 0);

        spacerSizer->Add(spacer1, 0, wxALIGN_CENTER | wxALL, 5);
        spacerSizer->Add(spacer2, 0, wxALIGN_CENTER | wxALL, 5);
  
        // add items to top sizer
        topSizer->Add(spacerSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
        topSizer->Add(scriptButton, 0, wxALIGN_RIGHT | wxALL, 5);

        // static box for the Input's
        wxStaticBox *item2 = new wxStaticBox(parent, ID_STATIC_COORD, wxT(""));
        wxStaticBoxSizer *middleSizer = new wxStaticBoxSizer(item2, wxVERTICAL);

        // gridsizer for inside the Input static box
        wxGridSizer *item3 = new wxGridSizer(2, 0, 0);

        // label for coordinate frames combo box
        wxStaticText *item4 = new wxStaticText(parent, ID_TEXT,
            wxT("Coordinate frame:"), wxDefaultPosition, wxDefaultSize, 0);
        item3->Add(item4, 0, wxALIGN_CENTER | wxALL, 5);
        
        // list of coordinate frames
        Integer id = theBurn->GetParameterID("CoordinateFrame");
        StringArray frames = theBurn->GetStringArrayParameter(id);
        int listSize = frames.size();
        
        // check to see if any coordinate frames exist
        if (listSize > 0)
        {
            wxString choices[listSize];
            for (int i=0; i<listSize; i++)
                choices[i] = wxString(frames[i].c_str());

            // combo box for avaliable coordinate frames 
            coordCB = new wxComboBox(parent, ID_CB_COORD, wxT(""), 
                wxDefaultPosition, wxSize(150,-1), listSize, choices, wxCB_DROPDOWN);
        }
        else
        { 
            wxString strs6[] =
            {
                wxT("No coordinate frame available") 
            };    
            coordCB = new wxComboBox(parent, ID_CB_COORD, wxT(""),
                wxDefaultPosition, wxSize(150,-1), 1, strs6, wxCB_DROPDOWN);
        }
            
        wxStaticText *item5 = new wxStaticText(parent, ID_TEXT,
            wxT("Vector format:"), wxDefaultPosition, wxDefaultSize, 0);
        item3->Add(item5, 0, wxALIGN_CENTER | wxALL, 5);
       
        // combo box for Vector format
        wxString strs7[] =
        {
           wxT("Cartesian") 
//           wxT("Spherical") 
        };
        vectorCB = new wxComboBox(parent, ID_CB_VECTOR, wxT(""),
            wxDefaultPosition, wxSize(120,-1), 1, strs7, wxCB_DROPDOWN);

        item3->Add(coordCB, 0, wxALIGN_CENTER | wxALL, 5);
        item3->Add(vectorCB, 0, wxALIGN_CENTER | wxALL, 5);

        middleSizer->Add(item3, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5);

        // static box for the vector
        wxStaticBox *vectorBox = new wxStaticBox(parent, ID_STATIC_VECTOR, wxT("Vector"));
        wxStaticBoxSizer *vectorSizer = new wxStaticBoxSizer(vectorBox, wxVERTICAL);

        // panel that has the labels and text fields for the vectors
        // set it to null
        vectorPanel = (wxPanel *)NULL; 
        
        // adds default descriptors/labels 
        AddVector(parent);
        vectorSizer->Add(vectorPanel, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);

        middleSizer->Add( vectorSizer, 0, wxGROW| wxALIGN_CENTER_VERTICAL| wxALL, 5);

        // create label and text field for the central body
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

        // gets the values from theBurn
        UpdateValues();
    
        //disable apply button until a field is changed
        applyButton->Disable();
        helpButton->Disable();
    }
    else
    {
        // show error message
    }
}

//------------------------------------------------------------------------------
// void AddVector(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the default objects to put in the vector static box
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::AddVector(wxWindow *parent)
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
// void UpdateValues()
//------------------------------------------------------------------------------
/**
 * @note Gets the values from theManeuver and puts them in the text fields
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::UpdateValues()
{
    Integer id, el;
    Real element;
    
    // Coordinate Frame   
    id = theBurn->GetParameterID("CoordinateFrame");
    std::string coordFrame = theBurn->GetStringParameter(id);
    StringArray frames = theBurn->GetStringArrayParameter(id);
    int index = 0;
    for (StringArray::iterator iter = frames.begin(); 
        iter != frames.end(); ++iter) 
    {
        if (coordFrame == *iter) 
            coordCB->SetSelection(index);
        else
            ++index;
    }

    // Vector Format
    id = theBurn->GetParameterID("VectorFormat");
    std::string vectorFormat = theBurn->GetStringParameter(id);

    if (vectorFormat == "Cartesian") 
        vectorCB->SetSelection(0);
    else
        vectorCB->SetSelection(1);

    // Element1
    el = theBurn->GetParameterID("Element1");
    element = theBurn->GetRealParameter(el);
    wxString el1;
    el1.Printf("%f", element);
    textCtrl1->SetValue(el1);
    
    // Element2
    el = theBurn->GetParameterID("Element2");
    element = theBurn->GetRealParameter(el);
    wxString el2;
    el2.Printf("%f", element);
    textCtrl2->SetValue(el2);

    // Element3
    el = theBurn->GetParameterID("Element3");
    element = theBurn->GetRealParameter(el);
    wxString el3;
    el3.Printf("%f", element);
    textCtrl3->SetValue(el3);

    OnVectorChange();
}

//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnOK()
{
    OnApply();
    
    // Close page from main notebook    
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();

}

//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * @note Does not save entered data and destroys the notebook
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnCancel()
{
    // Close page from main notebook
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * @note Saves the data to theBurn
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnApply()
{
    Integer id;
    wxString elemString;
    
    // save coordinate frame
    wxString frameString = coordCB->GetStringSelection();
    id = theBurn->GetParameterID("CoordinateFrame");
    std::string coordFrame = std::string (frameString.c_str());
    theBurn->SetStringParameter(id, coordFrame);
    
    // save vector format
    wxString vectorString = vectorCB->GetStringSelection();
    id = theBurn->GetParameterID("VectorFormat");
    std::string vectorFormat = std::string (vectorString.c_str());
    theBurn->SetStringParameter(id, vectorFormat);

    // save element1
    elemString = textCtrl1->GetValue();
    id = theBurn->GetParameterID("Element1");
    theBurn->SetRealParameter(id, atof(elemString));

    // save element2
    elemString = textCtrl2->GetValue();
    id = theBurn->GetParameterID("Element2");
    theBurn->SetRealParameter(id, atof(elemString));

    // save element3
    elemString = textCtrl3->GetValue();
    id = theBurn->GetParameterID("Element3");
    theBurn->SetRealParameter(id, atof(elemString));

    applyButton->Disable();

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnTextChange()
{
    applyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnVectorChange()
//------------------------------------------------------------------------------
/**
 * @note Changes the vector descriptors and labels based on the state combo box
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnVectorChange()
{
    applyButton->Enable();

    Integer id;
    wxString coordStr, vectorStr, el;
    std::string element;
    
    // get the string of the combo box selections
    coordStr = coordCB->GetStringSelection();
    vectorStr = coordCB->GetStringSelection();

    // get the ID of the coordinate frame parameter 
    id = theBurn->GetParameterID("CoordinateFrame");

    // store the coordinate from for cancellation
    std::string coordFrame = theBurn->GetStringParameter(id);
    
    // check the combo box selection
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

    // reset coordinate frame to original selection if cancel button selected
    id = theBurn->GetParameterID("CoordinateFrame");
    theBurn->SetStringParameter(id, coordFrame);
}

//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * @note Writes data in script form
 */
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::OnScript()
{
    // will implement later
}

