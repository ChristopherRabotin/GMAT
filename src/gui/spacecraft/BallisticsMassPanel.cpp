//------------------------------------------------------------------------------
//                            BallisticsMassPanel
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
// Created: 2004/04/01
/**
 * This class contains information needed to setup users spacecraft ballistics
 * and mass through GUI
 * 
 */
//------------------------------------------------------------------------------
#include "BallisticsMassPanel.hpp"
#include "MessageInterface.hpp"
//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BallisticsMassPanel, wxPanel)
   EVT_TEXT(ID_TEXTCTRL, BallisticsMassPanel::OnTextChange)
   EVT_TEXT(ID_TEXT_CTRL, BallisticsMassPanel::OnTextChange)
END_EVENT_TABLE()
//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// BallisticsMassPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs BallisticsMassPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
BallisticsMassPanel::BallisticsMassPanel(wxWindow *parent,
                                         Spacecraft *spacecraft,
                                         wxButton *theApplyButton)
                    :wxPanel(parent)
{
    this->theSpacecraft = spacecraft;
    this->theApplyButton = theApplyButton;
    Create();
}

BallisticsMassPanel::~BallisticsMassPanel()
{
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
 * @note Creates the page for ballistics and mass information
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::Create()
{
    wxStaticBox *item1 = new wxStaticBox( this, -1, wxT("") );
    wxStaticBoxSizer *item0 = new wxStaticBoxSizer( item1, wxVERTICAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *dryMassStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Dry Mass"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dryMassStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    dryMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( dryMassTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *dragCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coefficient of Drag"), wxDefaultPosition, 
                            wxDefaultSize, 0 );
    item2->Add( dragCoeffStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    dragCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( dragCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *reflectCoeffStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Coefficient of Reflectivity"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( reflectCoeffStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    reflectCoeffTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( reflectCoeffTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *dragAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("Drag Area"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( dragAreaStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    dragAreaTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( dragAreaTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *srpAreaStaticText = new wxStaticText( this, ID_TEXT, 
                            wxT("SRP Area"), 
                            wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( srpAreaStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

    srpAreaTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( srpAreaTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    this->SetAutoLayout( TRUE );  
    this->SetSizer( item0 );
    item0->Fit( this );
    item0->SetSizeHints( this );
}

void BallisticsMassPanel::LoadData()
{
    int massID = theSpacecraft->GetParameterID("Mass");
    int coeffDragID = theSpacecraft->GetParameterID("CoefficientDrag");
    int incidentAreaID = theSpacecraft->GetParameterID("IncidentArea");
    int reflectCoeffID = theSpacecraft->GetParameterID("ReflectivityCoefficient");

    Real mass = theSpacecraft->GetRealParameter(massID);
    Real dragCoeff = theSpacecraft->GetRealParameter(coeffDragID);
    Real incidentArea = theSpacecraft->GetRealParameter(incidentAreaID);
    Real reflectCoeff = theSpacecraft->GetRealParameter(reflectCoeffID);

    wxString massStr;
    wxString dragCoeffStr;
    wxString incidentAreaStr;
    wxString reflectCoeffStr;
    
    massStr.Printf("%f", mass);
    dragCoeffStr.Printf("%f", dragCoeff);
    incidentAreaStr.Printf("%f", incidentArea);
    reflectCoeffStr.Printf("%f", reflectCoeff);

    dryMassTextCtrl->SetValue(massStr);
    dragCoeffTextCtrl->SetValue(dragCoeffStr);
    reflectCoeffTextCtrl->SetValue(reflectCoeffStr);
    dragAreaTextCtrl->SetValue(incidentAreaStr);
    srpAreaTextCtrl->SetValue(incidentAreaStr);

//    epochValue->SetValue(epochStr);

}

void BallisticsMassPanel::SaveData()
{
    int massID = theSpacecraft->GetParameterID("Mass");
    int coeffDragID = theSpacecraft->GetParameterID("CoefficientDrag");
    int incidentAreaID = theSpacecraft->GetParameterID("IncidentArea");
    int reflectCoeffID = theSpacecraft->GetParameterID("ReflectivityCoefficient");

    wxString massStr = dryMassTextCtrl->GetValue();
    wxString dragCoeffStr = dragCoeffTextCtrl->GetValue();
    wxString incidentAreaStr = dragAreaTextCtrl->GetValue();
    wxString reflectCoeffStr = reflectCoeffTextCtrl->GetValue();
    
    theSpacecraft->SetRealParameter(massID, atof(massStr));
    theSpacecraft->SetRealParameter(coeffDragID, atof(dragCoeffStr));
    theSpacecraft->SetRealParameter(incidentAreaID, atof(incidentAreaStr));
    theSpacecraft->SetRealParameter(reflectCoeffID, atof(reflectCoeffStr));
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
/**
 * @note Activates the Apply button when text is changed
 */
//------------------------------------------------------------------------------
void BallisticsMassPanel::OnTextChange()
{
    theApplyButton->Enable();
}
