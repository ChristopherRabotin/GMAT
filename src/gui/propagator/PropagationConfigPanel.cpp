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
#include "GravityField.hpp"
#include "MessageInterface.hpp"

#define DEBUG_PROP_PANEL 0

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PropagationConfigPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)

   EVT_BUTTON(ID_BUTTON_ADD_BODY, PropagationConfigPanel::OnAddBodyButton)
   EVT_BUTTON(ID_BUTTON_GRAV_SEARCH, PropagationConfigPanel::OnGravSearchButton)
   EVT_BUTTON(ID_BUTTON_SETUP, PropagationConfigPanel::OnSetupButton)
   EVT_BUTTON(ID_BUTTON_MAG_SEARCH, PropagationConfigPanel::OnMagSearchButton)
   EVT_BUTTON(ID_BUTTON_PM_EDIT, PropagationConfigPanel::OnPMEditButton)
   EVT_BUTTON(ID_BUTTON_SRP_EDIT, PropagationConfigPanel::OnSRPEditButton)
   EVT_TEXT(ID_TEXTCTRL_PROP, PropagationConfigPanel::OnIntegratorTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_GRAV, PropagationConfigPanel::OnGravityTextUpdate)
   EVT_TEXT(ID_TEXTCTRL_MAGF, PropagationConfigPanel::OnMagneticTextUpdate)
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
   forceList.clear();
   
   isForceModelChanged = false;
   isGravTextChanged = false;
   isPotFileChanged = false;
   isMagfTextChanged = false;
   isBodiesChanged = false;
   isIntegratorChanged = false;
   
   // Default integrator values
   newPropName      = "";
   thePropSetup     = NULL;
   thePropagator    = NULL;
   newProp          = NULL;
   integratorString = "RKV 8(9)";
    
   // Default force model values
   useSRP              = false;
   useDragForce        = false;
   numOfForces         = 0;
   theForceModel       = NULL;
   theSRP              = NULL;
   theDragForce        = NULL;
   theGravForce        = NULL;

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
// Integer FindBody(const std::string &bodyName, const std::string &gravType,...)
//------------------------------------------------------------------------------
Integer PropagationConfigPanel::FindBody(const std::string &bodyName,
                                         const std::string &gravType,
                                         const std::string &dragType,
                                         const std::string &magfType)
{
   for (Integer i=0; i<(Integer)forceList.size(); i++)
   {
      if (forceList[i]->bodyName == bodyName)
         return i;
   }

   // add body
   forceList.push_back(new ForceType(bodyName, gravType, dragType, magfType));

#if DEBUG_PROP_PANEL
   ShowForceList("FindBody() after add body to forceList");
#endif
   
   return (Integer)(forceList.size() - 1);
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void PropagationConfigPanel::Initialize()
{  

   if (theGuiInterpreter != NULL)
   {        
      theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
      thePropSetup = theGuiInterpreter->GetPropSetup(propSetupName);

      // initialize integrator type array
      integratorArray.Add("RKV 8(9)");
      integratorArray.Add("RKN 6(8)");
      integratorArray.Add("RKF 5(6)");
    
      // initialize gravity model type array
      gravModelArray.push_back("None");
      gravModelArray.push_back("Point Mass");
      gravModelArray.push_back("JGM-2");
      gravModelArray.push_back("JGM-3");
      gravModelArray.push_back("Other");
      
      // initialize drag model type array
      dragModelArray.push_back("None");
      dragModelArray.push_back("Exponential");
      dragModelArray.push_back("MSISE90");
      dragModelArray.push_back("Jacchia-Roberts");
      
      // initialize mag. filed model type array
      magfModelArray.push_back("None");
      
      if (thePropSetup != NULL)
      {
         thePropagator = thePropSetup->GetPropagator();
         newProp = thePropagator;
         theForceModel = thePropSetup->GetForceModel();
         numOfForces   = thePropSetup->GetNumForces();
         
#if DEBUG_PROP_PANEL
         ShowPropData("Initialize() entering");
#endif
         Integer paramId;
         PhysicalModel *force;
         PointMassForce *pmf;
         CelestialBody *body;
         std::string typeName;
         std::string bodyName;
         
         for (Integer i = 0; i < numOfForces; i++)
         {
            force = theForceModel->GetForce(i);
            typeName = force->GetTypeName();
            
            if (force->GetTypeName() == "PointMassForce")
            {
               pmf = (PointMassForce *)force;
               //thePMForces.push_back(pmf);
               bodyName = pmf->GetBodyName();
               body = theSolarSystem->GetBody(bodyName);
            
               //theBodies.push_back(body);
               primaryBodiesArray.Add(bodyName.c_str());
               primaryBodiesGravityArray.Add(typeName.c_str());                    
               ////thePMForces.push_back((PointMassForce *)force);

               currentBodyId = FindBody(bodyName, gravModelArray[POINT_MASS]);
               forceList[currentBodyId]->bodyName = bodyName;
               forceList[currentBodyId]->gravType = gravModelArray[POINT_MASS];
               
               //theBodies.push_back(thePMForces[i]->GetBody());
               //primaryBodiesArray.Add(theBodies[i]->GetName().c_str());
               //primaryBodiesGravityArray.Add(thePMForces[i]->GetTypeName().c_str());
                    
               // waw: Future implementation
               //degreeID = theBodies[i]->GetParameterID("Degree");
               //orderID = theBodies[i]->GetParameterID("Order");
               //degreeArray.Add(wxVariant((long)theBodies[i]->GetIntegerParameter(degreeID)));
               //orderArray.Add(wxVariant((long)theBodies[i]->GetIntegerParameter(orderID)));
            }
            else if (force->GetTypeName() == "SolarRadiationPressure")
            {
               paramId = thePropSetup->GetParameterID("SRP");
               wxString use = thePropSetup->GetStringParameter(paramId).c_str();
                    
               if (use.CmpNoCase("On") == 0)
                  useSRP = true;
               else
                  useSRP = false;
                        
               theSRP = (SolarRadiationPressure*)force;
            }
            else if (force->GetTypeName() == "DragForce")
            {
               paramId = thePropSetup->GetParameterID("Drag");
               wxString use = thePropSetup->GetStringParameter(paramId).c_str();
                    
               if (use.CmpNoCase("On") == 0)
                  useDragForce = true;
               else
                  useDragForce = false;
                        
               theDragForce = (DragForce*)force;
               paramId = theDragForce->GetParameterID("AtmosphereModel");
               atmosModelString = theDragForce->GetStringParameter(paramId).c_str();

               paramId = theDragForce->GetParameterID("AtmosphereBody");
               bodyName = theDragForce->GetStringParameter(paramId);
               
               currentBodyId = FindBody(bodyName);
               forceList[currentBodyId]->bodyName = bodyName;
               forceList[currentBodyId]->dragType = atmosModelString;
            }
         }
         
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
         currentBodyName = std::string(primaryBodyString.c_str());
         currentBodyId = FindBody(currentBodyName);
         savedBodiesArray = primaryBodiesArray;
         
         //MessageInterface::ShowMessage("primaryBodyString=%s\n", primaryBodyString.c_str());
                
         if (primaryBodiesGravityArray[0].CmpNoCase("PointMassForce") == 0)
            gravityFieldString  = wxT("Point Mass");
         else
            gravityFieldString  = wxT("None");
                
         numOfBodies = (Integer)primaryBodiesArray.GetCount();
         
#if DEBUG_PROP_PANEL
         ShowPropData("Initialize() exiting");
#endif
      }
      else
      {
         MessageInterface::ShowMessage
            ("PropagationConfigPanel():Initialize() thePropSetup is NULL\n");
      }
   }
}

//------------------------------------------------------------------------------
// void Setup(wxWindow *parent)
//------------------------------------------------------------------------------
void PropagationConfigPanel::Setup(wxWindow *parent)
{              
   //MessageInterface::ShowMessage("Setup() entered\n");
   
   // wxStaticText
   //loj: 5/19/04 changed wxSize(250,30) to wxSize(80,20)
   integratorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"),
                        wxDefaultPosition, wxSize(80,20),
                        wxST_NO_AUTORESIZE);
   setting1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Step Size: "),
                        wxDefaultPosition, wxSize(80,20),
                        wxST_NO_AUTORESIZE );
   setting2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Int Error: "),
                        wxDefaultPosition, wxSize(80,20),
                        wxST_NO_AUTORESIZE );
   setting3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Step Size: "),
                        wxDefaultPosition, wxSize(80,20),
                        wxST_NO_AUTORESIZE );
   setting4StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Size: "),
                        wxDefaultPosition, wxSize(80,20),
                        wxST_NO_AUTORESIZE );
   setting5StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max failed steps: "),
                        wxDefaultPosition, wxSize(80,20),
                        wxST_NO_AUTORESIZE );
                           
   type1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   type2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   degree1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   order1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   type3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   degree2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   order2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   potFileStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Other Potential Field File:"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
    // wxTextCtrl
   setting1TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );
   setting2TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );
   setting3TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );
   setting4TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );
   setting5TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(80,-1), 0 );
   bodyTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
   gravityDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );
   gravityOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );
   potFileTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(300,-1), 0 );
   pmEditTextCtrl =
      new wxTextCtrl( parent, -1, wxT(""), wxDefaultPosition,
                      wxSize(360,-1), wxTE_READONLY );
   magneticDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );
   magneticOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(40,-1), 0 );

   // wxButton
   bodyButton =
      new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Add Body"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   searchGravityButton =
      new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   setupButton =
      new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   editMassButton =
      new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Edit"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   searchMagneticButton =
      new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   editPressureButton =
      new wxButton( parent, ID_BUTTON_SRP_EDIT, wxT("Edit"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   // wxString
   wxString strArray1[] = 
   {
      integratorArray[0],
      integratorArray[1],
      integratorArray[2]
   };

   wxString strArray2[] =
   {
   };
   
   wxString strArray3[] = 
   {
      wxT(gravModelArray[0].c_str()),
      wxT(gravModelArray[1].c_str()),
      wxT(gravModelArray[2].c_str()),
      wxT(gravModelArray[3].c_str()),
      wxT(gravModelArray[4].c_str())
   };
   
   wxString strArray4[] = 
   {
      wxT(dragModelArray[0].c_str()),
      wxT(dragModelArray[1].c_str()),
      wxT(dragModelArray[2].c_str()),
      wxT(dragModelArray[3].c_str())
   };
   
   wxString strArray5[] = 
   {
      wxT(magfModelArray[0].c_str())
   };
    
   // wxComboBox
   integratorComboBox =
      new wxComboBox( parent, ID_CB_INTGR, wxT(integratorString),
                      wxDefaultPosition, wxSize(80,-1), IntegratorCount,
                      strArray1, wxCB_DROPDOWN|wxCB_READONLY );
   bodyComboBox =
      new wxComboBox( parent, ID_CB_BODY, wxT(primaryBodyString),
                      //wxDefaultPosition,  wxSize(100,-1), numOfBodies,
                      wxDefaultPosition,  wxSize(100,-1), 0,
                      strArray2, wxCB_DROPDOWN|wxCB_READONLY );
   
   if ( !primaryBodiesArray.IsEmpty() )
      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
         bodyComboBox->Append(primaryBodiesArray[i]);
   
   gravComboBox =
      new wxComboBox( parent, ID_CB_GRAV, wxT(gravityFieldString),
                      wxDefaultPosition, wxSize(100,-1), GravModelCount,
                      strArray3, wxCB_DROPDOWN|wxCB_READONLY );
   atmosComboBox =
      new wxComboBox( parent, ID_CB_ATMOS, wxT(strArray4[0]),
                      wxDefaultPosition, wxSize(100,-1), DragModelCount,
                      strArray4, wxCB_DROPDOWN|wxCB_READONLY );
   magfComboBox =
      new wxComboBox( parent, ID_CB_MAG, wxT(strArray5[0]),
                      wxDefaultPosition, wxSize(100,-1), MagfModelCount,
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
   //loj: 5/20/04 wxStaticBoxSizer *item36 = new wxStaticBoxSizer( item37, wxHORIZONTAL );
   wxStaticBoxSizer *item36 = new wxStaticBoxSizer( item37, wxVERTICAL );
   wxBoxSizer *item361 = new wxBoxSizer( wxHORIZONTAL ); //loj: 5/20/04
   wxBoxSizer *item362 = new wxBoxSizer( wxHORIZONTAL ); //loj: 5/20/04
   
   wxStaticBox *item46 = new wxStaticBox( parent, -1, wxT("Atmospheric Model") );
   wxStaticBoxSizer *item45 = new wxStaticBoxSizer( item46, wxHORIZONTAL );
   wxStaticBox *item51 = new wxStaticBox( parent, -1, wxT("Magnetic Field") );
   wxStaticBoxSizer *item50 = new wxStaticBoxSizer( item51, wxHORIZONTAL );
   wxStaticBox *staticBox7 = new wxStaticBox( parent, -1, wxT("Point Masses") );
   wxStaticBoxSizer *staticBoxSizer7 = new wxStaticBoxSizer( staticBox7, wxVERTICAL );
   wxStaticBox *item65 = new wxStaticBox( parent, -1, wxT("Solar Radiation Pressure") );
   wxStaticBoxSizer *item64 = new wxStaticBoxSizer( item65, wxHORIZONTAL );

   Integer bsize = 3; // border size
   
   // Add to wx*Sizers  
   flexGridSizer1->Add( integratorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( integratorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting1TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting2TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting3TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting4TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting5TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

   boxSizer3->Add( bodyComboBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   boxSizer3->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   boxSizer3->Add( bodyButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
    
   item361->Add( type1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item361->Add( gravComboBox, 0, wxALIGN_CENTRE|wxALL, bsize); 
   item361->Add( degree1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item361->Add( gravityDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item361->Add( order1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item361->Add( gravityOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item361->Add( searchGravityButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //loj: 5/20/04 added
   item362->Add( potFileStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item362->Add( potFileTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);

   item36->Add( item361, 0, wxALIGN_LEFT|wxALL, bsize);
   item36->Add( item362, 0, wxALIGN_LEFT|wxALL, bsize);
   
   item45->Add( type2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);   
   item45->Add( atmosComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item45->Add( setupButton, 0, wxALIGN_CENTRE|wxALL, bsize);   
    
   item50->Add( type3StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( magfComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( degree2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( order2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( searchMagneticButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   flexGridSizer2->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add( editMassButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( srpCheckBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( editPressureButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   staticBoxSizer3->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
    
   //staticBoxSizer7->Add( flexGridSizer2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer7->Add( flexGridSizer2, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
    
   staticBoxSizer1->Add( flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   staticBoxSizer2->Add( staticBoxSizer3, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer2->Add( staticBoxSizer7, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer2->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
    
   boxSizer2->Add( staticBoxSizer1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer2->Add( staticBoxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
    
   boxSizer1->Add( boxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);

   theMiddleSizer->Add(boxSizer1, 0, wxGROW, bsize);
   
   //------------------------------
   //waw: Future implementations
   //------------------------------
   editPressureButton->Show(false);  // TBD by Code 595
    
   gravComboBox->Enable(true);
   searchGravityButton->Enable(false);
   magfComboBox->Enable(true);
   magneticDegreeTextCtrl->Enable(false);
   magneticOrderTextCtrl->Enable(false);
   searchMagneticButton->Enable(false);
   
   bodyButton->Enable(true);
   srpCheckBox->Enable(true);
   editMassButton->Enable(false);
   setupButton->Enable(false);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::LoadData()
{
   //MessageInterface::ShowMessage("LoadData() entered\n");
   std::string propType = newProp->GetTypeName();
   //MessageInterface::ShowMessage("std::string propType = newProp->GetTypeName();\n");
   Integer typeId = 0;
    
   if (propType == "RungeKutta89")
      typeId = RKV89;    
   else if (propType == "DormandElMikkawyPrince68")
      typeId = RKN68;    
   else if (propType == "RungeKuttaFehlberg56")
      typeId = RKF56;    

   integratorComboBox->SetSelection(typeId);
   integratorString = integratorArray[typeId]; 
   DisplayIntegratorData(false);
   DisplayForceData();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveData()
{
   //-----------------------------------
   // Saving the Integrator
   //-----------------------------------
   if (isIntegratorChanged)
   {
#if DEBUG_PROP_PANEL
   ShowPropData("SaveData() BEFORE saving Integrator");
#endif
      isIntegratorChanged = false;
        
      newProp->SetRealParameter("StepSize", atof(setting1TextCtrl->GetValue()));
      newProp->SetRealParameter("ErrorThreshold", atof(setting2TextCtrl->GetValue()));
      newProp->SetRealParameter("MinStep", atof(setting3TextCtrl->GetValue()));
      newProp->SetRealParameter("MaxStep", atof(setting4TextCtrl->GetValue()));
      newProp->SetIntegerParameter("MaxStepAttempts", atoi(setting5TextCtrl->GetValue()));
        
      thePropSetup->SetPropagator(newProp);
#if DEBUG_PROP_PANEL
   ShowPropData("SaveData() AFTER  saving Integrator");
#endif
   }
   
   //-----------------------------------
   // Saving the force model
   //-----------------------------------
   if (isForceModelChanged)
   {
      isForceModelChanged = false;
      
#if DEBUG_PROP_PANEL
      ShowForceList("SaveData() BEFORE saving ForceModel");
#endif
      ForceModel *newFm = new ForceModel();
      
      Integer paramId;
      PointMassForce *pmForce;

      //----------------------------------------------------
      // save gravity force model
      //----------------------------------------------------
      for (unsigned int i=0; i<forceList.size(); i++)
      {
         if (forceList[i]->gravType != gravModelArray[NONE_GM])
         {            
            if (forceList[i]->gravType == gravModelArray[POINT_MASS])
            {
               pmForce = new PointMassForce();
               pmForce->SetBodyName(forceList[i]->bodyName);
               newFm->AddForce(pmForce);
            }
            else
            {
               if (forceList[i]->gravType == gravModelArray[JGM2])
               {
                  theGravForce = new GravityField("", forceList[i]->bodyName);
                  potFilename = theGuiInterpreter->GetPotentialFileName("JGM2");
               }
               else if (forceList[i]->gravType == gravModelArray[JGM3])
               {
                  potFilename = theGuiInterpreter->GetPotentialFileName("JGM2");
               }
               else // using potential file name as type
               {
                  potFilename = std::string(potFileTextCtrl->GetValue().c_str());
               }

               theGravForce = new GravityField("", forceList[i]->bodyName);
               
               theGravForce->SetStringParameter("Filename", potFilename);
               theGravForce->SetIntegerParameter
                  ("Degree", atoi(gravityDegreeTextCtrl->GetValue()));
               theGravForce->SetIntegerParameter
                  ("Order",  atoi(gravityOrderTextCtrl->GetValue()));
               
               newFm->AddForce(theGravForce);

#if DEBUG_PROP_PANEL
               MessageInterface::ShowMessage
                  ("potFilename=%s degree=%d order=%d\n",
                   theGravForce->GetStringParameter("Filename").c_str(),
                   theGravForce->GetIntegerParameter("Degree"),
                   theGravForce->GetIntegerParameter("Order"));
#endif
            }
         }
      }
      
      //----------------------------------------------------
      // save drag force model
      //----------------------------------------------------
         
      for (unsigned int i=0; i<forceList.size(); i++)
      {
         if (forceList[i]->dragType != dragModelArray[NONE_DM])
         {
            theDragForce = new DragForce();
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            theDragForce->SetStringParameter(paramId, forceList[i]->dragType);
            paramId = theDragForce->GetParameterID("AtmosphereBody");
            theDragForce->SetStringParameter(paramId, forceList[i]->bodyName);
            newFm->AddForce(theDragForce);
         }
      }

      // save forces to the prop setup
      thePropSetup->SetForceModel(newFm);
      numOfForces = thePropSetup->GetNumForces();

      if (theForceModel->GetName() == "")
         delete theForceModel;
      
      theForceModel = newFm;
      
#if DEBUG_PROP_PANEL
      ShowForceList("SaveData() AFTER  saving ForceModel");
#endif
   }
   else if (isGravTextChanged || isPotFileChanged)
   {
      if (isGravTextChanged)
      {
         isGravTextChanged = false;
         theGravForce->SetIntegerParameter
            ("Degree", atoi(gravityDegreeTextCtrl->GetValue()));
         theGravForce->SetIntegerParameter
            ("Order",  atoi(gravityOrderTextCtrl->GetValue()));
      }
      
      if (isPotFileChanged)
      {
         isPotFileChanged = false;
         potFilename = std::string(potFileTextCtrl->GetValue().c_str());
         theGravForce->SetStringParameter("Filename", potFilename);
      }
      
#if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage("Degree, Order, or potFilename change saved\n");
      MessageInterface::ShowMessage
         ("potFilename=%s degree=%d order=%d\n",
          theGravForce->GetStringParameter("Filename").c_str(),
          theGravForce->GetIntegerParameter("Degree"),
          theGravForce->GetIntegerParameter("Order"));
#endif
      
   } // end if(isForceModelChange)
   
   
//        // the primary body data  
//        for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//        {       
//           if (!primaryBodiesArray.IsEmpty())
//           {
//              for (Integer j = 0; j < (Integer)savedBodiesArray.GetCount(); j++)
//                 if (primaryBodiesArray[i].CmpNoCase(savedBodiesArray[j].c_str()) == 0)
//                    primaryBodiesArray[i] = wxT("NULL");
//           }
//        }
//        for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//        { 
//           if ( (primaryBodiesArray[i].CmpNoCase("NULL") != 0)&&
//                (primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0) )
//           {
//              thePMForces[i] = (PointMassForce *)theGuiInterpreter->
//                 CreatePhysicalModel("PointMassForce", "");
//              thePMForces[i]->SetBody(theBodies[i]);
//              theForceModel->AddForce(thePMForces[i]);       
//           }
//        }
//        // the point mass body data
//        CelestialBody *body;
//        PointMassForce *pm;
//        if (!pointmassBodiesArray.IsEmpty())
//        {
//           for (Integer i = 0; i < (Integer)pointmassBodiesArray.GetCount(); i++)
//           {
//              body = theSolarSystem->GetBody(pointmassBodiesArray[i].c_str());
//              pm = (PointMassForce *)theGuiInterpreter->CreatePhysicalModel("PointMassForce", "");
//              pm->SetBody(body);
//              theForceModel->AddForce(pm);
//           }    
//        }
//        // the drag force data
//        if (useDragForce)
//        {
//           if ( atmosModelString.CmpNoCase("Exponential") == 0 )
//           {
//              if (theDragForce == NULL)
//                 theDragForce = (DragForce *)theGuiInterpreter->
//                    CreatePhysicalModel("DragForce", "Exponential");
//              Integer atmosTypeID = theDragForce->GetParameterID("AtmosphereModel");
//              theDragForce->SetStringParameter(atmosTypeID, "Exponential");
//              theForceModel->AddForce(theDragForce);
            
//              // Save to the PropSetup
//              Integer dragID = thePropSetup->GetParameterID("Drag");
//              thePropSetup->SetStringParameter(dragID, "On");
//           }
//           else if ( atmosModelString.CmpNoCase("MSISE90") == 0 )
//           {
//              if (!theDragForce)
//                 theDragForce = (DragForce *)theGuiInterpreter->
//                    CreatePhysicalModel("DragForce", "MSISE90");
            
//              Integer atmosTypeID = theDragForce->GetParameterID("AtmosphereModel");
//              theDragForce->SetStringParameter(atmosTypeID, "MSISE90");
//              theForceModel->AddForce(theDragForce);
            
//              // Save to the PropSetup
//              Integer dragID = thePropSetup->GetParameterID("Drag");
//              thePropSetup->SetStringParameter(dragID, "On");
//           }
//        }
//        else
//        {
//           // Save to the PropSetup
//           Integer dragID = thePropSetup->GetParameterID("Drag");
//           thePropSetup->SetStringParameter(dragID, "Off");
        
//           // waw: Future implementation to remove drag force from force model
//           //        PhysicalModel *force;
//           //            
//           //        for (Integer i = 0; i < numOfForces; i++)
//           //        {
//           //            force = theForceModel->GetForce(i);
//           //           
//           //            if (force->GetTypeName() == "DragForce")
//           //            {
//           //                useDragForce = false;
//           //                wxString forceName = theSRP->GetName().c_str();
//           //                theForceModel->DeleteForce(forceName.c_str());
//           //            }
//           //        }    
//        }
//        // the srp data
//        if (useSRP)
//        {
//           if (theSRP == NULL)
//              theSRP = (SolarRadiationPressure *)theGuiInterpreter->
//                 CreatePhysicalModel("SolarRadiationPressure", "SRP");

//           theForceModel->AddForce(theSRP);
        
//           Integer srpID = thePropSetup->GetParameterID("SRP");
//           thePropSetup->SetStringParameter(srpID, "On");
//        }
//        else
//        {
//           Integer srpID = thePropSetup->GetParameterID("SRP");
//           thePropSetup->SetStringParameter(srpID, "Off");
       
//           // waw: Future implementation to remove srp force from force model
//           //       PhysicalModel *force;
//           //       
//           //       for (Integer i = 0; i < numOfForces; i++)
//           //       {
//           //           force = theForceModel->GetForce(i);
//           //           
//           //           if (force->GetTypeName() == "SolarRadiationPressure")
//           //           {
//           //              useSRP = false;
//           //              wxString forceName = theSRP->GetName().c_str();
//           //              theForceModel->DeleteForce(forceName.c_str());
//           //           }
//           //       }
//        }
//        // save forces to the prop setup
//        if (theForceModel != NULL)
//           thePropSetup->SetForceModel(theForceModel);
           
}

//------------------------------------------------------------------------------
// void DisplayIntegratorData(bool integratorChanged)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{
   //MessageInterface::ShowMessage("DisplayIntegratorData() integratorChanged=%d\n",
   //                              integratorChanged);
   
   if (integratorChanged)
   {
      if (integratorString.IsSameAs(integratorArray[RKV89]))
      {
         newPropName = propSetupName + "RKV89";
        
         newProp = theGuiInterpreter->GetPropagator(newPropName);
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("RungeKutta89", newPropName);
      }
      else if (integratorString.IsSameAs(integratorArray[RKN68]))
      {
         newPropName = propSetupName + "RKN68";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("DormandElMikkawyPrince68", newPropName);
      }
      else if (integratorString.IsSameAs(integratorArray[RKF56]))
      {   
         newPropName = propSetupName + "RKF56";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("RungeKuttaFehlberg56", newPropName);
      }
   }
   else
   {
      newProp = thePropSetup->GetPropagator();
   }
    
#if DEBUG_PROP_PANEL
   ShowPropData("DisplayIntegratorData() exiting...");
#endif
         
   // fill in data   
   setting1TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("StepSize")));
   setting2TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("ErrorThreshold")));
   setting3TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("MinStep")));
   setting4TextCtrl->SetValue(wxVariant(newProp->GetRealParameter("MaxStep")));
   setting5TextCtrl->SetValue(wxVariant((long)newProp->GetIntegerParameter("MaxStepAttempts")));
}

//------------------------------------------------------------------------------
// void DisplayForceData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayForceData()
{
   DisplayPrimaryBodyData();
   DisplayGravityFieldData();
   DisplayAtmosphereModelData();
   DisplayMagneticFieldData();
   DisplaySRPData();
}

//------------------------------------------------------------------------------
// void DisplayPrimaryBodyData()
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// void DisplayGravityFieldData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayGravityFieldData()
{            
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("DisplayGravityFieldData() currentBodyName=%s gravType=%s\n",
       currentBodyName.c_str(), forceList[currentBodyId]->gravType.c_str());
#endif
   
   searchGravityButton->Enable(false);
   
   if (forceList[currentBodyId]->gravType == gravModelArray[POINT_MASS])
   {
      gravComboBox->SetSelection(POINT_MASS);
      gravityDegreeTextCtrl->SetValue("0");
      gravityOrderTextCtrl->SetValue("0");
      gravityDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
      potFileStaticText->Enable(false);
      potFileTextCtrl->Enable(false);
   }
   else if (forceList[currentBodyId]->gravType == gravModelArray[JGM2] ||
            forceList[currentBodyId]->gravType == gravModelArray[JGM3])
   {
      if (forceList[currentBodyId]->gravType == gravModelArray[JGM2])
         gravComboBox->SetSelection(JGM2);
      else
         gravComboBox->SetSelection(JGM3);
         
      gravityDegreeTextCtrl->SetValue("4");
      gravityOrderTextCtrl->SetValue("4");
      gravityDegreeTextCtrl->Enable(true);
      gravityOrderTextCtrl->Enable(true);
   }
   else if (forceList[currentBodyId]->gravType == gravModelArray[OTHER])
   {
      gravComboBox->SetSelection(OTHER);
      gravityDegreeTextCtrl->SetValue("4");
      gravityOrderTextCtrl->SetValue("4");
      gravityDegreeTextCtrl->Enable(true);
      gravityOrderTextCtrl->Enable(true);
      potFileStaticText->Enable(true);
      potFileTextCtrl->Enable(true);
      searchGravityButton->Enable(true);
   }

   // TextCtrl->SetValue() fires EVT_TEXT event
   isGravTextChanged = false;

//     for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//     {
//        if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString) == 0 )
//        {
//           if ( primaryBodiesGravityArray[i].CmpNoCase("PointMassForce") == 0 )
//           {
//              gravComboBox->SetValue("Point Mass");
//              //                gravityDegreeTextCtrl->SetValue("0");
//              //                gravityOrderTextCtrl->SetValue("0");
//              //                gravityDegreeTextCtrl->Enable(false);
//              //                gravityOrderTextCtrl->Enable(false);
//           }
//           else
//           {
//              gravComboBox->SetValue("None");
//              //                gravityDegreeTextCtrl->Enable(true);
//              //                gravityOrderTextCtrl->Enable(true);
                
//              //                if (degreeArray.IsEmpty())
//              //                   gravityDegreeTextCtrl->SetValue("0");
//              //                else
//              //                   gravityDegreeTextCtrl->SetValue(degreeArray.Item(i));
//              //                if (orderArray.IsEmpty())
//              //                   gravityOrderTextCtrl->SetValue("0");
//              //                else
//              //                   gravityOrderTextCtrl->SetValue(orderArray.Item(i));
//           }
//        }
//     }
}

//------------------------------------------------------------------------------
// void DisplayAtmosphereModelData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayAtmosphereModelData()
{
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() currentBodyName=%s dragType=%s\n",
       currentBodyName.c_str(), forceList[currentBodyId]->dragType.c_str());
#endif
  
   if (forceList[currentBodyId]->dragType == dragModelArray[NONE_DM])
   {
      atmosComboBox->SetSelection(NONE_DM);
      setupButton->Enable(false);
   }
   else if (forceList[currentBodyId]->dragType == dragModelArray[EXPONENTIAL])
   {
      atmosComboBox->SetSelection(EXPONENTIAL); //loj: why not working?
      setupButton->Enable(true);
   }
   else if (forceList[currentBodyId]->dragType == dragModelArray[MSISE90])
   {
      atmosComboBox->SetSelection(MSISE90);
      setupButton->Enable(true);
   }
   else if (forceList[currentBodyId]->dragType == dragModelArray[JR])
   {
      atmosComboBox->SetSelection(JR);
      setupButton->Enable(true);
   }
   
//     // Atmospheric model available for Earth, Venus & Mars    
//     if ( primaryBodyString.Cmp(SolarSystem::EARTH_NAME.c_str()) == 0 ) 
//        //( primaryBodyString.Cmp(SolarSystem::VENUS_NAME.c_str()) == 0 )
//        //( primaryBodyString.Cmp(SolarSystem::MARS_NAME.c_str()) == 0 )
//     {                   
//        atmosComboBox->SetValue(atmosModelString.c_str());
//        //setupButton->Enable(true); 
//     }
//     else
//     {
//        atmosComboBox->SetValue("None");        
//        setupButton->Enable(false);
//     }
}

//------------------------------------------------------------------------------
// void DisplayMagneticFieldData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayMagneticFieldData()
{
   if (forceList[currentBodyId]->magfType == magfModelArray[NONE_MM])
   {
      magfComboBox->SetSelection(NONE_MM);
   }
}

//------------------------------------------------------------------------------
// void DisplaySRPData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplaySRPData()
{
   srpCheckBox->SetValue(useSRP);
}

//------------------------------------------------------------------------------
// void OnIntegratorSelection()
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// void OnBodySelection()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnBodySelection()
{
   std::string selBody = bodyComboBox->GetStringSelection().c_str();
   
   if (currentBodyName != selBody)
   {
      primaryBodyString = bodyComboBox->GetStringSelection();
      currentBodyName = selBody;
      currentBodyId = FindBody(currentBodyName);
      
      DisplayGravityFieldData();
      DisplayAtmosphereModelData();
      DisplayMagneticFieldData();
      //theApplyButton->Enable(true);
   }
}

//------------------------------------------------------------------------------
// void OnGravitySelection()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravitySelection()
{   
   gravTypeName = std::string(gravComboBox->GetStringSelection().c_str());

   if (forceList[currentBodyId]->gravType != gravTypeName)
   {
      
#if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage
         ("OnGravitySelection() grav changed from=%s to=%s for body=%s\n",
          forceList[currentBodyId]->gravType.c_str(), gravTypeName.c_str(),
          forceList[currentBodyId]->bodyName.c_str());
#endif
      
      forceList[currentBodyId]->gravType = gravTypeName;
      DisplayGravityFieldData();
            
      isForceModelChanged = true;
      theApplyButton->Enable(true);
   }
      
//waw: Future implementation
//   wxString gravityTypeString = gravComboBox->GetStringSelection();
//   primaryBodyString = bodyComboBox->GetStringSelection();
//
//     for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//     {
//        if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString) == 0 )
//        {     
//           if (gravityTypeString.CmpNoCase("Point Mass") == 0)
//           {
//              primaryBodiesGravityArray[i] = "PointMassForce";
//              editMassButton->Enable(true);
//              //gravityDegreeTextCtrl->Enable(false);
//              //gravityOrderTextCtrl->Enable(false);
//           }
//           else //waw:future implementation
//           {
//              primaryBodiesGravityArray[i] = "NULL";
//              editMassButton->Enable(false);
//              //gravityDegreeTextCtrl->Enable(true);
//              //gravityOrderTextCtrl->Enable(true);
//           }
//           isForceModelChanged = true;
//           theApplyButton->Enable(true);
//        }
//     }
}

//------------------------------------------------------------------------------
// void OnAtmosphereSelection()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAtmosphereSelection()
{
#if DEBUG_PROP_SETUP
   MessageInterface::ShowMessage("OnAtmosphereSelection() body=%s\n",
                                 forceList[currentBodyId]->bodyName.c_str());
#endif
   
   dragTypeName = std::string(atmosComboBox->GetStringSelection().c_str());

   if (forceList[currentBodyId]->dragType != dragTypeName)
   {
#if DEBUG_PROP_SETUP
      MessageInterface::ShowMessage
         ("OnAtmosphereSelection() grav changed from=%s to=%s for body=%s\n",
          forceList[currentBodyId]->dragType.c_str(), dragTypeName.c_str(),
          forceList[currentBodyId]->bodyName.c_str());
#endif
      
      forceList[currentBodyId]->dragType = dragTypeName;
      DisplayAtmosphereModelData();
         
      isForceModelChanged = true;
      theApplyButton->Enable(true);
   }
   
//     atmosModelString = atmosComboBox->GetStringSelection();
//     if ( ( atmosModelString.CmpNoCase("Exponential") == 0 ) ||
//          ( atmosModelString.CmpNoCase("MSISE90") == 0 ) )
//     {
//        jrFileStaticText->Show(false);
//        jrFileTextCtrl->Show(false);
//        jrFileButton->Show(false);
        
//        useDragForce = true;
//     }
//     else if ( atmosModelString.CmpNoCase("Jacchia-Roberts") == 0 )
//     {
//        jrFileStaticText->Show(true);
//        jrFileTextCtrl->Show(true);
//        jrFileButton->Show(true);
        
//        useDragForce = false;
//     }
//     else if ( atmosModelString.CmpNoCase("None") == 0 )
//     {
//        jrFileStaticText->Show(false);
//        jrFileTextCtrl->Show(false);
//        jrFileButton->Show(false);
        
//        useDragForce = false;
//     }
//     isForceModelChanged = true;
//     theApplyButton->Enable(true);
}

// wxButton events
//------------------------------------------------------------------------------
// void OnAddBodyButton()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAddBodyButton()
{   
   CelesBodySelectDialog bodyDlg(this, primaryBodiesArray);
   bodyDlg.ShowModal();
        
   if (bodyDlg.IsBodySelected())
   {
      wxArrayString &names = bodyDlg.GetBodyNames();
      
      for(Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         primaryBodiesArray.Add(names[i]);
         bodyTextCtrl->AppendText(names[i] + " ");
         bodyComboBox->Append(names[i]);
         bodyComboBox->SetValue(names[i]);

         currentBodyName = std::string(names[i].c_str());
         currentBodyId = FindBody(currentBodyName);
         forceList[currentBodyId]->bodyName = currentBodyName;
      }

      DisplayGravityFieldData();
      DisplayAtmosphereModelData();
      DisplayMagneticFieldData();

      theApplyButton->Enable(true);
      isForceModelChanged = true;
   }
    
//     CelesBodySelectDialog bodyDlg(this, primaryBodiesArray);
//     bodyDlg.ShowModal();
        
//     if (bodyDlg.IsBodySelected())
//     {        
//        CelestialBody *body;
//        wxArrayString &names = bodyDlg.GetBodyNames();
//        for(Integer i=0; i < (Integer)names.GetCount(); i++)
//        {
//           primaryBodiesArray.Add(names[i]);
//           body = theSolarSystem->GetBody(names[i].c_str());
//           theBodies.push_back(body);
//           primaryBodiesGravityArray.Add("PointMassForce");
//           bodyTextCtrl->AppendText(names[i] + " ");
//           bodyComboBox->Append(names[i]);
//        }       
//        isForceModelChanged = true;
//        isBodiesChanged = true;
//        theApplyButton->Enable(true);
//     }
    
//     bool pmChanged = false;
//     for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
//     {
//        for (Integer j = 0; j < (Integer)pointmassBodiesArray.GetCount(); j++)
//        {
//           if (primaryBodiesArray[i].CmpNoCase(pointmassBodiesArray[j]) == 0)
//           {
//              wxString body = pointmassBodiesArray.Item(j);
//              pointmassBodiesArray.Remove(body);
//              pmChanged = true;
//           }
        
//        }
//     }
    
//     if (pmChanged)
//     {
//        pmEditTextCtrl->Clear();
        
//        if (!pointmassBodiesArray.IsEmpty())
//        {
//           for (Integer i = 0; i < (Integer)pointmassBodiesArray.GetCount(); i++)
//           {
//              pmEditTextCtrl->AppendText(pointmassBodiesArray.Item(i));
//              pmEditTextCtrl->AppendText(" ");
//           }
//        }
//     }
}

//------------------------------------------------------------------------------
// void OnGravSearchButton()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravSearchButton()
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
        
      //filename = dialog.GetPath().c_str();
      filename = dialog.GetPath();
        
      potFileTextCtrl->SetValue(filename);
      potFilename = std::string(filename.c_str());
      
      gravityDegreeTextCtrl->SetValue("4");
      gravityOrderTextCtrl->SetValue("4");
      gravityDegreeTextCtrl->Enable(true);
      gravityOrderTextCtrl->Enable(true);
      //isForceModelChanged = true;
      theApplyButton->Enable(true);
   }
}

//------------------------------------------------------------------------------
// void OnSetupButton()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSetupButton()
{
   if (theDragForce == NULL)
   {
      isForceModelChanged = true;
      SaveData();
   }
   
   if (theDragForce != NULL)
   {      
      if (forceList[currentBodyId]->dragType == dragModelArray[EXPONENTIAL])
      {
         ExponentialDragDialog dragDlg(this, theDragForce);
         dragDlg.ShowModal();
      }
      else if (forceList[currentBodyId]->dragType == dragModelArray[MSISE90])
      {
         MSISE90Dialog dragDlg(this, theDragForce);
         dragDlg.ShowModal();
      }
      else if (forceList[currentBodyId]->dragType == dragModelArray[JR])
      {
         JacchiaRobertsDialog dragDlg(this, theDragForce);
         dragDlg.ShowModal();
      }

      theApplyButton->Enable(true); //loj: do we need this here?
   }
}

//------------------------------------------------------------------------------
// void OnMagSearchButton()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnMagSearchButton()
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
        
      filename = dialog.GetPath().c_str();
        
      magfComboBox->Append(filename); 
   }
    
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnPMEditButton()
//------------------------------------------------------------------------------
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
      theApplyButton->Enable(true);
   }
}   

//------------------------------------------------------------------------------
// void OnSRPEditButton()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPEditButton()
{
   theApplyButton->Enable(true);
}

// wxTextCtrl Events
//------------------------------------------------------------------------------
// void OnIntegratorTextUpdate()
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravityTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);

   if (event.GetEventObject() == gravityDegreeTextCtrl ||
       event.GetEventObject() == gravityOrderTextCtrl)
   {
      isGravTextChanged = true;
      isPotFileChanged = false;
   }
   else if (event.GetEventObject() == potFileTextCtrl)
   {
      isGravTextChanged = false;
      isPotFileChanged = true;
   }
}

//------------------------------------------------------------------------------
// void OnMagneticTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnMagneticTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
   isMagfTextChanged = true;
   
//     // waw: Future build implementation
//     if ( event.GetEventObject() == magneticDegreeTextCtrl )
//     {
//     }
//     else if ( event.GetEventObject() == magneticOrderTextCtrl )
//     {
//     }
}

// wxCheckBox Events
//------------------------------------------------------------------------------
// void OnSRPCheckBoxChange()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPCheckBoxChange()
{
   useSRP = srpCheckBox->GetValue();
   isForceModelChanged = true;
   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void ShowPropData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowPropData(const std::string &header)
{
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   MessageInterface::ShowMessage("thePropSetup=%d name=%s\n",
                                 thePropSetup, thePropSetup->GetName().c_str());
   MessageInterface::ShowMessage("newProp=%d, name=%s\n", newProp,
                                 newProp->GetName().c_str());
   MessageInterface::ShowMessage("theForceModel=%d, name=%s\n", theForceModel,
                                 theForceModel->GetName().c_str());
   MessageInterface::ShowMessage("numOfForces=%d\n", numOfForces);
   
   Integer paramId;
   std::string forceType;
   std::string forceBody;
   PhysicalModel *force;
   
   for (int i=0; i<numOfForces; i++)
   {
      force = theForceModel->GetForce(i);
      forceType = force->GetTypeName();

      if (forceType == "DragForce")
      {
         paramId = force->GetParameterID("AtmosphereBody");
         forceBody = force->GetStringParameter(paramId);
         forceBody = force->GetStringParameter("Body");
      }
      else 
      {
         forceBody = force->GetStringParameter("Body");
      }
      
      MessageInterface::ShowMessage("forceBody=%s, forceType=%s\n", forceBody.c_str(),
                                    forceType.c_str());
   }
   MessageInterface::ShowMessage("============================================\n");
#endif
}

//------------------------------------------------------------------------------
// void ShowForceList()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowForceList(const std::string &header)
{
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   for (unsigned int i=0; i<forceList.size(); i++)
   {
      MessageInterface::ShowMessage
         ("id=%d, body=%s, gravType=%s, dragType=%s, magfType=%s\n", i,
          forceList[i]->bodyName.c_str(),
          forceList[i]->gravType.c_str(),
          forceList[i]->dragType.c_str(),
          forceList[i]->magfType.c_str());
   }
   MessageInterface::ShowMessage("============================================\n");
#endif
}
