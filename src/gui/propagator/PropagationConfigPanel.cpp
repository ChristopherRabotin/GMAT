//$Header$
//------------------------------------------------------------------------------
//                              PropagationConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Propagation Configuration window.
 */
//------------------------------------------------------------------------------

// gui includes
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/variant.h>
#include <wx/string.h>

#include "gmatwxdefs.hpp"
#include "ViewTextFrame.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"
#include "GmatMainNotebook.hpp"
#include "PropagationConfigPanel.hpp"
//#include "PointMassSelectionDialog.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Propagator.hpp"
#include "Integrator.hpp"
#include "RungeKutta89.hpp"
#include "PropSetup.hpp"
#include "PhysicalModel.hpp"
#include "ForceModel.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"

#include <ctype.h>

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagationConfigPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, PropagationConfigPanel::OnScriptButton)
    EVT_BUTTON(ID_BUTTON_OK, PropagationConfigPanel::OnOKButton)
    EVT_BUTTON(ID_BUTTON_APPLY, PropagationConfigPanel::OnApplyButton)
    EVT_BUTTON(ID_BUTTON_CANCEL, PropagationConfigPanel::OnCancelButton)
    EVT_BUTTON(ID_BUTTON_ADD_BODY, PropagationConfigPanel::OnAddButton)
    EVT_BUTTON(ID_BUTTON_GRAV_SEARCH, PropagationConfigPanel::OnGravSearchButton)
    EVT_BUTTON(ID_BUTTON_SETUP, PropagationConfigPanel::OnSetupButton)
    EVT_BUTTON(ID_BUTTON_MAG_SEARCH, PropagationConfigPanel::OnMagSearchButton)
    EVT_BUTTON(ID_BUTTON_PM_EDIT, PropagationConfigPanel::OnPMEditButton)
    EVT_BUTTON(ID_BUTTON_SRP_EDIT, PropagationConfigPanel::OnSRPEditButton)
    EVT_TEXT(ID_TEXTCTRL_INTG1, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG2, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG3, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG4, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG5, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG6, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG7, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG8, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_INTG9, PropagationConfigPanel::OnIntegratorTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_GRAV1, PropagationConfigPanel::OnGravityTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_GRAV2, PropagationConfigPanel::OnGravityTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_MAGN1, PropagationConfigPanel::OnMagneticTextUpdate)
    EVT_TEXT(ID_TEXTCTRL_MAGN2, PropagationConfigPanel::OnMagneticTextUpdate)
    EVT_COMBOBOX(ID_CB_INTGR, PropagationConfigPanel::OnIntegratorSelection)
    EVT_COMBOBOX(ID_CB_BODY, PropagationConfigPanel::OnBodySelection)
    EVT_COMBOBOX(ID_CB_GRAV, PropagationConfigPanel::OnGravitySelection)
    EVT_COMBOBOX(ID_CB_ATMOS, PropagationConfigPanel::OnAtmosphereSelection)
    EVT_CHECKBOX(ID_CHECKBOX, PropagationConfigPanel::OnSRPCheckBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagationConfigPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagationConfigPanel::PropagationConfigPanel(wxWindow *parent, const wxString &propName)
    : wxPanel(parent)
{
    propNameString = propName;
    
    Initialize();
    Setup(this);
    GetData();
}

void PropagationConfigPanel::Initialize()
{  
    theGuiInterpreter = GmatAppData::GetGuiInterpreter(); 
    thePropSetup = theGuiInterpreter->GetPropSetup(propNameString.c_str());
    theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
    
    thePropagator = thePropSetup->GetPropagator();
    theForceModel = thePropSetup->GetForceModel();
    
    theCelestialBody = theSolarSystem->GetBody(SolarSystem::EARTH_NAME);
    
    bodiesInUse = theSolarSystem->GetBodiesInUse();
    
    orderID = theCelestialBody->GetParameterID("Order");
    degreeID = theCelestialBody->GetParameterID("Degree");
    
    // waw: TBD
    primaryBodyString = SolarSystem::EARTH_NAME.c_str();
       
    // Default values
    numOfIntegrators = 3;
    numOfBodies = bodiesInUse.size(); 
    numOfAtmosTypes = 3;
    numOfForces = 1;  // TBD thePropSetup->GetNumForces();
    numOfGraFields = 3;
    numOfMagFields = 1;  // TBD - Not for Build 2
}

void PropagationConfigPanel::Setup(wxWindow *parent)
{          
    // wxStaticText
    item8 = new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"), wxDefaultPosition, wxDefaultSize, 0 );
    setting1StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 1"), wxDefaultPosition, wxDefaultSize, 0 );
    setting2StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 2"), wxDefaultPosition, wxDefaultSize, 0 );
    setting3StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 3"), wxDefaultPosition, wxDefaultSize, 0 );
    setting4StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 4"), wxDefaultPosition, wxDefaultSize, 0 );
    setting5StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 5"), wxDefaultPosition, wxDefaultSize, 0 );
    setting6StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 6"), wxDefaultPosition, wxDefaultSize, 0 );
    setting7StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 7"), wxDefaultPosition, wxDefaultSize, 0 );
    setting8StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 8"), wxDefaultPosition, wxDefaultSize, 0 );
    setting9StaticText = new wxStaticText( parent, ID_TEXT, wxT("Setting 9"), wxDefaultPosition, wxDefaultSize, 0 );
    item42 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item47 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item38 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    item40 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item54 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item56 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item52 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxTextCtrl
    setting1TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG1, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting2TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG2, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting3TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG3, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting4TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG4, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting5TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG5, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting6TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG6, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting7TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG7, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting8TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG8, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    setting9TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG9, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    bodyTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), wxTE_READONLY );
    gravityDegreeTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_GRAV1, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
    gravityOrderTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_GRAV2, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
    pmEditTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(550,-1), wxTE_READONLY );
    magneticDegreeTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_MAGN1, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
    magneticOrderTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_MAGN2, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
    
    // wxButton
    scriptButton = new wxButton( parent, ID_BUTTON_SCRIPT, wxT("Create Script"), wxDefaultPosition, wxDefaultSize, 0 );
    bodyButton = new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Add Body"), wxDefaultPosition, wxDefaultSize, 0 );
    searchGravityButton = new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    setupButton = new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"), wxDefaultPosition, wxDefaultSize, 0 );
    editMassButton = new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    okButton = new wxButton( parent, ID_BUTTON_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    applyButton = new wxButton( parent, ID_BUTTON_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    cancelButton = new wxButton( parent, ID_BUTTON_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    helpButton = new wxButton( parent, ID_BUTTON_HELP, wxT("Help"), wxDefaultPosition, wxDefaultSize, 0 );
    searchMagneticButton = new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    editPressureButton = new wxButton( parent, ID_BUTTON_SRP_EDIT, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wxString
    wxString strArray1[] = 
    {
        wxT("RKV 8(9)"),
        wxT("RKN 6(8)"),
        wxT("RKF 5(6)")
    };
    
    wxString strArray2[numOfBodies];
    if ( !bodiesInUse.empty() )
        for (int i = 0; i < numOfBodies; i++)
            strArray2[i] = bodiesInUse[i].c_str();
    
    wxString strArray3[] = 
    {
        wxT("None"),
        wxT("JGM-2"),
        wxT("JGM-3")
    };
    wxString strArray4[] = 
    {
        wxT("None"),
        wxT("Exponential"),
        wxT("MISISE-90")
    };
    wxString strArray5[] = 
    {
        wxT("None")
    };
    
    // wxComboBox
    integratorComboBox = new wxComboBox( parent, ID_CB_INTGR, wxT(strArray1[0]), wxDefaultPosition, wxSize(100,-1), numOfIntegrators, strArray1, wxCB_DROPDOWN|wxCB_READONLY );
    bodyComboBox = new wxComboBox( parent, ID_CB_BODY, wxT(strArray2[0]), wxDefaultPosition, wxSize(100,-1), numOfBodies, strArray2, wxCB_DROPDOWN|wxCB_READONLY );
    gravityTypeComboBox = new wxComboBox( parent, ID_CB_GRAV, wxT(strArray3[0]), wxDefaultPosition, wxSize(300,-1), numOfGraFields, strArray3, wxCB_DROPDOWN|wxCB_READONLY );
    atmosComboBox = new wxComboBox( parent, ID_CB_ATMOS, wxT(strArray4[0]), wxDefaultPosition, wxSize(150,-1), numOfAtmosTypes, strArray4, wxCB_DROPDOWN|wxCB_READONLY );
    magneticTypeComboBox = new wxComboBox( parent, ID_CB_MAG, wxT(strArray5[0]), wxDefaultPosition, wxSize(300,-1), numOfMagFields, strArray5, wxCB_DROPDOWN|wxCB_READONLY );
      
    // wxCheckBox
    srpCheckBox = new wxCheckBox( parent, ID_CHECKBOX, wxT("Use"), wxDefaultPosition, wxDefaultSize, 0 );
    
    // wx*Sizers      
    wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *boxSizer4 = new wxBoxSizer( wxHORIZONTAL );
    
    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 5, 0, 0 );
    wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 4, 0, 0 );
    wxFlexGridSizer *flexGridSizer3 = new wxFlexGridSizer( 2, 0, 2 );
     
    wxStaticBox *staticBox1 = new wxStaticBox( parent, -1, wxT("Numerical Integrator") );
    wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
    wxStaticBox *staticBox2 = new wxStaticBox( parent, -1, wxT("Environment Model") );
    wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );
    wxStaticBox *staticBox3 = new wxStaticBox( parent, -1, wxT("Primary Bodies") );
    wxStaticBoxSizer *staticBoxSizer3 = new wxStaticBoxSizer( staticBox3, wxVERTICAL );
    wxStaticBox *item37 = new wxStaticBox( parent, -1, wxT("Gravity Field") );
    wxStaticBoxSizer *item36 = new wxStaticBoxSizer( item37, wxHORIZONTAL );
    wxStaticBox *item46 = new wxStaticBox( parent, -1, wxT("Atmospheric Model") );
    wxStaticBoxSizer *item45 = new wxStaticBoxSizer( item46, wxHORIZONTAL );
    wxStaticBox *item51 = new wxStaticBox( parent, -1, wxT("Magnetic Field") );
    wxStaticBoxSizer *item50 = new wxStaticBoxSizer( item51, wxHORIZONTAL );
    wxStaticBox *staticBox7 = new wxStaticBox( parent, -1, wxT("Point Masses") );
    wxStaticBoxSizer *staticBoxSizer7 = new wxStaticBoxSizer( staticBox7, wxVERTICAL );
    wxStaticBox *item65 = new wxStaticBox( parent, -1, wxT("Solar Radiation Pressure") );
    wxStaticBoxSizer *item64 = new wxStaticBoxSizer( item65, wxHORIZONTAL );
    
    // Add to wx*Sizers    
    flexGridSizer1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer1->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer1->Add( scriptButton, 0, wxALIGN_RIGHT|wxALL, 5 );
    
    flexGridSizer2->Add( item8, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( integratorComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting1StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting1TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting2StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting2TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting3StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting3TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting4StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting4TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting5StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting5TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting6StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );  
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting6TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting7StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting7TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting8StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting8TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting9StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer2->Add( setting9TextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    boxSizer3->Add( bodyComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer3->Add( bodyTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer3->Add( bodyButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item36->Add( item42, 0, wxALIGN_CENTRE|wxALL, 5 );
    item36->Add( gravityTypeComboBox, 0, wxALIGN_CENTRE|wxALL, 5 ); 
    item36->Add( item38, 0, wxALIGN_CENTRE|wxALL, 5 );
    item36->Add( gravityDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item36->Add( item40, 0, wxALIGN_CENTRE|wxALL, 5 );
    item36->Add( gravityOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item36->Add( searchGravityButton, 0, wxALIGN_CENTRE|wxALL, 5 );

    item45->Add( item47, 0, wxALIGN_CENTRE|wxALL, 5 );   
    item45->Add( atmosComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item45->Add( setupButton, 0, wxALIGN_CENTRE|wxALL, 5 );   
    
    item50->Add( magneticTypeComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item50->Add( item52, 0, wxALIGN_CENTRE|wxALL, 5 );
    item50->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item50->Add( item54, 0, wxALIGN_CENTRE|wxALL, 5 );
    item50->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    item50->Add( item56, 0, wxALIGN_CENTRE|wxALL, 5 ); 
    item50->Add( searchMagneticButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    flexGridSizer3->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
    flexGridSizer3->Add( editMassButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item64->Add( srpCheckBox, 0, wxALIGN_CENTRE|wxALL, 5 );
    item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
    item64->Add( editPressureButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    staticBoxSizer3->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    staticBoxSizer3->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    staticBoxSizer3->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    staticBoxSizer3->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    staticBoxSizer7->Add( flexGridSizer3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    staticBoxSizer1->Add( flexGridSizer2, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    staticBoxSizer2->Add( staticBoxSizer3, 0, wxALIGN_CENTRE|wxALL, 5 );
    staticBoxSizer2->Add( staticBoxSizer7, 0, wxALIGN_CENTRE|wxALL, 5 );
    staticBoxSizer2->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    boxSizer2->Add( staticBoxSizer1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
    boxSizer2->Add( staticBoxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
           
    boxSizer4->Add( okButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer4->Add( applyButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer4->Add( cancelButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer4->Add( helpButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    boxSizer1->Add( flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer1->Add( boxSizer2, 0, wxALIGN_CENTRE|wxALL, 5 );
    boxSizer1->Add( boxSizer4, 0, wxALIGN_CENTRE|wxALL, 5 );
    
    // Disabled for Build 2
    magneticTypeComboBox->Enable(false);
    magneticDegreeTextCtrl->Enable(false);
    magneticOrderTextCtrl->Enable(false);
    searchMagneticButton->Enable(false);
    setupButton->Enable(false);
    editMassButton->Enable(false);
    editPressureButton->Enable(false);
    helpButton->Enable(false);
    
    applyButton->Enable(false);
    
    parent->SetAutoLayout(true);
    parent->SetSizer( boxSizer1 );
    boxSizer1->Fit( parent );
    boxSizer1->SetSizeHints( parent );
}

void PropagationConfigPanel::GetData()
{           
    DisplayIntegratorData();
    DisplayPrimaryBodyData();
    DisplayGravityFieldData();
    DisplayAtmosphereModelData();
    DisplayMagneticFieldData();
    DisplayPointMassData();
    DisplaySRPData();
}

void PropagationConfigPanel::SetData()
{
    integratorString = integratorComboBox->GetStringSelection();
        
    if (integratorString.Cmp("RKV 8(9)") == 0)
    {
        theRK89->SetRealParameter(Propagator::stepSizeParameter, atof(setting1TextCtrl->GetValue()) );
        theRK89->SetRealParameter(Integrator::errorControlHold, atof(setting2TextCtrl->GetValue()) );
        theRK89->SetRealParameter(Integrator::minimumStepSize, atof(setting3TextCtrl->GetValue()) );
        theRK89->SetRealParameter(Integrator::maximumStepSize, atof(setting4TextCtrl->GetValue()) );
        theRK89->SetRealParameter(Integrator::numStepAttempts, atof(setting5TextCtrl->GetValue()) );
    }
    
    if ( primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0 )
    {   
        theCelestialBody->SetIntegerParameter(orderID, atoi(gravityOrderTextCtrl->GetValue()));
        theCelestialBody->SetIntegerParameter(degreeID, atoi(gravityDegreeTextCtrl->GetValue()));
    
        //theCelestialBody->SetCentralBody(theEarth);
    }
    
    thePropSetup->SetPropagator(theRK89);
    
    /*  waw: Future implementation
    thePropSetup->SetForceModel(theForceModel);
    
    else if (integratorString.Cmp("RKN 6(8)") == 0)
    {
        // = setting1TextCtrl->GetValue();
        // = setting2TextCtrl->GetValue();
        // = setting3TextCtrl->GetValue();
        // = setting4TextCtrl->GetValue();
        // = setting5TextCtrl->GetValue();
    }
    else if (integratorString.Cmp("RKF 5(6)") == 0)
    {   
        // = setting1TextCtrl->GetValue();
        // = setting2TextCtrl->GetValue();
        // = setting3TextCtrl->GetValue();
        // = setting4TextCtrl->GetValue();
        // = setting5TextCtrl->GetValue();
        // = setting6TextCtrl->GetValue();
        // = setting7TextCtrl->GetValue();
        // = setting8TextCtrl->GetValue();
        // = setting9TextCtrl->GetValue();
    }
    
    // = primaryBodyString;
    // if ( !currentSelectedBodies.empty() )
    //     = currentSelectedBodies;
    
    // = bodyTextCtrl->GetValue();
    
    if (gravityTypeComboBox->GetStringSelection().Cmp("None") != 0)
    {
        ;  
        // = gravityTypeComboBox->GetStringSelection();
        // = gravityDegreeTextCtrl->GetValue();
        // = gravityOrderTextCtrl->GetValue();
    }
    
    if (atmosComboBox->GetStringSelection().Cmp("None") != 0)
    {
         ;  
         // = atmosComboBox->GetStringSelection();
    
    }
    if (magneticTypeComboBox->GetStringSelection().Cmp("None") != 0)
    {
        ;  
        // = magneticTypeComboBox->GetStringSelection();
        // = magneticDegreeTextCtrl->GetValue();
        // = magneticOrderTextCtrl->GetValue();
    }
    
    if ( pmEditTextCtrl->IsModified() )
    {
        ;
        // = pmEditTextCtrl->GetValue();
    }
    
    // = useSRP;
    
    //thePropSetup->SetPropagator(thePropagator);
    //thePropSetup->SetForceModel(theForceModel);
    */
}

void PropagationConfigPanel::DisplayIntegratorData()
{           
    integratorString = integratorComboBox->GetStringSelection();
    
    if (integratorString.Cmp("RKV 8(9)") == 0)
    {
        theRK89 = (RungeKutta89 *)thePropagator;
        
        setting1StaticText->SetLabel("Step Size: ");
        setting2StaticText->SetLabel("Max Int Error: ");
        setting3StaticText->SetLabel("Min Step Size: ");
        setting4StaticText->SetLabel("Max Step Size: ");
        setting5StaticText->SetLabel("Max failed steps: ");
        
        setting1TextCtrl->SetValue(wxVariant(theRK89->GetRealParameter(Propagator::stepSizeParameter)));
        setting2TextCtrl->SetValue(wxVariant(theRK89->GetRealParameter(Integrator::errorControlHold)));
        setting3TextCtrl->SetValue(wxVariant(theRK89->GetRealParameter(Integrator::minimumStepSize)));
        setting4TextCtrl->SetValue(wxVariant(theRK89->GetRealParameter(Integrator::maximumStepSize)));
        setting5TextCtrl->SetValue(wxVariant(theRK89->GetRealParameter(Integrator::numStepAttempts)));

        setting6StaticText->Show(false); 
        setting7StaticText->Show(false); 
        setting8StaticText->Show(false); 
        setting9StaticText->Show(false); 
        
        setting6TextCtrl->Show(false); 
        setting7TextCtrl->Show(false); 
        setting8TextCtrl->Show(false); 
        setting9TextCtrl->Show(false); 
    }
    else if (integratorString.Cmp("RKN 6(8)") == 0)
    {
        setting1StaticText->SetLabel("Step Size: ");
        setting2StaticText->SetLabel("Max Int Error: ");
        setting3StaticText->SetLabel("Min Step Size: ");
        setting4StaticText->SetLabel("Max Step Size: ");
        setting5StaticText->SetLabel("Max failed steps: ");
        
        setting1TextCtrl->SetValue(wxVariant()); 
        setting2TextCtrl->SetValue(wxVariant());
        setting3TextCtrl->SetValue(wxVariant());
        setting4TextCtrl->SetValue(wxVariant());
        setting5TextCtrl->SetValue(wxVariant());
        
        setting6StaticText->Show(false); 
        setting7StaticText->Show(false); 
        setting8StaticText->Show(false); 
        setting9StaticText->Show(false); 
        
        setting6TextCtrl->Show(false); 
        setting7TextCtrl->Show(false); 
        setting8TextCtrl->Show(false); 
        setting9TextCtrl->Show(false);  
    }
    else if (integratorString.Cmp("RKF 5(6)") == 0)
    {   
        setting1StaticText->SetLabel("Max Relative Error: ");
        setting2StaticText->SetLabel("Min Relative Error: ");
        setting3StaticText->SetLabel("Max Step Size: ");
        setting4StaticText->SetLabel("Max Step Size during Maneuver: ");
        setting5StaticText->SetLabel("Min Step Size: ");
        setting6StaticText->SetLabel("Initial Step Size: ");
        setting7StaticText->SetLabel("Step Size Fraction (High): ");
        setting8StaticText->SetLabel("Step Size Fraction (Low): ");
        setting9StaticText->SetLabel("Max Iterations: "); 
        
        setting1TextCtrl->SetValue(wxVariant()); 
        setting2TextCtrl->SetValue(wxVariant());
        setting3TextCtrl->SetValue(wxVariant());
        setting4TextCtrl->SetValue(wxVariant());
        setting5TextCtrl->SetValue(wxVariant());
        setting6TextCtrl->SetValue(wxVariant());
        setting7TextCtrl->SetValue(wxVariant());
        setting8TextCtrl->SetValue(wxVariant());
        setting9TextCtrl->SetValue(wxVariant());
        
        setting6StaticText->Show(true); 
        setting7StaticText->Show(true); 
        setting8StaticText->Show(true); 
        setting9StaticText->Show(true); 
        
        setting6TextCtrl->Show(true); 
        setting7TextCtrl->Show(true); 
        setting8TextCtrl->Show(true); 
        setting9TextCtrl->Show(true);          
    }
}

void PropagationConfigPanel::DisplayPrimaryBodyData()
{
    DisplayGravityFieldData();
    DisplayAtmosphereModelData();
    DisplayMagneticFieldData();
}

void PropagationConfigPanel::DisplayGravityFieldData()
{   
    primaryBodyString = bodyComboBox->GetStringSelection();
    
    if (primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0)
    {
        gravityDegreeTextCtrl->SetValue(wxVariant((long)theCelestialBody->GetIntegerParameter(degreeID)));
        gravityOrderTextCtrl->SetValue(wxVariant((long)theCelestialBody->GetIntegerParameter(orderID)));
    }
    else if (primaryBodyString.Cmp(SolarSystem::SUN_NAME.c_str()) == 0)
    {
        gravityDegreeTextCtrl->SetValue(wxVariant());
        gravityOrderTextCtrl->SetValue(wxVariant());
    }
    else if (primaryBodyString.Cmp(SolarSystem::MOON_NAME.c_str()) == 0)
    {
        gravityDegreeTextCtrl->SetValue(wxVariant());
        gravityOrderTextCtrl->SetValue(wxVariant());
    }
}

void PropagationConfigPanel::DisplayAtmosphereModelData()
{
    // To display Earth specific combo choice
    int x = atmosComboBox->FindString("None");
    int y = atmosComboBox->FindString("Exponential");
    int z = atmosComboBox->FindString("MISISE-90");
    
    // waw:  Venus & Mars for future implementation
    
    if ( primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0 )
    {                   
        if (x == -1)
            atmosComboBox->Append("None");
            
        if (y == -1)
            atmosComboBox->Append("Exponential");

        if (z == -1)
                atmosComboBox->Append("MISISE-90");    
        
        atmosComboBox->SetSelection(0); // TBD
        setupButton->Enable(true);
    }
    else
    {
        atmosComboBox->Clear();
        atmosComboBox->Append("None");
        atmosComboBox->SetSelection(0);
        
        setupButton->Enable(false);
    }
}

void PropagationConfigPanel::DisplayMagneticFieldData()
{
    // waw: Future build implementation
}

void PropagationConfigPanel::DisplayPointMassData()
{
}

void PropagationConfigPanel::DisplaySRPData()
{
    useSRP = false;  // waw: TBD
    srpCheckBox->SetValue(useSRP);
}

void PropagationConfigPanel::OnIntegratorSelection()
{
    DisplayIntegratorData();
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnBodySelection()
{
    primaryBodyString = bodyComboBox->GetStringSelection();
    
    DisplayGravityFieldData();
    DisplayAtmosphereModelData();
    DisplayMagneticFieldData();
    
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnGravitySelection()
{
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnAtmosphereSelection()
{
    applyButton->Enable(true);
}


//------------------------------------------------------------------------------
// wxMenuBar* CreateScriptWindowMenu(const std::string &docType)
//------------------------------------------------------------------------------
wxMenuBar* PropagationConfigPanel::CreateScriptWindowMenu(const std::string &docType)
{
    // Make a menubar
    wxMenu *fileMenu = new wxMenu;
    wxMenu *editMenu = (wxMenu *) NULL;
    
    fileMenu->Append(wxID_NEW, _T("&New..."));
    fileMenu->Append(wxID_OPEN, _T("&Open..."));

    if (docType == "sdi")
    {
        fileMenu->Append(wxID_CLOSE, _T("&Close"));
        fileMenu->Append(wxID_SAVE, _T("&Save"));
        fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_PRINT, _T("&Print..."));
        fileMenu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
        fileMenu->Append(wxID_PREVIEW, _T("Print Pre&view"));
    
        editMenu = new wxMenu;
        editMenu->Append(wxID_UNDO, _T("&Undo"));
        editMenu->Append(wxID_REDO, _T("&Redo"));
        editMenu->AppendSeparator();
        //editMenu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
    
        docMainFrame->editMenu = editMenu;
        fileMenu->AppendSeparator();
    }
    
    fileMenu->Append(wxID_EXIT, _T("E&xit"));
    
    // A nice touch: a history of files visited. Use this menu.
    mDocManager->FileHistoryUseMenu(fileMenu);
    
    //wxMenu *help_menu = new wxMenu;
    //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
    wxMenuBar *menuBar = new wxMenuBar;
    
    menuBar->Append(fileMenu, _T("&File"));
    
    if (editMenu)
        menuBar->Append(editMenu, _T("&Edit"));
    
    //menuBar->Append(help_menu, _T("&Help"));

    return menuBar;
}

void PropagationConfigPanel::CreateScript()
{
    //not MAC mode
    //----------------------------------------------------------------
#if !defined __WXMAC__
    // Create a document manager
    mDocManager = new wxDocManager;

    // Create a template relating text documents to their views
    mDocTemplate = 
        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                          CLASSINFO(TextDocument), CLASSINFO(MdiTextEditView));
    
    // Create the main frame window    
    mdiDocMainFrame =
        new MdiDocViewFrame(mDocManager, mdiDocMainFrame, _T("Script Window (MDI)"),
                            wxPoint(0, 0), wxSize(600, 500),
                            (wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE));
    
    // Give it an icon (this is ignored in MDI mode: uses resources)
    mdiDocMainFrame->SetIcon(wxIcon(_T("doc")));
    
    // Make a menubar
    wxMenuBar *menuBar = CreateScriptWindowMenu("mdi");
       
    // Associate the menu bar with the frame
    mdiDocMainFrame->SetMenuBar(menuBar);
    
    mdiDocMainFrame->Centre(wxBOTH);
    mdiDocMainFrame->Show(TRUE);
    //----------------------------------------------------------------
#else
    // Create a document manager
    mDocManager = new wxDocManager;

    // Create a template relating text documents to their views
    mDocTemplate = 
        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                          CLASSINFO(TextDocument), CLASSINFO(TextEditView));
    
    // Create the main frame window    
    docMainFrame =
        new DocViewFrame(mDocManager, this, -1, _T("Script Window"),
                         wxPoint(0, 0), wxSize(600, 500), wxDEFAULT_FRAME_STYLE);
        
    // Make a menubar
    wxMenuBar *menuBar = CreateScriptWindowMenu("sdi");
       
    // Associate the menu bar with the frame
    docMainFrame->SetMenuBar(menuBar);
    
    docMainFrame->Centre(wxBOTH);
    docMainFrame->Show(TRUE);
#endif
}

// wxButton Events
void PropagationConfigPanel::OnScriptButton()
{
    CreateScript();
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnOKButton()
{
    SetData();
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

void PropagationConfigPanel::OnApplyButton()
{
    SetData();
    applyButton->Enable(false);
}

void PropagationConfigPanel::OnCancelButton()
{
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

void OnHelpButton()
{
    // waw: Future build implementation
}

void PropagationConfigPanel::OnAddButton()
{
    wxString body = bodyComboBox->GetStringSelection();
    
    StringArray::iterator i;
    
    if ( !currentSelectedBodies.empty() )
    {
        Integer j = 0;
        
        for (i = currentSelectedBodies.begin(); i != currentSelectedBodies.end(); i++)
        {
            if ( body.IsSameAs(currentSelectedBodies[j].c_str(), false) )
                return;
                
            j++;
        }
    }
    
    currentSelectedBodies.push_back(body.c_str());
    bodyTextCtrl->AppendText(body + " ");
    
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnGravSearchButton()
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        
        gravityTypeComboBox->Append(filename); 
    }
        
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnSetupButton()
{
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnMagSearchButton()
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        
        filename = dialog.GetPath().c_str();
        
        magneticTypeComboBox->Append(filename); 
    }
    
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnPMEditButton()
{
    //pmSelectionDialog = 
    //    new PointMassSelectionDialog(this, ... use StringArray... );
    //pmSelectionDialog->Show(true);
}   

void PropagationConfigPanel::OnSRPEditButton()
{
    applyButton->Enable(true);
}

// wxTextCtrl Events
void PropagationConfigPanel::OnIntegratorTextUpdate()
{
    wxString set1 = setting1TextCtrl->GetValue();
    wxString set2 = setting2TextCtrl->GetValue();
    wxString set3 = setting3TextCtrl->GetValue();
    wxString set4 = setting4TextCtrl->GetValue();
    wxString set5 = setting5TextCtrl->GetValue();
    wxString set6 = setting6TextCtrl->GetValue();
    wxString set7 = setting7TextCtrl->GetValue();
    wxString set8 = setting8TextCtrl->GetValue();
    wxString set9 = setting9TextCtrl->GetValue();
    
    /* Look for IsDouble() method in stdlib
    if ( !set1.IsNumber() )
        setting1TextCtrl->Clear();
    else if ( !set2.IsNumber() )
        setting2TextCtrl->Clear();
    else if ( !set3.IsNumber() )
        setting3TextCtrl->Clear();
    else if ( !set4.IsNumber() )
        setting4TextCtrl->Clear();
    else if ( !set5.IsNumber() )
        setting5TextCtrl->Clear();
    else if ( !set6.IsNumber() )
        setting6TextCtrl->Clear();
    else if ( !set7.IsNumber() )
        setting7TextCtrl->Clear();
    else if ( !set8.IsNumber() )
        setting8TextCtrl->Clear();
    else if ( !set9.IsNumber() )
        setting9TextCtrl->Clear();
    else */
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnGravityTextUpdate()
{
    wxString deg = gravityDegreeTextCtrl->GetValue();
    wxString ord = gravityOrderTextCtrl->GetValue();
    
    if ( !deg.IsNumber() )
        gravityDegreeTextCtrl->Clear();
    else if ( !ord.IsNumber() )
        gravityOrderTextCtrl->Clear();
    else
        applyButton->Enable(true);
    
}

void PropagationConfigPanel::OnMagneticTextUpdate()
{
    applyButton->Enable(true);
}

// wxCheckBox Events
void PropagationConfigPanel::OnSRPCheckBoxChange()
{
    useSRP = srpCheckBox->GetValue();
    applyButton->Enable(true);
}
