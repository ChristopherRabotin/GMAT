//$Header$
//------------------------------------------------------------------------------
//                              PropagationConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
//
/**
 * This class contains the Propagation Configuration window.
 */
//------------------------------------------------------------------------------

// gui includes

#include "gmatwxdefs.hpp"
#include <string.h>
#include <wx/variant.h>
//#include "ViewTextFrame.hpp"
//#include "DocViewFrame.hpp"
//#include "TextEditView.hpp"
//#include "TextDocument.hpp"
//#include "MdiTextEditView.hpp"
//#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"
#include "GmatMainNotebook.hpp"
#include "CelesBodySelectDialog.hpp"
#include "PropagationConfigPanel.hpp"

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
#include "Planet.hpp"
#include "MessageInterface.hpp"

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
    EVT_TEXT(ID_CB_BODY, PropagationConfigPanel::OnBodySelection)
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
    propSetupName = std::string(propName.c_str());
    Initialize();
    Setup(this);
    LoadData();
    
    applyButton->Enable(false);
}

void PropagationConfigPanel::Initialize()
{  
    // Default integrator values
    //numOfIntegrators = 3;
    newPropName      = "";
    thePropSetup     = NULL;
    thePropagator    = NULL;
    newProp          = NULL;
    isIntegratorChanged = false;
    integratorString = "RKV 8(9)";
    
    // Default force model values
    numOfAtmosTypes     = 3;
    numOfGraFields      = 4;
    numOfMagFields      = 1;  //waw: TBD
    isForceModelChanged = false;
    numOfForces         = 0;
    theForceModel       = NULL;
    thePointMass        = NULL;
    
    theGuiInterpreter = GmatAppData::GetGuiInterpreter(); 

    if (theGuiInterpreter != NULL)
    {
        theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();

        if (theSolarSystem != NULL)
        {
            theEarth = theSolarSystem->GetBody(SolarSystem::EARTH_NAME);  
            theSun = theSolarSystem->GetBody(SolarSystem::SUN_NAME);    
            theMoon = theSolarSystem->GetBody(SolarSystem::MOON_NAME);
        }
        else
        {
            MessageInterface::ShowMessage("PropagationConfigPanel():Initialize() theSolarSystem is NULL\n");
        }
        
        thePropSetup = theGuiInterpreter->GetPropSetup(propSetupName);

        //loj: 3/12/04 debug ================================
        
        MessageInterface::ShowMessage("PropagationConfigPanel::Initialize() propTypeName = %s\n",
                                      thePropSetup->GetStringParameter("Type").c_str());
        MessageInterface::ShowMessage("PropagationConfigPanel::Initialize() fmName = %s\n",
                                      thePropSetup->GetStringParameter("FM").c_str());
                                      
        numOfForces = thePropSetup->GetNumForces();
        MessageInterface::ShowMessage("PropagationConfigPanel::Initialize() numOfForces = %d\n",
                                      numOfForces);
        
        ForceModel *fm = thePropSetup->GetForceModel();
        MessageInterface::ShowMessage("ForceModel name = %s\n", fm->GetName().c_str());
       
        for (int i=0; i<numOfForces; i++)
        {
            MessageInterface::ShowMessage("Force Type = %s, Name = %s\n",
                                          fm->GetForce(i)->GetTypeName().c_str(),
                                          fm->GetForce(i)->GetName().c_str());
        }
        //===================================================

        // initialize integrator type array
        integratorArray.Add("RKV 8(9)");
        integratorArray.Add("RKN 6(8)");
        integratorArray.Add("RKF 5(6)");
    
        if (thePropSetup != NULL)
        {
            thePropagator = thePropSetup->GetPropagator();

            //loj: do this in LoadData()
//              if (thePropagator != NULL)
//              {
//                  wxString type = thePropagator->GetTypeName().c_str();
//                  if (type.CmpNoCase("RungeKutta89") == 0)
//                      integratorString = "RKV 8(9)";
//                  else if (type.CmpNoCase("DormandElMikkawyPrince68") == 0)
//                      integratorString = "RKN 6(8)";
//                  else if (type.CmpNoCase("RungeKuttaFehlberg56") == 0)
//                      integratorString = "RKF 5(6)";
//              }
            
            theForceModel = thePropSetup->GetForceModel();
            numOfForces   = thePropSetup->GetNumForces();

            for (Integer i = 0; i < numOfForces; i++)
            {
                thePMForces.push_back((PointMassForce *)theForceModel->GetForce(i));
                thePlanets.push_back(thePMForces[i]->GetBody());
                primaryBodiesArray.Add(thePlanets[i]->GetName().c_str());
                primaryBodiesGravityArray.Add(thePMForces[i]->GetTypeName().c_str());
            }

            primaryBodyString = primaryBodiesArray.Item(0).c_str(); // waw: TBD
            savedBodiesArray = primaryBodiesArray;
                
            if (primaryBodiesGravityArray[0].CmpNoCase("PointMassForce") == 0)
                gravityFieldString  = wxT("Point Mass");
            else
                gravityFieldString  = wxT("None");
                
            numOfBodies = (Integer)primaryBodiesArray.GetCount();
        }
        else
        {
            MessageInterface::ShowMessage("PropagationConfigPanel():Initialize() thePropSetup is NULL\n");
        }
    }                     
}

void PropagationConfigPanel::Setup(wxWindow *parent)
{              
    // wxStaticText
    integratorStaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE);
    setting1StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 1"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting2StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 2"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting3StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 3"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting4StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 4"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting5StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 5"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting6StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 6"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting7StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 7"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting8StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 8"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
    setting9StaticText =
        new wxStaticText( parent, ID_TEXT, wxT("Setting 9"),
                          wxDefaultPosition, wxSize(250,30),
                          wxST_NO_AUTORESIZE );
                           
    item42 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item47 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item38 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
    item40 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item54 = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
    item56 = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    item52 = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );

    // wxTextCtrl
    setting1TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG1, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting2TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG2, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting3TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG3, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting4TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG4, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting5TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG5, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting6TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG6, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting7TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG7, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting8TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG8, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    setting9TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_INTG9, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
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
        integratorArray[0],
        integratorArray[1],
        integratorArray[2]
        //wxT("RKV 8(9)"),
        //wxT("RKN 6(8)"),
        //wxT("RKF 5(6)")
    };
    
    wxString strArray2[numOfBodies];
    if ( !primaryBodiesArray.IsEmpty() )
        for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
            strArray2[i] = primaryBodiesArray[i].c_str();
    
    wxString strArray3[] = 
    {
        wxT("None"),
        wxT("Point Mass"),
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
    integratorComboBox =
        new wxComboBox( parent, ID_CB_INTGR, wxT(integratorString),
                        wxDefaultPosition, wxSize(100,-1), IntegratorCount,
                        //wxDefaultPosition, wxSize(-1,-1), IntegratorCount,
                        strArray1, wxCB_DROPDOWN|wxCB_READONLY );
    bodyComboBox =
        new wxComboBox( parent, ID_CB_BODY, wxT(primaryBodyString),
                        wxDefaultPosition,  wxSize(100,-1), numOfBodies,
                        strArray2, wxCB_DROPDOWN|wxCB_READONLY );
    gravityTypeComboBox =
        new wxComboBox( parent, ID_CB_GRAV, wxT(gravityFieldString),
                        wxDefaultPosition, wxSize(300,-1), numOfGraFields,
                        strArray3, wxCB_DROPDOWN|wxCB_READONLY );
    atmosComboBox =
        new wxComboBox( parent, ID_CB_ATMOS, wxT(strArray4[0]),
                        wxDefaultPosition, wxSize(150,-1), numOfAtmosTypes,
                        strArray4, wxCB_DROPDOWN|wxCB_READONLY );
    magneticTypeComboBox =
        new wxComboBox( parent, ID_CB_MAG, wxT(strArray5[0]),
                        wxDefaultPosition, wxSize(300,-1), numOfMagFields,
                        strArray5, wxCB_DROPDOWN|wxCB_READONLY );
      
    // wxCheckBox
    srpCheckBox = new wxCheckBox( parent, ID_CHECKBOX, wxT("Use"),
                                  wxDefaultPosition, wxDefaultSize, 0 );
   
    // wx*Sizers      
    wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *boxSizer4 = new wxBoxSizer( wxHORIZONTAL );
    
    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 5, 0, 0 );
    wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 0 );
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
    
    flexGridSizer2->Add( integratorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( integratorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting1TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting2TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting3TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting4TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting5TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );  
    flexGridSizer2->Add( setting6TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting7StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting7TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting8StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting8TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting9StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
    flexGridSizer2->Add( setting9TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );

    boxSizer3->Add( bodyComboBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    boxSizer3->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    boxSizer3->Add( bodyButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    
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
    
    // waw: future implementation
    helpButton->Enable(false);
    
    parent->SetAutoLayout(true);
    parent->SetSizer( boxSizer1 );
    boxSizer1->Fit( parent );
    boxSizer1->SetSizeHints( parent );
}

void PropagationConfigPanel::LoadData()
{           
    std::string propType = thePropagator->GetTypeName();
    int typeId;
    
    if (propType == "RungeKutta89")
        typeId = RKV89_ID;    
    else if (propType == "DormandElMikkawyPrince68")
        typeId = RKN68_ID;    
    else if (propType == "RungeKuttaFehlberg56")
        typeId = RKF56_ID;    

    integratorComboBox->SetSelection(typeId);
    integratorString = integratorArray[typeId];
    
    DisplayIntegratorData(false);
    DisplayForceData();
}

void PropagationConfigPanel::SaveData()
{   
    //---------------------------------
    // Saving the integrator/propagator
    //---------------------------------
    if (isIntegratorChanged)
    {
        integratorString = integratorComboBox->GetStringSelection();
        
    
        if (integratorString.Cmp("RKV 8(9)") == 0 || integratorString.IsSameAs("RKN 6(8)"))
        {
            newProp->SetRealParameter("StepSize", atof(setting1TextCtrl->GetValue()) );
            newProp->SetRealParameter("ErrorThreshold", atof(setting2TextCtrl->GetValue()) );
            newProp->SetRealParameter("MinStep", atof(setting3TextCtrl->GetValue()) );
            newProp->SetRealParameter("MaxStep", atof(setting4TextCtrl->GetValue()) );
            newProp->SetIntegerParameter("MaxStepAttempts", atoi(setting5TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Propagator::STEP_SIZE, atof(setting1TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::ERROR_THRESHOLD, atof(setting2TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::MIN_STEP, atof(setting3TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::MAX_STEP, atof(setting4TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::MAX_STEP_ATTEMPTS, atof(setting5TextCtrl->GetValue()) );
        }
        //else if (integratorString.IsSameAs("RKN 6(8)"))
        //{
        //    newProp->SetRealParameter(Propagator::STEP_SIZE, atof(setting1TextCtrl->GetValue()) );
        //    newProp->SetRealParameter(Integrator::ERROR_THRESHOLD, atof(setting2TextCtrl->GetValue()) );
        //    newProp->SetRealParameter(Integrator::MIN_STEP, atof(setting3TextCtrl->GetValue()) );
        //    newProp->SetRealParameter(Integrator::MAX_STEP, atof(setting4TextCtrl->GetValue()) );
        //    newProp->SetRealParameter(Integrator::MAX_STEP_ATTEMPTS, atof(setting5TextCtrl->GetValue()) );
        //}
        else if (integratorString.IsSameAs("RKF 5(6)"))
        {
            newProp->SetRealParameter("StepSize", atof(setting1TextCtrl->GetValue()) );
            newProp->SetRealParameter("ErrorThreshold", atof(setting2TextCtrl->GetValue()) );
            newProp->SetRealParameter("MinStep", atof(setting3TextCtrl->GetValue()) );
            newProp->SetRealParameter("MaxStep", atof(setting4TextCtrl->GetValue()) );
            newProp->SetIntegerParameter("MaxStepAttempts", atoi(setting5TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Propagator::STEP_SIZE, atof(setting1TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::ERROR_THRESHOLD, atof(setting2TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::MIN_STEP, atof(setting3TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::MAX_STEP, atof(setting4TextCtrl->GetValue()) );
            //newProp->SetRealParameter(Integrator::MAX_STEP_ATTEMPTS, atof(setting5TextCtrl->GetValue()) );
        }
        
        thePropSetup->SetPropagator(newProp);
        MessageInterface::ShowMessage("PropagationConfigPanel():SetData() newPropType = %s name = %s\n",
            newProp->GetTypeName().c_str(), newProp->GetName().c_str());
    }
    
    //------------------------
    // Saving the force model
    //------------------------
    // the primary body data  
    for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
    {       
        if (!primaryBodiesArray.IsEmpty())
        {
            for (Integer j = 0; j < (Integer)savedBodiesArray.GetCount(); j++)
                if (primaryBodiesArray[i].CmpNoCase(savedBodiesArray[j].c_str()) == 0)
                    primaryBodiesArray[i] = wxT("NULL");
        }
    }
    for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
    { 
        if (primaryBodiesArray[i].CmpNoCase("NULL") != 0)
        {
            thePMForces[i] = (PointMassForce *)theGuiInterpreter->CreatePhysicalModel("PointMassForce", "");
            thePMForces[i]->SetBody(thePlanets[i]);
            theForceModel->AddForce(thePMForces[i]);        
        }
    }
    // the point mass body data
    Planet *body;
    PointMassForce *pm;
    if (!pointmassBodiesArray.IsEmpty())
    {
        for (Integer i = 0; i < (Integer)pointmassBodiesArray.GetCount(); i++)
        {
            body = new Planet(pointmassBodiesArray[i].c_str());
            pm = (PointMassForce *)theGuiInterpreter->CreatePhysicalModel("PointMassForce", "");
            pm->SetBody(body);
            theForceModel->AddForce(pm);
        }    
    }
    
    if (theForceModel != NULL)
        thePropSetup->SetForceModel(theForceModel);

//    if (isForceModelChanged)
//        UpdateForceModel();   
//    
//    if (atmosComboBox->GetStringSelection().Cmp("None") != 0)
//    {
//         ;  
//          = atmosComboBox->GetStringSelection();
//    
//    }
//    if (magneticTypeComboBox->GetStringSelection().Cmp("None") != 0)
//    {
//        ;  
//         = magneticTypeComboBox->GetStringSelection();
//         = magneticDegreeTextCtrl->GetValue();
//         = magneticOrderTextCtrl->GetValue();
//    }
//    
//     = useSRP;
}

void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{            
    //integratorString = integratorComboBox->GetStringSelection();
    
    if (integratorChanged)
    {
        if (integratorString.IsSameAs(integratorArray[RKV89_ID]))
        {
            newPropName = propSetupName + "RKV89";
        
            newProp = theGuiInterpreter->GetPropagator(newPropName);
            if (newProp == NULL)
            {
                MessageInterface::ShowMessage("PropConfigPanel::DisplayIntegratorData() "
                                              "Creating RungeKutta89\n");
                newProp = theGuiInterpreter->CreatePropagator("RungeKutta89", newPropName);
            }
        }
        else if (integratorString.IsSameAs(integratorArray[RKN68_ID]))
        {
            newPropName = propSetupName + "RKN68";
            newProp = theGuiInterpreter->GetPropagator(newPropName);
        
            if (newProp == NULL)
            {
                MessageInterface::ShowMessage("PropConfigPanel::DisplayIntegratorData() "
                                              "Creating DormandElMikkawyPrince68\n");
                newProp = theGuiInterpreter->CreatePropagator("DormandElMikkawyPrince68", newPropName);
            }

            //loj: 3/19/04 added but need to test this
            ForceModel *fm = theGuiInterpreter->CreateForceModel(newPropName + "ForceModel");
            PhysicalModel *earthGrav = theGuiInterpreter->CreatePhysicalModel("PointMassForce", "");
            fm->AddForce(earthGrav);
        }
        else if (integratorString.IsSameAs(integratorArray[RKF56_ID]))
        {   
            newPropName = propSetupName + "RKF56";
            newProp = theGuiInterpreter->GetPropagator(newPropName);
        
            if (newProp == NULL)
            {
                MessageInterface::ShowMessage("PropConfigPanel::DisplayIntegratorData() "
                                              "Creating RungeKuttaFehlberg56\n");
                newProp = theGuiInterpreter->CreatePropagator("RungeKuttaFehlberg56", newPropName);
            }
        }
    }
    else
    {
        newProp = thePropSetup->GetPropagator(); //loj: because script doesn't have propagator name
        //newProp = theGuiInterpreter->GetPropagator(propSetupName);
    }
    
    // fill in data
    if (integratorString.IsSameAs("RKV 8(9)") || integratorString.IsSameAs("RKN 6(8)"))
    {
        setting1StaticText->SetLabel("Step Size: ");
        setting2StaticText->SetLabel("Max Int Error: ");
        setting3StaticText->SetLabel("Min Step Size: ");
        setting4StaticText->SetLabel("Max Step Size: ");
        setting5StaticText->SetLabel("Max failed steps: ");
        
        setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("StepSize")));
        setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("ErrorThreshold")));
        setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("MinStep")));
        setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("MaxStep")));
        setting5TextCtrl->SetValue(wxVariant((long)newProp->GetIntegerParameter("MaxStepAttempts")));

        setting6StaticText->Show(false); 
        setting7StaticText->Show(false); 
        setting8StaticText->Show(false); 
        setting9StaticText->Show(false); 
        
        setting6TextCtrl->Show(false); 
        setting7TextCtrl->Show(false); 
        setting8TextCtrl->Show(false); 
        setting9TextCtrl->Show(false); 
    }
    else if (integratorString.IsSameAs("RKF 5(6)"))
    {   
        // use newProp to retrive and set value
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
        
        // setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting5TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting6TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting7TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting8TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        // setting9TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));

        setting6StaticText->Show(true); 
        setting7StaticText->Show(true); 
        setting8StaticText->Show(true); 
        setting9StaticText->Show(true); 
        
        setting6TextCtrl->Show(true); 
        setting7TextCtrl->Show(true); 
        setting8TextCtrl->Show(true); 
        setting9TextCtrl->Show(true);          
    }
    
//      if (integratorString.Cmp(integratorArray[RKV89_ID]) == 0)
//      {
//          newPropName = propSetupName + "RKV89";
        
//          newProp = theGuiInterpreter->GetPropagator(newPropName);
//          if (newProp == NULL)
//          {
//              MessageInterface::ShowMessage("Creating RungeKutta89\n");
//              newProp = theGuiInterpreter->CreatePropagator("RungeKutta89", newPropName);
//          }
        
//          setting1StaticText->SetLabel("Step Size: ");
//          setting2StaticText->SetLabel("Max Int Error: ");
//          setting3StaticText->SetLabel("Min Step Size: ");
//          setting4StaticText->SetLabel("Max Step Size: ");
//          setting5StaticText->SetLabel("Max failed steps: ");
        
//          setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Propagator::STEP_SIZE)));
//          setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::ERROR_THRESHOLD)));
//          setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::MIN_STEP)));
//          setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::MAX_STEP)));
//          setting5TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::MAX_STEP_ATTEMPTS)));

//          setting6StaticText->Show(false); 
//          setting7StaticText->Show(false); 
//          setting8StaticText->Show(false); 
//          setting9StaticText->Show(false); 
        
//          setting6TextCtrl->Show(false); 
//          setting7TextCtrl->Show(false); 
//          setting8TextCtrl->Show(false); 
//          setting9TextCtrl->Show(false); 
//      }
//      else if (integratorString.Cmp("RKN 6(8)") == 0)
//      {
//          newPropName = propSetupName + "RKN68";
//          newProp = theGuiInterpreter->GetPropagator(newPropName);
        
//          if (newProp == NULL)
//          {
//              MessageInterface::ShowMessage("Creating DormandElMikkawyPrince68\n");
//              newProp = theGuiInterpreter->CreatePropagator("DormandElMikkawyPrince68", newPropName);
//              MessageInterface::ShowMessage("PropagationConfigPanel:DisplayIntegratorData() newProp = %s\n",
//                                            newProp->GetTypeName().c_str());
//          }

//          setting1StaticText->SetLabel("Step Size: ");
//          setting2StaticText->SetLabel("Max Int Error: ");
//          setting3StaticText->SetLabel("Min Step Size: ");
//          setting4StaticText->SetLabel("Max Step Size: ");
//          setting5StaticText->SetLabel("Max failed steps: ");
        
//          setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Propagator::STEP_SIZE)));
//          setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::ERROR_THRESHOLD)));
//          setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::MIN_STEP)));
//          setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::MAX_STEP)));
//          setting5TextCtrl->SetValue(wxVariant(newProp->GetRealParameter(Integrator::MAX_STEP_ATTEMPTS)));
        
//          setting6StaticText->Show(false); 
//          setting7StaticText->Show(false); 
//          setting8StaticText->Show(false); 
//          setting9StaticText->Show(false); 
        
//          setting6TextCtrl->Show(false); 
//          setting7TextCtrl->Show(false); 
//          setting8TextCtrl->Show(false); 
//          setting9TextCtrl->Show(false);  
//      }
//      else if (integratorString.Cmp("RKF 5(6)") == 0)
//      {   
//          newPropName = propSetupName + "RKF56";
//          newProp = theGuiInterpreter->GetPropagator(newPropName);
        
//          if (newProp == NULL)
//          {
//              MessageInterface::ShowMessage("Creating RungeKuttaFehlberg56\n");
//              newProp = theGuiInterpreter->CreatePropagator("RungeKuttaFehlberg56", newPropName);
//          }
        
//          setting1StaticText->SetLabel("Max Relative Error: ");
//          setting2StaticText->SetLabel("Min Relative Error: ");
//          setting3StaticText->SetLabel("Max Step Size: ");
//          setting4StaticText->SetLabel("Max Step Size during Maneuver: ");
//          setting5StaticText->SetLabel("Min Step Size: ");
//          setting6StaticText->SetLabel("Initial Step Size: ");
//          setting7StaticText->SetLabel("Step Size Fraction (High): ");
//          setting8StaticText->SetLabel("Step Size Fraction (Low): ");
//          setting9StaticText->SetLabel("Max Iterations: "); 
        
//  //        setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting5TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting6TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting7TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting8TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
//  //        setting9TextCtrl->SetValue(wxVariant(newProp->GetRealParameter()));
        
//          setting6StaticText->Show(true); 
//          setting7StaticText->Show(true); 
//          setting8StaticText->Show(true); 
//          setting9StaticText->Show(true); 
        
//          setting6TextCtrl->Show(true); 
//          setting7TextCtrl->Show(true); 
//          setting8TextCtrl->Show(true); 
//          setting9TextCtrl->Show(true);          
//      }
}

void PropagationConfigPanel::DisplayForceData()
{    
    degreeID = theEarth->GetParameterID("Degree");
    orderID = theEarth->GetParameterID("Order");
    earthDegreeString.Printf("%d", theEarth->GetIntegerParameter(degreeID));
    earthOrderString.Printf("%d", theEarth->GetIntegerParameter(orderID));
    
    degreeID = theSun->GetParameterID("Degree");
    orderID = theSun->GetParameterID("Order");
    sunDegreeString.Printf("%d", theSun->GetIntegerParameter(degreeID));
    sunOrderString.Printf("%d", theSun->GetIntegerParameter(orderID));
    
    degreeID = theMoon->GetParameterID("Degree");
    orderID = theMoon->GetParameterID("Order");
    moonDegreeString.Printf("%d", theMoon->GetIntegerParameter(degreeID));
    moonOrderString.Printf("%d", theMoon->GetIntegerParameter(orderID));
    
    DisplayPrimaryBodyData();
    DisplayGravityFieldData();
    DisplayAtmosphereModelData();
    DisplayMagneticFieldData();
    DisplayPointMassData();
    DisplaySRPData();
}

void PropagationConfigPanel::DisplayPrimaryBodyData()
{
    Integer bodyIndex;
    for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
    {
        bodyTextCtrl->AppendText(primaryBodiesArray.Item(i) + " ");
        if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString.c_str()) == 0 )
            bodyIndex = i;
    }
    bodyComboBox->SetSelection(bodyIndex);
}

void PropagationConfigPanel::DisplayGravityFieldData()
{            
    primaryBodyString = bodyComboBox->GetStringSelection();
    Integer index = bodyComboBox->GetSelection();
    
    if (primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0)
    {     
      gravityDegreeTextCtrl->SetValue(earthDegreeString);
      gravityOrderTextCtrl->SetValue(earthOrderString);

      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
      {
          if ( primaryBodiesArray[i].CmpNoCase(SolarSystem::EARTH_NAME.c_str()) == 0 )
          {
              if ( primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0 )
                  gravityTypeComboBox->SetValue("Point Mass");
              else
                  gravityTypeComboBox->SetValue("None");
          }
      }
    }
    else if (primaryBodyString.Cmp(SolarSystem::SUN_NAME.c_str()) == 0)
    {       
      gravityDegreeTextCtrl->SetValue(sunDegreeString);
      gravityOrderTextCtrl->SetValue(sunOrderString);

      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
      {
          if ( primaryBodiesArray[i].CmpNoCase(SolarSystem::SUN_NAME.c_str()) == 0 )
          {
              if ( primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0 )
                  gravityTypeComboBox->SetValue("Point Mass");
              else
                  gravityTypeComboBox->SetValue("None");              
          }
      }
    }
    else if (primaryBodyString.Cmp(SolarSystem::MOON_NAME.c_str()) == 0)
    {
      gravityDegreeTextCtrl->SetValue(moonDegreeString);
      gravityOrderTextCtrl->SetValue(moonOrderString);

      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
      {
          if ( primaryBodiesArray[i].CmpNoCase(SolarSystem::MOON_NAME.c_str()) == 0 )
          {
              if ( primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0 )
                  gravityTypeComboBox->SetValue("Point Mass");
              else
                  gravityTypeComboBox->SetValue("None");
          }
      }
    }
    
    // For Point Mass Edit
    gravityFieldString = gravityTypeComboBox->GetStringSelection();
    
    if (gravityFieldString.Cmp("Point Mass") == 0)
    {
        editMassButton->Enable(true);
        gravityDegreeTextCtrl->Enable(false);
        gravityOrderTextCtrl->Enable(false);
    }
    else
    {
        editMassButton->Enable(false);
        gravityDegreeTextCtrl->Enable(true);
        gravityOrderTextCtrl->Enable(true);
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
    if (!integratorString.IsSameAs(integratorComboBox->GetStringSelection()))
    {
        isIntegratorChanged = true;
        integratorString = integratorComboBox->GetStringSelection();
        DisplayIntegratorData(true);
        applyButton->Enable(true);
    }
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
    wxString gravityTypeString = gravityTypeComboBox->GetStringSelection();
    
    if (gravityTypeString.CmpNoCase("Point Mass") == 0)
    {
        editMassButton->Enable(true);
        gravityDegreeTextCtrl->Enable(false);
        gravityOrderTextCtrl->Enable(false);
    }
    else
    {
        editMassButton->Enable(false);
        gravityDegreeTextCtrl->Enable(true);
        gravityOrderTextCtrl->Enable(true);
    }
    
    if (primaryBodyString.CmpNoCase(SolarSystem::EARTH_NAME.c_str()) == 0)
    {     
//        earthGravityString = gravityTypeString;
    }
    else if (primaryBodyString.CmpNoCase(SolarSystem::SUN_NAME.c_str()) == 0)
    {       
//        sunGravityString = gravityTypeString;
    }
    else if (primaryBodyString.CmpNoCase(SolarSystem::MOON_NAME.c_str()) == 0)
    {
//        moonGravityString = gravityTypeString;
    }

    isForceModelChanged = true;
    applyButton->Enable(true);
}

void PropagationConfigPanel::OnAtmosphereSelection()
{
    applyButton->Enable(true);
    isForceModelChanged = true;
}


//------------------------------------------------------------------------------
// wxMenuBar* CreateScriptWindowMenu(const std::string &docType)
//------------------------------------------------------------------------------
//wxMenuBar* PropagationConfigPanel::CreateScriptWindowMenu(const std::string &docType)
//{
//    // Make a menubar
//    wxMenu *fileMenu = new wxMenu;
//    wxMenu *editMenu = (wxMenu *) NULL;
//  
//    fileMenu->Append(wxID_NEW, _T("&New..."));
//    fileMenu->Append(wxID_OPEN, _T("&Open..."));
//
//    if (docType == "sdi")
//    {
//        fileMenu->Append(wxID_CLOSE, _T("&Close"));
//        fileMenu->Append(wxID_SAVE, _T("&Save"));
//        fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
//        fileMenu->AppendSeparator();
//        fileMenu->Append(wxID_PRINT, _T("&Print..."));
//        fileMenu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
//        fileMenu->Append(wxID_PREVIEW, _T("Print Pre&view"));
//   
//        editMenu = new wxMenu;
//        editMenu->Append(wxID_UNDO, _T("&Undo"));
//        editMenu->Append(wxID_REDO, _T("&Redo"));
//        editMenu->AppendSeparator();
//        //editMenu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
//  
//        docMainFrame->editMenu = editMenu;
//        fileMenu->AppendSeparator();
//    }
//    
//    fileMenu->Append(wxID_EXIT, _T("E&xit"));
//    
//    // A nice touch: a history of files visited. Use this menu.
//    mDocManager->FileHistoryUseMenu(fileMenu);
//    
//    //wxMenu *help_menu = new wxMenu;
//    //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
//    
//    wxMenuBar *menuBar = new wxMenuBar;
//    
//    menuBar->Append(fileMenu, _T("&File"));
//    
//    if (editMenu)
//        menuBar->Append(editMenu, _T("&Edit"));
//    
//    //menuBar->Append(help_menu, _T("&Help"));
//
//    return menuBar;
//}
//
//void PropagationConfigPanel::CreateScript()
//{
//    //not MAC mode
//    //----------------------------------------------------------------
//    // NOTE:  it is temporary until it is fixed for Mac
//    // #if !defined __WXMAC__
//    //----------------------------------------------------------------
//  #if 1 
//    // Create a document manager
//    mDocManager = new wxDocManager;
//
//    // Create a template relating text documents to their views
//    mDocTemplate = 
//        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
//                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
//                          CLASSINFO(TextDocument), CLASSINFO(MdiTextEditView));
//  
//    // Create the main frame window    
//    mdiDocMainFrame =
//        new MdiDocViewFrame(mDocManager, mdiDocMainFrame, _T("Script Window (MDI)"),
//                            wxPoint(0, 0), wxSize(600, 500),
//                            (wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE));
//    
//    // Give it an icon (this is ignored in MDI mode: uses resources)
//    mdiDocMainFrame->SetIcon(wxIcon(_T("doc")));
//  
//    // Make a menubar
//    wxMenuBar *menuBar = CreateScriptWindowMenu("mdi");
//       
//    // Associate the menu bar with the frame
//    mdiDocMainFrame->SetMenuBar(menuBar);
//    
//    mdiDocMainFrame->Centre(wxBOTH);
//    mdiDocMainFrame->Show(TRUE);
//    //----------------------------------------------------------------
//  #else
//    // Create a document manager
//    mDocManager = new wxDocManager;
//
//    // Create a template relating text documents to their views
//    mDocTemplate = 
//        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
//                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
//                          CLASSINFO(TextDocument), CLASSINFO(TextEditView));
//    
//    // Create the main frame window    
//    docMainFrame =
//        new DocViewFrame(mDocManager, this, -1, _T("Script Window"),
//                         wxPoint(0, 0), wxSize(600, 500), wxDEFAULT_FRAME_STYLE);
//      
//    // Make a menubar
//    wxMenuBar *menuBar = CreateScriptWindowMenu("sdi");
//      
//    // Associate the menu bar with the frame
//    docMainFrame->SetMenuBar(menuBar);
//    
//    docMainFrame->Centre(wxBOTH);
//    docMainFrame->Show(TRUE);
//  #endif
//}

// wxButton Events
void PropagationConfigPanel::OnScriptButton()
{
//    CreateScript();
//    applyButton->Enable(true);
}

void PropagationConfigPanel::OnOKButton()
{
    if (applyButton->IsEnabled())
    {
        SaveData();     
    }
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

void PropagationConfigPanel::OnApplyButton()
{
    SaveData();
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
    CelesBodySelectDialog bodyDlg(this, primaryBodiesArray);
    bodyDlg.ShowModal();
        
    if (bodyDlg.IsBodySelected())
    {        
        Planet *body;
        wxArrayString &names = bodyDlg.GetBodyNames();
        for(Integer i=0; i < (Integer)names.GetCount(); i++)
        {
            primaryBodiesArray.Add(names[i]);
            body = new Planet(names[i].c_str());
            thePlanets.push_back(body);
            primaryBodiesGravityArray.Add("Point Mass");
            bodyTextCtrl->AppendText(names[i] + " ");
            bodyComboBox->Append(names[i]);
        }       
        applyButton->Enable(true);
    }
    
    bool pmChanged = false;
    for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
    {
        for (Integer j = 0; j < pointmassBodiesArray.GetCount(); j++)
        {
            if (primaryBodiesArray[i].CmpNoCase(pointmassBodiesArray[j]) == 0)
            {
                wxString body = pointmassBodiesArray.Item(j);
                pointmassBodiesArray.Remove(body);
                pmChanged = true;
            }
        
        }
    }
    
    if (pmChanged)
    {
        pmEditTextCtrl->Clear();
        
        if (!pointmassBodiesArray.IsEmpty())
        {
            for (Integer i = 0; i < (Integer)pointmassBodiesArray.GetCount(); i++)
            {
                pmEditTextCtrl->AppendText(pointmassBodiesArray.Item(i));
                pmEditTextCtrl->AppendText(" ");
            }
        }
            
    }
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
    CelesBodySelectDialog bodyDlg(this, primaryBodiesArray);
    bodyDlg.ShowModal();
    
    if (bodyDlg.IsBodySelected())
    {        
        Planet *body;
        wxArrayString &names = bodyDlg.GetBodyNames();
        
        for (Integer i=0; i < (Integer)names.GetCount(); i++)
        {
            pmEditTextCtrl->AppendText(names[i] + " ");
            pointmassBodiesArray.Add(names[i]);
        }
        applyButton->Enable(true);
    }
}   

void PropagationConfigPanel::OnSRPEditButton()
{
    applyButton->Enable(true);
}

// wxTextCtrl Events
void PropagationConfigPanel::OnIntegratorTextUpdate()
{
    isIntegratorChanged = true;
    
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
    
    if (!deg.IsNumber())
    {
        gravityDegreeTextCtrl->Clear();
        return;
    }
    else if (!ord.IsNumber())
    {
        gravityOrderTextCtrl->Clear();
        return;
    }
    else
    {
        if (primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0)
        {      
            earthDegreeString = deg;
            earthOrderString = ord;
        }
        else if (primaryBodyString.Cmp(SolarSystem::SUN_NAME.c_str()) == 0)
        {
            sunDegreeString = deg;
            sunOrderString = ord;
        }
        else if (primaryBodyString.Cmp(SolarSystem::MOON_NAME.c_str()) == 0)
        {
            moonDegreeString = deg;
            moonOrderString = ord;
        }  
        applyButton->Enable(true);     
    } 
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

//loj: 3/3/04 added
ForceModel* PropagationConfigPanel::UpdateForceModel()
{
    if (isForceModelChanged)
    {
        // create and add force to force model
        //loj: for example
        //PhysicalModel *pm = theGuiInterpreter->CreatePhysicalModel("forcename", propName+"forcename");
        //theForceModel->AddForce(pm);
    }

    return theForceModel;
}

