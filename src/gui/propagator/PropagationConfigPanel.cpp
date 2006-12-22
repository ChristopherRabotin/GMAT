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
#include "GmatStaticBoxSizer.hpp"
#include "wx/platform.h"

// base includes
#include "MessageInterface.hpp"
#include "PropagatorException.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"  // for ToDouble()

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

//#define DEBUG_PROP_PANEL_SETUP 1
//#define DEBUG_PROP_PANEL 1
//#define DEBUG_PROP_SAVE 1
//#define DEBUG_PROP_INTEGRATOR 1
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
   propSetupName = propName.c_str();
   
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
   
   earthGravModelArray.Clear();
   lunaGravModelArray.Clear();
   venusGravModelArray.Clear();
   marsGravModelArray.Clear();
   othersGravModelArray.Clear();
   dragModelArray.Clear();
   magfModelArray.Clear();
   errorControlArray.Clear();
   
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

   canClose = true;
   
   EnableUpdate(false);
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
   
   earthGravModelArray.Clear();
   lunaGravModelArray.Clear();
   venusGravModelArray.Clear();
   marsGravModelArray.Clear();
   othersGravModelArray.Clear();
   dragModelArray.Clear();
   magfModelArray.Clear();
   errorControlArray.Clear();
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
//   else if (propType == "Cowell")
//      typeId = CW;

   // Display primary bodies
   if ( !primaryBodiesArray.IsEmpty() )
      for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
         bodyComboBox->Append(primaryBodiesArray[i]);   

   integratorComboBox->SetSelection(typeId);
   integratorString = integratorArray[typeId]; 
   
   // Display the Origin (Central Body)
   originComboBox->SetValue(propOriginName);
   
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
   #if DEBUG_PROP_SAVE
   MessageInterface::ShowMessage
      ("SaveData() thePropagatorName=%s\n", thePropagator->GetTypeName().c_str());
   MessageInterface::ShowMessage("   isIntegratorChanged=%d\n", isIntegratorChanged);
   MessageInterface::ShowMessage("   isIntegratorDataChanged=%d\n",isIntegratorDataChanged);
   MessageInterface::ShowMessage("   isForceModelChanged=%d\n", isForceModelChanged);
   MessageInterface::ShowMessage("   isDegOrderChanged=%d\n", isDegOrderChanged);
   MessageInterface::ShowMessage("   isPotFileChanged=%d\n", isPotFileChanged);
   #endif
   
   canClose = true;
   
   //-------------------------------------------------------
   // Saving the Integrator
   //-------------------------------------------------------
   if (isIntegratorChanged)
   {
      #if DEBUG_PROP_SAVE
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
      
      // Since the propagator is cloned in the base code, get new pointer
      thePropagator = thePropSetup->GetPropagator();
      
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
      std::string bodyName;
      
      //----------------------------------------------------
      // save point mass force model
      //----------------------------------------------------
      for (Integer i=0; i < (Integer)pmForceList.size(); i++)
      {      
         thePMF = new PointMassForce();
         bodyName = pmForceList[i]->bodyName.c_str();
         thePMF->SetBodyName(bodyName);
         pmForceList[i]->pmf = thePMF;
         newFm->AddForce(thePMF);
      }
      
      //----------------------------------------------------
      // save gavity force model
      //----------------------------------------------------
      for (Integer i=0; i < (Integer)forceList.size(); i++)
      {
         if (forceList[i]->gravType != "None")
         {
            // if gravity force pointer is NULL then create.
            theGravForce = forceList[i]->gravf;
            bodyName = forceList[i]->bodyName.c_str();
            
            // Create new GravityField since ForceModel destructor will delete
            // all PhysicalModel
            #if DEBUG_PROP_SAVE
            MessageInterface::ShowMessage
               ("SaveData() Creating GravityField for %s\n", bodyName.c_str());
            #endif
            
            theGravForce = new GravityField("", bodyName);
            theGravForce->SetSolarSystem(theSolarSystem);
            theGravForce->SetBodyName(bodyName);
            theGravForce->SetStringParameter("PotentialFile",
                                             forceList[i]->potFilename.c_str());
            forceList[i]->gravf = theGravForce;            
            newFm->AddForce(theGravForce);
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, 
               "Did not save %s as Primary Body.\n"
               "Because there is no Gravity Field Type associated with that body.",
                       forceList[i]->bodyName.c_str());
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

            bodyName = forceList[i]->bodyName.c_str();
            theDragForce = new DragForce(forceList[i]->dragType.c_str());
            theCelestialBody = theSolarSystem->GetBody(bodyName); 
            theAtmosphereModel = theCelestialBody->GetAtmosphereModel();
            
            #if DEBUG_PROP_SAVE
            ShowForceList("Entering if (theAtmosphereModel == NULL)");
            #endif
            
            if (theAtmosphereModel == NULL)  
            {
               theAtmosphereModel = (AtmosphereModel*)theGuiInterpreter->CreateObject
                  (forceList[i]->dragType.c_str(), forceList[i]->dragType.c_str());
               
               #if DEBUG_PROP_SAVE
               ShowForceList("Exiting if (theAtmosphereModel == NULL)");
               #endif
            }
            
            theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);
            
            #if DEBUG_PROP_SAVE
            ShowForceList("theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);");
            #endif
            
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            bodyName = forceList[i]->bodyName.c_str();
            theDragForce->SetStringParameter(paramId, forceList[i]->dragType.c_str());
            theDragForce->SetStringParameter("BodyName", bodyName);
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
         if (forceList[i]->useSrp)
         {
            theSRP = new SolarRadiationPressure();
            forceList[i]->srpf = theSRP;
            bodyName = forceList[i]->bodyName.c_str();
            theSRP->SetStringParameter("BodyName", bodyName);
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
      
      // ForceModel is deleted in PropSetup::SetForceModel()
      //theForceModel = newFm;
      
      // Since the force model and it's physical models are cloned in the
      // base code, get new pointers
      theForceModel = thePropSetup->GetForceModel();
      PhysicalModel *pm;
      int size = forceList.size();
      for (int i=0; i<numOfForces; i++)
      {
         pm = theForceModel->GetForce(i);
         
         #if DEBUG_PROP_SAVE
         MessageInterface::ShowMessage
            ("===> pm=(%p)%s\n", pm, pm->GetTypeName().c_str());
         #endif
         
         for (int j=0; j<size; j++)
         {
            if ((forceList[j]->bodyName).IsSameAs(pm->GetBodyName().c_str()))
            {
               if (pm->GetTypeName() == "PointMassForce")
                  forceList[j]->pmf = (PointMassForce*)pm;
               else if (pm->GetTypeName() == "GravityField")
                  forceList[j]->gravf = (GravityField*)pm;
               else if (pm->GetTypeName() == "DragForce")
                  forceList[j]->dragf = (DragForce*)pm;
               else if (pm->GetTypeName() == "SolarRadiationPressure")
                  forceList[j]->srpf = (SolarRadiationPressure*)pm;
            }
         }
      }
      
      #if DEBUG_PROP_SAVE
      ShowForceList("SaveData() AFTER  saving ForceModel");
      #endif
   } // end if(isForceModelChange)
   else
   {
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
            
   }
}


//------------------------------------------------------------------------------
// Integer FindBody(const std::string &bodyName, const std::string &gravType,...)
//------------------------------------------------------------------------------
/*
 * If body is found retun index, otherwise create a new ForceType and return
 * new index
 */
//------------------------------------------------------------------------------
Integer PropagationConfigPanel::FindBody(const wxString &bodyName,
                                         const wxString &gravType,
                                         const wxString &dragType,
                                         const wxString &magfType)
{
   for (Integer i=0; i<(Integer)forceList.size(); i++)
   {
      if (forceList[i]->bodyName == bodyName)
         return i;
   }
   
   forceList.push_back(new ForceType(bodyName, gravType, dragType, magfType));
   
   // Set gravity model file
   if (theFileMap.find(gravType) != theFileMap.end())
   {
      std::string potFileType = theFileMap[gravType].c_str();
      wxString wxPotFileName = theGuiInterpreter->GetFileName(potFileType).c_str();
      //MessageInterface::ShowMessage("===> potFile=%s\n", potFileType.c_str());
      forceList.back()->potFilename = wxPotFileName;
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
   thePropSetup = (PropSetup*)theGuiInterpreter->GetObject(propSetupName);
   
   //Note: All the settings should match enum types in the header.
   
   // initialize integrator type array for ComboBox
//   integratorArray.Add("RKV 8(9)");
//   integratorArray.Add("RKN 6(8)");
//   integratorArray.Add("RKF 5(6)");
//   integratorArray.Add("PD  4(5)");
//   integratorArray.Add("PD  7(8)");
//   integratorArray.Add("BS");
//   integratorArray.Add("ABM");
//   integratorArray.Add("Cowell");
   integratorArray.Add("RungeKutta89");
   integratorArray.Add("RungeKutta68");
   integratorArray.Add("RungeKutta56");
   integratorArray.Add("PrinceDormand45");
   integratorArray.Add("PrinceDormand78");
   integratorArray.Add("BulirschStoer");
   integratorArray.Add("AdamsBashforthMoulton");
//   integratorArray.Add("Cowell");
  
   // initialize integrator type array for creating
   integratorTypeArray.Add("RungeKutta89");
   integratorTypeArray.Add("DormandElMikkawyPrince68");
   integratorTypeArray.Add("RungeKuttaFehlberg56");
   integratorTypeArray.Add("PrinceDormand45");
   integratorTypeArray.Add("PrinceDormand78");
   integratorTypeArray.Add("BulirschStoer");
   integratorTypeArray.Add("AdamsBashforthMoulton");
//   integratorTypeArray.Add("Cowell");

   // initialize gravity model type arrays
   earthGravModelArray.Add("None");
   earthGravModelArray.Add("JGM-2");
   earthGravModelArray.Add("JGM-3");
   earthGravModelArray.Add("EGM-96");
   earthGravModelArray.Add("Other");
   
   lunaGravModelArray.Add("None");
   lunaGravModelArray.Add("LP-165");
   lunaGravModelArray.Add("Other");
   
   venusGravModelArray.Add("None");
   venusGravModelArray.Add("MGNP-180U");
   venusGravModelArray.Add("Other");
   
   marsGravModelArray.Add("None");
   marsGravModelArray.Add("Mars-50C");
   marsGravModelArray.Add("Other");
   
   othersGravModelArray.Add("None");
   othersGravModelArray.Add("Other");
   
   // initialize drag model type array
   dragModelArray.Add("None");
   dragModelArray.Add("Exponential");
   dragModelArray.Add("MSISE90");
   dragModelArray.Add("JacchiaRoberts");
   
   // initialize error control type array
   errorControlArray.Add("None");
   errorControlArray.Add("RSSStep");
   errorControlArray.Add("RSSState");
   errorControlArray.Add("LargestStep");
   errorControlArray.Add("LargestState");

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
   magfModelArray.Add("None");
   
   if (thePropSetup != NULL)
   {
      thePropagator = thePropSetup->GetPropagator();
      theForceModel = thePropSetup->GetForceModel();
      numOfForces   = thePropSetup->GetNumForces();
      
      // Getting the origin
      propOriginName = theForceModel->GetStringParameter("CentralBody").c_str();
      
      // Getting the error control
      errorControlTypeName = theForceModel->GetStringParameter("ErrorControl").c_str();
      
      PhysicalModel *force;     
      Integer paramId;
      wxString wxBodyName;
      wxString tempStr;
      wxString useSRP;
      
      paramId = theForceModel->GetParameterID("SRP");
      useSRP = theForceModel->GetStringParameter(paramId).c_str();
           
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
            wxBodyName = thePMF->GetStringParameter("BodyName").c_str();
            secondaryBodiesArray.Add(wxBodyName);    
            pmForceList.push_back(new ForceType(wxBodyName, "None", 
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
            wxBodyName = theGravForce->GetStringParameter("BodyName").c_str();
            wxString potFilename = theGravForce->GetStringParameter("PotentialFile").c_str(); 
            
            currentBodyId = FindBody(wxBodyName);
            forceList[currentBodyId]->bodyName = wxBodyName;
            forceList[currentBodyId]->potFilename = potFilename;
            
            if (wxBodyName == "Earth")
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
            else if (wxBodyName == "Luna")
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
            else if (wxBodyName == "Venus")
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
            else if (wxBodyName == "Mars")
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
               if ( primaryBodiesArray[i].CmpNoCase(wxBodyName) == 0 )
                  found = true;
            }
            
            if (!found)
               primaryBodiesArray.Add(wxBodyName);     
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
            
            wxBodyName = theDragForce->GetStringParameter("BodyName").c_str();
            
            currentBodyId = FindBody(wxBodyName);
            forceList[currentBodyId]->bodyName = wxBodyName;
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
               if ( primaryBodiesArray[i].CmpNoCase(wxBodyName) == 0 )
                  found = true;
            }
            
            if (!found)
               primaryBodiesArray.Add(wxBodyName.c_str());
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
                        
            wxBodyName = force->GetStringParameter("BodyName").c_str();
            
            currentBodyId = FindBody(wxBodyName);
            
            #if DEBUG_PROP_PANEL
            MessageInterface::ShowMessage
               ("PropagationConfigPanel::Initialize() wxBodyName=%s\n",
                wxBodyName.c_str());
            #endif
            
            currentBodyId = FindBody(wxBodyName);
            forceList[currentBodyId]->useSrp = true;
            forceList[currentBodyId]->bodyName = wxBodyName;
            forceList[currentBodyId]->srpf = theSRP;
         }
      }
      
      #if DEBUG_PROP_PANEL
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::Initialize() Initializing Primary bodies array.\n");
      #endif
      
      if (!primaryBodiesArray.IsEmpty())  
      { 
         primaryBodyString = primaryBodiesArray.Item(0).c_str();
         currentBodyName = primaryBodyString;
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
   
   Integer bsize = 2; // border size
   
   //-----------------------------------------------------------------
   // Integrator
   //-----------------------------------------------------------------
   wxStaticText *integratorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE);
   
   wxString *intgArray = new wxString[IntegratorCount];
   for (Integer i=0; i<IntegratorCount; i++)
      intgArray[i] = integratorArray[i];
   
   integratorComboBox =
      new wxComboBox( parent, ID_CB_INTGR, wxT(integratorString),
                      wxDefaultPosition, wxDefaultSize, IntegratorCount,
                      intgArray, wxCB_DROPDOWN|wxCB_READONLY );
   
   // Initial Step Size
   wxStaticText *initialStepSizeStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Initial Step Size"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );
   
   initialStepSizeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );

   wxStaticText *unitsInitStepSizeStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxSize(10,20), wxST_NO_AUTORESIZE );
   // Accuracy
   wxStaticText *accuracyStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Accuracy"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );
   accuracyTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   
   // Minimum Step Size
   wxStaticText *minStepStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Step Size"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );
   minStepTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   wxStaticText *unitsMinStepStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxSize(10,20), wxST_NO_AUTORESIZE );
   
   // Maximum Step Size
   wxStaticText *maxStepStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Size"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );
   maxStepTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   wxStaticText *unitsMaxStepStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("sec"),
                        wxDefaultPosition, wxSize(10,20), wxST_NO_AUTORESIZE );
   
   // Maximum Step Attempt
   wxStaticText *maxStepAttemptStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Max Step Attempts"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );
   maxStepAttemptTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   
   // Minimum Integration Error
   minIntErrorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Min Integration Error"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );
   minIntErrorTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   
   // Nominal Integration Error
   nomIntErrorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Nominal Integration Error"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE );  
   nomIntErrorTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_PROP, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), 0 );
   
   wxFlexGridSizer *intFlexGridSizer = new wxFlexGridSizer( 3, 0, 0 );
   intFlexGridSizer->Add( integratorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( integratorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( initialStepSizeStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( initialStepSizeTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsInitStepSizeStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( accuracyStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( accuracyTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minStepTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsMinStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( unitsMaxStepStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepAttemptStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( maxStepAttemptTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minIntErrorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( minIntErrorTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( nomIntErrorStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( nomIntErrorTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   intFlexGridSizer->Add( 20, 20, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   GmatStaticBoxSizer *intStaticSizer =
      new GmatStaticBoxSizer( wxVERTICAL, this, "Integrator");
   intStaticSizer->Add( intFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Error Control
   //-----------------------------------------------------------------
   wxStaticText *errorCtrlStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Error Control"),
                        wxDefaultPosition, wxSize(70,20), wxST_NO_AUTORESIZE );
   
   errorComboBox =
      new wxComboBox( parent, ID_CB_ERROR, errorControlArray[0],
                      wxDefaultPosition, wxSize(100,-1),
                      errorControlArray, wxCB_DROPDOWN|wxCB_READONLY );
   
   wxFlexGridSizer *errorFlexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
   errorFlexGridSizer->Add( errorCtrlStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   errorFlexGridSizer->Add( errorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Central Body
   //-----------------------------------------------------------------
   wxStaticText *centralBodyStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Central Body"),
                        wxDefaultPosition, wxSize(70,20), wxST_NO_AUTORESIZE);
   originComboBox  =
      theGuiManager->GetConfigBodyComboBox(this, ID_CB_ORIGIN, wxSize(100,-1));
   
   wxFlexGridSizer *centralBodySizer = new wxFlexGridSizer( 2, 0, 2 );
   centralBodySizer->Add( centralBodyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   centralBodySizer->Add( originComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Primary Bodies
   //-----------------------------------------------------------------
   wxString bodyArray[]  = {};
   bodyComboBox =
      new wxComboBox( parent, ID_CB_BODY, wxT(primaryBodyString),
                      wxDefaultPosition,  wxSize(80,-1), 0,
                      bodyArray, wxCB_DROPDOWN|wxCB_READONLY );
   bodyTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), wxTE_READONLY );
   bodyButton =
      new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *bodySizer = new wxBoxSizer( wxHORIZONTAL );
   bodySizer->Add( bodyComboBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   bodySizer->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   bodySizer->Add( bodyButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Gravity Field
   //-----------------------------------------------------------------
   wxStaticText *type1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxString gravArray[]  = {};
   gravComboBox =
      new wxComboBox( parent, ID_CB_GRAV, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0,
                      gravArray, wxCB_DROPDOWN|wxCB_READONLY );
   wxStaticText *degree1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   gravityDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   wxStaticText *order1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   gravityOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   searchGravityButton =
      new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *degOrdSizer = new wxBoxSizer( wxHORIZONTAL );
   degOrdSizer->Add( type1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravComboBox, 0, wxALIGN_CENTRE|wxALL, bsize); 
   degOrdSizer->Add( degree1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( order1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( searchGravityButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   potFileStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Model File"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   potFileTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(290,-1), 0 );
   
   wxBoxSizer *potFileSizer = new wxBoxSizer( wxHORIZONTAL );   
   potFileSizer->Add( potFileStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   potFileSizer->Add( potFileTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *gravStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Gravity Field");
   gravStaticSizer->Add( degOrdSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   gravStaticSizer->Add( potFileSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Atmosphere Model
   //-----------------------------------------------------------------
   wxStaticText *type2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   atmosComboBox =
      new wxComboBox( parent, ID_CB_ATMOS, dragModelArray[0],
                      wxDefaultPosition, wxDefaultSize,
                      dragModelArray, wxCB_DROPDOWN|wxCB_READONLY );
   dragSetupButton =
      new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *atmosSizer = new wxBoxSizer( wxHORIZONTAL );
   atmosSizer->Add( type2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);   
   atmosSizer->Add( atmosComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   atmosSizer->Add( dragSetupButton, 0, wxALIGN_CENTRE|wxALL, bsize);       
   
   GmatStaticBoxSizer *atmosStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Atmosphere Model");
   atmosStaticSizer->Add( atmosSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Magnetic Field
   //-----------------------------------------------------------------
   wxStaticText *type3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   magfComboBox =
      new wxComboBox( parent, ID_CB_MAG, magfModelArray[0],
                      wxDefaultPosition, wxDefaultSize,
                      magfModelArray, wxCB_DROPDOWN|wxCB_READONLY );
   
   wxStaticText *degree2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Degree"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   magneticDegreeTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   wxStaticText *order2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Order"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   magneticOrderTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_MAGF, wxT(""), wxDefaultPosition,
                      wxSize(30,-1), 0 );
   searchMagneticButton =
      new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *magfSizer = new wxBoxSizer( wxHORIZONTAL );
   magfSizer->Add( type3StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magfComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( degree2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( order2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( searchMagneticButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *magfStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Magnetic Field");
   magfStaticSizer->Add( magfSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Point Masses
   //-----------------------------------------------------------------
   wxStaticText *pointMassStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Point Masses"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   pmEditTextCtrl =
      new wxTextCtrl( parent, -1, wxT(""), wxDefaultPosition,
                      wxSize(235,-1), wxTE_READONLY );
   editPmfButton =
      new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxFlexGridSizer *pointMassSizer = new wxFlexGridSizer( 3, 0, 2 );
   pointMassSizer->Add( pointMassStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   pointMassSizer->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   pointMassSizer->Add( editPmfButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // SRP
   //-----------------------------------------------------------------
   srpCheckBox =
      new wxCheckBox( parent, ID_CHECKBOX, wxT("Use Solar Radiation Pressure"),
                      wxDefaultPosition, wxDefaultSize, 0 );
   
   //-----------------------------------------------------------------
   // Primary Bodies
   //-----------------------------------------------------------------
   GmatStaticBoxSizer *primaryStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Primary Bodies");
   primaryStaticSizer->Add( bodySizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Add( gravStaticSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Add( atmosStaticSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   primaryStaticSizer->Add( magfStaticSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Force Model
   //-----------------------------------------------------------------
   GmatStaticBoxSizer *fmStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Force Model");
   fmStaticSizer->Add( errorFlexGridSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   fmStaticSizer->Add( centralBodySizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   fmStaticSizer->Add( primaryStaticSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   fmStaticSizer->Add( pointMassSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   fmStaticSizer->Add( srpCheckBox, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Add panelSizer
   //-----------------------------------------------------------------
   wxBoxSizer *intBoxSizer = new wxBoxSizer( wxVERTICAL );
   intBoxSizer->Add( intStaticSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   intBoxSizer->Add( 0, 0, 1);
   
   leftBoxSizer = new wxBoxSizer( wxVERTICAL );
   leftBoxSizer->Add( intBoxSizer, 1, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
   wxBoxSizer *pageSizer = new wxBoxSizer( wxHORIZONTAL );
   pageSizer->Add( leftBoxSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   pageSizer->Add( fmStaticSizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, bsize);
   
   wxBoxSizer *panelSizer = new wxBoxSizer( wxVERTICAL );
   panelSizer->Add( pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW, bsize);
   
   
   //-----------------------------------------------------------------
   // Disable components for future implementations
   //-----------------------------------------------------------------
   //atmosComboBox->Enable(true);
   
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
   
   #if DEBUG_PROP_INTEGRATOR
   MessageInterface::ShowMessage
      ("DisplayIntegratorData() integratorChanged=%d, integratorString=<%s>\n",
       integratorChanged, integratorString.c_str());
   #endif
   
   if (integratorChanged)
   {
      std::string integratorType = integratorTypeArray[propIndex].c_str();
      thePropagatorName = propSetupName + "_" + integratorType;
      thePropagator = (Propagator*)theGuiInterpreter->GetObject(thePropagatorName);
      if (thePropagator == NULL)
         thePropagator = (Propagator*)
            theGuiInterpreter->CreateObject(integratorType, thePropagatorName);
   }

   
#ifdef __WXMAC__
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {   
      minIntErrorStaticText->Enable(true);
      nomIntErrorStaticText->Enable(true);
      minIntErrorTextCtrl->Enable(true);
      nomIntErrorTextCtrl->Enable(true);
   }
   else
   {   
      minIntErrorStaticText->Enable(false);
      nomIntErrorStaticText->Enable(false);
      minIntErrorTextCtrl->Enable(false);
      nomIntErrorTextCtrl->Enable(false);
   }
#else
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {
      minIntErrorStaticText->Show(true);
      nomIntErrorStaticText->Show(true);
      minIntErrorTextCtrl->Show(true);
      nomIntErrorTextCtrl->Show(true);
   }
   else
   {   
      minIntErrorStaticText->Show(false);
      nomIntErrorStaticText->Show(false);
      minIntErrorTextCtrl->Show(false);
      nomIntErrorTextCtrl->Show(false);
   }
#endif

   leftBoxSizer->Layout();
    
   Real i1 = thePropagator->GetRealParameter("InitialStepSize");
   Real i2 = thePropagator->GetRealParameter("Accuracy");
   Real i3 = thePropagator->GetRealParameter("MinStep");
   Real i4 = thePropagator->GetRealParameter("MaxStep");
   Integer i5 = (long)thePropagator->GetIntegerParameter("MaxStepAttempts");

   initialStepSizeTextCtrl->SetValue(ToString(i1));
   accuracyTextCtrl->SetValue(ToString(i2));
   minStepTextCtrl->SetValue(ToString(i3));
   maxStepTextCtrl->SetValue(ToString(i4));
   maxStepAttemptTextCtrl->SetValue(ToString(i5));
 
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {  
      Real i6 = thePropagator->GetRealParameter("LowerError");   
      Real i7 = thePropagator->GetRealParameter("TargetError");
      
      minIntErrorTextCtrl->SetValue(ToString(i6));
      nomIntErrorTextCtrl->SetValue(ToString(i7));
   }
   
   #if DEBUG_PROP_INTEGRATOR
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
      if ( primaryBodiesArray[i].CmpNoCase(primaryBodyString) == 0 )
         bodyIndex = i;
   }
   bodyComboBox->SetSelection(bodyIndex);
}

//------------------------------------------------------------------------------
// void DisplayGravityFieldData(const wxString& bodyName)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayGravityFieldData(const wxString& bodyName)
{
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage
      ("DisplayGravityFieldData() currentBodyName=%s gravType=%s\n",
       currentBodyName.c_str(), forceList[currentBodyId]->gravType.c_str());
   ShowForceList("DisplayGravityFieldData() entered");
   #endif
   
   gravComboBox->Clear();
   
   wxString gravType = forceList[currentBodyId]->gravType;
   
   // for gravity model ComboBox
   if (bodyName == "Earth")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Earth gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)EarthGravModelCount; i++)
         gravComboBox->Append(earthGravModelArray[i]);

   }
   else if (bodyName == "Luna")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Luna gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)LunaGravModelCount; i++)
         gravComboBox->Append(lunaGravModelArray[i]);
   }
   else if (bodyName == "Venus")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Venus gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)VenusGravModelCount; i++)
         gravComboBox->Append(venusGravModelArray[i]);

   }
   else if (bodyName == "Mars")
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Mars gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)MarsGravModelCount; i++)
         gravComboBox->Append(marsGravModelArray[i]);

   }
   else // other bodies
   {
      #if DEBUG_GRAV_FIELD
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying other gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)OthersGravModelCount; i++)
         gravComboBox->Append(othersGravModelArray[i]);

   }
   
   searchGravityButton->Enable(false);
   potFileStaticText->Enable(false);
   potFileTextCtrl->Enable(false);
   gravityDegreeTextCtrl->Enable(true);
   gravityOrderTextCtrl->Enable(true);
   potFileTextCtrl->SetValue(forceList[currentBodyId]->potFilename);
   
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
      }
   }
   
   gravComboBox->SetValue(gravType);
   gravityDegreeTextCtrl->SetValue(forceList[currentBodyId]->gravDegree);
   gravityOrderTextCtrl->SetValue(forceList[currentBodyId]->gravOrder);
   
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

   // Set current drag force pointer
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
//    if ( strcmp(bodyComboBox->GetStringSelection().c_str(),
//                SolarSystem::EARTH_NAME.c_str()) == 0 )
      srpCheckBox->SetValue(forceList[currentBodyId]->useSrp);
//    else
//       srpCheckBox->SetValue(false);
}

//------------------------------------------------------------------------------
// void DisplayErrorControlData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayErrorControlData()
{
   #if DEBUG_PROP_PANEL
   MessageInterface::ShowMessage("On DisplayErrorControlData()\n");
   #endif

   wxString wxEcTypeName = errorControlTypeName.c_str();
   
   if (wxEcTypeName == errorControlArray[NONE_EC])
      errorComboBox->SetSelection(NONE_EC);
   else if (wxEcTypeName == errorControlArray[RSSSTEP])
      errorComboBox->SetSelection(RSSSTEP);
   else if (wxEcTypeName == errorControlArray[RSSSTATE])
      errorComboBox->SetSelection(RSSSTATE);
   else if (wxEcTypeName == errorControlArray[LARGESTSTEP])
      errorComboBox->SetSelection(LARGESTSTEP);
   else if (wxEcTypeName == errorControlArray[LARGESTSTATE])
      errorComboBox->SetSelection(LARGESTSTATE);
}

//------------------------------------------------------------------------------
// void SaveDegOrder()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveDegOrder()
{
   /// @todo ltr: implement < the maximum specified by the model validation 
   try
   {

      Integer ivalue;
      canClose = true;
      std::string degree, order;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \""
                        + thePropSetup->GetName() + 
                        "\" is not an allowed value.\n"
                        "The allowed values are: [%s].";                        
            
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
               
               Integer deg, ord;
               
               degree = forceList[i]->gravDegree.c_str();      
               order  = forceList[i]->gravOrder.c_str();      
               
               deg = atoi(degree.c_str());
               ord = atoi(order.c_str());
               
               #if DEBUG_PROP_SAVE
               MessageInterface::ShowMessage
                  ("PropagationConfigPanel::SaveDegOrder()  deg = %d, "
                   "ord =%d \n", deg, ord);
               #endif
               
               // check to see if degree is less than order
               if (deg < ord)
               {
                  MessageInterface::PopupMessage (Gmat::ERROR_, 
                     "Degree can not be less than Order.\n"
                     "The allowed values are: [Integer >= 0 "
                     "and < the maximum specified by the model, "
                     "Order <= Degree].");                        
                  canClose = false;
               }
               else
               {
                  // save degree   
                  // check to see if input is an integer, 
                  // greater than or equal to zero
                  // and less than or equal to the model maximum degree
                  if ((GmatStringUtil::ToInteger(degree,&ivalue)) &&
                      (ivalue >= 0)) //&& (ivalue <= fileDegree))
                     theGravForce->SetIntegerParameter("Degree", ivalue);
                  else
                  {
                     MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
                        degree.c_str(), "Degree",
                        "Integer >= 0 and < the maximum specified by the model, "
                        "Order <= Degree");
                     canClose = false;
                  }
                  
                  // save order   
                  // check to see if input is an integer, 
                  // greater than or equal to zero
                  // and less than or equal to the model maximum order
                  if ((GmatStringUtil::ToInteger(order,&ivalue)) && 
                      (ivalue >= 0)) //&& (ivalue <= fileOrder))
                     theGravForce->SetIntegerParameter("Order", ivalue);
                  else
                  {
                     MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
                        order.c_str(), "Order",
                        "Integer >= 0 and < the maximum specified by the model, "
                        "Order <= Degree");
                     canClose = false;
                  }
               } // else degree > order
            } // end of (theGravForce != NULL)
         } // end of (if forceList[i]->gravType != "None")
      } // end of for
   
      isDegOrderChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("PropagationConfigPanel:SaveDegOrder() error occurred!\n%s\n", e.GetMessage().c_str());
      canClose = false;
      return;
   }
}


//------------------------------------------------------------------------------
// void SavePotFile()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SavePotFile()
{
   // save data to core engine
   try
   {

      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                         thePropSetup->GetName() + "\" is not an allowed value.  "
                        "\nThe allowed values are: [ %s ].";

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
                  
               inputString = forceList[i]->potFilename.c_str();
               std::ifstream filename(inputString.c_str());
               
               // Check if the file doesn't exist then stop
               if (!filename) 
               {
                  MessageInterface::PopupMessage
                     (Gmat::ERROR_, msg.c_str(), inputString.c_str(),
                      "Model File", "File must exist");
                  
                  return;
               }
               
               filename.close();               
               theGravForce->SetStringParameter("PotentialFile",
                                                forceList[i]->potFilename.c_str());
            }
         }
      }
   
      isPotFileChanged = false;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("PropagationConfigPanel:SavePotFile() error occurred!\n%s\n", e.GetMessage().c_str());
      canClose = false;
      return;
   }
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
   
// LTR 09/15/06 - moved to where setting of Max Step Size is done
//   Real min = atof(minStepTextCtrl->GetValue());
//   Real max = atof(maxStepTextCtrl->GetValue());
//      
//   // @todo waw: temporarily commented out, to be uncommented 
//   // once Edwin updates his scripts to support this
//   // if (max <= min) 
//   if (max < min)
//   {
//      MessageInterface::PopupMessage
//         (Gmat::WARNING_, "Maximum Step can not be less than Minimum Step.\n"
//          "Propagation updates have not been saved");
//      return false;
//   }    
   
   try
   {
      Integer id, ivalue;
      Real rvalue, min, max;
      canClose = true;
      std::string inputString, minStr, maxStr;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" + 
                         thePropSetup->GetName() + "\" is not an allowed value. \n"
                        "The allowed values are: [%s].";
      
      // save initial step size
      id = thePropagator->GetParameterID("InitialStepSize");
      inputString = initialStepSizeTextCtrl->GetValue();

      // check to see if input is a real
      if (GmatStringUtil::ToDouble(inputString,&rvalue))
         thePropagator->SetRealParameter(id, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Initial Step Size","Real Number");
         canClose = false;
      }

      // save accuracy
      id = thePropagator->GetParameterID("Accuracy");
      inputString = accuracyTextCtrl->GetValue();
      // check to see if input is a real
      if (GmatStringUtil::ToDouble(inputString,&rvalue))     
         thePropagator->SetRealParameter(id, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Accuracy","Real Number >= 0.0");
         canClose = false;
      }

      // save min step size
      id = thePropagator->GetParameterID("MinStep");
      inputString = minStepTextCtrl->GetValue();      
      // check to see if input is a real
      if (GmatStringUtil::ToDouble(inputString,&rvalue))     
         thePropagator->SetRealParameter(id, rvalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Min Step Size","Real Number >= 0.0, MinStep <= MaxStep");
         canClose = false;
      }

      // save max step size
      id = thePropagator->GetParameterID("MaxStep");
      inputString = maxStepTextCtrl->GetValue();      
      // check to see if input is a real
      if (GmatStringUtil::ToDouble(inputString,&rvalue))     
      {
         // check to see if max step size is less than min
         minStr = minStepTextCtrl->GetValue();      
         maxStr = maxStepTextCtrl->GetValue();      
         if (GmatStringUtil::ToDouble(minStr,&min) && 
             GmatStringUtil::ToDouble(maxStr,&max))      
            // @todo waw: temporarily commented out, to be uncommented 
            // once Edwin updates his scripts to support this
            // if (min >= max) 
            if (max < min)
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, "Max Step Size can not be less than Mini Step Size.\n"
                     "The allowed values are: [Real Number >= 0.0, MinStep <= MaxStep]."); 
               canClose = false;
            }
            else  
               thePropagator->SetRealParameter(id, rvalue);
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Max Step Size","Real Number >= 0.0, MinStep <= MaxStep");
         canClose = false;
      }

      // save max step attempts
      id = thePropagator->GetParameterID("MaxStepAttempts");
      inputString = maxStepAttemptTextCtrl->GetValue();      
      // check to see if input is an integer
      if (GmatStringUtil::ToInteger(inputString,&ivalue))    
         thePropagator->SetIntegerParameter(id, ivalue);
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
            inputString.c_str(), "Max Step Attempts","Integer > 0");
         canClose = false;
      }
      
      if (integratorString.IsSameAs(integratorArray[ABM]))
      {
         // save min integration error
         id = thePropagator->GetParameterID("LowerError");
         inputString = minIntErrorTextCtrl->GetValue();      
         // check to see if input is a real
         if (GmatStringUtil::ToDouble(inputString,&rvalue))     
            thePropagator->SetRealParameter(id, rvalue);
         else
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
               inputString.c_str(), "Min Integration Error","Real Number > 0.0");
            canClose = false;
         }

         // save nominal integration error
         id = thePropagator->GetParameterID("TargetError");
         inputString = nomIntErrorTextCtrl->GetValue();      
         // check to see if input is a real
         if (GmatStringUtil::ToDouble(inputString,&rvalue))     
            thePropagator->SetRealParameter(id, rvalue);
         else
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
               inputString.c_str(), "Nominal Integration Error","Real Number > 0.0");
            canClose = false;
         }
      }       
      
      #if DEBUG_PROP_SAVE
      ShowPropData("SaveData() AFTER  saving Integrator");
      #endif
      
      return true;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("PropagationConfigPanel:SaveIntegratorData() error occurred!\n%s\n",
          e.GetMessage().c_str());
      return false;
   }
}

//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString PropagationConfigPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
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
      isIntegratorDataChanged = false; //loj: 12/20/06
      EnableUpdate(true);
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
   propOriginName = originComboBox->GetValue();
   
   // We don't want to set to true when only origin is changed
   //isForceModelChanged = true;
   isOriginChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnBodySelection(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnBodySelection(wxCommandEvent &event)
{
   if (primaryBodiesArray.IsEmpty())
      return;

   wxString selBody = bodyComboBox->GetStringSelection();

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
   
   if (strcmp(selBody, "Earth") == 0)
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
   
   gravTypeName = gravComboBox->GetStringSelection();
   
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
         std::string fileType = theFileMap[gravTypeName].c_str();
         //MessageInterface::ShowMessage("===> Found %s\n", fileType.c_str());
         
         try
         {
            forceList[currentBodyId]->potFilename =
               theGuiInterpreter->GetFileName(fileType).c_str();
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
         forceList[currentBodyId]->potFilename = potFileTextCtrl->GetValue();
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
      isPotFileChanged = true;
      EnableUpdate(true);
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
   
   dragTypeName = atmosComboBox->GetStringSelection();

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
      EnableUpdate(true);
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
   
   wxString eType = errorComboBox->GetStringSelection();

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
      EnableUpdate(true);
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
      
         EnableUpdate(true);
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
      wxString bodyName;
      
      oldList = forceList;
      forceList.clear();
      primaryBodiesArray.Clear();
      
      for (Integer i = 0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i];
         primaryBodiesArray.Add(bodyName.c_str()); 
         
         // Set default gravity model file for display
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
   
   EnableUpdate(true);
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
      wxString filename;
      
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
      
      forceList[currentBodyId]->potFilename = filename;
      
      if (forceList[currentBodyId]->bodyName == "Earth")
         forceList[currentBodyId]->gravType = earthGravModelArray[E_OTHER];
      else if (forceList[currentBodyId]->bodyName == "Luna")
         forceList[currentBodyId]->gravType = lunaGravModelArray[L_OTHER];
      else if (forceList[currentBodyId]->bodyName == "Mars")
         forceList[currentBodyId]->gravType = marsGravModelArray[M_OTHER];
      else //other bodies 
         forceList[currentBodyId]->gravType = othersGravModelArray[O_OTHER];

      //loj: Do we need to show? body name didn't change
      //waw: Yes, we need to update the degree & order displays (10/17/06)
      DisplayGravityFieldData(forceList[currentBodyId]->bodyName);
      isDegOrderChanged = true;
      EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnSetupButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSetupButton(wxCommandEvent &event)
{
   DragForce *dragForce;

   // if DragForce has not been created, create it first by calling SaveData()
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
      
      // Since all data are saved in the Dialog, we don't want to save it again
      //EnableUpdate(true);
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
   EnableUpdate(true);
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
         EnableUpdate(true);
         isForceModelChanged = true;
         return;
      }
      
      wxString bodyName;

      pmForceList.clear();
      secondaryBodiesArray.Clear(); 
      pmEditTextCtrl->Clear();
      
      //--------------------------
      // Add bodies to pmForceList
      //--------------------------     
      for (Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i];
         pmForceList.push_back(new ForceType(bodyName));
         
         secondaryBodiesArray.Add(bodyName);
         pmEditTextCtrl->AppendText(names[i] + " ");
      }
      
      EnableUpdate(true);
      isForceModelChanged = true;
   }
}   

//------------------------------------------------------------------------------
// void OnSRPEditButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPEditButton(wxCommandEvent &event)
{
   EnableUpdate(true);
}

// wxTextCtrl Events
//------------------------------------------------------------------------------
// void OnIntegratorTextUpdate(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnIntegratorTextUpdate(wxCommandEvent &event)
{
   //isIntegratorChanged = true;
   isIntegratorDataChanged = true;
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravityTextUpdate(wxCommandEvent& event)
{
   EnableUpdate(true);

   if (event.GetEventObject() == gravityDegreeTextCtrl)
   {
      forceList[currentBodyId]->gravDegree = gravityDegreeTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == gravityOrderTextCtrl)
   {     
      forceList[currentBodyId]->gravOrder = gravityOrderTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == potFileTextCtrl)
   {
      forceList[currentBodyId]->potFilename = potFileTextCtrl->GetValue();
      isPotFileChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
}

//------------------------------------------------------------------------------
// void OnMagneticTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnMagneticTextUpdate(wxCommandEvent& event)
{
   EnableUpdate(true);
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
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void ShowPropData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowPropData(const std::string& header)
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
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   MessageInterface::ShowMessage("theForceModel=%p", theForceModel);
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
// bool ParseDATGravityFile(const wxString& fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseDATGravityFile(const wxString& fname)
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
         (Gmat::WARNING_, "Error reading gravity model file.");
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
// bool ParseGRVGravityFile(const wxString& fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseGRVGravityFile(const wxString& fname)
{
   Integer       fileOrder, fileDegree;

   std::ifstream inFile;
   
   inFile.open(fname.c_str());
   
   if (!inFile)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Error reading gravity model file.");
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
// bool ParseCOFGravityFile(const wxString& fname)
//------------------------------------------------------------------------------
void PropagationConfigPanel::ParseCOFGravityFile(const wxString& fname)
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
         (Gmat::WARNING_, "Error reading gravity model file.");
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
