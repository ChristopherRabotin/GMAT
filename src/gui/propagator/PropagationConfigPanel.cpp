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
#include "GmatAppData.hpp"
#include "CelesBodySelectDialog.hpp"
#include "ExponentialDragDialog.hpp"
#include "MSISE90Dialog.hpp"
#include "JacchiaRobertsDialog.hpp"
#include "PropagationConfigPanel.hpp"
#include "wx/platform.h"

// base includes
#include "MessageInterface.hpp"
#include "PropagatorException.hpp"
#include "StringTokenizer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

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
   EVT_COMBOBOX(ID_CB_BODY, PropagationConfigPanel::OnBodyComboBoxChange)
   EVT_COMBOBOX(ID_CB_ORIGIN, PropagationConfigPanel::OnOriginComboBoxChange)
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
   isPotFileChanged = false;
   isMagfTextChanged = false;
   isIntegratorChanged = false;
   isOriginChanged = false;
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// ~PropagationConfigPanel()
//------------------------------------------------------------------------------
PropagationConfigPanel::~PropagationConfigPanel()
{
   for (Integer i=0; i<(Integer)forceList.size(); i++)
      delete forceList[i];
   for (Integer i=0; i<(Integer)pmForceList.size(); i++)
      delete pmForceList[i]; 
   
   gravModelArray.clear();
   dragModelArray.clear();
   magfModelArray.clear();
   
   primaryBodiesArray.Clear();
   integratorArray.Clear();
   
   // Unregister GUI components (loj: 6/7/05 Added)
   theGuiManager->UnregisterComboBox("CoordinateSystem", originComboBox);
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
   // Enable the "Show Script" button
   mObject = thePropSetup;

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
   else if (propType == "Cowell")
      typeId = CW;

   // Display primary bodies
   if ( !primaryBodiesArray.IsEmpty() )
      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
         bodyComboBox->Append(primaryBodiesArray[i]);   

   integratorComboBox->SetSelection(typeId);
   integratorString = integratorArray[typeId]; 
   
   // Display the Origin (Central Body)
   originComboBox->SetValue(propOriginName.c_str());
   
   DisplayIntegratorData(false);
   DisplayForceData();
   
   Integer count = (Integer)forceList.size(); 
   
   if (count == 0)
   {
      gravComboBox->Enable(false);
      atmosComboBox->Enable(false);
      magfComboBox->Enable(false);
      srpCheckBox->Enable(false);
   }
   else
   {
      gravComboBox->Enable(true);
      atmosComboBox->Enable(true);
      magfComboBox->Enable(true);
      srpCheckBox->Enable(true);       
   } 
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
      
      Real min = atof(setting3TextCtrl->GetValue());
      Real max = atof(setting4TextCtrl->GetValue());
      
      if (max < min)
      {
         MessageInterface::PopupMessage
         (Gmat::WARNING_, "Maximum Step can not be less than Minimum Step.\n"
         "Propagation updates have not been saved");
         return;
      }    
        
      newProp->SetRealParameter("InitialStepSize", atof(setting1TextCtrl->GetValue()));
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
               //loj: 7/7/05 Using new FileManager
               //forceList[i]->potFilename = theGuiInterpreter->GetPotentialFileName("JGM2");
               forceList[i]->potFilename = theGuiInterpreter->GetFileName("JGM2_FILE");
            }
            else if (forceList[i]->gravType == gravModelArray[JGM3])
            {
               //forceList[i]->potFilename = theGuiInterpreter->GetPotentialFileName("JGM3");
               forceList[i]->potFilename = theGuiInterpreter->GetFileName("JGM3_FILE");
            }
            
            if (isPotFileChanged)
               isPotFileChanged = false;
            
            theGravForce = new GravityField("", forceList[i]->bodyName); 
                            
            Integer deg = atoi(forceList[i]->gravDegree.c_str());
            Integer ord = atoi(forceList[i]->gravOrder.c_str());
               
            if (deg < ord)
            {
               MessageInterface::PopupMessage
               (Gmat::WARNING_, "Gravity field degree should be greater than "
                                   "or equal to the order.");
               gravityDegreeTextCtrl->SetValue(forceList[currentBodyId]->gravDegree);
               gravityOrderTextCtrl->SetValue(forceList[currentBodyId]->gravOrder);
               return;
            }
               
            theGravForce->SetIntegerParameter
                  ("Degree", atoi(forceList[i]->gravDegree.c_str()));
            theGravForce->SetIntegerParameter
                  ("Order",  atoi(forceList[i]->gravOrder.c_str()));
            
            theGravForce->SetStringParameter("PotentialFile", forceList[i]->potFilename);
            
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
               
               //loj: 6/9/05 Commented out - it's set in the DragForce::Initialize()
               //theCelestialBody->SetAtmosphereModel(theAtmosphereModel);
               
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
         
         //-------------------------------------------------------
         // Saving the Origin (Central Body)
         //-------------------------------------------------------
         if (isOriginChanged)
         {
            isOriginChanged = false;
            newFm->SetStringParameter("CentralBody", propOriginName.c_str());
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
   
   //loj: 2/8/06 theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   thePropSetup = theGuiInterpreter->GetPropSetup(propSetupName);

   // initialize integrator type array
   integratorArray.Add("RKV 8(9)");
   integratorArray.Add("RKN 6(8)");
   integratorArray.Add("RKF 5(6)");
   integratorArray.Add("PD  4(5)");
   integratorArray.Add("PD  7(8)");
   integratorArray.Add("BS");
   integratorArray.Add("ABM");
   integratorArray.Add("Cowell");
    
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
      newProp = thePropSetup->GetPropagator();
      theForceModel = thePropSetup->GetForceModel();
      numOfForces   = thePropSetup->GetNumForces();
      
      propOriginName = theForceModel->GetStringParameter("CentralBody");
         
      PhysicalModel *force;     
      Integer paramId;
      std::string bodyName;
      wxString tempStr;
      wxString useSRP;
      
      paramId = theForceModel->GetParameterID("SRP");
      useSRP = theForceModel->GetStringParameter(paramId).c_str();
      
      //@todo: Earth only implemented for Build 3
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
               
            //waw: Added 09/28/05
            //Warn user about bodies already added as Primary body
            Integer fmSize = (Integer)forceList.size();
            Integer last = (Integer)pmForceList.size() - 1;
            
            for (Integer i = 0; i < fmSize; i++)
            {
                if (strcmp(pmForceList[last]->bodyName.c_str(), 
                           forceList[i]->bodyName.c_str()) == 0)
                {
                   MessageInterface::PopupMessage
                                 (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass", pmForceList[last]->bodyName.c_str());
                }
            }

         }
         else if (force->GetTypeName() == "GravityField")
         {
            theGravForce = (GravityField*)force;
            bodyName = theGravForce->GetStringParameter("BodyName");
            std::string potFilename = theGravForce->GetStringParameter("PotentialFile");                 
            
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
            forceList[currentBodyId]->potFilename = potFilename;
            
            //waw: Added 09/28/05
            //Warn user about bodies already added as Primary body
            Integer pmSize = (Integer)pmForceList.size();
            Integer last = (Integer)forceList.size() - 1;
            
            for (Integer i = 0; i < pmSize; i++)
            {
                if (strcmp(forceList[last]->bodyName.c_str(), 
                           pmForceList[i]->bodyName.c_str()) == 0)
                {
                   MessageInterface::PopupMessage
                                 (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass", pmForceList[last]->bodyName.c_str());
                }
            }
            
            if (forceList[currentBodyId]->potFilename == "")
            {
               MessageInterface::PopupMessage
               (Gmat::WARNING_, "Cannot Find Gravity Field File."); 
            }
            
            tempStr = "";
            tempStr << theGravForce->GetIntegerParameter("Degree");
            forceList[currentBodyId]->gravDegree = tempStr;

            tempStr = "";
            tempStr << theGravForce->GetIntegerParameter("Order");
            forceList[currentBodyId]->gravOrder = tempStr;
   
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
            
            bodyName = theDragForce->GetStringParameter("BodyName");
                        
            currentBodyId = FindBody(bodyName);
            forceList[currentBodyId]->bodyName = bodyName;
            forceList[currentBodyId]->dragType = atmosModelString;
            forceList[currentBodyId]->dragf = theDragForce;
            
            //waw: Added 09/28/05
            //Warn user about bodies already added as Primary body
            Integer pmSize = (Integer)pmForceList.size();
            Integer last = (Integer)forceList.size() - 1;
            
            for (Integer i = 0; i < pmSize; i++)
            {
                if (strcmp(forceList[last]->bodyName.c_str(), 
                           pmForceList[i]->bodyName.c_str()) == 0)
                {
                   MessageInterface::PopupMessage
                                 (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass", pmForceList[last]->bodyName.c_str());
                }
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
   
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxStaticText\n");
   #endif

   // wxStaticText                               
   centralBodyStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Central Body"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   
   integratorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Integrator Type"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   setting1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Initial Step Size: "),
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
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Attempts: "),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE );
   setting6StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Integration Error: "),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting7StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Nominal Integration Error: "),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );  
   degree1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   order1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   potFileStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Other Potential Field File:"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   type1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   type2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
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

   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxTextCtrl\n");
   #endif
                        
   // wxTextCtrl
   setting1TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   setting2TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   setting3TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   setting4TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   setting5TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   setting6TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   setting7TextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   bodyTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), wxTE_READONLY );
   gravityDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(25,-1), 0 );
   gravityOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(25,-1), 0 );
   potFileTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(165,-1), 0 );
   magneticDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(25,-1), 0 );
   magneticOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(25,-1), 0 );
   pmEditTextCtrl =
      new wxTextCtrl( parent, -1, wxT(""), wxDefaultPosition,
                      wxSize(235,-1), wxTE_READONLY );
                 
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create strArray*\n");
   #endif
   
   // wxString
   wxString *intgArray = new wxString[IntegratorCount];
   for (Integer i=0; i<IntegratorCount; i++)
      intgArray[i] = integratorArray[i];
     
   wxString bodyArray[] =
   {
   };
   
   wxString *gravArray = new wxString[GravModelCount];
   for (Integer i=0; i<GravModelCount; i++)
      gravArray[i] = gravModelArray[i].c_str();
     
   wxString *dragArray = new wxString[DragModelCount];
   for (Integer i=0; i<DragModelCount; i++)
      dragArray[i] = dragModelArray[i].c_str();
     
   wxString *magfArray = new wxString[MagfModelCount];
   for (Integer i=0; i<MagfModelCount; i++)
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
                      wxDefaultPosition, wxDefaultSize, IntegratorCount,
                      intgArray, wxCB_DROPDOWN|wxCB_READONLY );
   originComboBox  =
      theGuiManager->GetConfigBodyComboBox(this, ID_CB_ORIGIN, wxSize(100,-1));
   bodyComboBox =
      new wxComboBox( parent, ID_CB_BODY, wxT(primaryBodyString),
                      wxDefaultPosition,  wxDefaultSize, 0,
                      bodyArray, wxCB_DROPDOWN|wxCB_READONLY );
   gravComboBox =
      new wxComboBox( parent, ID_CB_GRAV, wxT(gravArray[0]),
                      wxDefaultPosition, wxDefaultSize, GravModelCount,
                      gravArray, wxCB_DROPDOWN|wxCB_READONLY );
   atmosComboBox =
      new wxComboBox( parent, ID_CB_ATMOS, wxT(dragArray[0]),
                      wxDefaultPosition, wxDefaultSize, DragModelCount,
                      dragArray, wxCB_DROPDOWN|wxCB_READONLY );
   magfComboBox =
      new wxComboBox( parent, ID_CB_MAG, wxT(magfArray[0]),
                      wxDefaultPosition, wxDefaultSize, MagfModelCount,
                      magfArray, wxCB_DROPDOWN|wxCB_READONLY );
                 
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() ComboBoxes created\n");
   #endif
   
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxButton\n");
   #endif
   
   // wxButton
   bodyButton =
      new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   searchGravityButton =
      new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   dragSetupButton =
      new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   searchMagneticButton =
      new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   editPmfButton =
      new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );

#if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create CheckBox\n");
   #endif
   
   // wxCheckBox
   srpCheckBox = new wxCheckBox( parent, ID_CHECKBOX, wxT("Use"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
                         
#if __WXMAC__
   // wxStaticText
   wxStaticText *title1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Numerical Integrator"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Origin"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title2StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Environment Model"),
                        wxDefaultPosition, wxSize(220,20),
                        wxST_NO_AUTORESIZE);
   title3StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title4StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Primary Bodies"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title4StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title5StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Gravity Field"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title5StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title6StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Atmospheric Model"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title6StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title7StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Magnetic Field"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title7StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title8StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Point Masses"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title8StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title9StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Solar Radiation Pressure"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title9StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
                                     
   // wx*Sizers      
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer5 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer6 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer7 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer8 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer9 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer10 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer11 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer12 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer13 = new wxBoxSizer( wxHORIZONTAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   
   Integer bsize = 5; // border size
   
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
   flexGridSizer1->Add( setting6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting6TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting7StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting7TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer4->Add( title1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer4->Add( flexGridSizer1, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer2->Add( title2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer2->Add( boxSizer3, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer3->Add( centralBodyStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer3->Add( originComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer7->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add( bodyComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add( bodyButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer8->Add( type1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer8->Add( gravComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer8->Add( degree1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer8->Add( gravityDegreeTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer8->Add( order1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer8->Add( gravityOrderTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer8->Add( searchGravityButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer9->Add( potFileStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer9->Add( potFileTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer9->Add( dragSetupButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer10->Add( type2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer10->Add( atmosComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer10->Add( dragSetupButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer11->Add( type3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer11->Add( magfComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer11->Add( degree2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer11->Add( magneticDegreeTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer11->Add( order2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer11->Add( magneticOrderTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer11->Add( searchMagneticButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer12->Add( pmEditTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer12->Add( editPmfButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer13->Add( srpCheckBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer1->Add( boxSizer4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer1->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
   boxSizer5->Add( title3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( title4StaticText, 0, wxGROW|wxALIGN_RIGHT|wxALL, bsize);
   boxSizer5->Add( boxSizer7, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( title5StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer8, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer9, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( title6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer10, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( title7StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer11, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( title8StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer12, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( title9StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer13, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer6->Add( boxSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer6->Add( 5, 5, wxALIGN_CENTRE|wxALL, bsize);
   boxSizer6->Add( boxSizer5, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(boxSizer6, 0, wxGROW, bsize);
#else
   // wx*Sizers      
   wxBoxSizer *boxSizer1 = new wxBoxSizer( wxVERTICAL );
   wxBoxSizer *boxSizer2 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer3 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxSizer4 = new wxBoxSizer( wxVERTICAL );
   
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 2, 0, 2 );
   
   wxStaticBox *staticBox1 = new wxStaticBox( parent, -1, wxT("Numerical Integrator") );
   wxStaticBoxSizer *staticBoxSizer1 = new wxStaticBoxSizer( staticBox1, wxVERTICAL );
   wxStaticBox *staticBox2 = new wxStaticBox( parent, -1, wxT("Environment Model") );
   wxStaticBoxSizer *staticBoxSizer2 = new wxStaticBoxSizer( staticBox2, wxVERTICAL );
   wxStaticBox *staticBox3 = new wxStaticBox( parent, -1, wxT("Primary Bodies") );
   wxStaticBoxSizer *staticBoxSizer3 = new wxStaticBoxSizer( staticBox3, wxVERTICAL );
   wxStaticBox *staticBox4 = new wxStaticBox( parent, -1, wxT("Origin") );
   wxStaticBoxSizer *staticBoxSizer4 = new wxStaticBoxSizer( staticBox4, wxHORIZONTAL );
   wxStaticBox *item37 = new wxStaticBox( parent, -1, wxT("Gravity Field") );
   wxStaticBoxSizer *item36 = new wxStaticBoxSizer( item37, wxVERTICAL );
   
   wxBoxSizer *item361 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *item362 = new wxBoxSizer( wxHORIZONTAL );
   
   wxStaticBox *item46 = new wxStaticBox( parent, -1, wxT("Atmospheric Model") );
   wxStaticBoxSizer *item45 = new wxStaticBoxSizer( item46, wxHORIZONTAL );
   wxStaticBox *item51 = new wxStaticBox( parent, -1, wxT("Magnetic Field") );
   wxStaticBoxSizer *item50 = new wxStaticBoxSizer( item51, wxHORIZONTAL );
   wxStaticBox *staticBox7 = new wxStaticBox( parent, -1, wxT("Point Masses") );
   wxStaticBoxSizer *staticBoxSizer7 = new wxStaticBoxSizer( staticBox7, wxVERTICAL );
   wxStaticBox *item65 = new wxStaticBox( parent, -1, wxT("Solar Radiation Pressure") );
   wxStaticBoxSizer *item64 = new wxStaticBoxSizer( item65, wxHORIZONTAL );
   
   Integer bsize = 2; // border size
   
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
   flexGridSizer1->Add( setting6StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add( setting6TextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
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
    
   staticBoxSizer3->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
    
   staticBoxSizer7->Add( flexGridSizer2, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
    
   staticBoxSizer1->Add( flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   staticBoxSizer2->Add( staticBoxSizer3, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer2->Add( staticBoxSizer7, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer2->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   staticBoxSizer4->Add(centralBodyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer4->Add(originComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   boxSizer4->Add( staticBoxSizer1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer4->Add( staticBoxSizer4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
     
   boxSizer2->Add( boxSizer4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer2->Add( staticBoxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
    
   boxSizer1->Add( boxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(boxSizer1, 0, wxGROW, bsize);
#endif
   
   //------------------------------
   //waw: Future implementations
   //------------------------------    
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
      else if (integratorString.IsSameAs(integratorArray[CW]))
      {   
         newPropName = propSetupName + "CW";
         newProp = theGuiInterpreter->GetPropagator(newPropName);
        
         if (newProp == NULL)
            newProp = theGuiInterpreter->CreatePropagator("Cowell", newPropName);
      }
   }
   else
   {
      newProp = thePropSetup->GetPropagator();
   }
   
#if __WXMAC__
   if (integratorString.IsSameAs(integratorArray[RKV89]))
   {
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
   else if (integratorString.IsSameAs(integratorArray[RKN68]))
   {
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
   else if (integratorString.IsSameAs(integratorArray[RKF56]))
   {   
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
   else if (integratorString.IsSameAs(integratorArray[PD45]))
   {   
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
   else if (integratorString.IsSameAs(integratorArray[PD78]))
   {   
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
   else if (integratorString.IsSameAs(integratorArray[BS]))
   {   
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
   else if (integratorString.IsSameAs(integratorArray[ABM]))
   {   
         setting6StaticText->Enable(true);
         setting7StaticText->Enable(true);
         setting6TextCtrl->Enable(true);
         setting7TextCtrl->Enable(true);
   }
   else if (integratorString.IsSameAs(integratorArray[CW]))
   {   
         setting6StaticText->Enable(false);
         setting7StaticText->Enable(false);
         setting6TextCtrl->Enable(false);
         setting7TextCtrl->Enable(false);
   }
#else
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
   else if (integratorString.IsSameAs(integratorArray[CW]))
   {   
         setting6StaticText->Show(false);
         setting7StaticText->Show(false);
         setting6TextCtrl->Show(false);
         setting7TextCtrl->Show(false);
   }
#endif
    
    // fill in data     
    // waw: Changed to show all digits
    wxString s1, s2, s3, s4, s5;
    
    Real i1 = newProp->GetRealParameter("InitialStepSize");
    Real i2 = newProp->GetRealParameter("Accuracy");
    Real i3 = newProp->GetRealParameter("MinStep");
    Real i4 = newProp->GetRealParameter("MaxStep");
    Integer i5 = (long)newProp->GetIntegerParameter("MaxStepAttempts");

    s1.Printf("%.10f", i1);
    s2.Printf("%le", i2);
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
   
   gravityDegreeTextCtrl->SetValue(forceList[currentBodyId]->gravDegree.c_str());
   gravityOrderTextCtrl->SetValue(forceList[currentBodyId]->gravOrder.c_str());
   
   gravityDegreeTextCtrl->Enable(true);
   gravityOrderTextCtrl->Enable(true);
   
   if (forceList[currentBodyId]->gravType == gravModelArray[JGM2] ||
       forceList[currentBodyId]->gravType == gravModelArray[JGM3])
   {
      if (forceList[currentBodyId]->gravType == gravModelArray[JGM2])
         gravComboBox->SetSelection(JGM2);
      else
         gravComboBox->SetSelection(JGM3);
      
      potFileTextCtrl->SetValue("");
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
      
      potFileTextCtrl->SetValue("");
      gravityDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
   }
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
// void OnIntegratorSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnIntegratorSelection(wxCommandEvent &event)
{
   if (!integratorString.IsSameAs(integratorComboBox->GetStringSelection()))
   {
      isIntegratorChanged = true;
      integratorString = integratorComboBox->GetStringSelection();
      DisplayIntegratorData(true);
      theApplyButton->Enable(true);
   }
}

//loj: 7/7/05 Added. We don't want to enable Apply when just switching body.
//------------------------------------------------------------------------------
// void OnBodyComboBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnBodyComboBoxChange(wxCommandEvent &event)
{
   OnBodySelection(event);
   theApplyButton->Enable(false);
}

//------------------------------------------------------------------------------
// void OnOriginComboBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnOriginComboBoxChange(wxCommandEvent &event)
{
   propOriginName = originComboBox->GetValue().c_str();
   
   isOriginChanged = true;
   isForceModelChanged = true;
   
   theApplyButton->Enable(true);
}


//------------------------------------------------------------------------------
// void OnBodySelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnBodySelection(wxCommandEvent &event)
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
   
   if (strcmp(selBody.c_str(), "Earth") == 0)
   {
      atmosComboBox->Enable(true);
      dragSetupButton->Enable(true);
   }
   else
   {
      atmosComboBox->Enable(false);
      dragSetupButton->Enable(false);
   }
}

//------------------------------------------------------------------------------
// void OnGravitySelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravitySelection(wxCommandEvent &event)
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
// void OnAtmosphereSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAtmosphereSelection(wxCommandEvent &event)
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
// void OnAddBodyButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAddBodyButton(wxCommandEvent &event)
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
         
         gravComboBox->Enable(false);
         atmosComboBox->Enable(false);
         magfComboBox->Enable(false);
         srpCheckBox->Enable(false);
         gravityDegreeTextCtrl->Enable(false);
         gravityOrderTextCtrl->Enable(false);
      
         theApplyButton->Enable(true);
         isForceModelChanged = true;
         return;
      }
      else
      {
         gravComboBox->Enable(true);
         atmosComboBox->Enable(true);
         magfComboBox->Enable(true);
         srpCheckBox->Enable(true);
         gravityDegreeTextCtrl->Enable(true);
         gravityOrderTextCtrl->Enable(true);
      }
      
      std::vector<ForceType*> fl;
      std::string bodyName;
      
      fl = forceList;
      forceList.clear();
      primaryBodiesArray.Clear();

      for (Integer i = 0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i].c_str();
         primaryBodiesArray.Add(bodyName.c_str()); 
         currentBodyId = FindBody(bodyName,gravModelArray[JGM2]);
         
         for (Integer j = 0; j < (Integer)fl.size(); j++)
         {
            if (bodyName == fl[j]->bodyName)
            {
               forceList[currentBodyId] = fl[j];
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
//      bodyTextCtrl->AppendText(name + " ");
      bodyComboBox->Append(forceList[i]->bodyName.c_str());
      bodyComboBox->SetValue(forceList[i]->bodyName.c_str());
   }
      
   OnBodySelection(event);
   DisplayForceData();
      
   theApplyButton->Enable(true);
   isForceModelChanged = true;
}

//------------------------------------------------------------------------------
// void OnGravSearchButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravSearchButton(wxCommandEvent &event)
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
   if (dialog.ShowModal() == wxID_OK)
   {
      std::string filename;
        
      filename = dialog.GetPath();
          
          // Determine the type of file
          
//@todo: Not tested from GUI so did not include in Release 1.0, finish complete testing
//      if ((filename.find(".dat",0) != std::string::npos) ||
//          (filename.find(".DAT",0) != std::string::npos) )
//      {
//          ParseDATGravityFile(filename);
//      }
      
      if ((filename.find(".grv",0) != std::string::npos) ||
            (filename.find(".GRV",0) != std::string::npos) )
      {
          ParseGRVGravityFile(filename);
      }
      else if ((filename.find(".cof",0) != std::string::npos) ||
            (filename.find(".COF",0) != std::string::npos) )
      {
               ParseCOFGravityFile(filename);  
          }
          else
          {
             MessageInterface::PopupMessage
         (Gmat::WARNING_, "Gravity file \"" + filename + "\" is of unknown format.");
         return;
          }

      forceList[currentBodyId]->potFilename = std::string(filename.c_str());
      forceList[currentBodyId]->gravType = gravModelArray[OTHER];
          
      DisplayGravityFieldData();      
      theApplyButton->Enable(true);
   }
}

//------------------------------------------------------------------------------
// void OnSetupButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSetupButton(wxCommandEvent &event)
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
      
      theApplyButton->Enable(true);
   }
}

//------------------------------------------------------------------------------
// void OnMagSearchButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnMagSearchButton(wxCommandEvent &event)
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
// void OnPMEditButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnPMEditButton(wxCommandEvent &event)
{  
   CelesBodySelectDialog bodyDlg(this, secondaryBodiesArray, primaryBodiesArray);
   bodyDlg.ShowModal();
   
   if (bodyDlg.IsBodySelected())
   {
      wxArrayString &names = bodyDlg.GetBodyNames();
      
      if (names.IsEmpty())
      {
         pmForceList.clear();
         secondaryBodiesArray.Clear(); 
         pmEditTextCtrl->Clear();
         theApplyButton->Enable(true);
         isForceModelChanged = true;
         return;
      }
      
      std::vector<ForceType*> fl;
      
      std::string bodyName;

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
         pmForceList.push_back(new ForceType(bodyName));
         
         secondaryBodiesArray.Add(bodyName.c_str());
         pmEditTextCtrl->AppendText(names[i] + " ");
      }
      
      theApplyButton->Enable(true);
      isForceModelChanged = true;
   }
}   

//------------------------------------------------------------------------------
// void OnSRPEditButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPEditButton(wxCommandEvent &event)
{
   theApplyButton->Enable(true);
}

// wxTextCtrl Events
//------------------------------------------------------------------------------
// void OnIntegratorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnIntegratorTextUpdate(wxCommandEvent &event)
{
   isIntegratorChanged = true;

   theApplyButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravityTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);

   if (event.GetEventObject() == gravityDegreeTextCtrl)
   {
      forceList[currentBodyId]->gravDegree = gravityDegreeTextCtrl->GetValue().c_str();
      isForceModelChanged = true;
   }
   else if (event.GetEventObject() == gravityOrderTextCtrl)
   {     
      forceList[currentBodyId]->gravOrder = gravityOrderTextCtrl->GetValue().c_str();
      isForceModelChanged = true;
   }
   else if (event.GetEventObject() == potFileTextCtrl)
   {
      isPotFileChanged = true;
      isForceModelChanged = true;
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
// void OnSRPCheckBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPCheckBoxChange(wxCommandEvent &event)
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

//------------------------------------------------------------------------------
// bool ParseDATGravityFile(std::string fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseDATGravityFile(std::string fname)
{  
//   Integer      cc, dd, sz=0;
//   Integer      iscomment, rtn;
//   Integer      n=0, m=0;
//   Integer      fileDegree, fileOrder;
//   Real         Cnm=0.0, Snm=0.0, dCnm=0.0, dSnm=0.0;
//   char         buf[CelestialBody::BUFSIZE];
//   FILE        *fp;
//
//   for (cc = 2;cc <= HF_MAX_DEGREE; ++cc)
//   {
//      for (dd = 0; dd <= cc; ++dd)
//      {
//         sz++;
//      }
//   }
//   
//   /* read coefficients from file */
//   fp = fopen( fname.c_str(), "r");
//   if (!fp)
//   {
//      MessageInterface::PopupMessage
//         (Gmat::WARNING_, "Error reading gravity potential file.");
//         return;
//   }
//   
//   PrepareGravityArrays();
//   iscomment = 1;
//   
//   while ( iscomment )
//   {
//      rtn = fgetc( fp );
//        
//      if ( (char)rtn == '#' )
//      {
//         fgets( buf, CelestialBody::BUFSIZE, fp );
//      }
//      else
//      {
//         ungetc( rtn, fp );
//         iscomment = 0;
//      }
//   }
//
//   fscanf(fp, "%lg\n", &mu ); mu = (Real)mu / 1.0e09;      // -> Km^3/sec^2
//   fscanf(fp, "%lg\n", &a ); a = (Real)a / 1000.0;         // -> Km
//   fgets( buf, CelestialBody::BUFSIZE, fp );
//   
//   while ( ( (char)(rtn=fgetc(fp)) != '#' ) && (rtn != EOF) )
//   {
//      ungetc( rtn, fp );
//      fscanf( fp, "%i %i %le %le\n", &n, &m, &dCnm, &dSnm );
//      if ( n <= GRAV_MAX_DRIFT_DEGREE  && m <= n )
//      {
//         dCbar[n][m] = (Real)dCnm;
//         dSbar[n][m] = (Real)dSnm;
//      }
//   }
//
//   fgets( buf, CelestialBody::BUFSIZE, fp );
//
//   fileDegree = 0;
//   fileOrder  = 0;
//   cc=0;n=0;m=0;
//   
//   do
//   {
//      if ( n <= HF_MAX_DEGREE && m <= HF_MAX_ORDER )
//      {
//         Cbar[n][m] = (Real)Cnm;
//         Sbar[n][m] = (Real)Snm;
//      }
//      if (n > fileDegree) fileDegree = n;
//      if (n > fileOrder)  fileOrder  = n;
//      
//      cc++;
//   } while ( ( cc<=sz ) && ( fscanf( fp, "%i %i %le %le\n", &n, &m, &Cnm, &Snm ) > 0 ));
//   
//   // Save as string
//   forceList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
//   forceList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
   return;
}

//------------------------------------------------------------------------------
// bool ParseGRVGravityFile(std::string fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseGRVGravityFile(std::string fname)
{
   Integer       fileOrder, fileDegree;

   std::ifstream inFile;
   
   inFile.open(fname.c_str());
   
   if (!inFile)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Error reading gravity potential file.");
         return;
   }

   std::string s;
   std::string firstStr;
   
   while (!inFile.eof())
   {
      getline(inFile,s);
      std::istringstream lineStr;
      lineStr.str(s);
          
      // ignore comment lines
      if (s[0] != '#')
      {
         lineStr >> firstStr;
         
                 if (strcasecmp(firstStr.c_str(),"Degree") == 0)
            lineStr >> fileDegree;
         else if (strcasecmp(firstStr.c_str(),"Order") == 0)
            lineStr >> fileOrder;
      }
   }

   // Save as string
   forceList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
   forceList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
}

//------------------------------------------------------------------------------
// bool ParseCOFGravityFile(std::string fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseCOFGravityFile(std::string fname)
{
   Integer       fileOrder, fileDegree;
   Integer       int1;
   Real          real1, real2, real3;

   std::ifstream inFile;
   inFile.open(fname.c_str());
   
   bool done = false;
      
   if (!inFile)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Error reading gravity potential file.");
         return;
   }

   std::string s;
   std::string firstStr;
   
   while (!done)
   {
      getline(inFile,s);
          
      std::istringstream lineStr;
          
      lineStr.str(s);
          
      // ignore comment lines
      if (s[0] != 'C')
      {
         lineStr >> firstStr;
                 
         if (firstStr == "POTFIELD")
                 {
            lineStr >> fileDegree >> fileOrder >> int1 >> real1 >> real2 >> real3;
                        done = true;
                 }
      }
   }

   // Save as string    
   forceList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
   forceList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
}

//------------------------------------------------------------------------------
// void PrepareGravityArrays()
//------------------------------------------------------------------------------
void PropagationConfigPanel::PrepareGravityArrays()
{
   Integer m, n;
   
   for (n=0; n <= HarmonicField::HF_MAX_DEGREE; ++n)
      for ( m=0; m <= HarmonicField::HF_MAX_ORDER; ++m)
      {
         Cbar[n][m] = 0.0;
         Sbar[n][m] = 0.0;
      }
      
   for (n = 0; n <= GRAV_MAX_DRIFT_DEGREE; ++n) {
      for (m = 0; m <= GRAV_MAX_DRIFT_DEGREE; ++m) {
         dCbar[n][m] = 0.0;
         dSbar[n][m] = 0.0;
      }
   }   
}
