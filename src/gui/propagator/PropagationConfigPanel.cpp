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
 * This class contains the Propagation configuration window.
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
//#define DEBUG_PROP_SAVE 1
//#define DEBUG_GRAV_FIELD 1

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
   EVT_COMBOBOX(ID_CB_ERROR, PropagationConfigPanel::OnErrorControlSelection)
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
   thePropagatorName = "";
   thePropSetup      = NULL;
   thePropagator     = NULL;
   integratorString  = "RKV 8(9)";
   
   forceList.clear();
   pmForceList.clear();
   earthGravModelArray.clear();
   lunaGravModelArray.clear();
   venusGravModelArray.clear();
   marsGravModelArray.clear();
   othersGravModelArray.clear();
   dragModelArray.clear();
   magfModelArray.clear();
   errorControlArray.clear();
   
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
   isDegOrderChanged = false;
   isPotFileChanged = false;
   isMagfTextChanged = false;
   isIntegratorChanged = false;
   isIntegratorDataChanged = false;
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
   
   earthGravModelArray.clear();
   lunaGravModelArray.clear();
   venusGravModelArray.clear();
   marsGravModelArray.clear();
   othersGravModelArray.clear();
   dragModelArray.clear();
   magfModelArray.clear();
   errorControlArray.clear();
   theFileMap.clear();
   
   primaryBodiesArray.Clear();
   secondaryBodiesArray.Clear();
   integratorArray.Clear();

   
   // Unregister GUI components
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
   
   std::string propType = thePropagator->GetTypeName();
   
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
      //magfComboBox->Enable(false);
      //srpCheckBox->Enable(false);
   }
   else
   {
      gravComboBox->Enable(true);
      atmosComboBox->Enable(true);
      //magfComboBox->Enable(true);
      //srpCheckBox->Enable(true);       
   } 
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveData()
{
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("SaveData() thePropagatorName=%s\n", thePropagator->GetTypeName().c_str());
   #endif

   canClose = true;
   
   //-------------------------------------------------------
   // Saving the Integrator
   //-------------------------------------------------------
   if (isIntegratorChanged)
   {
      #if DEBUG_PROP_PANEL
      ShowPropData("SaveData() BEFORE saving Integrator");
      #endif
      
      isIntegratorChanged = false;

      if (isIntegratorDataChanged)
      {
         isIntegratorDataChanged = false;
         if (!SaveIntegratorData())
            canClose = false;
      }
      
      thePropSetup->SetPropagator(thePropagator);
      
   }
   else if (isIntegratorDataChanged)
   {
      isIntegratorDataChanged = false;
      if (!SaveIntegratorData())
         canClose = false;
   }

   
   //-------------------------------------------------------
   // Saving the force model
   //-------------------------------------------------------
   if (isForceModelChanged)
   {      
      #if DEBUG_PROP_SAVE
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

      //----------------------------------------------------
      // save gavity force model
      //----------------------------------------------------
      std::string theBody;
      for (Integer i=0; i < (Integer)forceList.size(); i++)
      {
         if (forceList[i]->gravType != "None")
         {
            // if gravity force pointer is NULL then create.
            theGravForce = forceList[i]->gravf;
            theBody = forceList[i]->bodyName.c_str();

            // Create new GravityField since ForceModel destructor will delete
            // all PhysicalModel
            #if DEBUG_PROP_SAVE
            MessageInterface::ShowMessage
               ("SaveData() Creating GravityField for %s\n", theBody.c_str());
            #endif
            
            theGravForce = new GravityField("", theBody);
            theGravForce->SetSolarSystem(theSolarSystem);
            theGravForce->SetBodyName(theBody);
            theGravForce->SetStringParameter("PotentialFile", forceList[i]->potFilename);
            forceList[i]->gravf = theGravForce;
            
            newFm->AddForce(theGravForce);
         }
      }
      
      if (isDegOrderChanged)
         SaveDegOrder();
      
      if (isPotFileChanged)
         SavePotFile();
      
      //----------------------------------------------------
      // save drag force model
      //----------------------------------------------------
      Integer paramId;
      
      for (Integer i=0; i < (Integer)forceList.size(); i++)
      {
         if (forceList[i]->dragType != dragModelArray[NONE_DM])
         {
            #if DEBUG_PROP_SAVE
            ShowForceList("SaveData() BEFORE  saving DragForce");
            #endif
            
            theDragForce = new DragForce(forceList[i]->dragType);
            theCelestialBody = theSolarSystem->GetBody(forceList[i]->bodyName); 
            theAtmosphereModel = theCelestialBody->GetAtmosphereModel();
            
            #if DEBUG_PROP_SAVE
            ShowForceList("Entering if (theAtmosphereModel == NULL)");
            #endif
            
            if (theAtmosphereModel == NULL)  
            {
               theAtmosphereModel = theGuiInterpreter->CreateAtmosphereModel
                  (forceList[i]->dragType.c_str(), forceList[i]->dragType.c_str(),
                   forceList[i]->bodyName.c_str());
               
               //loj: 6/9/05 Commented out - it's set in the DragForce::Initialize()
               //theCelestialBody->SetAtmosphereModel(theAtmosphereModel);
               
               #if DEBUG_PROP_SAVE
               ShowForceList("Exiting if (theAtmosphereModel == NULL)");
               #endif
            }
            
            theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);
            
            #if DEBUG_PROP_SAVE
            ShowForceList("theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);");
            #endif
            
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            theDragForce->SetStringParameter(paramId, forceList[i]->dragType.c_str());
            theDragForce->SetStringParameter("BodyName", forceList[i]->bodyName.c_str());
            forceList[i]->dragf = theDragForce;
            newFm->AddForce(theDragForce);
            
            #if DEBUG_PROP_SAVE
            ShowForceList("SaveData() AFTER  saving DragForce");
            #endif 
         }
      }
      
      //----------------------------------------------------
      // save SRP data
      //----------------------------------------------------
      for (Integer i=0; i < (Integer)forceList.size(); i++)
      {
         // only Earth for B3
         if (forceList[i]->useSrp && forceList[i]->bodyName == SolarSystem::EARTH_NAME)
         {
            theSRP = new SolarRadiationPressure();
            forceList[i]->srpf = theSRP;
            newFm->AddForce(theSRP);
            
            paramId= newFm->GetParameterID("SRP");
            newFm->SetStringParameter(paramId, "On");

            #if DEBUG_PROP_SAVE
            ShowForceList("SaveData() AFTER  saving SRP");
            #endif
            
         }
      }
      
      //----------------------------------------------------
      // Saving the error control
      //----------------------------------------------------
      newFm->SetStringParameter("ErrorControl", 
                                    errorComboBox->GetStringSelection().c_str());
      
      //----------------------------------------------------
      // Saving the Origin (Central Body)
      //----------------------------------------------------
      if (isOriginChanged)
      {
         newFm->SetStringParameter("CentralBody", propOriginName.c_str());
         isOriginChanged = false;
      }
      
      //----------------------------------------------------
      // Saving forces to the prop setup
      //----------------------------------------------------
      thePropSetup->SetForceModel(newFm);
      numOfForces = thePropSetup->GetNumForces();

      //loj: 7/11/06 forceModel is deleted in PropSetup::SetForceModel()
      //if (theForceModel->GetName() == "")
      //   delete theForceModel;
      
      theForceModel = newFm;
      
      #if DEBUG_PROP_SAVE
      ShowForceList("SaveData() AFTER  saving ForceModel");
      #endif
   } // end if(isForceModelChange)
   else
   {
      #if DEBUG_PROP_SAVE
      ShowForceList("SaveData() BEFORE  saving ForceModel");
      #endif
      
      //----------------------------------------------------
      // Saving the Origin (Central Body)
      //----------------------------------------------------
      if (isOriginChanged)
      {
         theForceModel->SetStringParameter("CentralBody", propOriginName.c_str());
         isOriginChanged = false;
      }
      
      if (isDegOrderChanged)
         SaveDegOrder();
      
      // Save only GravComboBox or PotFileText is changed
      if (isPotFileChanged)
         SavePotFile();
      
      #if DEBUG_PROP_SAVE
      ShowForceList("SaveData() AFTER  saving ForceModel");
      #endif
      
   }

   #if DEBUG_PROP_SAVE
   ShowForceModel("SaveData() AFTER  saving ForceModel");
   #endif
   
}

//------------------------------------------------------------------------------
// Integer FindBody(const std::string &bodyName, const std::string &gravType,...)
//------------------------------------------------------------------------------
/*
 * If body is found retun index, otherwise create a new ForceType and return
 * new index
 */
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

   // Set potential file
   if (theFileMap.find(gravType) != theFileMap.end())
   {
      std::string potFileType = theFileMap[gravType];
      //MessageInterface::ShowMessage("===> potFile=%s\n", potFileType.c_str());
      forceList.back()->potFilename = theGuiInterpreter->GetFileName(potFileType);
   }
   
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
   
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   thePropSetup = theGuiInterpreter->GetPropSetup(propSetupName);

   //Note: All the settings should match enum types in the header.
   
   // initialize integrator type array for ComboBox
   integratorArray.Add("RKV 8(9)");
   integratorArray.Add("RKN 6(8)");
   integratorArray.Add("RKF 5(6)");
   integratorArray.Add("PD  4(5)");
   integratorArray.Add("PD  7(8)");
   integratorArray.Add("BS");
   integratorArray.Add("ABM");
   integratorArray.Add("Cowell");
   
   // initialize integrator type array for creating
   propagatorTypeArray.push_back("RungeKutta89");
   propagatorTypeArray.push_back("DormandElMikkawyPrince68");
   propagatorTypeArray.push_back("RungeKuttaFehlberg56");
   propagatorTypeArray.push_back("PrinceDormand45");
   propagatorTypeArray.push_back("PrinceDormand78");
   propagatorTypeArray.push_back("BulirschStoer");
   propagatorTypeArray.push_back("AdamsBashforthMoulton");
   propagatorTypeArray.push_back("Cowell");

   // initialize gravity model type arrays
   earthGravModelArray.push_back("None");
   earthGravModelArray.push_back("JGM-2");
   earthGravModelArray.push_back("JGM-3");
   earthGravModelArray.push_back("EGM-96");
   earthGravModelArray.push_back("Other");
   
   lunaGravModelArray.push_back("None");
   lunaGravModelArray.push_back("LP-165");
   lunaGravModelArray.push_back("Other");
   
   venusGravModelArray.push_back("None");
   venusGravModelArray.push_back("MGNP-180U");
   venusGravModelArray.push_back("Other");
   
   marsGravModelArray.push_back("None");
   marsGravModelArray.push_back("Mars-50C");
   marsGravModelArray.push_back("Other");
   
   othersGravModelArray.push_back("None");
   othersGravModelArray.push_back("Other");
   
   // initialize drag model type array
   dragModelArray.push_back("None");
   dragModelArray.push_back("Exponential");
   dragModelArray.push_back("MSISE90");
   dragModelArray.push_back("JacchiaRoberts");
   
   // initialize error control type array
   errorControlArray.push_back("None");
   errorControlArray.push_back("RSSStep");
   errorControlArray.push_back("RSSState");
   errorControlArray.push_back("LargestStep");
   errorControlArray.push_back("LargestState");

   // for actual file keyword used in FileManager
   theFileMap["JGM-2"] = "JGM2_FILE";
   theFileMap["JGM-3"] = "JGM3_FILE";
   theFileMap["EGM-96"] = "EGM96_FILE";
   theFileMap["LP-165"] = "LP165P_FILE";
   theFileMap["MGNP-180U"] = "MGNP180U_FILE";
   theFileMap["Mars-50C"] = "MARS50C_FILE";
   
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Initialize() Initialized local arrays.\n");
   #endif
   
   // initialize mag. filed model type array
   magfModelArray.push_back("None");
   
   if (thePropSetup != NULL)
   {
      thePropagator = thePropSetup->GetPropagator();
      //thePropagator = (Propagator*)theProp->Clone();
      theForceModel = thePropSetup->GetForceModel();
      numOfForces   = thePropSetup->GetNumForces();
      
      // Getting the origin
      propOriginName = theForceModel->GetStringParameter("CentralBody");
      
      // Getting the error control
      errorControlTypeName = theForceModel->GetStringParameter("ErrorControl");
      
      PhysicalModel *force;     
      Integer paramId;
      std::string bodyName;
      wxString tempStr;
      wxString useSRP;
      
      paramId = theForceModel->GetParameterID("SRP");
      useSRP = theForceModel->GetStringParameter(paramId).c_str();
      
//      if (useSRP.CmpNoCase("On") == 0)
//      {                                   
//         currentBodyId = FindBody(SolarSystem::EARTH_NAME);
//         forceList[currentBodyId]->bodyName = SolarSystem::EARTH_NAME;
//         forceList[currentBodyId]->useSrp = true;
//         forceList[currentBodyId]->srpf = theSRP;
//      }
     
      for (Integer i = 0; i < numOfForces; i++)
      {
         force = theForceModel->GetForce(i);
         
         if (force->GetTypeName() == "PointMassForce")
         {
            #if DEBUG_PROP_PANEL
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() Initializing PointMassForce\n");
            #endif
            
            thePMF = (PointMassForce *)force;
            bodyName = thePMF->GetStringParameter("BodyName");
            secondaryBodiesArray.Add(bodyName.c_str());    
            pmForceList.push_back(new ForceType(bodyName, "None", 
               dragModelArray[NONE_DM], magfModelArray[NONE_MM])); 
               
            //Warn user about bodies already added as Primary body
            Integer fmSize = (Integer)forceList.size();
            Integer last = (Integer)pmForceList.size() - 1;
            
            for (Integer i = 0; i < fmSize; i++)
            {
                if (strcmp(pmForceList[last]->bodyName.c_str(), 
                           forceList[i]->bodyName.c_str()) == 0)
                {
                   MessageInterface::PopupMessage
                      (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass",
                       pmForceList[last]->bodyName.c_str());
                }
            }

         }
         else if (force->GetTypeName() == "GravityField")
         {
            #if DEBUG_GRAV_FORCE
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() Initializing GravityField force.\n");
            #endif
            
            theGravForce = (GravityField*)force;
            bodyName = theGravForce->GetStringParameter("BodyName");
            std::string potFilename = theGravForce->GetStringParameter("PotentialFile"); 
            
            currentBodyId = FindBody(bodyName);
            forceList[currentBodyId]->bodyName = bodyName;
            forceList[currentBodyId]->potFilename = potFilename;
            
            if (bodyName == "Earth")
            {
               #if DEBUG_GRAV_FIELD
               MessageInterface::ShowMessage
                  ("Initialize() Getting gravity model type for Earth.\n   potFilename=%s\n",
                   potFilename.c_str());
               #endif
               
               EarthGravModelType eGravModelType;
               
               if (potFilename.find("JGM2") != std::string::npos)
                  eGravModelType = JGM2;    
               else if (potFilename.find("JGM3") != std::string::npos)
                  eGravModelType = JGM3;   
               else if (potFilename.find("EGM") != std::string::npos)
                  eGravModelType = EGM96;  
               else 
                  eGravModelType = E_OTHER;
               
               forceList[currentBodyId]->gravType = earthGravModelArray[eGravModelType];
               forceList[currentBodyId]->potFilename =
                  theFileMap[earthGravModelArray[eGravModelType]];
            }
            else if (bodyName == "Luna")
            {
               #if DEBUG_GRAV_FIELD
               MessageInterface::ShowMessage
                  ("Initialize() Getting gravity model type for Luna\n");
               #endif
               
               LunaGravModelType lGravModelType;
               
               if (potFilename.find("lp165p") != std::string::npos)
                  lGravModelType = LP165;     
               else 
                  lGravModelType = L_OTHER;
               
               forceList[currentBodyId]->gravType = lunaGravModelArray[lGravModelType];
               forceList[currentBodyId]->potFilename =
                  theFileMap[lunaGravModelArray[lGravModelType]];
            }
            else if (bodyName == "Venus")
            {
               #if DEBUG_GRAV_FIELD
               MessageInterface::ShowMessage
                  ("Initialize() Getting gravity model type for Venus\n");
               #endif
               
               VenusGravModelType vGravModelType;
               
               if (potFilename.find("MGN") != std::string::npos)
                  vGravModelType = MGNP180U;     
               else 
                  vGravModelType = V_OTHER;
                  
               forceList[currentBodyId]->gravType = venusGravModelArray[vGravModelType];
               forceList[currentBodyId]->potFilename =
                  theFileMap[venusGravModelArray[vGravModelType]];
            }
            else if (bodyName == "Mars")
            {
               #if DEBUG_GRAV_FIELD
               MessageInterface::ShowMessage
                  ("Initialize() Getting gravity model type for Mars\n");
               #endif
               
               MarsGravModelType mGravModelType;
               
               if (potFilename.find("Mars50c") != std::string::npos)
                  mGravModelType = MARS50C;     
               else 
                  mGravModelType = M_OTHER;
               
               forceList[currentBodyId]->gravType = marsGravModelArray[mGravModelType];
               forceList[currentBodyId]->potFilename =
                  theFileMap[marsGravModelArray[mGravModelType]];
            }   
            else //other bodies
            {
               #if DEBUG_GRAV_FIELD
               MessageInterface::ShowMessage
                  ("Initialize() Getting gravity model type for other bodies\n");
               #endif
               
               OthersGravModelType oGravModelType;
               
               oGravModelType = O_OTHER;
               
               forceList[currentBodyId]->gravType = othersGravModelArray[oGravModelType];
               forceList[currentBodyId]->potFilename =
                  theFileMap[othersGravModelArray[oGravModelType]];
            }
            
            #if DEBUG_GRAV_FIELD
            MessageInterface::ShowMessage("Initialize() Initializing the gravity force\n");
            #endif
            
            forceList[currentBodyId]->gravf = theGravForce;
            forceList[currentBodyId]->potFilename = potFilename;
            
            //Warn user about bodies already added as Primary body
            Integer pmSize = (Integer)pmForceList.size();
            Integer last = (Integer)forceList.size() - 1;
            
            for (Integer i = 0; i < pmSize; i++)
            {
                if (strcmp(forceList[last]->bodyName.c_str(), 
                           pmForceList[i]->bodyName.c_str()) == 0)
                {
                   MessageInterface::PopupMessage
                      (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass",
                       pmForceList[last]->bodyName.c_str());
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
            #if DEBUG_PROP_PANEL
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() Initializing DragForce.\n");
            #endif
            
            theDragForce = (DragForce*)force;  
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            atmosModelString = theDragForce->GetStringParameter(paramId).c_str();
            
            bodyName = theDragForce->GetStringParameter("BodyName");
                        
            currentBodyId = FindBody(bodyName);
            forceList[currentBodyId]->bodyName = bodyName;
            forceList[currentBodyId]->dragType = atmosModelString;
            forceList[currentBodyId]->dragf = theDragForce;
            
            //Warn user about bodies already added as Primary body
            Integer pmSize = (Integer)pmForceList.size();
            Integer last = (Integer)forceList.size() - 1;
            
            for (Integer i = 0; i < pmSize; i++)
            {
                if (strcmp(forceList[last]->bodyName.c_str(), 
                           pmForceList[i]->bodyName.c_str()) == 0)
                {
                   MessageInterface::PopupMessage
                      (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass",
                       pmForceList[last]->bodyName.c_str());
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
         else if (force->GetTypeName() == "SolarRadiationPressure")
         {
            #if DEBUG_PROP_PANEL
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() Initializing SRP force.\n");
            #endif
                           
            #if DEBUG_PROP_PANEL
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() First check if Earth is set "
                "as primary body.\n");
            #endif
            
            bool foundEarth = false;
            
            for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
            {
               if ( primaryBodiesArray[i].CmpNoCase(SolarSystem::EARTH_NAME.c_str()) == 0 )
                  foundEarth = true;
            } 
            
            #if DEBUG_PROP_PANEL
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() Checking if SRP is set for "
                "earth only.\n");
            #endif
            
            if ( strcmp(bodyName.c_str(), SolarSystem::EARTH_NAME.c_str()) == 0 )
            {
               currentBodyId = FindBody(bodyName);
               forceList[currentBodyId]->useSrp = true;
               forceList[currentBodyId]->bodyName = SolarSystem::EARTH_NAME;
               forceList[currentBodyId]->srpf = theSRP;
            }
            else
            {
               forceList[currentBodyId]->useSrp = false;
               forceList[currentBodyId]->srpf = NULL;
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "GMAT currently supports SRP for Earth only.  "
                   "SRP for other bodies will be turned off."); 
            }   
         }
      }
           
      #if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::Initialize() Initializing Primary bodies array.\n");
      #endif
      
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
      ShowForceList("Initialize() exiting");
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
      new wxStaticText( parent, ID_TEXT, wxT("Initial Step Size"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Accuracy"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Step Size"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting4StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Size"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting5StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Attempts"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting6StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Lower Error"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );
   setting7StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Target Error"),
                        wxDefaultPosition, wxSize(170,20),
                        wxST_NO_AUTORESIZE );  
   degree1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   order1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   potFileStaticText =
      //new wxStaticText( parent, ID_TEXT, wxT("Other Potential Field File:"),
      new wxStaticText( parent, ID_TEXT, wxT("Potential Field File"),
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
   type4StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
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
                      wxSize(30,-1), 0 );
   gravityOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   potFileTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      //wxSize(165,-1), 0 );
                      wxSize(290,-1), 0 );
   magneticDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   magneticOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
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
     
   wxString bodyArray[]  = {};
   wxString gravArray[]  = {};
     
   wxString *dragArray = new wxString[EarthDragModelCount];
   for (Integer i=0; i<EarthDragModelCount; i++)
      dragArray[i] = dragModelArray[i].c_str();
     
   wxString *magfArray = new wxString[MagfModelCount];
   for (Integer i=0; i<MagfModelCount; i++)
      magfArray[i] = magfModelArray[i].c_str();
      
   wxString *errorArray = new wxString[ErrorControlCount];
   for (Integer i=0; i<ErrorControlCount; i++)
      errorArray[i] = errorControlArray[i].c_str();
                 
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Setup() create wxComboBox\n"
       "IntegratorCount=%d, GravModelCount=%d, EarthDragModelCount=%d, "
       "MagfModelCount=%d\n", IntegratorCount, GravModelCount, EarthDragModelCount,
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
                      wxDefaultPosition,  wxSize(80,-1), 0,
                      bodyArray, wxCB_DROPDOWN|wxCB_READONLY );
   gravComboBox =
      new wxComboBox( parent, ID_CB_GRAV, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0,
                      gravArray, wxCB_DROPDOWN|wxCB_READONLY );
   atmosComboBox =
      new wxComboBox( parent, ID_CB_ATMOS, wxT(dragArray[0]),
                      wxDefaultPosition, wxDefaultSize, EarthDragModelCount,
                      dragArray, wxCB_DROPDOWN|wxCB_READONLY );
   magfComboBox =
      new wxComboBox( parent, ID_CB_MAG, wxT(magfArray[0]),
                      wxDefaultPosition, wxDefaultSize, MagfModelCount,
                      magfArray, wxCB_DROPDOWN|wxCB_READONLY );
                      
   errorComboBox =
      new wxComboBox( parent, ID_CB_ERROR, wxT(errorArray[0]),
                      wxDefaultPosition, wxDefaultSize, ErrorControlCount,
                      errorArray, wxCB_DROPDOWN|wxCB_READONLY );
                 
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
      new wxStaticText( parent, ID_TEXT, wxT("Atmosphere Model"),
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
   title7StaticText->Enable(false);
   
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
   wxStaticText *title10StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Error Control"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title10StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
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
   wxBoxSizer *boxSizer14 = new wxBoxSizer( wxHORIZONTAL );
   leftBoxSizer = new wxBoxSizer( wxVERTICAL );
   
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
   
   leftBoxSizer->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
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
   
   boxSizer14->Add( type4StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer14->Add( errorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer1->Add( boxSizer4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   //boxSizer1->Add( boxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer1->Add( leftBoxSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
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
   boxSizer5->Add( title10StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add( boxSizer14, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
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
   leftBoxSizer = new wxBoxSizer( wxVERTICAL );
   
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
   
   wxStaticBox *item46 = new wxStaticBox( parent, -1, wxT("Atmosphere Model") );
   wxStaticBoxSizer *item45 = new wxStaticBoxSizer( item46, wxHORIZONTAL );
   wxStaticBox *item51 = new wxStaticBox( parent, -1, wxT("Magnetic Field") );
   wxStaticBoxSizer *item50 = new wxStaticBoxSizer( item51, wxHORIZONTAL );
   wxStaticBox *staticBox7 = new wxStaticBox( parent, -1, wxT("Point Masses") );
   wxStaticBoxSizer *staticBoxSizer7 = new wxStaticBoxSizer( staticBox7, wxVERTICAL );
   wxStaticBox *item65 = new wxStaticBox( parent, -1, wxT("Solar Radiation Pressure") );
   wxStaticBoxSizer *item64 = new wxStaticBoxSizer( item65, wxHORIZONTAL );
   wxStaticBox *staticBox10 = new wxStaticBox( parent, -1, wxT("Error Control") );
   wxStaticBoxSizer *staticBoxSizer10 = new wxStaticBoxSizer( staticBox10, wxHORIZONTAL );
   
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
    
   item64->Add( 2, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   item64->Add( srpCheckBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   staticBoxSizer10->Add( type4StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer10->Add( errorComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   staticBoxSizer3->Add( boxSizer3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer3->Add( item50, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   staticBoxSizer7->Add( flexGridSizer2, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   staticBoxSizer1->Add( flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   staticBoxSizer2->Add( staticBoxSizer3, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer2->Add( staticBoxSizer7, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer2->Add( item64, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   staticBoxSizer2->Add( staticBoxSizer10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   staticBoxSizer4->Add(centralBodyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticBoxSizer4->Add(originComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   boxSizer4->Add( staticBoxSizer1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer4->Add( 0, 0, 1);
   boxSizer4->Add( staticBoxSizer4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
   leftBoxSizer->Add( boxSizer4, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
   //boxSizer2->Add( boxSizer4, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer2->Add( leftBoxSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   boxSizer2->Add( staticBoxSizer2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
   boxSizer1->Add( boxSizer2, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(boxSizer1, 0, wxGROW, bsize);
#endif

   atmosComboBox->Enable(true);
   
   //------------------------------
   //@todo: Future implementations
   //------------------------------ 
   magfComboBox->Enable(false);
   magneticDegreeTextCtrl->Enable(false);
   magneticOrderTextCtrl->Enable(false);
   searchMagneticButton->Enable(false);
   type3StaticText->Enable(false);
   degree2StaticText->Enable(false);
   order2StaticText->Enable(false);
   
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void DisplayIntegratorData(bool integratorChanged)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{
   int propIndex = integratorComboBox->GetSelection();
   
   #if DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("DisplayIntegratorData() integratorChanged=%d, integratorString=<%s>\n",
       integratorChanged, integratorString.c_str());
   #endif
   
   if (integratorChanged)
   {
      thePropagatorName = propSetupName + propagatorTypeArray[propIndex];
      thePropagator = theGuiInterpreter->GetPropagator(thePropagatorName);
      if (thePropagator == NULL)
         thePropagator =
            theGuiInterpreter->CreatePropagator(propagatorTypeArray[propIndex],
                                                thePropagatorName);
   }

   
#if __WXMAC__
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {   
      setting6StaticText->Enable(true);
      setting7StaticText->Enable(true);
      setting6TextCtrl->Enable(true);
      setting7TextCtrl->Enable(true);
   }
   else
   {   
      setting6StaticText->Enable(false);
      setting7StaticText->Enable(false);
      setting6TextCtrl->Enable(false);
      setting7TextCtrl->Enable(false);
   }
#else
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {
      setting6StaticText->Show(true);
      setting7StaticText->Show(true);
      setting6TextCtrl->Show(true);
      setting7TextCtrl->Show(true);
   }
   else
   {   
      setting6StaticText->Show(false);
      setting7StaticText->Show(false);
      setting6TextCtrl->Show(false);
      setting7TextCtrl->Show(false);
   }
#endif

   leftBoxSizer->Layout();
    
   Real i1 = thePropagator->GetRealParameter("InitialStepSize");
   Real i2 = thePropagator->GetRealParameter("Accuracy");
   Real i3 = thePropagator->GetRealParameter("MinStep");
   Real i4 = thePropagator->GetRealParameter("MaxStep");
   Integer i5 = (long)thePropagator->GetIntegerParameter("MaxStepAttempts");

   setting1TextCtrl->SetValue(ToString(i1));
   setting2TextCtrl->SetValue(ToString(i2));
   setting3TextCtrl->SetValue(ToString(i3));
   setting4TextCtrl->SetValue(ToString(i4));
   setting5TextCtrl->SetValue(ToString(i5));
 
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {  
      Real i6 = thePropagator->GetRealParameter("LowerError");   
      Real i7 = thePropagator->GetRealParameter("TargetError");
      
      setting6TextCtrl->SetValue(ToString(i6));
      setting7TextCtrl->SetValue(ToString(i7));
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
   {  	
   	   DisplayErrorControlData();
      return;  
   }
   
   DisplayPrimaryBodyData(); 
   DisplayGravityFieldData(currentBodyName); 
   DisplayAtmosphereModelData(); 
   DisplayMagneticFieldData();  
   DisplaySRPData();
   DisplayErrorControlData();
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
// void DisplayGravityFieldData(std::string bodyName)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayGravityFieldData(std::string bodyName)
{
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("DisplayGravityFieldData() currentBodyName=%s gravType=%s\n",
       currentBodyName.c_str(), forceList[currentBodyId]->gravType.c_str());
   ShowForceList("DisplayGravityFieldData() entered");
   #endif
   
   gravComboBox->Clear();

   std::string gravType = forceList[currentBodyId]->gravType;
   
   // for gravity model ComboBox
   if (bodyName == "Earth")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Earth gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)EarthGravModelCount; i++)
         gravComboBox->Append(earthGravModelArray[i].c_str());

   }
   else if (bodyName == "Luna")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Luna gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)LunaGravModelCount; i++)
         gravComboBox->Append(lunaGravModelArray[i].c_str());
   }
   else if (bodyName == "Venus")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Venus gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)VenusGravModelCount; i++)
         gravComboBox->Append(venusGravModelArray[i].c_str());

   }
   else if (bodyName == "Mars")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Mars gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)MarsGravModelCount; i++)
         gravComboBox->Append(marsGravModelArray[i].c_str());

   }
   else // other bodies
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying other gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)OthersGravModelCount; i++)
         gravComboBox->Append(othersGravModelArray[i].c_str());

   }
   
   searchGravityButton->Enable(false);
   potFileStaticText->Enable(false);
   potFileTextCtrl->Enable(false);
   gravityDegreeTextCtrl->Enable(true);
   gravityOrderTextCtrl->Enable(true);
   //potFileTextCtrl->SetValue("");
   potFileTextCtrl->SetValue(forceList[currentBodyId]->potFilename.c_str());
   
   if (gravType == "None")
   {
      gravityDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
   }
   else
   {
      if (forceList[currentBodyId]->gravType == "Other")
      {
         searchGravityButton->Enable(true);
         potFileStaticText->Enable(true);
         potFileTextCtrl->Enable(true);
         //potFileTextCtrl->SetValue(forceList[currentBodyId]->potFilename.c_str());
      }
   }
   
   gravComboBox->SetValue(gravType.c_str());
   gravityDegreeTextCtrl->SetValue(forceList[currentBodyId]->gravDegree.c_str());
   gravityOrderTextCtrl->SetValue(forceList[currentBodyId]->gravOrder.c_str());
   
   #if DEBUG_PROP_PANEL
   ShowForceList("DisplayGravityFieldData() exiting");
   #endif
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

   // Set current drag force pointer (loj: 6/19/06)
   theDragForce = forceList[currentBodyId]->dragf;
   
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
   if ( strcmp(bodyComboBox->GetStringSelection().c_str(),
               SolarSystem::EARTH_NAME.c_str()) == 0 )
      srpCheckBox->SetValue(forceList[currentBodyId]->useSrp);
   else
      srpCheckBox->SetValue(false);
}

//------------------------------------------------------------------------------
// void DisplayErrorControlData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayErrorControlData()
{
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("On DisplayErrorControlData()\n");
   #endif
   
   if (errorControlTypeName == errorControlArray[NONE_EC])
      errorComboBox->SetSelection(NONE_EC);
   else if (errorControlTypeName == errorControlArray[RSSSTEP])
      errorComboBox->SetSelection(RSSSTEP);
   else if (errorControlTypeName == errorControlArray[RSSSTATE])
      errorComboBox->SetSelection(RSSSTATE);
   else if (errorControlTypeName == errorControlArray[LARGESTSTEP])
      errorComboBox->SetSelection(LARGESTSTEP);
   else if (errorControlTypeName == errorControlArray[LARGESTSTATE])
      errorComboBox->SetSelection(LARGESTSTATE);
}

//------------------------------------------------------------------------------
// void SaveDegOrder()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveDegOrder()
{
   for (Integer i=0; i < (Integer)forceList.size(); i++)
   {
      if (forceList[i]->gravType != "None")
      {
         theGravForce = forceList[i]->gravf;
         if (theGravForce != NULL)
         {
            #if DEBUG_PROP_SAVE
            MessageInterface::ShowMessage
               ("SaveDegOrder() Saving Body:%s, degree=%s, order=%s\n",
                forceList[i]->bodyName.c_str(), forceList[i]->gravDegree.c_str(),
                forceList[i]->gravOrder.c_str());
            #endif
            
            theGravForce->SetIntegerParameter
               ("Degree", atoi(forceList[i]->gravDegree.c_str()));
            theGravForce->SetIntegerParameter
               ("Order",  atoi(forceList[i]->gravOrder.c_str()));
         }
      }
   }
   
   isDegOrderChanged = false;
}


//------------------------------------------------------------------------------
// void SavePotFile()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SavePotFile()
{
   for (Integer i=0; i < (Integer)forceList.size(); i++)
   {
      if (forceList[i]->gravType != "None")
      {
         theGravForce = forceList[i]->gravf;
         if (theGravForce != NULL)
         {
            #if DEBUG_PROP_SAVE
            MessageInterface::ShowMessage
               ("SavePotFile() Saving Body:%s, potFile=%s\n",
                forceList[i]->bodyName.c_str(), forceList[i]->potFilename.c_str());
            #endif
            
            theGravForce->SetStringParameter
               ("PotentialFile", forceList[i]->potFilename);
         }
      }
   }
   
   isPotFileChanged = false;
}


//------------------------------------------------------------------------------
// bool SaveIntegratorData()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveIntegratorData()
{
   #if DEBUG_PROP_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveIntegratorData() entered\n");
   #endif
   
   Real min = atof(setting3TextCtrl->GetValue());
   Real max = atof(setting4TextCtrl->GetValue());
      
   // @todo waw: temporarily commented out, to be uncommented 
   // once Edwin updates his scripts to support this
   // if (max <= min) 
   if (max < min)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Maximum Step can not be less than Minimum Step.\n"
          "Propagation updates have not been saved");
      return false;
   }    
   
   try
   {
      thePropagator->SetRealParameter("InitialStepSize", atof(setting1TextCtrl->GetValue()));
      thePropagator->SetRealParameter("Accuracy", atof(setting2TextCtrl->GetValue()));
      thePropagator->SetRealParameter("MinStep", atof(setting3TextCtrl->GetValue()));
      thePropagator->SetRealParameter("MaxStep", atof(setting4TextCtrl->GetValue()));
      thePropagator->SetIntegerParameter("MaxStepAttempts", atoi(setting5TextCtrl->GetValue()));
      
      if (integratorString.IsSameAs(integratorArray[ABM]))
      {
         thePropagator->SetRealParameter("LowerError", atof(setting6TextCtrl->GetValue()));
         thePropagator->SetRealParameter("TargetError", atof(setting7TextCtrl->GetValue()));
      }       
      
      #if DEBUG_PROP_SAVE
      ShowPropData("SaveData() AFTER  saving Integrator");
      #endif

      return true;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("PropConfigPanel:SaveData() error occurred!\n%s\n", e.GetMessage().c_str());
      return false;
   }
}

//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString PropagationConfigPanel::ToString(Real rval)
{
   wxString element;
   std::stringstream buffer;
   buffer.precision(GmatBase::GetDataPrecision());
   
   buffer << rval;
   element.Printf ("%s",buffer.str().c_str());
   return element;
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

//------------------------------------------------------------------------------
// void OnBodyComboBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnBodyComboBoxChange(wxCommandEvent &event)
{
   OnBodySelection(event);
   // We don't want to enable Apply when just switching body.
   theApplyButton->Enable(false);
}

//------------------------------------------------------------------------------
// void OnOriginComboBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnOriginComboBoxChange(wxCommandEvent &event)
{
   propOriginName = originComboBox->GetValue().c_str();
   
   // We don't want to set to true when only origin is changed
   //isForceModelChanged = true;
   isOriginChanged = true;
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
      
      DisplayGravityFieldData(currentBodyName);
      DisplayAtmosphereModelData();
      DisplayMagneticFieldData();
      DisplaySRPData();
   }
   
   if (strcmp(selBody.c_str(), "Earth") == 0)
   {
      atmosComboBox->Enable(true);
      dragSetupButton->Enable(true);
      srpCheckBox->Enable(true);
   }
   else
   {
      atmosComboBox->Enable(false);
      dragSetupButton->Enable(false);
      srpCheckBox->Enable(false);
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
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("OnGravitySelection() grav changed from=%s to=%s for body=%s\n",
          forceList[currentBodyId]->gravType.c_str(), gravTypeName.c_str(),
          forceList[currentBodyId]->bodyName.c_str());
      #endif
      
      forceList[currentBodyId]->gravType = gravTypeName;
      
      if (gravTypeName != "None" && gravTypeName != "Other")
      {
         std::string fileType = theFileMap[gravTypeName];
         //MessageInterface::ShowMessage("===> Found %s\n", fileType.c_str());
         
         try
         {
            forceList[currentBodyId]->potFilename =
               theGuiInterpreter->GetFileName(fileType);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, e.GetMessage() +
                "\nPlease select Other and specify file name\n");
         }
      }
      else if (gravTypeName == "Other")
      {
         forceList[currentBodyId]->potFilename = potFileTextCtrl->GetValue().c_str();
      }

      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("OnGravitySelection() bodyName=%s, potFile=%s\n",
          forceList[currentBodyId]->bodyName.c_str(),
          forceList[currentBodyId]->potFilename.c_str());
      #endif
      
      DisplayGravityFieldData(forceList[currentBodyId]->bodyName);
      
      // We don't want to  set to true only if gravity model is changed
      //isForceModelChanged = true; 
      isPotFileChanged = true; //loj: 6/19/06 added
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
      #if DEBUG_PROP_PANEL
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
}

//------------------------------------------------------------------------------
// void OnErrorControlSelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnErrorControlSelection(wxCommandEvent &event)
{     
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("OnErrorControlSelection()\n");
   #endif
   
   std::string eType = errorComboBox->GetStringSelection().c_str();

   if (errorControlTypeName != eType)
   {    
   	   #if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage
         ("OnErrorControlSelection() error control changed from=%s to=%s\n",
          errorControlTypeName.c_str(), eType.c_str());
      #endif
      
      errorControlTypeName = eType;
      DisplayErrorControlData();
       
      isForceModelChanged = true;  
      theApplyButton->Enable(true);
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
         //magfComboBox->Enable(false);
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
         //magfComboBox->Enable(true);
         srpCheckBox->Enable(true);
         gravityDegreeTextCtrl->Enable(true);
         gravityOrderTextCtrl->Enable(true);
      }
      
      std::vector<ForceType*> oldList;
      std::string bodyName;
      
      oldList = forceList;
      forceList.clear();
      primaryBodiesArray.Clear();
      
      for (Integer i = 0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i].c_str();
         primaryBodiesArray.Add(bodyName.c_str()); 

         // Set default potential file for display
         if (bodyName == "Earth")
            currentBodyId = FindBody(bodyName, earthGravModelArray[JGM2]);
         else if (bodyName == "Luna")
            currentBodyId = FindBody(bodyName, lunaGravModelArray[LP165]);
         else if (bodyName == "Venus")
            currentBodyId = FindBody(bodyName, venusGravModelArray[MGNP180U]);
         else if (bodyName == "Mars")
            currentBodyId = FindBody(bodyName, marsGravModelArray[MARS50C]);
         else
            currentBodyId = FindBody(bodyName,othersGravModelArray[O_NONE_GM]);
         
         
         // Copy old body force model
         for (Integer j = 0; j < (Integer)oldList.size(); j++)
            if (bodyName == oldList[j]->bodyName)
               forceList[currentBodyId] = oldList[j];
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
      if ((filename.find(".dat",0) != std::string::npos) ||
          (filename.find(".DAT",0) != std::string::npos) )
      {
         ParseDATGravityFile(filename);
      }
      else if ((filename.find(".grv",0) != std::string::npos) ||
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
      
      if (forceList[currentBodyId]->bodyName == "Earth")
         forceList[currentBodyId]->gravType = earthGravModelArray[E_OTHER];
      else if (forceList[currentBodyId]->bodyName == "Luna")
         forceList[currentBodyId]->gravType = lunaGravModelArray[L_OTHER];
      else if (forceList[currentBodyId]->bodyName == "Mars")
         forceList[currentBodyId]->gravType = marsGravModelArray[M_OTHER];
      else //other bodies 
         forceList[currentBodyId]->gravType = othersGravModelArray[O_OTHER];

      //loj: Do we need to show? body name didn't change
      //DisplayGravityFieldData(forceList[currentBodyId]->bodyName);
      
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
      
      std::string bodyName;

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
   //isIntegratorChanged = true;
   isIntegratorDataChanged = true;
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
      isDegOrderChanged = true;
      // Do not set to true if only text changed (loj: 6/20/06)
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == gravityOrderTextCtrl)
   {     
      forceList[currentBodyId]->gravOrder = gravityOrderTextCtrl->GetValue().c_str();
      isDegOrderChanged = true;
      // Do not set to true if only text changed (loj: 6/20/06)
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == potFileTextCtrl)
   {
      forceList[currentBodyId]->potFilename = potFileTextCtrl->GetValue().c_str(); //loj:added
      isPotFileChanged = true;
      // Do not set to true if only text changed (loj: 6/20/06)
      //isForceModelChanged = true;
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
   forceList[currentBodyId]->useSrp = srpCheckBox->GetValue();
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
   MessageInterface::ShowMessage("thePropagator=%d, name=%s\n", thePropagator,
                                 thePropagator->GetName().c_str());
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
   //#if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s%s\n", header.c_str(), " --- ForceList");
   for (unsigned int i=0; i<forceList.size(); i++)
   {
      MessageInterface::ShowMessage
         ("id=%d, body=%s, gravType=%s, dragType=%s, magfType=%s\npotFile=%s\n"
          "pmf=%p, gravf=%p, dragf=%p, srpf=%p\n", i, forceList[i]->bodyName.c_str(),
          forceList[i]->gravType.c_str(), forceList[i]->dragType.c_str(),
          forceList[i]->magfType.c_str(), forceList[i]->potFilename.c_str(),
          forceList[i]->pmf, forceList[i]->gravf, forceList[i]->dragf,
          forceList[i]->srpf);
   }
   MessageInterface::ShowMessage("============================================\n");
   //#endif
}   


//------------------------------------------------------------------------------
// void ShowForceModel()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowForceModel(const std::string &header)
{
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s%s\n", header.c_str(), " --- ForceModel");
   Integer numForces  = theForceModel->GetNumForces();
   MessageInterface::ShowMessage
      ("CentralBody=%s, numForces=%d\n",
       theForceModel->GetStringParameter("CentralBody").c_str(), numForces);
   PhysicalModel *pm;
   
   for (int i=0; i<numForces; i++)
   {
      pm = theForceModel->GetForce(i);
      MessageInterface::ShowMessage
         ("id=%d, body=%s, type=%s, addr=%p\n", i, pm->GetBodyName().c_str(),
          pm->GetTypeName().c_str(), pm);
   }
   MessageInterface::ShowMessage("============================================\n");
}   


//------------------------------------------------------------------------------
// bool ParseDATGravityFile(std::string fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseDATGravityFile(std::string fname)
{  
   Integer      cc, dd, sz=0;
   Integer      iscomment, rtn;
   Integer      n=0, m=0;
   Integer      fileDegree, fileOrder;
   Real         Cnm=0.0, Snm=0.0, dCnm=0.0, dSnm=0.0;
   // @to do should mu & radius be constant?? - waw
   Real         mu=398600.4415; // gravity parameter of central body
   Real         a=6378.1363;  // radius of central body ( mean equatorial )
   char         buf[CelestialBody::BUFSIZE];
   FILE        *fp;

   for (cc = 2;cc <= HarmonicField::HF_MAX_DEGREE; ++cc)
   {
      for (dd = 0; dd <= cc; ++dd)
      {
         sz++;
      }
   }
   
   /* read coefficients from file */
   fp = fopen( fname.c_str(), "r");
   if (!fp)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Error reading gravity potential file.");
         return;
   }
   
   PrepareGravityArrays();
   iscomment = 1;
   
   while ( iscomment )
   {
      rtn = fgetc( fp );
        
      if ( (char)rtn == '#' )
      {
         fgets( buf, CelestialBody::BUFSIZE, fp );
      }
      else
      {
         ungetc( rtn, fp );
         iscomment = 0;
      }
   }

   fscanf(fp, "%lg\n", &mu ); mu = (Real)mu / 1.0e09;      // -> Km^3/sec^2
   fscanf(fp, "%lg\n", &a ); a = (Real)a / 1000.0;         // -> Km
   fgets( buf, CelestialBody::BUFSIZE, fp );
   
   while ( ( (char)(rtn=fgetc(fp)) != '#' ) && (rtn != EOF) )
   {
      ungetc( rtn, fp );
      fscanf( fp, "%i %i %le %le\n", &n, &m, &dCnm, &dSnm );
      if ( n <= GRAV_MAX_DRIFT_DEGREE  && m <= n )
      {
         dCbar[n][m] = (Real)dCnm;
         dSbar[n][m] = (Real)dSnm;
      }
   }

   fgets( buf, CelestialBody::BUFSIZE, fp );

   fileDegree = 0;
   fileOrder  = 0;
   cc=0;n=0;m=0;
   
   do
   {
      if ( n <= HarmonicField::HF_MAX_DEGREE && m <= HarmonicField::HF_MAX_ORDER )
      {
         Cbar[n][m] = (Real)Cnm;
         Sbar[n][m] = (Real)Snm;
      }
      if (n > fileDegree) fileDegree = n;
      if (n > fileOrder)  fileOrder  = n;
      
      cc++;
   } while ( ( cc<=sz ) && ( fscanf( fp, "%i %i %le %le\n", &n, &m, &Cnm, &Snm ) > 0 ));
   
   // Save as string
   forceList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
   forceList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
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
      
   for (n = 0; n <= GRAV_MAX_DRIFT_DEGREE; ++n) 
   {
      for (m = 0; m <= GRAV_MAX_DRIFT_DEGREE; ++m) 
      {
         dCbar[n][m] = 0.0;
         dSbar[n][m] = 0.0;
      }
   }   
}
