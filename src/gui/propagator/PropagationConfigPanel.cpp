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
#include "GmatAppData.hpp"
#include "CelesBodySelectDialog.hpp"
#include "ExponentialDragDialog.hpp"
#include "MSISE90Dialog.hpp"
#include "JacchiaRobertsDialog.hpp"
#include "PropagationConfigPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Propagator.hpp"
#include "PropSetup.hpp"
#include "ForceModel.hpp"
#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PropagationConfigPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_BUTTON(ID_BUTTON_ADD_BODY, PropagationConfigPanel::OnAddButton)
   EVT_BUTTON(ID_BUTTON_GRAV_SEARCH, PropagationConfigPanel::OnGravSearchButton)
   EVT_BUTTON(ID_BUTTON_SETUP, PropagationConfigPanel::OnSetupButton)
   EVT_BUTTON(ID_BUTTON_MAG_SEARCH, PropagationConfigPanel::OnMagSearchButton)
   EVT_BUTTON(ID_BUTTON_PM_EDIT, PropagationConfigPanel::OnPMEditButton)
   EVT_BUTTON(ID_BUTTON_SRP_EDIT, PropagationConfigPanel::OnSRPEditButton)
   EVT_TEXT(ID_TEXTCTRL_PROP, PropagationConfigPanel::OnIntegratorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_GRAV, PropagationConfigPanel::OnGravityTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_MAGN, PropagationConfigPanel::OnMagneticTextUpdate)
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
   : GmatPanel(parent)
{
   propSetupName = std::string(propName.c_str());
   Create();
   Show();
   theApplyButton->Disable(); //loj: 5/11/04 added
}


PropagationConfigPanel::~PropagationConfigPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void PropagationConfigPanel::Create()
{
   Initialize(); 
   Setup(this);
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void PropagationConfigPanel::Initialize()
{  
   // Default integrator values
   newPropName      = "";
   thePropSetup     = NULL;
   thePropagator    = NULL;
   newProp          = NULL;
   isIntegratorChanged = false;
   integratorString = "RKV 8(9)";
    
   // Default force model values
   numOfAtmosTypes     = 4;
   numOfGraFields      = 4;
   numOfMagFields      = 1; 
   isForceModelChanged = false;
   isBodiesChanged     = false;
   useSRP              = false;
   useDragForce        = false;
   numOfForces         = 0;
   theForceModel       = NULL;
   theSRP              = NULL;
   theDragForce        = NULL;

   if (theGuiInterpreter != NULL)
   {        
      theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
      thePropSetup = theGuiInterpreter->GetPropSetup(propSetupName);

      // initialize integrator type array
      integratorArray.Add("RKV 8(9)");
      integratorArray.Add("RKN 6(8)");
      integratorArray.Add("RKF 5(6)");
    
      if (thePropSetup != NULL)
      {
         thePropagator = thePropSetup->GetPropagator();            
         theForceModel = thePropSetup->GetForceModel();
         numOfForces   = thePropSetup->GetNumForces();

         PhysicalModel *force;
         PointMassForce *pmf;
         CelestialBody *body;
         std::string typeName;
         std::string bodyName;
         
         //MessageInterface::ShowMessage("numOfForces=%d\n", numOfForces);
         for (Integer i = 0; i < numOfForces; i++)
         {
            force = theForceModel->GetForce(i);
            typeName = force->GetTypeName();
            
            //MessageInterface::ShowMessage("typeName=%s\n", typeName.c_str());
            
            if (force->GetTypeName() == "PointMassForce")
            {
               pmf = (PointMassForce *)force;
               thePMForces.push_back(pmf);
               bodyName = pmf->GetBodyName();
               body = theSolarSystem->GetBody(bodyName);
               
               //MessageInterface::ShowMessage("bodyName=%s\n", bodyName.c_str());
               //MessageInterface::ShowMessage("body->GetBody=%s\n", body->GetName().c_str());
               
               theBodies.push_back(body);
               primaryBodiesArray.Add(bodyName.c_str());
               primaryBodiesGravityArray.Add(typeName.c_str());
            }
            else if (force->GetTypeName() == "SolarRadiationPressure")
            {
               Integer srpID = thePropSetup->GetParameterID("SRP");
               wxString use = thePropSetup->GetStringParameter(srpID).c_str();
                    
               if (use.CmpNoCase("On") == 0)
                  useSRP = true;
               else
                  useSRP = false;
                        
               theSRP = (SolarRadiationPressure*)force;
            }
            else if (force->GetTypeName() == "DragForce")
            {
               Integer dragID = thePropSetup->GetParameterID("Drag");
               wxString use = thePropSetup->GetStringParameter(dragID).c_str();
                    
               if (use.CmpNoCase("On") == 0)
                  useDragForce = true;
               else
                  useDragForce = false;
                        
               theDragForce = (DragForce*)force;
               Integer atmosTypeID = theDragForce->GetParameterID("AtmosphereModel");
               atmosModelString = theDragForce->GetStringParameter(atmosTypeID).c_str();
            }
         }
         //MessageInterface::ShowMessage("end of numOfForces\n");

         if (primaryBodiesArray.IsEmpty())
         {
            StringArray ss = theSolarSystem->GetBodiesInUse();
            for (Integer i = 0; i < (Integer)ss.size(); i++)
            {
               primaryBodiesArray.Add(ss[i].c_str());
               primaryBodiesGravityArray.Add("NULL");
            }
         }
         
         primaryBodyString = primaryBodiesArray.Item(0).c_str();
         //MessageInterface::ShowMessage("primaryBodyString=%s\n", primaryBodyString.c_str());
         savedBodiesArray = primaryBodiesArray;
                
         if (primaryBodiesGravityArray[0].CmpNoCase("PointMassForce") == 0)
            gravityFieldString  = wxT("Point Mass");
         else
            gravityFieldString  = wxT("None");
                
         numOfBodies = (Integer)primaryBodiesArray.GetCount();
         //MessageInterface::ShowMessage("numOfBodies=%d\n", numOfBodies);
      }
      else
      {
         MessageInterface::ShowMessage("PropagationConfigPanel():Initialize() thePropSetup is NULL\n");
      }
   }
}

//------------------------------------------------------------------------------
// void Setup(wxWindow *parent)
//------------------------------------------------------------------------------
void PropagationConfigPanel::Setup(wxWindow *parent)
{              
   // wxStaticText
   integratorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"),
                        wxDefaultPosition, wxSize(250,30),
                        wxST_NO_AUTORESIZE);
   setting1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Step Size: "),
                        wxDefaultPosition, wxSize(250,30),
                        wxST_NO_AUTORESIZE );
   setting2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Int Error: "),
                        wxDefaultPosition, wxSize(250,30),
                        wxST_NO_AUTORESIZE );
   setting3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Step Size: "),
                        wxDefaultPosition, wxSize(250,30),
                        wxST_NO_AUTORESIZE );
   setting4StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Size: "),
                        wxDefaultPosition, wxSize(250,30),
                        wxST_NO_AUTORESIZE );
   setting5StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max failed steps: "),
                        wxDefaultPosition, wxSize(250,30),
                        wxST_NO_AUTORESIZE );
                           
   type1StaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
   type2StaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
   degree1StaticText = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
   order1StaticText = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );
   type3StaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
   degree2StaticText = new wxStaticText( parent, ID_TEXT, wxT("Degree"), wxDefaultPosition, wxDefaultSize, 0 );
   order2StaticText = new wxStaticText( parent, ID_TEXT, wxT("Order"), wxDefaultPosition, wxDefaultSize, 0 );

    // wxTextCtrl
   setting1TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   setting2TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   setting3TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   setting4TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   setting5TextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
   bodyTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(150,-1), wxTE_READONLY );
   gravityDegreeTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
   gravityOrderTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
   pmEditTextCtrl = new wxTextCtrl( parent, -1, wxT(""), wxDefaultPosition, wxSize(550,-1), wxTE_READONLY );
   magneticDegreeTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_MAGN, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );
   magneticOrderTextCtrl = new wxTextCtrl( parent, ID_TEXTCTRL_MAGN, wxT(""), wxDefaultPosition, wxSize(50,-1), 0 );

   // wxButton
   bodyButton = new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Add Body"), wxDefaultPosition, wxDefaultSize, 0 );
   searchGravityButton = new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
   setupButton = new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"), wxDefaultPosition, wxDefaultSize, 0 );
   editMassButton = new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
   searchMagneticButton = new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
   editPressureButton = new wxButton( parent, ID_BUTTON_SRP_EDIT, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxString
   wxString strArray1[] = 
   {
      integratorArray[0],
      integratorArray[1],
      integratorArray[2]
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
      wxT("MSISE90"),
      wxT("Jacchia-Roberts")
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

   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 2 );
        
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
   flexGridSizer1->Add( integratorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( integratorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting1TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting2TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting3TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting4TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );
   flexGridSizer1->Add( setting5TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, 5 );

   boxSizer3->Add( bodyComboBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
   boxSizer3->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
   boxSizer3->Add( bodyButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    
   item36->Add( type1StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item36->Add( gravityTypeComboBox, 0, wxALIGN_CENTRE|wxALL, 5 ); 
   item36->Add( degree1StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item36->Add( gravityDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item36->Add( order1StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item36->Add( gravityOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item36->Add( searchGravityButton, 0, wxALIGN_CENTRE|wxALL, 5 );

   item45->Add( type2StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );   
   item45->Add( atmosComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
   item45->Add( setupButton, 0, wxALIGN_CENTRE|wxALL, 5 );   
    
   item50->Add( type3StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item50->Add( magneticTypeComboBox, 0, wxALIGN_CENTRE|wxALL, 5 );
   item50->Add( degree2StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item50->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item50->Add( order2StaticText, 0, wxALIGN_CENTRE|wxALL, 5 );
   item50->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   item50->Add( searchMagneticButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   flexGridSizer2->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, 5 );
   flexGridSizer2->Add( editMassButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
   item64->Add( srpCheckBox, 0, wxALIGN_CENTRE|wxALL, 5 );
   item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, 5 );
   item64->Add( editPressureButton, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   staticBoxSizer3->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   staticBoxSizer3->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   staticBoxSizer3->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   staticBoxSizer3->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
   staticBoxSizer7->Add( flexGridSizer2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
   staticBoxSizer1->Add( flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, 5 );
    
   staticBoxSizer2->Add( staticBoxSizer3, 0, wxALIGN_CENTRE|wxALL, 5 );
   staticBoxSizer2->Add( staticBoxSizer7, 0, wxALIGN_CENTRE|wxALL, 5 );
   staticBoxSizer2->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
   boxSizer2->Add( staticBoxSizer1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
   boxSizer2->Add( staticBoxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
    
   boxSizer1->Add( boxSizer2, 0, wxALIGN_CENTRE|wxALL, 5 );

   theMiddleSizer->Add(boxSizer1, 0, wxGROW, 5);
   
   //------------------------------
   //waw: Future implementations
   //------------------------------
   editPressureButton->Show(false);  // TBD by Code 595
    
   gravityTypeComboBox->Enable(false);
   searchGravityButton->Enable(false);
   magneticTypeComboBox->Enable(false);
   magneticDegreeTextCtrl->Enable(false);
   magneticOrderTextCtrl->Enable(false);
   searchMagneticButton->Enable(false);
   
   // temporary
   bodyButton->Enable(false);
   srpCheckBox->Enable(false);
   editMassButton->Enable(false);
}

void PropagationConfigPanel::LoadData()
{
//MessageInterface::ShowMessage("Entering LoadData()");
   std::string propType = thePropagator->GetTypeName();
//MessageInterface::ShowMessage("std::string propType = thePropagator->GetTypeName();\n");
   Integer typeId = 0;
    
   if (propType == "RungeKutta89")
      typeId = RKV89_ID;    
   else if (propType == "DormandElMikkawyPrince68")
      typeId = RKN68_ID;    
   else if (propType == "RungeKuttaFehlberg56")
      typeId = RKF56_ID;    

   integratorComboBox->SetSelection(typeId);
   integratorString = integratorArray[typeId]; 
   DisplayIntegratorData(false);
//MessageInterface::ShowMessage("DisplayIntegratorData()\n");
   DisplayForceData();
//MessageInterface::ShowMessage("DisplayForceData()\n");
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveData()
{   
   //---------------------------------
   // Saving the integrator/propagator
   //---------------------------------
   if (isIntegratorChanged)
   {
      isIntegratorChanged = false;
      newProp->SetRealParameter("StepSize", atof(setting1TextCtrl->GetValue()) );
      newProp->SetRealParameter("ErrorThreshold", atof(setting2TextCtrl->GetValue()) );
      newProp->SetRealParameter("MinStep", atof(setting3TextCtrl->GetValue()) );
      newProp->SetRealParameter("MaxStep", atof(setting4TextCtrl->GetValue()) );
      newProp->SetIntegerParameter("MaxStepAttempts", atoi(setting5TextCtrl->GetValue()) );
      
      thePropSetup->SetPropagator(newProp);
   }

   //------------------------
   // Saving the force model
   //------------------------
   // the primary body data  
   if (isForceModelChanged)
   {
//MessageInterface::ShowMessage("Starting saving forcemodel...\n");
      isForceModelChanged = false;
      // Check if body is already saved
      if (isBodiesChanged)
      {
//MessageInterface::ShowMessage("Starting saving pm bodies...\n");
         isBodiesChanged = false;
         
         for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
         {       
            if (!primaryBodiesArray.IsEmpty())
            {
            for (Integer j = 0; j < (Integer)savedBodiesArray.GetCount(); j++)
               if (primaryBodiesArray[i].CmpNoCase(savedBodiesArray[j].c_str()) == 0)
                  primaryBodiesArray[i] = wxT("NULL");
            }
         }
         // If body is not in the saved array, then save
         for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
         { 
            if ( (primaryBodiesArray[i].CmpNoCase("NULL") != 0)&&
                 (primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0) )
            {
               thePMForces[i] = (PointMassForce *)theGuiInterpreter->
                  CreatePhysicalModel("PointMassForce", "");
               thePMForces[i]->SetBody(theBodies[i]);
               theForceModel->AddForce(thePMForces[i]);       
            }
         }
         // the point mass body data
         CelestialBody *body;
         PointMassForce *pm;
         if (!pointmassBodiesArray.IsEmpty())
         {
            for (Integer i = 0; i < (Integer)pointmassBodiesArray.GetCount(); i++)
            {
               body = theSolarSystem->GetBody(pointmassBodiesArray[i].c_str());
               pm = (PointMassForce *)theGuiInterpreter->CreatePhysicalModel("PointMassForce", "");
               pm->SetBody(body);
               theForceModel->AddForce(pm);
            }    
         }
      }
      // the drag force data
      if (useDragForce)
      {
         if ( atmosModelString.CmpNoCase("Exponential") == 0 )
         {
            if (!theDragForce)
                theDragForce = (DragForce *)theGuiInterpreter->
                   CreatePhysicalModel("DragForce", "Exponential");
            
            Integer atmosTypeID = theDragForce->GetParameterID("AtmosphereModel");
            theDragForce->SetStringParameter(atmosTypeID, "Exponential");
            theForceModel->AddForce(theDragForce);
            
            // Save to the PropSetup
            Integer dragID = thePropSetup->GetParameterID("Drag");
            thePropSetup->SetStringParameter(dragID, "On");
         }
         else if ( atmosModelString.CmpNoCase("MSISE90") == 0 )
         {
            if (!theDragForce)
               theDragForce = (DragForce *)theGuiInterpreter->
                  CreatePhysicalModel("DragForce", "MSISE90");
            
            Integer atmosTypeID = theDragForce->GetParameterID("AtmosphereModel");
            theDragForce->SetStringParameter(atmosTypeID, "MSISE90");
            theForceModel->AddForce(theDragForce);
            
            // Save to the PropSetup
            Integer dragID = thePropSetup->GetParameterID("Drag");
            thePropSetup->SetStringParameter(dragID, "On");
         }
      }
      else
      {
         // Save to the PropSetup
         Integer dragID = thePropSetup->GetParameterID("Drag");
         thePropSetup->SetStringParameter(dragID, "Off");
        
         // waw: Future implementation to remove drag force from force model
         //        PhysicalModel *force;
         //            
         //        for (Integer i = 0; i < numOfForces; i++)
         //        {
         //            force = theForceModel->GetForce(i);
         //           
         //            if (force->GetTypeName() == "DragForce")
         //            {
         //                useDragForce = false;
         //                wxString forceName = theSRP->GetName().c_str();
         //                theForceModel->DeleteForce(forceName.c_str());
         //            }
         //        }    
      }
    
      // the srp data
      if (useSRP)
      {
//MessageInterface::ShowMessage("Saving SRP...\n");
         if (theSRP == NULL)
         {
            theSRP = (SolarRadiationPressure *)theGuiInterpreter->
               CreatePhysicalModel("SolarRadiationPressure", "SRP");
         }
         theForceModel->AddForce(theSRP);
        
         Integer srpID = thePropSetup->GetParameterID("SRP");
         thePropSetup->SetStringParameter(srpID, "On");
      }
      else
      {
         Integer srpID = thePropSetup->GetParameterID("SRP");
         thePropSetup->SetStringParameter(srpID, "Off");
       
         // waw: Future implementation to remove srp force from force model
         //       PhysicalModel *force;
         //       
         //       for (Integer i = 0; i < numOfForces; i++)
         //       {
         //           force = theForceModel->GetForce(i);
         //           
         //           if (force->GetTypeName() == "SolarRadiationPressure")
         //           {
         //              useSRP = false;
         //              wxString forceName = theSRP->GetName().c_str();
         //              theForceModel->DeleteForce(forceName.c_str());
         //           }
         //       }
      }
      // save forces to the prop setup
      if (theForceModel != NULL)
         thePropSetup->SetForceModel(theForceModel);
//MessageInterface::ShowMessage("Finished saving forcemodel.\n");
   }
}

void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{                      
   if (integratorChanged)
   {
//MessageInterface::ShowMessage("if (integratorChanged)\n");
//MessageInterface::ShowMessage("typeName=%s\n", typeName.c_str());
      if (integratorString.IsSameAs(integratorArray[RKV89_ID]))
      {
         newPropName = propSetupName + "RKV89";
        
         newProp = theGuiInterpreter->GetPropagator(newPropName);
         if (newProp == NULL)
         {
            newProp = theGuiInterpreter->CreatePropagator("RungeKutta89", newPropName);
         }
      }
      else if (integratorString.IsSameAs(integratorArray[RKN68_ID]))
      {
         newPropName = propSetupName + "RKN68";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
         {
            newProp = theGuiInterpreter->CreatePropagator("DormandElMikkawyPrince68", newPropName);
         }
      }
      else if (integratorString.IsSameAs(integratorArray[RKF56_ID]))
      {   
         newPropName = propSetupName + "RKF56";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
         {
            newProp = theGuiInterpreter->CreatePropagator("RungeKuttaFehlberg56", newPropName);
         }            
      }
   }
   else
   {
      newProp = thePropSetup->GetPropagator();
   }
    
   // fill in data   
   setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("StepSize")));
   setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("ErrorThreshold")));
   setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("MinStep")));
   setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("MaxStep")));
   setting5TextCtrl->SetValue(wxVariant((long)newProp->GetIntegerParameter("MaxStepAttempts")));
}

void PropagationConfigPanel::DisplayForceData()
{
   DisplayPrimaryBodyData();
   DisplayGravityFieldData();
   DisplayAtmosphereModelData();
   DisplayMagneticFieldData();
   DisplaySRPData();
}

void PropagationConfigPanel::DisplayPrimaryBodyData()
{
   Integer bodyIndex = 0;
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
   
   for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
   {
      if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString) == 0 )
      {
         if ( primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0 )
         {
            gravityTypeComboBox->SetValue("Point Mass");
            degree1StaticText->Show(false);
            order1StaticText->Show(false);
            gravityDegreeTextCtrl->Show(false);
            gravityOrderTextCtrl->Show(false);
         }
         else
         {
            gravityTypeComboBox->SetValue("None");
            degree1StaticText->Show(true);
            order1StaticText->Show(true);
            gravityDegreeTextCtrl->Show(true);
            gravityOrderTextCtrl->Show(true);
                
            gravityDegreeTextCtrl->SetValue("0");
            gravityOrderTextCtrl->SetValue("0");
         }
      }
   }
}

void PropagationConfigPanel::DisplayAtmosphereModelData()
{ 
   // Atmospheric model available for Earth, Venus & Mars    
   if ( primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0 ) 
      //( primaryBodyString.Cmp(SolarSystem::VENUS_NAME.c_str()) == 0 )
      //( primaryBodyString.Cmp(SolarSystem::MARS_NAME.c_str()) == 0 )
   {                   
      atmosComboBox->SetValue(atmosModelString.c_str());
      //setupButton->Enable(true); 
   }
   else
   {
      atmosComboBox->SetValue("None");        
      setupButton->Enable(false);
   }
}

void PropagationConfigPanel::DisplayMagneticFieldData()
{
   // waw: Future build implementation
}

void PropagationConfigPanel::DisplaySRPData()
{
   srpCheckBox->SetValue(useSRP);
}

void PropagationConfigPanel::OnIntegratorSelection()
{
   if (!integratorString.IsSameAs(integratorComboBox->GetStringSelection()))
   {
      isIntegratorChanged = true;
      integratorString = integratorComboBox->GetStringSelection();
      DisplayIntegratorData(true);
      theApplyButton->Enable(true);
   }
}

void PropagationConfigPanel::OnBodySelection()
{
   if (!primaryBodyString.IsSameAs(bodyComboBox->GetStringSelection()))
   {
      primaryBodyString = bodyComboBox->GetStringSelection();
      DisplayGravityFieldData();
      DisplayAtmosphereModelData();
      DisplayMagneticFieldData();
      // isForceModelChanged = true; waw: Future implementation
      // theApplyButton->Enable(true);
   }
}

void PropagationConfigPanel::OnGravitySelection()
{
//waw: Future implementation
//   wxString gravityTypeString = gravityTypeComboBox->GetStringSelection();
//   primaryBodyString = bodyComboBox->GetStringSelection();
//
//   for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//   {
//      if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString) == 0 )
//      {     
//         if (gravityTypeString.CmpNoCase("Point Mass") == 0)
//         {
//            primaryBodiesGravityArray[i] = "PointMassForce";
//            editMassButton->Enable(true);
//            gravityDegreeTextCtrl->Enable(false);
//            gravityOrderTextCtrl->Enable(false);
//         }
//         else
//         {
//            primaryBodiesGravityArray[i] = "NULL";
//            editMassButton->Enable(false);
//            gravityDegreeTextCtrl->Enable(true);
//            gravityOrderTextCtrl->Enable(true);
//         }
//         isForceModelChanged = true;
//         theApplyButton->Enable(true);
//      }
//   }
}

void PropagationConfigPanel::OnAtmosphereSelection()
{
   atmosModelString = atmosComboBox->GetStringSelection();
   if ( ( atmosModelString.CmpNoCase("Exponential") == 0 ) ||
        ( atmosModelString.CmpNoCase("MSISE90") == 0 ) )
   {
      //useDragForce = true;
   }
   else if ( atmosModelString.CmpNoCase("None") == 0 )
   {
      //useDragForce = false;
   }
   //theApplyButton->Enable(true);
   //isForceModelChanged = true;
}

void PropagationConfigPanel::OnAddButton()
{   
   CelesBodySelectDialog bodyDlg(this, primaryBodiesArray);
   bodyDlg.ShowModal();
        
   if (bodyDlg.IsBodySelected())
   {        
      CelestialBody *body;
      wxArrayString &names = bodyDlg.GetBodyNames();
      for(Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         primaryBodiesArray.Add(names[i]);
         body = theSolarSystem->GetBody(names[i].c_str());
         theBodies.push_back(body);
         primaryBodiesGravityArray.Add("PointMassForce");
         bodyTextCtrl->AppendText(names[i] + " ");
         bodyComboBox->Append(names[i]);
      }       
      isForceModelChanged = true;
      isBodiesChanged = true;
      theApplyButton->Enable(true);
   }
    
   bool pmChanged = false;
   for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
   {
      for (Integer j = 0; j < (Integer)pointmassBodiesArray.GetCount(); j++)
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
      
      isForceModelChanged = true;
      theApplyButton->Enable(true);
   }
}

void PropagationConfigPanel::OnSetupButton()
{   
   if ( atmosModelString.CmpNoCase("Exponential") == 0 )
   {    
      // waw: Future build implementation  
      //if (theDragForce == NULL)
      //   theDragForce = (DragForce *)theGuiInterpreter->CreatePhysicalModel("DragForce", "Exponential");
            
      ExponentialDragDialog dragDlg(this, theDragForce);
      dragDlg.ShowModal();
      //theDragForce = dragDlg.GetForce();
      //useDragForce = true;
      //isForceModelChanged = true;
      //theApplyButton->Enable(true);
   }
   else if ( atmosModelString.CmpNoCase("MSISE90") == 0 )
   {        
      if (theDragForce == NULL)
         theDragForce = (DragForce *)theGuiInterpreter->CreatePhysicalModel("DragForce", "MSISE90");
            
      MSISE90Dialog dragDlg(this, theDragForce);
      dragDlg.ShowModal();
      //theDragForce = dragDlg.GetForce();
      //useDragForce = true;
      //isForceModelChanged = true;
      //theApplyButton->Enable(true);
   }
   else if ( atmosModelString.CmpNoCase("Jacchia-Roberts") == 0 )
   {        
      // waw: Future build implementation
      //if (theDragForce == NULL)
      //   theDragForce = (DragForce *)theGuiInterpreter->CreatePhysicalModel("DragForce", "JacchiaRoberts");
            
      JacchiaRobertsDialog dragDlg(this, theDragForce);
      dragDlg.ShowModal();
      //theDragForce = dragDlg.GetForce();
      //useDragForce = true;
      //isForceModelChanged = true;
      //theApplyButton->Enable(true);
   }
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
    
   theApplyButton->Enable(true);
}

void PropagationConfigPanel::OnPMEditButton()
{
   CelesBodySelectDialog bodyDlg(this, primaryBodiesArray);
   bodyDlg.ShowModal();
    
   if (bodyDlg.IsBodySelected())
   {        
      wxArrayString &names = bodyDlg.GetBodyNames();
        
      for (Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         pmEditTextCtrl->AppendText(names[i] + " ");
         pointmassBodiesArray.Add(names[i]);
      }
      isForceModelChanged = true;
      isBodiesChanged = true;
      theApplyButton->Enable(true);
   }
}   

void PropagationConfigPanel::OnSRPEditButton()
{
// waw:  Future build implementation
//   isForceModelChanged = true;
//   theApplyButton->Enable(true);
}

void PropagationConfigPanel::OnIntegratorTextUpdate()
{
   isIntegratorChanged = true;
    
   wxString set1 = setting1TextCtrl->GetValue();
   wxString set2 = setting2TextCtrl->GetValue();
   wxString set3 = setting3TextCtrl->GetValue();
   wxString set4 = setting4TextCtrl->GetValue();
   wxString set5 = setting5TextCtrl->GetValue();

   theApplyButton->Enable(true);
}

void PropagationConfigPanel::OnGravityTextUpdate(wxCommandEvent& event)
{
   // waw:  Future build implementation
   if ( event.GetEventObject() == gravityDegreeTextCtrl )
   {
   }
   else if ( event.GetEventObject() == gravityOrderTextCtrl )
   {
   }
}

void PropagationConfigPanel::OnMagneticTextUpdate(wxCommandEvent& event)
{
   // waw: Future build implementation
   if ( event.GetEventObject() == magneticDegreeTextCtrl )
   {
   }
   else if ( event.GetEventObject() == magneticOrderTextCtrl )
   {
   }
}

// wxCheckBox Events
void PropagationConfigPanel::OnSRPCheckBoxChange()
{
   useSRP = srpCheckBox->GetValue();
   isForceModelChanged = true;
   theApplyButton->Enable(true);
}
