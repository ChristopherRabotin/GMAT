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
#include "MessageInterface.hpp"


//#define DEBUG_PROP_PANEL_SETUP 1
//#define DEBUG_PROP_PANEL 1

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
PropagationConfigPanel::PropagationConfigPanel(wxWindow *parent,
                                               const wxString &propName)
   : GmatPanel(parent)
{
   propSetupName = std::string(propName.c_str());
   
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel() entered propSetupName=%s\n",
       propSetupName.c_str());
#endif
   
   // Default integrator values
   newPropName      = "";
   thePropSetup     = NULL;
   thePropagator    = NULL;
   newProp          = NULL;
   integratorString = "RKV 8(9)";
    
   forceList.clear();
   pmForceList.clear();
   gravModelArray.clear();
   dragModelArray.clear();
   magfModelArray.clear();
   
   // Default force model values
   useDragForce        = false;
   numOfForces         = 0;
   theForceModel       = NULL;
   thePMF              = NULL;
   theSRP              = NULL;
   theDragForce        = NULL;
   theGravForce        = NULL;
   theAtmosphereModel  = NULL;
   
   // Default body values
   theCelestialBody    = NULL;

   Create();
   Show();
   
   isForceModelChanged = false;
   isGravTextChanged = false;
   isPotFileChanged = false;
   isMagfTextChanged = false;
   isBodiesChanged = false;
   isIntegratorChanged = false;
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// ~PropagationConfigPanel()
//------------------------------------------------------------------------------
PropagationConfigPanel::~PropagationConfigPanel()
{
   for (unsigned int i=0; i<forceList.size(); i++)
      delete forceList[i];
   for (unsigned int i=0; i<pmForceList.size(); i++)
      delete pmForceList[i]; 
        
   gravModelArray.clear();
   dragModelArray.clear();
   magfModelArray.clear();

   primaryBodiesArray.Clear();
   integratorArray.Clear();
}

//------------------------------------------
// proteced methods inherited from GmatPanel
//------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagationConfigPanel::Create()
{
   if (theGuiInterpreter != NULL)
   {
      Initialize(); 
      Setup(this);
   }
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::LoadData()
{
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("LoadData() entered\n");
#endif
   
   std::string propType = newProp->GetTypeName();
   
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("propType=%s\n", propType.c_str());
#endif

   Integer typeId = 0;
    
   if (propType == "RungeKutta89")
      typeId = RKV89;    
   else if (propType == "DormandElMikkawyPrince68")
      typeId = RKN68;    
   else if (propType == "RungeKuttaFehlberg56")
      typeId = RKF56;
   else if (propType == "PrinceDormand45")
      typeId = PD45;
   else if (propType == "PrinceDormand78")
      typeId = PD78;
   else if (propType == "BulirschStoer")
      typeId = BS;
   else if (propType == "AdamsBashforthMoulton")
      typeId = ABM;

   // fill body combobox
   if ( !primaryBodiesArray.IsEmpty() )
      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
         bodyComboBox->Append(primaryBodiesArray[i]);

// waw: 10/14/04 commented out   
   // fill other potential file name
//   if (forceList[currentBodyId]->gravType == gravModelArray[OTHER])
//       potFileTextCtrl->SetValue(potFilename.c_str());

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
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("SaveData() newPropName=%s\n", newProp->GetTypeName().c_str());
#endif
   
   //-------------------------------------------------------
   // Saving the Integrator
   //-------------------------------------------------------
   if (isIntegratorChanged)
   {
#if DEBUG_PROP_PANEL
      ShowPropData("SaveData() BEFORE saving Integrator");
#endif
      isIntegratorChanged = false;
        
      newProp->SetRealParameter("StepSize", atof(setting1TextCtrl->GetValue()));
      newProp->SetRealParameter("Accuracy", atof(setting2TextCtrl->GetValue()));
      newProp->SetRealParameter("MinStep", atof(setting3TextCtrl->GetValue()));
      newProp->SetRealParameter("MaxStep", atof(setting4TextCtrl->GetValue()));
      newProp->SetIntegerParameter("MaxStepAttempts", atoi(setting5TextCtrl->GetValue()));
      
      if (integratorString.IsSameAs(integratorArray[ABM]))
      {
         newProp->SetRealParameter("LowerError", atof(setting6TextCtrl->GetValue()));
         newProp->SetRealParameter("TargetError", atof(setting7TextCtrl->GetValue()));
      }       
      thePropSetup->SetPropagator(newProp);
#if DEBUG_PROP_PANEL
   ShowPropData("SaveData() AFTER  saving Integrator");
#endif
   }
   
   //-------------------------------------------------------
   // Saving the force model
   //-------------------------------------------------------
   if (isForceModelChanged)
   {      
#if DEBUG_PROP_PANEL
      ShowForceList("SaveData() BEFORE saving ForceModel");
#endif
      isForceModelChanged = false;
      ForceModel *newFm = new ForceModel();

      //----------------------------------------------------
      // save point mass force model
      //----------------------------------------------------      
      for (Integer i=0; i < (Integer)pmForceList.size(); i++)
      {      
         thePMF = new PointMassForce();
         thePMF->SetBodyName(pmForceList[i]->bodyName);
         pmForceList[i]->pmf = thePMF;
         newFm->AddForce(thePMF);
      }
      
      for (Integer i=0; i < (Integer)forceList.size(); i++)
      {
         Integer paramId;
         
         //----------------------------------------------------
         // save gavity force model
         //----------------------------------------------------      
         if (forceList[i]->gravType != gravModelArray[NONE_GM])
         {                  
            if (forceList[i]->gravType == gravModelArray[JGM2])
            {
               potFilename = theGuiInterpreter->GetPotentialFileName("JGM2");
            }
            else if (forceList[i]->gravType == gravModelArray[JGM3])
            {
               potFilename = theGuiInterpreter->GetPotentialFileName("JGM3");
            }
            else
            {
               potFilename = std::string(potFileTextCtrl->GetValue().c_str());
               
               if (isPotFileChanged)
                  isPotFileChanged = false;
            }
            
            theGravForce = new GravityField("", forceList[i]->bodyName); 
            if (isGravTextChanged)
            {
               isGravTextChanged = false;
               theGravForce->SetIntegerParameter
                  ("Degree", atoi(gravityDegreeTextCtrl->GetValue()));
               theGravForce->SetIntegerParameter
                  ("Order",  atoi(gravityOrderTextCtrl->GetValue()));
            }                
            theGravForce->SetStringParameter("Filename", potFilename);

            forceList[i]->gravf = theGravForce;
            newFm->AddForce(theGravForce);
            
#if DEBUG_PROP_PANEL
      ShowForceList("SaveData() AFTER  saving GravityField");
#endif 
         }
         //----------------------------------------------------
         // save drag force model
         //----------------------------------------------------           
         if (forceList[i]->dragType != dragModelArray[NONE_DM])
         {
#if DEBUG_PROP_PANEL
      ShowForceList("SaveData() BEFORE  saving DragForce");
#endif 
            theDragForce = new DragForce(forceList[i]->dragType);

            //theCelestialBody = theDragForce->GetBody();
            
            //if (theCelestialBody == NULL)
            theCelestialBody = theSolarSystem->GetBody(forceList[i]->bodyName); 
                             
            theAtmosphereModel = theCelestialBody->GetAtmosphereModel();
            
#if DEBUG_PROP_PANEL
      ShowForceList("Entering if (theAtmosphereModel == NULL)");
#endif
            if (theAtmosphereModel == NULL)  
            {
               theAtmosphereModel = theGuiInterpreter->CreateAtmosphereModel
                  (forceList[i]->dragType.c_str(), forceList[i]->dragType.c_str(),
                  forceList[i]->bodyName.c_str());
               theCelestialBody->SetAtmosphereModel(theAtmosphereModel);
#if DEBUG_PROP_PANEL
      ShowForceList("Exiting if (theAtmosphereModel == NULL)");
#endif
            }
                      
            theDragForce->SetInternalAtmosphereModel(theAtmosphereModel); 
#if DEBUG_PROP_PANEL
      ShowForceList("theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);");
#endif                              
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            theDragForce->SetStringParameter(paramId, forceList[i]->dragType.c_str());
            //loj: 10/25/04
            //paramId = theDragForce->GetParameterID("AtmosphereBody");
            //paramId = theDragForce->GetParameterID("BodyName");
            theDragForce->SetStringParameter("BodyName", forceList[i]->bodyName.c_str());
            forceList[i]->dragf = theDragForce;
            newFm->AddForce(theDragForce);
            
#if DEBUG_PROP_PANEL
            ShowForceList("SaveData() AFTER  saving DragForce");
#endif 
         }
         //----------------------------------------------------
         // save SRP data
         //----------------------------------------------------
         // only Earth for B3
         if (forceList[i]->useSrp && forceList[i]->bodyName == SolarSystem::EARTH_NAME)
         {
            theSRP = new SolarRadiationPressure();
            forceList[i]->srpf = theSRP;
            newFm->AddForce(theSRP);
            
            paramId= newFm->GetParameterID("SRP");
            newFm->SetStringParameter(paramId, "On");

#if DEBUG_PROP_PANEL
      ShowForceList("SaveData() AFTER  saving SRP");
#endif 
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
   } // end if(isForceModelChange)
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
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("PropagationConfigPanel::Initialize() entered\n");
#endif
   
   theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
   thePropSetup = theGuiInterpreter->GetPropSetup(propSetupName);

   // initialize integrator type array
   integratorArray.Add("RKV 8(9)");
   integratorArray.Add("RKN 6(8)");
   integratorArray.Add("RKF 5(6)");
   integratorArray.Add("PD  4(5)");
   integratorArray.Add("PD  7(8)");
   integratorArray.Add("BS");
   integratorArray.Add("ABM");
    
   // initialize gravity model type array
   gravModelArray.push_back("None");
   gravModelArray.push_back("JGM-2");
   gravModelArray.push_back("JGM-3");
   gravModelArray.push_back("Other");
      
   // initialize drag model type array
   dragModelArray.push_back("None");
   dragModelArray.push_back("Exponential");
   dragModelArray.push_back("MSISE90");
   dragModelArray.push_back("JacchiaRoberts");
   
   // initialize mag. filed model type array
   magfModelArray.push_back("None");
      
   if (thePropSetup != NULL)
   {
      thePropagator = thePropSetup->GetPropagator();
      newProp = thePropagator;
      theForceModel = thePropSetup->GetForceModel();
      numOfForces   = thePropSetup->GetNumForces();
         
      PhysicalModel *force;     
      Integer paramId;
      std::string bodyName;
      wxString tempStr;
      wxString useSRP;
      
      paramId = theForceModel->GetParameterID("SRP");
      useSRP = theForceModel->GetStringParameter(paramId).c_str();
      //waw: Earth only for Build 3
      if (useSRP.CmpNoCase("On") == 0)
      {                                   
         currentBodyId = FindBody(SolarSystem::EARTH_NAME);
         forceList[currentBodyId]->bodyName = SolarSystem::EARTH_NAME;
         forceList[currentBodyId]->useSrp = true;
         forceList[currentBodyId]->srpf = theSRP;
      }
              
      for (Integer i = 0; i < numOfForces; i++)
      {
         force = theForceModel->GetForce(i);
         
         if (force->GetTypeName() == "PointMassForce")
         {
            thePMF = (PointMassForce *)force;
            bodyName = thePMF->GetStringParameter("BodyName");
            secondaryBodiesArray.Add(bodyName.c_str());    
            pmForceList.push_back(new ForceType(bodyName, gravModelArray[NONE_GM], 
               dragModelArray[NONE_DM], magfModelArray[NONE_MM])); 
         }
         else if (force->GetTypeName() == "GravityField")
         {
            theGravForce = (GravityField*)force;
            bodyName = theGravForce->GetStringParameter("BodyName");
            potFilename = theGravForce->GetStringParameter("Filename");                 

            GravModelType gravModelType;
            if (potFilename.find("JGM2") != std::string::npos)
               gravModelType = JGM2;
            else if (potFilename.find("JGM3") != std::string::npos)
               gravModelType = JGM3;
            else 
               gravModelType = OTHER;

            currentBodyId = FindBody(bodyName);
            forceList[currentBodyId]->bodyName = bodyName;
            forceList[currentBodyId]->gravType = gravModelArray[gravModelType];
            forceList[currentBodyId]->gravf = theGravForce;
            
            tempStr = "";
            tempStr << theGravForce->GetIntegerParameter("Degree");
            forceList[currentBodyId]->gravDegree = tempStr;
            tempStr = "";
            tempStr << theGravForce->GetIntegerParameter("Order");
            forceList[currentBodyId]->gravOrder = tempStr;
            
            if (gravModelType == OTHER)
            {
               forceList[currentBodyId]->potFilename = potFilename;
               potFileTextCtrl->SetValue(potFilename.c_str());  
            }  

            bool found = false;
            for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
            {
               if ( primaryBodiesArray[i].CmpNoCase(bodyName.c_str()) == 0 )
                  found = true;
            }
            
            if (!found)
               primaryBodiesArray.Add(bodyName.c_str());     
         }
         else if (force->GetTypeName() == "DragForce")
         {
            theDragForce = (DragForce*)force;  
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            atmosModelString = theDragForce->GetStringParameter(paramId).c_str();
            
            //paramId = theDragForce->GetParameterID("AtmosphereBody");
            //paramId = theDragForce->GetParameterID("BodyName"); //loj: 10/25/04
            bodyName = theDragForce->GetStringParameter("BodyName");
                        
            currentBodyId = FindBody(bodyName);
            forceList[currentBodyId]->bodyName = bodyName;
            forceList[currentBodyId]->dragType = atmosModelString;
            forceList[currentBodyId]->dragf = theDragForce;
            
            bool found = false;
            for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
            {
               if ( primaryBodiesArray[i].CmpNoCase(bodyName.c_str()) == 0 )
                  found = true;
            }
            
            if (!found)
               primaryBodiesArray.Add(bodyName.c_str());
         }
      }
 
      if (!primaryBodiesArray.IsEmpty())  
      { 
         primaryBodyString = primaryBodiesArray.Item(0).c_str();
         currentBodyName = std::string(primaryBodyString.c_str());
         currentBodyId = FindBody(currentBodyName);
      }

#if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage("primaryBodyString=%s\n", primaryBodyString.c_str());
#endif
                
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

//------------------------------------------------------------------------------
// void Setup(wxWindow *parent)
//------------------------------------------------------------------------------
void PropagationConfigPanel::Setup(wxWindow *parent)
{
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() entered\n");
#endif
   
   // wxStaticText
   //loj: 5/19/04 changed wxSize(250,30) to wxSize(80,20)
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxStaticText\n");
#endif
   
   integratorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   setting1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Step Size: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Int Error: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Step Size: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting4StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Size: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting5StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max failed steps: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting6StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Integration Error: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting7StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Nominal Integration Error: "),
                        wxDefaultPosition, wxSize(120,20),
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
   
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxTextCtrl\n");
#endif
    // wxTextCtrl
   setting1TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
   setting2TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
   setting3TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
   setting4TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
   setting5TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
   setting6TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
   setting7TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(120,-1), 0 );
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

#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxButton\n");
#endif
   
   // wxButton
   bodyButton =
      new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Add/Remove Body"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   searchGravityButton =
      new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   dragSetupButton =
      new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   editPmfButton =
      new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Add/Remove Body"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   searchMagneticButton =
      new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   editSrpButton =
      new wxButton( parent, ID_BUTTON_SRP_EDIT, wxT("Edit"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create strArray*\n");
#endif
   
   // wxString
   wxString *intgArray = new wxString[IntegratorCount];
   for (int i=0; i<IntegratorCount; i++)
      intgArray[i] = integratorArray[i];
   
   wxString bodyArray[] =
   {
   };

   wxString *gravArray = new wxString[GravModelCount];
   for (int i=0; i<GravModelCount; i++)
      gravArray[i] = gravModelArray[i].c_str();

   wxString *dragArray = new wxString[DragModelCount];
   for (int i=0; i<DragModelCount; i++)
      dragArray[i] = dragModelArray[i].c_str();
   
   wxString *magfArray = new wxString[MagfModelCount];
   for (int i=0; i<MagfModelCount; i++)
      magfArray[i] = magfModelArray[i].c_str();

#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxComboBox\n"
       "IntegratorCount=%d, GravModelCount=%d, DragModelCount=%d, "
       "MagfModelCount=%d\n", IntegratorCount, GravModelCount, DragModelCount,
       MagfModelCount);
#endif
   
   // wxComboBox
   integratorComboBox =
      new wxComboBox( parent, ID_CB_INTGR, wxT(integratorString),
                      wxDefaultPosition, wxSize(80,-1), IntegratorCount,
                      intgArray, wxCB_DROPDOWN|wxCB_READONLY );
   bodyComboBox =
      new wxComboBox( parent, ID_CB_BODY, wxT(primaryBodyString),
                      wxDefaultPosition,  wxSize(100,-1), 0,
                      bodyArray, wxCB_DROPDOWN|wxCB_READONLY );
   
   gravComboBox =
      new wxComboBox( parent, ID_CB_GRAV, wxT(gravArray[0]),
                      wxDefaultPosition, wxSize(100,-1), GravModelCount,
                      gravArray, wxCB_DROPDOWN|wxCB_READONLY );
   atmosComboBox =
      new wxComboBox( parent, ID_CB_ATMOS, wxT(dragArray[0]),
                      wxDefaultPosition, wxSize(100,-1), DragModelCount,
                      dragArray, wxCB_DROPDOWN|wxCB_READONLY );
   magfComboBox =
      new wxComboBox( parent, ID_CB_MAG, wxT(magfArray[0]),
                      wxDefaultPosition, wxSize(100,-1), MagfModelCount,
                      magfArray, wxCB_DROPDOWN|wxCB_READONLY );
      
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() ComboBoxes created\n");
#endif
   
   // wxCheckBox
   srpCheckBox = new wxCheckBox( parent, ID_CHECKBOX, wxT("Use"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
   
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxSizer\n");
#endif
   
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
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting1TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting2TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting3TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting4TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting5TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting6TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( setting7StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting7TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);

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
   item45->Add( dragSetupButton, 0, wxALIGN_CENTRE|wxALL, bsize);       
   
   item50->Add( type3StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( magfComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( degree2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( order2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   item50->Add( searchMagneticButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   flexGridSizer2->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer2->Add( editPmfButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( srpCheckBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( editSrpButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
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
   editSrpButton->Show(false);  // TBD by Code 595
    
   gravComboBox->Enable(true);
   searchGravityButton->Enable(false);
   magfComboBox->Enable(true);
   magneticDegreeTextCtrl->Enable(false);
   magneticOrderTextCtrl->Enable(false);
   searchMagneticButton->Enable(false);
   
   bodyButton->Enable(true);
   srpCheckBox->Enable(true);
   dragSetupButton->Enable(false);
   
#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() exiting\n");
#endif
}

//------------------------------------------------------------------------------
// void DisplayIntegratorData(bool integratorChanged)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{ 
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
      else if (integratorString.IsSameAs(integratorArray[PD45]))
      {   
         newPropName = propSetupName + "PD45";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("PrinceDormand45", newPropName);
      }
      else if (integratorString.IsSameAs(integratorArray[PD78]))
      {   
         newPropName = propSetupName + "PD78";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("PrinceDormand78", newPropName);
      }
      else if (integratorString.IsSameAs(integratorArray[BS]))
      {   
         newPropName = propSetupName + "BS";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("BulirschStoer", newPropName);
      }
      else if (integratorString.IsSameAs(integratorArray[ABM]))
      {   
         newPropName = propSetupName + "ABM";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("AdamsBashforthMoulton", newPropName);
      }
   }
   else
   {
      newProp = thePropSetup->GetPropagator();
   }
   
   if (integratorString.IsSameAs(integratorArray[RKV89]))
   {
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
   else if (integratorString.IsSameAs(integratorArray[RKN68]))
   {
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
   else if (integratorString.IsSameAs(integratorArray[RKF56]))
   {   
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
   else if (integratorString.IsSameAs(integratorArray[PD45]))
   {   
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
   else if (integratorString.IsSameAs(integratorArray[PD78]))
   {   
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
   else if (integratorString.IsSameAs(integratorArray[BS]))
   {   
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
   else if (integratorString.IsSameAs(integratorArray[ABM]))
   {   
         setting6StaticText->Show(true);
         setting7StaticText->Show(true);
         setting6TextCtrl->Show(true);
         setting7TextCtrl->Show(true);
   }
    
    // fill in data     
    // waw: Changed to show all digits
    wxString s1, s2, s3, s4, s5;
    
    Real i1 = newProp->GetRealParameter("StepSize");
    Real i2 = newProp->GetRealParameter("Accuracy");
    Real i3 = newProp->GetRealParameter("MinStep");
    Real i4 = newProp->GetRealParameter("MaxStep");
    Integer i5 = (long)newProp->GetIntegerParameter("MaxStepAttempts");

    s1.Printf("%.10f", i1);
    s2.Printf("%.10f", i2);
    s3.Printf("%.10f", i3);
    s4.Printf("%.10f", i4);
    s5.Printf("%d", i5);

   setting1TextCtrl->SetValue(s1);
   setting2TextCtrl->SetValue(s2);
   setting3TextCtrl->SetValue(s3);
   setting4TextCtrl->SetValue(s4);
   setting5TextCtrl->SetValue(s5);
 
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {
      wxString s6, s7;
      
      Real i6 = newProp->GetRealParameter("LowerError");
      Real i7 = newProp->GetRealParameter("TargetError");
      
      s6.Printf("%.10f", i6);
      s7.Printf("%.10f", i7);
      
      setting6TextCtrl->SetValue(s6);
      setting7TextCtrl->SetValue(s7);
   } 
#if DEBUG_PROP_PANEL
   ShowPropData("DisplayIntegratorData() exiting...");
#endif
}

//------------------------------------------------------------------------------
// void DisplayForceData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayForceData()
{
   if (!pmForceList.empty())
      DisplayPointMassData(); 
   
   if (forceList.empty())
      return;  
   
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
   potFileStaticText->Enable(false);
   potFileTextCtrl->Enable(false);

   gravityDegreeTextCtrl->SetValue(forceList[currentBodyId]->gravDegree);
   gravityOrderTextCtrl->SetValue(forceList[currentBodyId]->gravOrder);
     
   gravityDegreeTextCtrl->Enable(true);
   gravityOrderTextCtrl->Enable(true);
 
   if (forceList[currentBodyId]->gravType == gravModelArray[JGM2] ||
       forceList[currentBodyId]->gravType == gravModelArray[JGM3])
   {
      if (forceList[currentBodyId]->gravType == gravModelArray[JGM2])
         gravComboBox->SetSelection(JGM2);
      else
         gravComboBox->SetSelection(JGM3);
   }
   else if (forceList[currentBodyId]->gravType == gravModelArray[OTHER])
   {
      gravComboBox->SetSelection(OTHER);
      potFileTextCtrl->SetValue(forceList[currentBodyId]->potFilename.c_str());
      potFileStaticText->Enable(true);
      potFileTextCtrl->Enable(true);
      searchGravityButton->Enable(true);
   }
   else if (forceList[currentBodyId]->gravType == gravModelArray[NONE_GM])
   {
      gravComboBox->SetSelection(NONE_GM);
      gravityDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
   }

   isGravTextChanged = false;
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
      dragSetupButton->Enable(false);
   }
   else if (forceList[currentBodyId]->dragType == dragModelArray[EXPONENTIAL])
   {
      atmosComboBox->SetSelection(EXPONENTIAL);
      dragSetupButton->Enable(false);
   }
   else if (forceList[currentBodyId]->dragType == dragModelArray[MSISE90])
   {
      atmosComboBox->SetSelection(MSISE90);
      dragSetupButton->Enable(true);
   }
   else if (forceList[currentBodyId]->dragType == dragModelArray[JR])
   {
      atmosComboBox->SetSelection(JR);
      dragSetupButton->Enable(true);
   }
}

//------------------------------------------------------------------------------
// void DisplayPointMassData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayPointMassData()
{    
   pmEditTextCtrl->Clear();
   if (!secondaryBodiesArray.IsEmpty())
   {
      for (Integer i = 0; i < (Integer)secondaryBodiesArray.GetCount(); i++)
         pmEditTextCtrl->AppendText(secondaryBodiesArray.Item(i) + " ");
   }
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
   srpCheckBox->SetValue(forceList[currentBodyId]->useSrp);
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
   if (primaryBodiesArray.IsEmpty())
      return;
      
   std::string selBody = bodyComboBox->GetStringSelection().c_str();
   
   if (currentBodyName != selBody)
   {
      primaryBodyString = bodyComboBox->GetStringSelection();
      currentBodyName = selBody;
      currentBodyId = FindBody(currentBodyName);
     
      DisplayGravityFieldData();
      DisplayAtmosphereModelData();
      DisplayMagneticFieldData();
   }
}

//------------------------------------------------------------------------------
// void OnGravitySelection()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravitySelection()
{   
   if (primaryBodiesArray.IsEmpty())
      return;
      
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
}

//------------------------------------------------------------------------------
// void OnAtmosphereSelection()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAtmosphereSelection()
{
   if (primaryBodiesArray.IsEmpty())
      return;
      
#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("OnAtmosphereSelection() body=%s\n",
                                 forceList[currentBodyId]->bodyName.c_str());
#endif
      
   dragTypeName = std::string(atmosComboBox->GetStringSelection().c_str());

   if (forceList[currentBodyId]->dragType != dragTypeName)
   {    
      forceList[currentBodyId]->dragType = dragTypeName;
      DisplayAtmosphereModelData();
         
      isForceModelChanged = true;
      theApplyButton->Enable(true);
#if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage
         ("OnAtmosphereSelection() grav changed from=%s to=%s for body=%s\n",
          forceList[currentBodyId]->dragType.c_str(), dragTypeName.c_str(),
          forceList[currentBodyId]->bodyName.c_str());
#endif
   }
}

// wxButton events
//------------------------------------------------------------------------------
// void OnAddBodyButton()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAddBodyButton()
{     
   CelesBodySelectDialog bodyDlg(this, primaryBodiesArray, secondaryBodiesArray);
   bodyDlg.ShowModal();
        
   if (bodyDlg.IsBodySelected())
   {
      wxArrayString &names = bodyDlg.GetBodyNames();

      if (names.IsEmpty())
      {
         forceList.clear();
         primaryBodiesArray.Clear(); 
         bodyComboBox->Clear();
         bodyTextCtrl->Clear();
         theApplyButton->Enable(true);
         isForceModelChanged = true;
         return;
      }
               
      std::vector<ForceType*> fl;
      
      std::string bodyName;
      std::string gravType;
      std::string dragType;
      std::string magfType;
      wxString gravDegree;
      wxString gravOrder;
      wxString magfDegree;
      wxString magfOrder;
      std::string potFilename;
      PointMassForce *pmf = NULL;
      GravityField *gravf =  NULL;
      DragForce *dragf = NULL;
      SolarRadiationPressure *srpf = NULL;
      bool useSrp = false;
         
      fl = forceList;
      forceList.clear();
      primaryBodiesArray.Clear();
               
      //----------------------------
      // when user addes more bodies
      //----------------------------
      if (names.GetCount() > fl.size())
      {                           
         for (Integer i = 0; i < (Integer)names.GetCount(); i++)
         {
            bodyName = names[i].c_str();
            
            // If no previous body selection was made
            if (fl.empty())
            {
               gravType = gravModelArray[JGM2];
               dragType = dragModelArray[NONE_DM];
               magfType = magfModelArray[NONE_MM];   
               gravDegree = "4";
               gravOrder = "4";
               magfDegree = "0";
               magfOrder = "0";
               potFilename = "";
               pmf = NULL;
               gravf = NULL;
               dragf = NULL;
               srpf = NULL;
               useSrp = false;                
            }
            else
            {                        
               for (Integer j = 0; j < (Integer)fl.size(); j++)
               {
                  if (strcmp(bodyName.c_str(), fl[j]->bodyName.c_str()) == 0)
                  {
                     gravType = fl[j]->gravType;
                     dragType = fl[j]->dragType;
                     magfType = fl[j]->magfType;
                     gravDegree = fl[j]->gravDegree;
                     gravOrder = fl[j]->gravOrder;
                     magfDegree = fl[j]->magfDegree;
                     magfOrder = fl[j]->magfOrder;
                     potFilename = fl[j]->potFilename;
                     pmf = fl[j]->pmf;
                     gravf = fl[j]->gravf;
                     dragf = fl[j]->dragf;
                     srpf = fl[j]->srpf;
                  }            
                  else
                  {
                     gravType = gravModelArray[JGM2];
                     dragType = dragModelArray[NONE_DM];
                     magfType = magfModelArray[NONE_MM];   
                     gravDegree = "4";
                     gravOrder = "4";
                     magfDegree = "0";
                     magfOrder = "0";
                     potFilename = "";
                     pmf = NULL;
                     gravf = NULL;
                     dragf = NULL;
                     srpf = NULL;
                     useSrp = false;       
                  }
               }
            }
            currentBodyId = FindBody(bodyName,gravType,dragType,magfType);
            
            forceList[currentBodyId]->gravDegree = gravDegree;
            forceList[currentBodyId]->gravOrder = gravOrder;
            forceList[currentBodyId]->magfDegree = magfDegree;
            forceList[currentBodyId]->magfOrder = magfOrder;
            forceList[currentBodyId]->potFilename = potFilename;
            forceList[currentBodyId]->pmf = pmf;
            forceList[currentBodyId]->gravf = gravf;
            forceList[currentBodyId]->dragf = dragf;
            forceList[currentBodyId]->srpf = srpf;
            forceList[currentBodyId]->useSrp = useSrp;
            
            primaryBodiesArray.Add(bodyName.c_str());
         }
      }
      //---------------------------------------------
      // when user removes a body or bodies from list
      //---------------------------------------------
      else
      {
         for (Integer i = 0; i < (Integer)fl.size(); i++)
         {
            bodyName = fl[i]->bodyName.c_str();
                        
            for (Integer j = 0; j < (Integer)names.GetCount(); j++)
            {
               if (strcmp(bodyName.c_str(), names[j].c_str()) == 0)
               {
                  gravType = fl[j]->gravType;
                  dragType = fl[j]->dragType;
                  magfType = fl[j]->magfType;
                  gravDegree = fl[j]->gravDegree;
                  gravOrder = fl[j]->gravOrder;
                  magfDegree = fl[j]->magfDegree;
                  magfOrder = fl[j]->magfOrder;
                  potFilename = fl[j]->potFilename;
                  pmf = fl[j]->pmf;
                  gravf = fl[j]->gravf;
                  dragf = fl[j]->dragf;
                  srpf = fl[j]->srpf;
                  useSrp = fl[j]->useSrp;
                  
                  currentBodyId = FindBody(bodyName,gravType,dragType,magfType);

                  forceList[currentBodyId]->gravDegree = gravDegree;
                  forceList[currentBodyId]->gravOrder = gravOrder;
                  forceList[currentBodyId]->magfDegree = magfDegree;
                  forceList[currentBodyId]->magfOrder = magfOrder;
                  forceList[currentBodyId]->potFilename = potFilename;
                  forceList[currentBodyId]->pmf = pmf;
                  forceList[currentBodyId]->gravf = gravf;
                  forceList[currentBodyId]->dragf = dragf;
                  forceList[currentBodyId]->srpf = srpf;
                  forceList[currentBodyId]->useSrp = useSrp;
                  
                  primaryBodiesArray.Add(bodyName.c_str());
               }            
            }            
         }
      }
      
      //----------------------------------------------
      // Append body names to combobox and text field
      //----------------------------------------------
      bodyComboBox->Clear();
      bodyTextCtrl->Clear();
      wxString name;
      for (Integer i = 0; i < (Integer)forceList.size(); i++)
      {
         name = forceList[i]->bodyName.c_str();
         bodyTextCtrl->AppendText(name + " ");
         bodyComboBox->Append(forceList[i]->bodyName.c_str());
         bodyComboBox->SetValue(forceList[i]->bodyName.c_str());
      }
      OnBodySelection();    

      theApplyButton->Enable(true);
      isForceModelChanged = true;
   }
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
   DragForce *dragForce;
   
   if (forceList[currentBodyId]->dragf == NULL)
   {
      isForceModelChanged = true;
      SaveData();
   }

   dragForce = forceList[currentBodyId]->dragf;
   if (dragForce != NULL)
   {      
      if (forceList[currentBodyId]->dragType == dragModelArray[EXPONENTIAL])
      {
         // TBD by Code 595
         //ExponentialDragDialog dragDlg(this, dragForce);
         //dragDlg.ShowModal();
      }
      else if (forceList[currentBodyId]->dragType == dragModelArray[MSISE90])
      {
         MSISE90Dialog dragDlg(this, dragForce);
         dragDlg.ShowModal();
      }
      else if (forceList[currentBodyId]->dragType == dragModelArray[JR])
      {
         JacchiaRobertsDialog dragDlg(this, dragForce);
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
   CelesBodySelectDialog bodyDlg(this, secondaryBodiesArray, primaryBodiesArray);
   bodyDlg.ShowModal();
        
   if (bodyDlg.IsBodySelected())
   {
      wxArrayString &names = bodyDlg.GetBodyNames();
      
      if (names.IsEmpty())
      {
         pmForceList.clear();
//         secondaryBodiesArray.Clear(); 
         pmEditTextCtrl->Clear();
         theApplyButton->Enable(true);
         isForceModelChanged = true;
         return;
      }
      
      std::vector<ForceType*> fl;
      
      std::string bodyName;
      std::string gravType;
      std::string dragType;
      std::string magfType;
      wxString gravDegree;
      wxString gravOrder;
      wxString magfDegree;
      wxString magfOrder;
      std::string potFilename;
      PointMassForce *pmf;
      GravityField *gravf;
      DragForce *dragf;
      SolarRadiationPressure *srpf;
      bool useSrp;

      fl = pmForceList;      
      pmForceList.clear();
      secondaryBodiesArray.Clear(); 
      pmEditTextCtrl->Clear();
      
      //--------------------------
      // Add bodies to pmForceList
      //--------------------------     
      for (Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i].c_str();
         gravType = gravModelArray[NONE_GM];
         dragType = dragModelArray[NONE_DM];
         magfType = magfModelArray[NONE_MM];   
         gravDegree = "4";
         gravOrder = "4";
         magfDegree = "0";
         magfOrder = "0";
         potFilename = "";
         pmf = NULL;
         gravf = NULL;
         dragf = NULL;
         srpf = NULL;
         useSrp = false;       

         pmForceList.push_back(new ForceType(bodyName, gravType, dragType, magfType));
         pmForceList[i]->gravDegree = gravDegree;
         pmForceList[i]->gravOrder = gravOrder;
         pmForceList[i]->magfDegree = magfDegree;
         pmForceList[i]->magfOrder = magfOrder;
         pmForceList[i]->potFilename = potFilename;
         pmForceList[i]->pmf = pmf;
         pmForceList[i]->gravf = gravf;
         pmForceList[i]->dragf = dragf;
         pmForceList[i]->srpf = srpf;
         pmForceList[i]->useSrp = useSrp;
            
         secondaryBodiesArray.Add(bodyName.c_str());
         pmEditTextCtrl->AppendText(names[i] + " ");
      } 
      theApplyButton->Enable(true);
      isForceModelChanged = true;
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
   
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {
      wxString set6 = setting6TextCtrl->GetValue();
      wxString set7 = setting7TextCtrl->GetValue();
   }  

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
}

// wxCheckBox Events
//------------------------------------------------------------------------------
// void OnSRPCheckBoxChange()
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPCheckBoxChange()
{
   if (primaryBodiesArray.IsEmpty())
      return;
      
   Integer id = FindBody(SolarSystem::EARTH_NAME);
   forceList[id]->useSrp = srpCheckBox->GetValue();
   
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
   
   //Integer paramId;
   std::string forceType;
   std::string forceBody;
   PhysicalModel *force;
   
   for (int i=0; i<numOfForces; i++)
   {
      force = theForceModel->GetForce(i);
      forceType = force->GetTypeName();

      if (forceType == "DragForce")
      {
         //paramId = force->GetParameterID("AtmosphereBody");
         //forceBody = force->GetStringParameter(paramId);
         forceBody = force->GetStringParameter("BodyName");
      }
      else 
      {
         forceBody = force->GetStringParameter("BodyName");
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

