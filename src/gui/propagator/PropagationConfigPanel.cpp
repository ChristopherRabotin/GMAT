//$Header$
//------------------------------------------------------------------------------
//                              PropagationConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Waka Waktola
// Created: 2003/08/29
//
/**
 * This class contains the Propagation configuration window.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "CelesBodySelectDialog.hpp"
#include "DragInputsDialog.hpp"
#include "PropagationConfigPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"

#include "wx/platform.h"

//#define DEBUG_PROP_PANEL_SETUP
//#define DEBUG_PROP_PANEL_LOAD
//#define DEBUG_PROP_PANEL_SAVE
//#define DEBUG_PROP_PANEL_GRAV
//#define DEBUG_PROP_PANEL_ATMOS
//#define DEBUG_PROP_PANEL_ERROR
//#define DEBUG_PROP_INTEGRATOR

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
   EVT_COMBOBOX(ID_CB_INTGR, PropagationConfigPanel::OnIntegratorComboBox)
   EVT_COMBOBOX(ID_CB_BODY, PropagationConfigPanel::OnPrimaryBodyComboBox)
   EVT_COMBOBOX(ID_CB_ORIGIN, PropagationConfigPanel::OnOriginComboBox)
   EVT_COMBOBOX(ID_CB_GRAV, PropagationConfigPanel::OnGravityModelComboBox)
   EVT_COMBOBOX(ID_CB_ATMOS, PropagationConfigPanel::OnAtmosphereModelComboBox)
   EVT_CHECKBOX(ID_CHECKBOX, PropagationConfigPanel::OnSRPCheckBoxChange)
   EVT_COMBOBOX(ID_CB_ERROR, PropagationConfigPanel::OnErrorControlComboBox)
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
   
   #ifdef DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage
      ("PropagationConfigPanel() entered propSetupName=%s\n",
       propSetupName.c_str());
   #endif
   
   // Default integrator values
   thePropagatorName = "";
   thePropSetup      = NULL;
   thePropagator     = NULL;
   integratorString  = "RKV 8(9)";
   
   primaryBodyList.clear();
   pointMassBodyList.clear();
   
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
   usePropOriginForSrp = false;
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
   isAtmosChanged = false;
   isDegOrderChanged = false;
   isPotFileChanged = false;
   isMagfTextChanged = false;
   isIntegratorChanged = false;
   isIntegratorDataChanged = false;
   isOriginChanged = false;
   isErrControlChanged = false;
   
   canClose = true;
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// ~PropagationConfigPanel()
//------------------------------------------------------------------------------
PropagationConfigPanel::~PropagationConfigPanel()
{
   for (Integer i=0; i<(Integer)primaryBodyList.size(); i++)
      delete primaryBodyList[i];
   for (Integer i=0; i<(Integer)pointMassBodyList.size(); i++)
      delete pointMassBodyList[i]; 
   
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
   theGuiManager->UnregisterComboBox("CoordinateSystem", theOriginComboBox);
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
   
   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("PropConfigPanel::LoadData() entered\n");
   #endif
   
   thePropagator = thePropSetup->GetPropagator();
   theForceModel = thePropSetup->GetForceModel();
   numOfForces   = thePropSetup->GetNumForces();
   
   try
   {
      mFmPrefaceComment = theForceModel->GetCommentLine();
      propOriginName = theForceModel->GetStringParameter("CentralBody").c_str();
      errorControlTypeName = theForceModel->GetStringParameter("ErrorControl").c_str();
      
      PhysicalModel *force;     
      Integer paramId;
      wxString wxBodyName;
      wxString wxForceType;
      wxString tempStr;
      wxString useSRP;
      
      paramId = theForceModel->GetParameterID("SRP");
      useSRP = theForceModel->GetOnOffParameter(paramId).c_str();
      usePropOriginForSrp = (useSRP == "On") ? true : false;
      
      for (Integer i = 0; i < numOfForces; i++)
      {
         force = theForceModel->GetForce(i);
         if (force == NULL)
         {
            MessageInterface::ShowMessage
               ("**** INTERNAL ERROR *** PropagationConfigPanel::LoadData() "
                "Cannot continue, force pointer is NULL for index %d\n", i);
            return;
         }
         
         wxForceType = force->GetTypeName().c_str();
         wxBodyName = force->GetStringParameter("BodyName").c_str();
         
         #ifdef DEBUG_PROP_PANEL_LOAD
         MessageInterface::ShowMessage
            ("   Getting %s for body %s\n", wxForceType.c_str(), wxBodyName.c_str());
         #endif
         
         if (wxForceType == "PointMassForce")
         {            
            thePMF = (PointMassForce *)force;
            secondaryBodiesArray.Add(wxBodyName);    
            pointMassBodyList.push_back(new ForceType(wxBodyName, "None", 
                                                      dragModelArray[NONE_DM],
                                                      magfModelArray[NONE_MM],
                                                      thePMF));
            
            //Warn user about bodies already added as Primary body
            Integer fmSize = (Integer)primaryBodyList.size();
            Integer last = (Integer)pointMassBodyList.size() - 1;
            
            for (Integer i = 0; i < fmSize; i++)
            {
               if (strcmp(pointMassBodyList[last]->bodyName.c_str(), 
                          primaryBodyList[i]->bodyName.c_str()) == 0)
               {
                  MessageInterface::PopupMessage
                     (Gmat::WARNING_,
                      "Cannot set %s both as Primary body and Point Mass",
                      pointMassBodyList[last]->bodyName.c_str());
               }
            }
         }
         else if (wxForceType == "GravityField")
         {
            theGravForce = (GravityField*)force;
            wxString potFilename = theGravForce->GetStringParameter("PotentialFile").c_str();
            
            currentBodyId = FindPrimaryBody(wxBodyName);
            primaryBodyList[currentBodyId]->bodyName = wxBodyName;
            primaryBodyList[currentBodyId]->potFilename = potFilename;
            
            #ifdef DEBUG_PROP_PANEL_GRAV
            MessageInterface::ShowMessage
               ("   Getting gravity model type for %s, potFilename=%s\n",
                wxBodyName.c_str(), potFilename.c_str());
            #endif
            
            // Make potential upper case for comparison
            potFilename.MakeUpper();
            
            if (wxBodyName == "Earth")
            {
               EarthGravModelType eGravModelType;
               
               if (potFilename.find("JGM2") != std::string::npos)
                  eGravModelType = JGM2;    
               else if (potFilename.find("JGM3") != std::string::npos)
                  eGravModelType = JGM3;   
               else if (potFilename.find("EGM") != std::string::npos)
                  eGravModelType = EGM96;  
               else 
                  eGravModelType = E_OTHER;
               
               primaryBodyList[currentBodyId]->gravType = earthGravModelArray[eGravModelType];
               primaryBodyList[currentBodyId]->potFilename =
                  theFileMap[earthGravModelArray[eGravModelType]];               
            }
            else if (wxBodyName == "Luna")
            {
               LunaGravModelType lGravModelType;
               
               if (potFilename.find("LP165P") != std::string::npos)
                  lGravModelType = LP165;     
               else 
                  lGravModelType = L_OTHER;
               
               primaryBodyList[currentBodyId]->gravType = lunaGravModelArray[lGravModelType];
               primaryBodyList[currentBodyId]->potFilename =
                  theFileMap[lunaGravModelArray[lGravModelType]];
            }
            else if (wxBodyName == "Venus")
            {               
               VenusGravModelType vGravModelType;
               
               if (potFilename.find("MGN") != std::string::npos)
                  vGravModelType = MGNP180U;     
               else 
                  vGravModelType = V_OTHER;
               
               primaryBodyList[currentBodyId]->gravType = venusGravModelArray[vGravModelType];
               primaryBodyList[currentBodyId]->potFilename =
                  theFileMap[venusGravModelArray[vGravModelType]];
            }
            else if (wxBodyName == "Mars")
            {
               MarsGravModelType mGravModelType;
               
               if (potFilename.find("MARS50C") != std::string::npos)
                  mGravModelType = MARS50C;     
               else 
                  mGravModelType = M_OTHER;
               
               primaryBodyList[currentBodyId]->gravType = marsGravModelArray[mGravModelType];
               primaryBodyList[currentBodyId]->potFilename =
                  theFileMap[marsGravModelArray[mGravModelType]];
            }   
            else //other bodies
            {
               OthersGravModelType oGravModelType;
               
               oGravModelType = O_OTHER;
               
               primaryBodyList[currentBodyId]->gravType = othersGravModelArray[oGravModelType];
               primaryBodyList[currentBodyId]->potFilename =
                  theFileMap[othersGravModelArray[oGravModelType]];
            }
            
            #ifdef DEBUG_PROP_PANEL_GRAV
            MessageInterface::ShowMessage("   Getting the gravity force\n");
            #endif
            
            primaryBodyList[currentBodyId]->gravf = theGravForce;
            
            // Set actual full potenential file path (loj: 2007.10.26)
            wxString gravTypeName = primaryBodyList[currentBodyId]->gravType;
            std::string fileType = theFileMap[gravTypeName].c_str();
            if (gravTypeName != "None" && gravTypeName != "Other")
            {
               primaryBodyList[currentBodyId]->potFilename =
                  theGuiInterpreter->GetFileName(fileType).c_str();
            }
            
            // Warn user about bodies already added as Primary body
            Integer pmSize = (Integer)pointMassBodyList.size();
            Integer last = (Integer)primaryBodyList.size() - 1;
            
            for (Integer i = 0; i < pmSize; i++)
            {
               if (strcmp(primaryBodyList[last]->bodyName.c_str(), 
                          pointMassBodyList[i]->bodyName.c_str()) == 0)
               {
                  MessageInterface::PopupMessage
                     (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass",
                      pointMassBodyList[last]->bodyName.c_str());
               }
            }
            
            if (primaryBodyList[currentBodyId]->potFilename == "")
            {
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "Cannot find Potential File for %s.\n",
                   wxBodyName.c_str()); 
            }
            
            tempStr = "";
            tempStr << theGravForce->GetIntegerParameter("Degree");
            primaryBodyList[currentBodyId]->gravDegree = tempStr;
            
            tempStr = "";
            tempStr << theGravForce->GetIntegerParameter("Order");
            primaryBodyList[currentBodyId]->gravOrder = tempStr;
            
            bool found = false;
            for (Integer i = 0; i < (Integer)primaryBodiesArray.GetCount(); i++)
            {
               if ( primaryBodiesArray[i].CmpNoCase(wxBodyName) == 0 )
                  found = true;
            }
            
            if (!found)
               primaryBodiesArray.Add(wxBodyName);     
         }
         else if (wxForceType == "DragForce")
         {            
            theDragForce = (DragForce*)force;  
            paramId = theDragForce->GetParameterID("AtmosphereModel");
            atmosModelString = theDragForce->GetStringParameter(paramId).c_str();
            
            currentBodyId = FindPrimaryBody(wxBodyName);
            primaryBodyList[currentBodyId]->bodyName = wxBodyName;
            primaryBodyList[currentBodyId]->dragType = atmosModelString;
            primaryBodyList[currentBodyId]->dragf = theDragForce;
            
            //Warn user about bodies already added as Primary body
            Integer pmSize = (Integer)pointMassBodyList.size();
            Integer last = (Integer)primaryBodyList.size() - 1;
            
            for (Integer i = 0; i < pmSize; i++)
            {
               if (strcmp(primaryBodyList[last]->bodyName.c_str(), 
                          pointMassBodyList[i]->bodyName.c_str()) == 0)
               {
                  MessageInterface::PopupMessage
                     (Gmat::WARNING_, "Cannot set %s both as Primary body and Point Mass",
                      pointMassBodyList[last]->bodyName.c_str());
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
         else if (wxForceType == "SolarRadiationPressure")
         {
            // Currently SRP can only be applied to force model central body,
            // so we don't need to set to promary body list (loj:2007.10.19)
            //currentBodyId = FindPrimaryBody(wxBodyName);
            //primaryBodyList[currentBodyId]->useSrp = true;
            //primaryBodyList[currentBodyId]->bodyName = wxBodyName;
            //primaryBodyList[currentBodyId]->srpf = theSRP;
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   
   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   Getting Primary bodies array.\n");
   #endif
   
   if (!primaryBodiesArray.IsEmpty())
   { 
      primaryBodyString = primaryBodiesArray.Item(0).c_str();
      currentBodyName = primaryBodyString;
      currentBodyId = FindPrimaryBody(currentBodyName);
   }
   
   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   primaryBodyString=%s\n", primaryBodyString.c_str());
   #endif
   
   numOfBodies = (Integer)primaryBodiesArray.GetCount();
   std::string propType = thePropagator->GetTypeName();
   
   #ifdef DEBUG_PROP_PANEL_LOAD
   MessageInterface::ShowMessage("   propType=%s\n", propType.c_str());
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
         thePrimaryBodyComboBox->Append(primaryBodiesArray[i]);   

   theIntegratorComboBox->SetSelection(typeId);
   integratorString = integratorArray[typeId]; 
   
   // Display the force model origin (central body)
   theOriginComboBox->SetValue(propOriginName);
   
   DisplayIntegratorData(false);
   DisplayForceData();
   
   Integer count = (Integer)primaryBodyList.size(); 
   
   if (count == 0)
      EnablePrimaryBodyItems(false, false);
   else
      EnablePrimaryBodyItems(true);
   
   #ifdef DEBUG_PROP_PANEL_LOAD
   ShowPropData("LoadData() PropData on exit");
   ShowForceList("LoadData() ForceList on exit");
   #endif
   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::SaveData()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("SaveData() thePropagatorName=%s\n", thePropagator->GetTypeName().c_str());
   MessageInterface::ShowMessage("   isIntegratorChanged=%d\n", isIntegratorChanged);
   MessageInterface::ShowMessage("   isIntegratorDataChanged=%d\n",isIntegratorDataChanged);
   MessageInterface::ShowMessage("   isForceModelChanged=%d\n", isForceModelChanged);
   MessageInterface::ShowMessage("   isDegOrderChanged=%d\n", isDegOrderChanged);
   MessageInterface::ShowMessage("   isPotFileChanged=%d\n", isPotFileChanged);
   MessageInterface::ShowMessage("   isAtmosChanged=%d\n", isAtmosChanged);
   MessageInterface::ShowMessage("   isOriginChanged=%d\n", isOriginChanged);
   MessageInterface::ShowMessage("   isErrControlChanged=%d\n", isErrControlChanged);
   #endif
   
   canClose = true;
   
   //-----------------------------------------------------------------
   // check for empty primary bodies or point mass
   //-----------------------------------------------------------------
   if (primaryBodyList.size() == 0 && pointMassBodyList.size() == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Please select primary bodies or point mass bodies\n");
      canClose = false;
      return;
   }
   
   
   //-----------------------------------------------------------------
   // check for valid gravity model before saving
   //-----------------------------------------------------------------
   for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
   {
      if (primaryBodyList[i]->gravType == "None")
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Please select Gravity Field Model for %s\n",
             primaryBodyList[i]->bodyName.c_str());
         canClose = false;
         return;
      }
      //loj: 2007.10.26
      // Do we need to check empty potential file?
      // Fow now allow to use default coefficients from the body.      
      //else if (primaryBodyList[i]->gravType == "Other" &&
      //         primaryBodyList[i]->potFilename == "")
      //{
      //   MessageInterface::PopupMessage
      //      (Gmat::WARNING_, "Please select potential file for %s\n",
      //       primaryBodyList[i]->bodyName.c_str());
      //   canClose = false;
      //   return;
      //}
   }
   
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   
   //-------------------------------------------------------
   // Saving the Integrator
   //-------------------------------------------------------
   if (isIntegratorChanged)
   {
      #ifdef DEBUG_PROP_PANEL_SAVE
      ShowPropData("SaveData() BEFORE saving Integrator");
      #endif
      
      isIntegratorChanged = false;
      
      if (isIntegratorDataChanged)
         if (SaveIntegratorData())
            isIntegratorDataChanged = false;
      
      thePropSetup->SetPropagator(thePropagator);
      
      // Since the propagator is cloned in the base code, get new pointer
      thePropagator = thePropSetup->GetPropagator();
      
   }
   else if (isIntegratorDataChanged)
   {
      if (SaveIntegratorData())
         isIntegratorDataChanged = false;
   }
   
   
   //-------------------------------------------------------
   // Saving the force model
   //-------------------------------------------------------
   if (isForceModelChanged)
   {      
      #ifdef DEBUG_PROP_PANEL_SAVE
      ShowForceList("SaveData() BEFORE saving ForceModel");
      #endif
      
      // save force model name for later use
      std::string fmName = theForceModel->GetName();
      
      isForceModelChanged = false;
      ForceModel *newFm = new ForceModel();
      newFm->SetCommentLine(mFmPrefaceComment);
      std::string bodyName;
      
      //----------------------------------------------------
      // save point mass force model
      //----------------------------------------------------
      for (Integer i=0; i < (Integer)pointMassBodyList.size(); i++)
      {      
         thePMF = new PointMassForce();
         bodyName = pointMassBodyList[i]->bodyName.c_str();
         thePMF->SetBodyName(bodyName);
         pointMassBodyList[i]->pmf = thePMF;
         newFm->AddForce(thePMF);
      }
      
      //----------------------------------------------------
      // save gavity force model
      //----------------------------------------------------
      try
      {
         for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
         {
            GravityField *gf = primaryBodyList[i]->gravf;

            Integer deg = -999;
            Integer ord = -999;
            
            // save deg and order for later use
            if (gf != NULL)
            {
               deg = gf->GetIntegerParameter("Degree");
               ord = gf->GetIntegerParameter("Order");
            }
            
            // Create new GravityField since ForceModel destructor will delete
            // all PhysicalModel
            #ifdef DEBUG_PROP_PANEL_SAVE
            MessageInterface::ShowMessage
               ("SaveData() Creating GravityField for %s\n", bodyName.c_str());
            #endif
            
            // create new GravityField
            bodyName = primaryBodyList[i]->bodyName.c_str();
            theGravForce = new GravityField("", bodyName);
            theGravForce->SetSolarSystem(theSolarSystem);
            theGravForce->SetStringParameter("BodyName", bodyName);
            theGravForce->SetStringParameter("PotentialFile",
                                             primaryBodyList[i]->potFilename.c_str());
            
            if (deg != -999)
            {
               theGravForce->SetIntegerParameter("Degree", deg);
               theGravForce->SetIntegerParameter("Order", ord);
            }
            
            primaryBodyList[i]->gravf = theGravForce;            
            newFm->AddForce(theGravForce);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      }
      
      if (isDegOrderChanged)
         SaveDegOrder();
      
      if (isPotFileChanged)
         SavePotFile();
      
      //----------------------------------------------------
      // save drag force model
      //----------------------------------------------------
      Integer paramId;
      
      try
      {
         for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
         {
            if (primaryBodyList[i]->dragType == dragModelArray[NONE_DM])
            {
               primaryBodyList[i]->dragf = NULL;
            }
            else
            {
               #ifdef DEBUG_PROP_PANEL_SAVE
               ShowForceList("SaveData() BEFORE  saving DragForce");
               #endif
               
               Real fluxF107 = -999.999;
               Real fluxF107A = -999.999;
               Real kp = -999.999;

               DragForce *df = primaryBodyList[i]->dragf;
               // save drag flux info for later use
               if (df != NULL)
               {
                  fluxF107 = df->GetRealParameter("F107");
                  fluxF107A = df->GetRealParameter("F107A");
                  kp = df->GetRealParameter("MagneticIndex");
               }
               
               // create new DragForce
               bodyName = primaryBodyList[i]->bodyName.c_str();
               theDragForce = new DragForce(primaryBodyList[i]->dragType.c_str());
               theCelestialBody = theSolarSystem->GetBody(bodyName); 
               theAtmosphereModel = theCelestialBody->GetAtmosphereModel();
               
               #ifdef DEBUG_PROP_PANEL_SAVE
               ShowForceList("Entering if (theAtmosphereModel == NULL)");
               #endif
               
               if (theAtmosphereModel == NULL)  
               {
                  theAtmosphereModel = (AtmosphereModel*)theGuiInterpreter->CreateObject
                     (primaryBodyList[i]->dragType.c_str(), primaryBodyList[i]->dragType.c_str());
               
                  #ifdef DEBUG_PROP_PANEL_SAVE
                  ShowForceList("Exiting if (theAtmosphereModel == NULL)");
                  #endif
               }
               
               theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);
               
               #ifdef DEBUG_PROP_PANEL_SAVE
               ShowForceList("theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);");
               #endif
               
               paramId = theDragForce->GetParameterID("AtmosphereModel");
               bodyName = primaryBodyList[i]->bodyName.c_str();
               theDragForce->SetStringParameter(paramId, primaryBodyList[i]->dragType.c_str());
               theDragForce->SetStringParameter("BodyName", bodyName);
               
               // if drag force was previous defined, set previous flux value
               if (fluxF107 != -999.999)
               {
                  theDragForce->SetRealParameter("F107", fluxF107);
                  theDragForce->SetRealParameter("F107A", fluxF107A);
                  theDragForce->SetRealParameter("MagneticIndex", kp);
               }
               
               primaryBodyList[i]->dragf = theDragForce;
               newFm->AddForce(theDragForce);
               
               #ifdef DEBUG_PROP_PANEL_SAVE
               ShowForceList("SaveData() AFTER  saving DragForce");
               #endif 
            }
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      }
      
      //----------------------------------------------------
      // save SRP data
      //----------------------------------------------------
      try
      {
         paramId= newFm->GetParameterID("SRP");
         
         if (usePropOriginForSrp)
         {
            theSRP = new SolarRadiationPressure();
            bodyName = propOriginName;
            theSRP->SetStringParameter("BodyName", bodyName);
            newFm->AddForce(theSRP);
            newFm->SetOnOffParameter(paramId, "On");
         }
         else
         {
            newFm->SetOnOffParameter(paramId, "Off");
         }
         
         #ifdef DEBUG_PROP_PANEL_SAVE
         ShowForceList("SaveData() AFTER  saving SRP");
         #endif
         
         // Since SRP is only applied to force model central body,
         // we don't need to go through primary body list (loj: 2007.10.19)
         //for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
         //{
         //   if (primaryBodyList[i]->useSrp)
         //   {
         //      theSRP = new SolarRadiationPressure();
         //      primaryBodyList[i]->srpf = theSRP;
         //      bodyName = primaryBodyList[i]->bodyName.c_str();
         //      theSRP->SetStringParameter("BodyName", bodyName);
         //      newFm->AddForce(theSRP);
         //
         //      paramId= newFm->GetParameterID("SRP");
         //      newFm->SetOnOffParameter(paramId, "On");
         //
         //      #ifdef DEBUG_PROP_PANEL_SAVE
         //      ShowForceList("SaveData() AFTER  saving SRP");
         //      #endif
         //   }
         //}
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      }
      
      //----------------------------------------------------
      // Saving the error control and Origin
      // Always set this to new forcemodel
      //----------------------------------------------------
      try
      {
         newFm->SetStringParameter("ErrorControl", errorControlTypeName.c_str());
         newFm->SetStringParameter("CentralBody", propOriginName.c_str());
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
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
      theForceModel->SetName(fmName);
      
      PhysicalModel *pm;
      int size = primaryBodyList.size();
      for (int i=0; i<numOfForces; i++)
      {
         pm = theForceModel->GetForce(i);
         
         #ifdef DEBUG_PROP_PANEL_SAVE
         MessageInterface::ShowMessage
            ("   pm=(%p)%s(%s)\n", pm, pm->GetTypeName().c_str(),
             pm->GetBodyName().c_str());
         #endif
         
         for (int j=0; j<size; j++)
         {
            if ((primaryBodyList[j]->bodyName).IsSameAs(pm->GetBodyName().c_str()))
            {
               if (pm->GetTypeName() == "PointMassForce")
                  primaryBodyList[j]->pmf = (PointMassForce*)pm;
               else if (pm->GetTypeName() == "GravityField")
                  primaryBodyList[j]->gravf = (GravityField*)pm;
               else if (pm->GetTypeName() == "DragForce")
                  primaryBodyList[j]->dragf = (DragForce*)pm;
               else if (pm->GetTypeName() == "SolarRadiationPressure")
                  primaryBodyList[j]->srpf = (SolarRadiationPressure*)pm;
            }
         }
      }
      
      #ifdef DEBUG_PROP_PANEL_SAVE
      ShowForceList("SaveData() AFTER  saving ForceModel");
      #endif
   } // end if(isForceModelChange)
   else
   {
      //----------------------------------------------------
      // Saving Error Control and the Origin (Central Body)
      //----------------------------------------------------
      try
      {
         if (isErrControlChanged)
         {
            theForceModel->SetStringParameter
               ("ErrorControl", theErrorComboBox->GetStringSelection().c_str());
            isErrControlChanged = false;
         }
         
         if (isOriginChanged)
         {
            theForceModel->SetStringParameter("CentralBody", propOriginName.c_str());
            isOriginChanged = false;
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      }
      
      if (isDegOrderChanged)
         SaveDegOrder();
      
      // Save only GravComboBox or PotFileText is changed
      if (isPotFileChanged)
         SavePotFile();
      
      if (isAtmosChanged)
         SaveAtmosModel();
   }
}


//------------------------------------------------------------------------------
// Integer FindPrimaryBody(const wxString &bodyName, bool create = true,
//                         const wxString &gravType, const wxString &dragType,
//                         const wxString &magfType)
//------------------------------------------------------------------------------
/*
 * Finds the primary body from the primaryBodyList and returns index if body
 * was found return index. If body was not found and creaet is false,
 * it returns -1, otherwise create a new ForceType and return new index
 *
 * @param  bodyName  Name of the body to look for
 * @param  create    If true, it creates new ForceType
 * @param  gravType  The type name of GravityForce
 * @param  dragType  The type name of DragForce
 * @param  magfType  The type name of MagneticForce
 *
 * @return body index or -1 if body not found and create is false
 *
 */
//------------------------------------------------------------------------------
Integer PropagationConfigPanel::FindPrimaryBody(const wxString &bodyName,
                                                bool create,
                                                const wxString &gravType,
                                                const wxString &dragType,
                                                const wxString &magfType)
{
   for (Integer i=0; i<(Integer)primaryBodyList.size(); i++)
   {
      if (primaryBodyList[i]->bodyName == bodyName)
         return i;
   }
   
   if (!create)
      return -1;
   
   primaryBodyList.push_back(new ForceType(bodyName, gravType, dragType, magfType));
   
   // Set gravity model file
   if (theFileMap.find(gravType) != theFileMap.end())
   {
      std::string potFileType = theFileMap[gravType].c_str();
      wxString wxPotFileName = theGuiInterpreter->GetFileName(potFileType).c_str();
      //MessageInterface::ShowMessage("===> potFile=%s\n", potFileType.c_str());
      primaryBodyList.back()->potFilename = wxPotFileName;
   }
   
   #ifdef DEBUG_PROP_PANEL_FIND_BODY
   ShowForceList("FindPrimaryBody() after add body to primaryBodyList");
   #endif
   
   return (Integer)(primaryBodyList.size() - 1);
}


//------------------------------------------------------------------------------
// Integer FindPointMassBody(const wxString &bodyName)
//------------------------------------------------------------------------------
/*
 * Finds the point mass body from the pointMassBodyList and returns index if body
 * was found return index. If body was not found, it returns -1,
 *
 * @param  bodyName  Name of the body to look for
 *
 * @return body index or -1 if body was not found
 *
 */
//------------------------------------------------------------------------------
Integer PropagationConfigPanel::FindPointMassBody(const wxString &bodyName)
{
   for (Integer i=0; i<(Integer)pointMassBodyList.size(); i++)
   {
      if (pointMassBodyList[i]->bodyName == bodyName)
         return i;
   }
   
   return -1;
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void PropagationConfigPanel::Initialize()
{  
   #ifdef DEBUG_PROP_PANEL_INIT
   MessageInterface::ShowMessage("PropagationConfigPanel::Initialize() entered\n");
   #endif
   
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   thePropSetup = (PropSetup*)theGuiInterpreter->GetConfiguredObject(propSetupName);
   
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
   
   #ifdef DEBUG_PROP_PANEL_INIT
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::Initialize() Initialized local arrays.\n");
   #endif
   
   // initialize mag. filed model type array
   magfModelArray.Add("None");
   
}


//------------------------------------------------------------------------------
// void Setup(wxWindow *parent)
//------------------------------------------------------------------------------
void PropagationConfigPanel::Setup(wxWindow *parent)
{
   #ifdef DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() entered\n"); 
   #endif
   
   Integer bsize = 2; // border size
   
   //-----------------------------------------------------------------
   // Integrator
   //-----------------------------------------------------------------
   // Type
   wxStaticText *integratorStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Type"),
                        wxDefaultPosition, wxSize(170,20), wxST_NO_AUTORESIZE);
   
   wxString *intgArray = new wxString[IntegratorCount];
   for (Integer i=0; i<IntegratorCount; i++)
      intgArray[i] = integratorArray[i];
   
   theIntegratorComboBox =
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
   intFlexGridSizer->Add( theIntegratorComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
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
   // Force Model
   //-----------------------------------------------------------------
   // Error Control
   wxStaticText *errorCtrlStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Error Control"),
                        wxDefaultPosition, wxSize(70,20), wxST_NO_AUTORESIZE );
   
   theErrorComboBox =
      new wxComboBox( parent, ID_CB_ERROR, errorControlArray[0],
                      wxDefaultPosition, wxSize(100,-1),
                      errorControlArray, wxCB_DROPDOWN|wxCB_READONLY );
   
   wxFlexGridSizer *errorFlexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
   errorFlexGridSizer->Add( errorCtrlStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   errorFlexGridSizer->Add( theErrorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Central Body
   wxStaticText *centralBodyStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Central Body"),
                        wxDefaultPosition, wxSize(70,20), wxST_NO_AUTORESIZE);
   theOriginComboBox  =
      theGuiManager->GetConfigBodyComboBox(this, ID_CB_ORIGIN, wxSize(100,-1));
   
   wxFlexGridSizer *centralBodySizer = new wxFlexGridSizer( 2, 0, 2 );
   centralBodySizer->Add( centralBodyStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   centralBodySizer->Add( theOriginComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Primary Bodies
   wxArrayString bodyArray;
   thePrimaryBodyComboBox =
      new wxComboBox( parent, ID_CB_BODY, wxT(primaryBodyString),
                      wxDefaultPosition,  wxSize(80,-1),// 0,
                      bodyArray, wxCB_DROPDOWN|wxCB_READONLY );
   bodyTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(160,-1), wxTE_READONLY );
   wxButton *primaryBodySelectButton =
      new wxButton( parent, ID_BUTTON_ADD_BODY, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *bodySizer = new wxBoxSizer( wxHORIZONTAL );
   bodySizer->Add( thePrimaryBodyComboBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   bodySizer->Add( bodyTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   bodySizer->Add( primaryBodySelectButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   // Gravity
   wxStaticText *type1StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Model"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   wxArrayString gravArray;
   theGravModelComboBox =
      new wxComboBox( parent, ID_CB_GRAV, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), // 0,
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
   theGravModelSearchButton =
      new wxButton( parent, ID_BUTTON_GRAV_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *degOrdSizer = new wxBoxSizer( wxHORIZONTAL );
   degOrdSizer->Add( type1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( theGravModelComboBox, 0, wxALIGN_CENTRE|wxALL, bsize); 
   degOrdSizer->Add( degree1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( order1StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( gravityOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   degOrdSizer->Add( theGravModelSearchButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   potFileStaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Potential File"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   potFileTextCtrl =
      new wxTextCtrl( parent, ID_TEXTCTRL_GRAV, wxT(""), wxDefaultPosition,
                      wxSize(290,-1), 0 );
   
   wxBoxSizer *potFileSizer = new wxBoxSizer( wxHORIZONTAL );   
   potFileSizer->Add( potFileStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   potFileSizer->Add( potFileTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   GmatStaticBoxSizer *gravStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Gravity");
   gravStaticSizer->Add( degOrdSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   gravStaticSizer->Add( potFileSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Drag
   wxStaticText *type2StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Atmosphere Model"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   theAtmosModelComboBox =
      new wxComboBox( parent, ID_CB_ATMOS, dragModelArray[0],
                      wxDefaultPosition, wxDefaultSize,
                      dragModelArray, wxCB_DROPDOWN|wxCB_READONLY );
   theDragSetupButton =
      new wxButton( parent, ID_BUTTON_SETUP, wxT("Setup"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *atmosSizer = new wxBoxSizer( wxHORIZONTAL );
   atmosSizer->Add( type2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);   
   atmosSizer->Add( theAtmosModelComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   atmosSizer->Add( theDragSetupButton, 0, wxALIGN_CENTRE|wxALL, bsize);       
   
   GmatStaticBoxSizer *atmosStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Drag");
   atmosStaticSizer->Add( atmosSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Magnetic Field
   wxStaticText *type3StaticText =
      new wxStaticText( parent, ID_TEXT, wxT("Model"),
                        wxDefaultPosition, wxDefaultSize, 0 );
   
   theMagfModelComboBox =
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
   theMagModelSearchButton =
      new wxButton( parent, ID_BUTTON_MAG_SEARCH, wxT("Search"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxBoxSizer *magfSizer = new wxBoxSizer( wxHORIZONTAL );
   magfSizer->Add( type3StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( theMagfModelComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( degree2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magneticDegreeTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( order2StaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( magneticOrderTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   magfSizer->Add( theMagModelSearchButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
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
   wxButton *editPmfButton =
      new wxButton( parent, ID_BUTTON_PM_EDIT, wxT("Select"),
                    wxDefaultPosition, wxDefaultSize, 0 );
   
   wxFlexGridSizer *pointMassSizer = new wxFlexGridSizer( 3, 0, 2 );
   pointMassSizer->Add( pointMassStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   pointMassSizer->Add( pmEditTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   pointMassSizer->Add( editPmfButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // SRP
   //-----------------------------------------------------------------
   theSrpCheckBox =
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
   fmStaticSizer->Add( theSrpCheckBox, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
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
   // disable components for future implemenation
   //-----------------------------------------------------------------   
   theMagfModelComboBox->Enable(false);
   magneticDegreeTextCtrl->Enable(false);
   magneticOrderTextCtrl->Enable(false);
   theMagModelSearchButton->Enable(false);
   type3StaticText->Enable(false);
   degree2StaticText->Enable(false);
   order2StaticText->Enable(false);
   
   //-----------------------------------------------------------------
   // initially disable components
   //-----------------------------------------------------------------   
   theDragSetupButton->Enable(false);
   
   #ifdef DEBUG_PROP_PANEL_SETUP
   MessageInterface::ShowMessage("PropagationConfigPanel::Setup() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void DisplayIntegratorData(bool integratorChanged)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayIntegratorData(bool integratorChanged)
{
   int propIndex = theIntegratorComboBox->GetSelection();
   
   #ifdef DEBUG_PROP_INTEGRATOR
   MessageInterface::ShowMessage
      ("DisplayIntegratorData() integratorChanged=%d, integratorString=<%s>\n",
       integratorChanged, integratorString.c_str());
   #endif
   
   if (integratorChanged)
   {
      std::string integratorType = integratorTypeArray[propIndex].c_str();
      thePropagatorName = propSetupName + "_" + integratorType;
      thePropagator = (Propagator*)theGuiInterpreter->GetConfiguredObject(thePropagatorName);
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
   
   #ifdef DEBUG_PROP_INTEGRATOR
   ShowPropData("DisplayIntegratorData() exiting...");
   #endif
}

//------------------------------------------------------------------------------
// void DisplayForceData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayForceData()
{
   DisplayErrorControlData();
   DisplaySRPData();
   
   if (!pointMassBodyList.empty())
      DisplayPointMassData(); 
   
   if (primaryBodyList.empty())
      return;  
   
   DisplayPrimaryBodyData(); 
   DisplayGravityFieldData(currentBodyName); 
   DisplayAtmosphereModelData(); 
   DisplayMagneticFieldData();  
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
   
   thePrimaryBodyComboBox->SetSelection(bodyIndex);
}

//------------------------------------------------------------------------------
// void DisplayGravityFieldData(const wxString& bodyName)
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayGravityFieldData(const wxString& bodyName)
{
   #ifdef DEBUG_PROP_PANEL_GRAV
   MessageInterface::ShowMessage
      ("DisplayGravityFieldData() currentBodyName=%s gravType=%s\n",
       currentBodyName.c_str(), primaryBodyList[currentBodyId]->gravType.c_str());
   ShowForceList("DisplayGravityFieldData() entered");
   #endif
   
   theGravModelComboBox->Clear();
   
   wxString gravType = primaryBodyList[currentBodyId]->gravType;
   
   // for gravity model ComboBox
   if (bodyName == "Earth")
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Earth gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)EarthGravModelCount; i++)
         theGravModelComboBox->Append(earthGravModelArray[i]);

   }
   else if (bodyName == "Luna")
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Luna gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)LunaGravModelCount; i++)
         theGravModelComboBox->Append(lunaGravModelArray[i]);
   }
   else if (bodyName == "Venus")
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Venus gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)VenusGravModelCount; i++)
         theGravModelComboBox->Append(venusGravModelArray[i]);

   }
   else if (bodyName == "Mars")
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying Mars gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)MarsGravModelCount; i++)
         theGravModelComboBox->Append(marsGravModelArray[i]);

   }
   else // other bodies
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("DisplayGravityFieldData() Displaying other gravity model\n");
      #endif
      
      for (Integer i = 0; i < (Integer)OthersGravModelCount; i++)
         theGravModelComboBox->Append(othersGravModelArray[i]);

   }
   
   theGravModelSearchButton->Enable(false);
   ////potFileStaticText->Enable(false);
   potFileTextCtrl->Enable(false);
   gravityDegreeTextCtrl->Enable(true);
   gravityOrderTextCtrl->Enable(true);
   potFileTextCtrl->SetValue(primaryBodyList[currentBodyId]->potFilename);
   
   if (gravType == "None")
   {
      gravityDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
   }
   else
   {
      if (primaryBodyList[currentBodyId]->gravType == "Other")
      {
         theGravModelSearchButton->Enable(true);
         potFileStaticText->Enable(true);
         potFileTextCtrl->Enable(true);
      }
   }
   
   theGravModelComboBox->SetValue(gravType);
   gravityDegreeTextCtrl->SetValue(primaryBodyList[currentBodyId]->gravDegree);
   gravityOrderTextCtrl->SetValue(primaryBodyList[currentBodyId]->gravOrder);
   
   #ifdef DEBUG_PROP_PANEL_GRAV
   ShowForceList("DisplayGravityFieldData() exiting");
   #endif
}

//------------------------------------------------------------------------------
// void DisplayAtmosphereModelData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayAtmosphereModelData()
{
   #ifdef DEBUG_PROP_PANEL_DISPLAY
   MessageInterface::ShowMessage
      ("DisplayAtmosphereModelData() currentBodyName=%s dragType=%s\n",
       currentBodyName.c_str(), primaryBodyList[currentBodyId]->dragType.c_str());
   #endif
   
   // Enable atmosphere model only for Earth
   if (currentBodyName == "Earth")
   {
      theAtmosModelComboBox->Enable(true);
   }
   else
   {
      theAtmosModelComboBox->Enable(false);
      theDragSetupButton->Enable(false);
   }
   
   // Set current drag force pointer
   theDragForce = primaryBodyList[currentBodyId]->dragf;
   
   if (primaryBodyList[currentBodyId]->dragType == dragModelArray[NONE_DM])
   {
      theAtmosModelComboBox->SetSelection(NONE_DM);
      theDragSetupButton->Enable(false);
   }
   else if (primaryBodyList[currentBodyId]->dragType == dragModelArray[EXPONENTIAL])
   {
      theAtmosModelComboBox->SetSelection(EXPONENTIAL);
      theDragSetupButton->Enable(false);
   }
   else if (primaryBodyList[currentBodyId]->dragType == dragModelArray[MSISE90])
   {
      theAtmosModelComboBox->SetSelection(MSISE90);
      theDragSetupButton->Enable(true);
   }
   else if (primaryBodyList[currentBodyId]->dragType == dragModelArray[JR])
   {
      theAtmosModelComboBox->SetSelection(JR);
      theDragSetupButton->Enable(true);
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
   if (primaryBodyList[currentBodyId]->magfType == magfModelArray[NONE_MM])
   {
      theMagfModelComboBox->SetSelection(NONE_MM);
   }
}


//------------------------------------------------------------------------------
// void DisplaySRPData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplaySRPData()
{
   theSrpCheckBox->SetValue(usePropOriginForSrp);
}


//------------------------------------------------------------------------------
// void DisplayErrorControlData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::DisplayErrorControlData()
{
   #ifdef DEBUG_PROP_PANEL_DISPLAY
   MessageInterface::ShowMessage("On DisplayErrorControlData()\n");
   #endif

   wxString wxEcTypeName = errorControlTypeName.c_str();
   
   if (wxEcTypeName == errorControlArray[NONE_EC])
      theErrorComboBox->SetSelection(NONE_EC);
   else if (wxEcTypeName == errorControlArray[RSSSTEP])
      theErrorComboBox->SetSelection(RSSSTEP);
   else if (wxEcTypeName == errorControlArray[RSSSTATE])
      theErrorComboBox->SetSelection(RSSSTATE);
   else if (wxEcTypeName == errorControlArray[LARGESTSTEP])
      theErrorComboBox->SetSelection(LARGESTSTEP);
   else if (wxEcTypeName == errorControlArray[LARGESTSTATE])
      theErrorComboBox->SetSelection(LARGESTSTATE);
}


//------------------------------------------------------------------------------
// void EnablePrimaryBodyItems(bool enable = true, bool clear = false)
//------------------------------------------------------------------------------
void PropagationConfigPanel::EnablePrimaryBodyItems(bool enable, bool clear)
{
   if (enable)
   {
      theGravModelComboBox->Enable(true);
      gravityDegreeTextCtrl->Enable(true);
      gravityOrderTextCtrl->Enable(true);
      
      if (theGravModelComboBox->GetStringSelection() == "Other")
      {
         theGravModelSearchButton->Enable(true);
         potFileTextCtrl->Enable(true);
      }
      else
      {
         theGravModelSearchButton->Enable(false);
         potFileTextCtrl->Enable(false);
      }
      
      if (thePrimaryBodyComboBox->GetValue() == "Earth")
      {
         theAtmosModelComboBox->Enable(true);
         if (theAtmosModelComboBox->GetValue() == dragModelArray[NONE_DM] ||
             theAtmosModelComboBox->GetValue() == dragModelArray[EXPONENTIAL])
            theDragSetupButton->Enable(false);
         else
            theDragSetupButton->Enable(true);         
      }
      else
      {
         theAtmosModelComboBox->Enable(false);
         theDragSetupButton->Enable(false);
      }
      
      //theMagfModelComboBox->Enable(true);
      //theSrpCheckBox->Enable(true);
   }
   else
   {
      if (clear)
      {
         primaryBodyList.clear();
         primaryBodiesArray.Clear(); 
         thePrimaryBodyComboBox->Clear();
         bodyTextCtrl->Clear();
      }
      
      theGravModelComboBox->Enable(false);
      gravityDegreeTextCtrl->Enable(false);
      gravityOrderTextCtrl->Enable(false);
      potFileTextCtrl->Enable(false);
      theGravModelSearchButton->Enable(false);
      theAtmosModelComboBox->Enable(false);
      theDragSetupButton->Enable(false);
      //theMagfModelComboBox->Enable(false);
      //theSrpCheckBox->Enable(false);
   }
}


//------------------------------------------------------------------------------
// void UpdatePrimaryBodyItems()
//------------------------------------------------------------------------------
void PropagationConfigPanel::UpdatePrimaryBodyItems()
{
   if (primaryBodiesArray.IsEmpty())
      return;
   
   wxString selBody = thePrimaryBodyComboBox->GetStringSelection();
   
   if (currentBodyName != selBody)
   {
      primaryBodyString = thePrimaryBodyComboBox->GetStringSelection();
      currentBodyName = selBody;
      currentBodyId = FindPrimaryBody(currentBodyName);
      
      DisplayGravityFieldData(currentBodyName);
      DisplayAtmosphereModelData();
      DisplayMagneticFieldData();
      DisplaySRPData();
   }
   
//    // Enable atmosphere model only for Earth
//    if (strcmp(selBody, "Earth") == 0)
//    {
//       theAtmosModelComboBox->Enable(true);
//       theDragSetupButton->Enable(true);
//    }
//    else
//    {
//       theAtmosModelComboBox->Enable(false);
//       theDragSetupButton->Enable(false);
//    }
}


//------------------------------------------------------------------------------
// bool SaveIntegratorData()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveIntegratorData()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveIntegratorData() entered\n");
   #endif
   
   Integer maxAttempts;
   Real initStep, accuracy, minStep, maxStep, minError, nomError;
   std::string str;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   str = initialStepSizeTextCtrl->GetValue();      
   CheckReal(initStep, str, "InitialStepSize", "Real Number");
   
   str = accuracyTextCtrl->GetValue();      
   CheckReal(accuracy, str, "Accuracy", "Real Number >= 0.0");
   
   str = minStepTextCtrl->GetValue();            
   CheckReal(minStep, str, "Min Step Size", "Real Number > 0.0, MinStep <= MaxStep");
   
   str = maxStepTextCtrl->GetValue();            
   CheckReal(maxStep, str, "Max Step Size", "Real Number > 0.0, MinStep <= MaxStep");
   
   str = maxStepAttemptTextCtrl->GetValue();            
   CheckInteger(maxAttempts, str, "Max Step Attempts", "Integer Number > 0");
   
   if (integratorString.IsSameAs(integratorArray[ABM]))
   {
      str = minIntErrorTextCtrl->GetValue();            
      CheckReal(minError, str, "Min Integration Error", "Real Number > 0");
      
      str = nomIntErrorTextCtrl->GetValue();            
      CheckReal(nomError, str, "Nominal Integration Error", "Real Number > 0");
   }
   
   if (!canClose)
      return false;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      Integer id;
      
      id = thePropagator->GetParameterID("InitialStepSize");
      thePropagator->SetRealParameter(id, initStep);
      
      id = thePropagator->GetParameterID("Accuracy");
      thePropagator->SetRealParameter(id, accuracy);

      id = thePropagator->GetParameterID("MinStep");
      thePropagator->SetRealParameter(id, minStep);
      
      id = thePropagator->GetParameterID("MaxStep");
      thePropagator->SetRealParameter(id, maxStep);
      
      id = thePropagator->GetParameterID("MaxStepAttempts");
      thePropagator->SetIntegerParameter(id, maxAttempts);
      
      if (integratorString.IsSameAs(integratorArray[ABM]))
      {
         id = thePropagator->GetParameterID("LowerError");
         thePropagator->SetRealParameter(id, minError);
      
         id = thePropagator->GetParameterID("TargetError");
         thePropagator->SetRealParameter(id, nomError);
      }
      
      #ifdef DEBUG_PROP_PANEL_SAVE
      ShowPropData("SaveData() AFTER  saving Integrator");
      #endif
      
      return true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SaveDegOrder()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveDegOrder()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveDegOrder() entered\n");
   #endif

   Integer degree, order;
   std::string str;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   str = gravityDegreeTextCtrl->GetValue();
   CheckInteger(degree, str, "Degree", "Integer Number >= 0"
            "and < the maximum specified by the model, Order <= Degree].");
   
   str = gravityOrderTextCtrl->GetValue();
   CheckInteger(order, str, "Order", "Integer Number >= 0"
            "and < the maximum specified by the model, Order <= Degree].");

   if (!canClose)
      return false;

   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   
   /// @todo ltr: implement < the maximum specified by the model validation 
   try
   {
      #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage("   degree=%d, order=%d\n", degree, order);
      #endif
      
      // check to see if degree is less than order
      if (degree < order)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Degree can not be less than Order.\n"
             "The allowed values are: [Integer >= 0 "
             "and < the maximum specified by the model, "
             "Order <= Degree].");                        
         canClose = false;
         return false;
      }
      
      // save degree and order
      wxString bodyName = thePrimaryBodyComboBox->GetValue();
      
      #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage("   bodyName=%s\n", bodyName.c_str());
      #endif
      
      // find gravity force pointer
      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
      {
         if (primaryBodyList[i]->gravType != "None")
         {
            theGravForce = primaryBodyList[i]->gravf;
            if (theGravForce != NULL && primaryBodyList[i]->bodyName == bodyName)
            {
               theGravForce->SetIntegerParameter("Degree", degree);
               theGravForce->SetIntegerParameter("Order", order);
            }
         }
      }
   
      isDegOrderChanged = false;
      return true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SavePotFile()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SavePotFile()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SavePotFile() entered\n");
   #endif
   
   // save data to core engine
   try
   {

      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                         thePropSetup->GetName() + "\" is not an allowed value.  "
                        "\nThe allowed values are: [ %s ].";

      for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
      {
         if (primaryBodyList[i]->gravType != "None")
         {
            theGravForce = primaryBodyList[i]->gravf;
            if (theGravForce != NULL)
            {
               #ifdef DEBUG_PROP_PANEL_SAVE
                  MessageInterface::ShowMessage
                    ("SavePotFile() Saving Body:%s, potFile=%s\n",
                    primaryBodyList[i]->bodyName.c_str(), primaryBodyList[i]->potFilename.c_str());
               #endif
                  
               inputString = primaryBodyList[i]->potFilename.c_str();
               std::ifstream filename(inputString.c_str());
               
//               // Check if the file doesn't exist then stop
//               if (!filename) 
//               {
//                  MessageInterface::PopupMessage
//                     (Gmat::ERROR_, msg.c_str(), inputString.c_str(),
//                      "Model File", "File must exist");
//                  
//                  return false;
//               }
               
               filename.close();               
               theGravForce->SetStringParameter("PotentialFile",
                                                primaryBodyList[i]->potFilename.c_str());
            }
         }
      }
   
      isPotFileChanged = false;
      return true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return false;
   }
}


//------------------------------------------------------------------------------
// bool SaveAtmosModel()
//------------------------------------------------------------------------------
bool PropagationConfigPanel::SaveAtmosModel()
{
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("PropagationConfigPanel::SaveAtmosModel() entered\n");
   #endif
   
   //-------------------------------------------------------
   // find drag force model
   //-------------------------------------------------------
   Integer paramId;
   bool dragForceFound = false;
   
   wxString bodyName = thePrimaryBodyComboBox->GetValue();
   wxString dragType = theAtmosModelComboBox->GetValue();
   
   #ifdef DEBUG_PROP_PANEL_SAVE
   MessageInterface::ShowMessage
      ("   bodyName=%s, dragType=%s\n", bodyName.c_str(), dragType.c_str());
   #endif
   
   for (Integer i=0; i < (Integer)primaryBodyList.size(); i++)
   {
      if (primaryBodyList[i]->dragType != dragModelArray[NONE_DM])
      {
         theDragForce = primaryBodyList[i]->dragf;
         if (theDragForce != NULL && primaryBodyList[i]->bodyName == bodyName)
         {
            dragForceFound = true;
            break;
         }
      }
   }
   
   if (!dragForceFound)
   {
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::SaveAtmosModel() Drag Force not found "
          "for body:%s\n", bodyName.c_str());
         
      return false;
   }
   
   theCelestialBody = theSolarSystem->GetBody(bodyName.c_str()); 
   theAtmosphereModel = theCelestialBody->GetAtmosphereModel();
   
   if (theAtmosphereModel == NULL)  
   {
      #ifdef DEBUG_PROP_PANEL_SAVE
      MessageInterface::ShowMessage
         ("PropagationConfigPanel::SaveAtmosModel() AtmosphereModel not found "
          "for body:%s\n", bodyName.c_str());
      #endif
   }
   
   //-------------------------------------------------------
   // save drag force model
   //-------------------------------------------------------
   try
   {
      theDragForce->SetInternalAtmosphereModel(theAtmosphereModel);
      paramId = theDragForce->GetParameterID("AtmosphereModel");
      theDragForce->SetStringParameter(paramId, dragType.c_str());
      theDragForce->SetStringParameter("BodyName", bodyName.c_str());
      
      isAtmosChanged = false;
      canClose = true;
      return true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
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
// void OnIntegratorComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnIntegratorComboBox(wxCommandEvent &event)
{
   if (!integratorString.IsSameAs(theIntegratorComboBox->GetStringSelection()))
   {
      isIntegratorChanged = true;
      integratorString = theIntegratorComboBox->GetStringSelection();
      DisplayIntegratorData(true);
      isIntegratorDataChanged = false;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnPrimaryBodyComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnPrimaryBodyComboBox(wxCommandEvent &event)
{
   UpdatePrimaryBodyItems();
}


//------------------------------------------------------------------------------
// void OnOriginComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnOriginComboBox(wxCommandEvent &event)
{
   propOriginName = theOriginComboBox->GetValue();
   
   isOriginChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnGravityModelComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnGravityModelComboBox(wxCommandEvent &event)
{   
   if (primaryBodiesArray.IsEmpty())
      return;
   
   gravTypeName = theGravModelComboBox->GetStringSelection();
   
   if (primaryBodyList[currentBodyId]->gravType != gravTypeName)
   {
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("OnGravityModelComboBox() grav changed from=%s to=%s for body=%s\n",
          primaryBodyList[currentBodyId]->gravType.c_str(), gravTypeName.c_str(),
          primaryBodyList[currentBodyId]->bodyName.c_str());
      #endif
      
      primaryBodyList[currentBodyId]->gravType = gravTypeName;
      
      if (gravTypeName != "None" && gravTypeName != "Other")
      {
         std::string fileType = theFileMap[gravTypeName].c_str();
         //MessageInterface::ShowMessage("===> Found %s\n", fileType.c_str());
         
         try
         {
            primaryBodyList[currentBodyId]->potFilename =
               theGuiInterpreter->GetFileName(fileType).c_str();
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, e.GetFullMessage() +
                "\nPlease select Other and specify file name\n");
         }
      }
      else if (gravTypeName == "Other")
      {
         primaryBodyList[currentBodyId]->potFilename = potFileTextCtrl->GetValue();
      }
      
      #ifdef DEBUG_PROP_PANEL_GRAV
      MessageInterface::ShowMessage
         ("OnGravityModelComboBox() bodyName=%s, potFile=%s\n",
          primaryBodyList[currentBodyId]->bodyName.c_str(),
          primaryBodyList[currentBodyId]->potFilename.c_str());
      #endif
      
      DisplayGravityFieldData(primaryBodyList[currentBodyId]->bodyName);
      
      isPotFileChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnAtmosphereModelComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnAtmosphereModelComboBox(wxCommandEvent &event)
{
   if (primaryBodiesArray.IsEmpty())
      return;
   
   #ifdef DEBUG_PROP_PANEL_ATMOS
   MessageInterface::ShowMessage("OnAtmosphereModelComboBox() body=%s\n",
                                 primaryBodyList[currentBodyId]->bodyName.c_str());
   #endif
   
   dragTypeName = theAtmosModelComboBox->GetStringSelection();
   
   // if we are creating new DragForce, set isForceModelChanged
   if (primaryBodyList[currentBodyId]->dragf == NULL)
      isForceModelChanged = true;
   
   if (primaryBodyList[currentBodyId]->dragType != dragTypeName)
   {    
      #ifdef DEBUG_PROP_PANEL_ATMOS
      MessageInterface::ShowMessage
         ("OnAtmosphereModelComboBox() drag changed from=%s to=%s for body=%s\n",
          primaryBodyList[currentBodyId]->dragType.c_str(), dragTypeName.c_str(),
          primaryBodyList[currentBodyId]->bodyName.c_str());
      #endif
      
      primaryBodyList[currentBodyId]->dragType = dragTypeName;
      DisplayAtmosphereModelData();
      
      isForceModelChanged = true;  
      isAtmosChanged = true;
      EnableUpdate(true);
   }
}


//------------------------------------------------------------------------------
// void OnErrorControlComboBox(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnErrorControlComboBox(wxCommandEvent &event)
{     
   #ifdef DEBUG_PROP_PANEL_ERROR
   MessageInterface::ShowMessage("OnErrorControlComboBox()\n");
   #endif
   
   wxString eType = theErrorComboBox->GetStringSelection();
   
   if (errorControlTypeName != eType)
   {    
      #ifdef DEBUG_PROP_PANEL_ERROR
      MessageInterface::ShowMessage
         ("OnErrorControlComboBox() error control changed from=%s to=%s\n",
          errorControlTypeName.c_str(), eType.c_str());
      #endif
      
      errorControlTypeName = eType;
      DisplayErrorControlData();
      
      // We don't want to create a new ForceModel if only Error Control is changed
      //isForceModelChanged = true;
      isErrControlChanged = true;
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
         EnablePrimaryBodyItems(false, true);
         
         EnableUpdate(true);
         isForceModelChanged = true;
         return;
      }
      else
      {
         EnablePrimaryBodyItems(true);         
      }
      
      std::vector<ForceType*> oldList;
      wxString bodyName;
      
      oldList = primaryBodyList;
      primaryBodyList.clear();
      primaryBodiesArray.Clear();
      
      for (Integer i = 0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i];
         primaryBodiesArray.Add(bodyName.c_str()); 
         
         // Set default gravity model file for display
         if (bodyName == "Earth")
            currentBodyId = FindPrimaryBody(bodyName, earthGravModelArray[JGM2]);
         else if (bodyName == "Luna")
            currentBodyId = FindPrimaryBody(bodyName, lunaGravModelArray[LP165]);
         else if (bodyName == "Venus")
            currentBodyId = FindPrimaryBody(bodyName, venusGravModelArray[MGNP180U]);
         else if (bodyName == "Mars")
            currentBodyId = FindPrimaryBody(bodyName, marsGravModelArray[MARS50C]);
         else
            currentBodyId = FindPrimaryBody(bodyName,othersGravModelArray[O_NONE_GM]);
         
         
         // Copy old body force model
         for (Integer j = 0; j < (Integer)oldList.size(); j++)
            if (bodyName == oldList[j]->bodyName)
               primaryBodyList[currentBodyId] = oldList[j];
      }
   }
   
   //----------------------------------------------
   // Append body names to combobox and text field
   //----------------------------------------------
   thePrimaryBodyComboBox->Clear();
   bodyTextCtrl->Clear();
   wxString name;
   for (Integer i = 0; i < (Integer)primaryBodyList.size(); i++)
   {
      name = primaryBodyList[i]->bodyName.c_str();
      thePrimaryBodyComboBox->Append(primaryBodyList[i]->bodyName.c_str());
      thePrimaryBodyComboBox->SetValue(primaryBodyList[i]->bodyName.c_str());
   }
   
   UpdatePrimaryBodyItems();
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
      wxString upperCaseFilename = filename;
      
      // Make filename upper case for comparison
      upperCaseFilename.MakeUpper();
      
      // Determine the type of file
      if (upperCaseFilename.find(".DAT",0) != std::string::npos)
      {
         ParseDATGravityFile(filename);
      }
      else if (upperCaseFilename.find(".GRV",0) != std::string::npos)
      {
         ParseGRVGravityFile(filename);
      }
      else if (upperCaseFilename.find(".COF",0) != std::string::npos)
      {
         ParseCOFGravityFile(filename);  
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Gravity file \"" + filename + "\" is of unknown format.");
         return;
      }
      
      primaryBodyList[currentBodyId]->potFilename = filename;
      
      if (primaryBodyList[currentBodyId]->bodyName == "Earth")
         primaryBodyList[currentBodyId]->gravType = earthGravModelArray[E_OTHER];
      else if (primaryBodyList[currentBodyId]->bodyName == "Luna")
         primaryBodyList[currentBodyId]->gravType = lunaGravModelArray[L_OTHER];
      else if (primaryBodyList[currentBodyId]->bodyName == "Mars")
         primaryBodyList[currentBodyId]->gravType = marsGravModelArray[M_OTHER];
      else //other bodies 
         primaryBodyList[currentBodyId]->gravType = othersGravModelArray[O_OTHER];
      
      //loj: Do we need to show? body name didn't change
      //waw: Yes, we need to update the degree & order displays (10/17/06)
      DisplayGravityFieldData(primaryBodyList[currentBodyId]->bodyName);
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
   if (primaryBodyList[currentBodyId]->dragf == NULL)
   {
      isForceModelChanged = true;
      SaveData();
   }
   else if (isAtmosChanged)
   {
      SaveAtmosModel();
   }
   
   dragForce = primaryBodyList[currentBodyId]->dragf;
   if (dragForce != NULL)
   {      
      if (primaryBodyList[currentBodyId]->dragType == dragModelArray[EXPONENTIAL])
      {
         // TBD by Code 595
         //DragInputsDialog dragDlg(this, dragForce, "ExponentialDragDialog");
         //dragDlg.ShowModal();
      }
      else if (primaryBodyList[currentBodyId]->dragType == dragModelArray[MSISE90])
      {
         DragInputsDialog dragDlg(this, dragForce, "MSISE90DragDialog");
         dragDlg.ShowModal();
      }
      else if (primaryBodyList[currentBodyId]->dragType == dragModelArray[JR])
      {
         DragInputsDialog dragDlg(this, dragForce, "JacchiaRobertsDialog");
         dragDlg.ShowModal();
      }
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
      theMagfModelComboBox->Append(filename); 
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
         pointMassBodyList.clear();
         secondaryBodiesArray.Clear(); 
         pmEditTextCtrl->Clear();
         EnableUpdate(true);
         isForceModelChanged = true;
         return;
      }
      
      wxString bodyName;

      pointMassBodyList.clear();
      secondaryBodiesArray.Clear(); 
      pmEditTextCtrl->Clear();
      
      //--------------------------
      // Add bodies to pointMassBodyList
      //--------------------------     
      for (Integer i=0; i < (Integer)names.GetCount(); i++)
      {
         bodyName = names[i];
         pointMassBodyList.push_back(new ForceType(bodyName));
         
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
      primaryBodyList[currentBodyId]->gravDegree = gravityDegreeTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == gravityOrderTextCtrl)
   {     
      primaryBodyList[currentBodyId]->gravOrder = gravityOrderTextCtrl->GetValue();
      isDegOrderChanged = true;
      // Do not set to true if only text changed
      //isForceModelChanged = true;
   }
   else if (event.GetEventObject() == potFileTextCtrl)
   {
      primaryBodyList[currentBodyId]->potFilename = potFileTextCtrl->GetValue();
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


//------------------------------------------------------------------------------
// void OnSRPCheckBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
void PropagationConfigPanel::OnSRPCheckBoxChange(wxCommandEvent &event)
{
   usePropOriginForSrp = theSrpCheckBox->GetValue();
   isForceModelChanged = true;
   EnableUpdate(true);
   
   // Since SRP can be applied to any force model central body,
   // we don't need to check for primary bodies (loj:2007.10.19)
   //if (FindPrimaryBody(currentBodyName, false) != -1)
   //{
   //   primaryBodyList[currentBodyId]->useSrp = theSrpCheckBox->GetValue();
   //   isForceModelChanged = true;
   //   EnableUpdate(true);
   //}
   //else
   //{      
   //   MessageInterface::PopupMessage
   //      (Gmat::WARNING_, "Solar radiation pressure force can be only applied to"
   //       " primary bodies.");
   //   theSrpCheckBox->SetValue(false);
   //}
}


//------------------------------------------------------------------------------
// void ShowPropData()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowPropData(const std::string& header)
{
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   MessageInterface::ShowMessage("   thePropSetup =%p, name=%s\n",
                                 thePropSetup, thePropSetup->GetName().c_str());
   MessageInterface::ShowMessage("   thePropagator=%p, name=%s\n", thePropagator,
                                 thePropagator->GetTypeName().c_str());
   MessageInterface::ShowMessage("   theForceModel=%p, name=%s\n", theForceModel,
                                 theForceModel->GetName().c_str());
   MessageInterface::ShowMessage("   numOfForces=%d\n", numOfForces);
   
   std::string forceType;
   std::string forceBody;
   PhysicalModel *force;
   
   for (int i=0; i<numOfForces; i++)
   {
      force = theForceModel->GetForce(i);
      forceType = force->GetTypeName();
      
      forceBody = force->GetStringParameter("BodyName");
      
      MessageInterface::ShowMessage("   forceBody=%s, forceType=%s\n", forceBody.c_str(),
                                    forceType.c_str());
   }
   MessageInterface::ShowMessage("============================================\n");
}


//------------------------------------------------------------------------------
// void ShowForceList()
//------------------------------------------------------------------------------
void PropagationConfigPanel::ShowForceList(const std::string &header)
{
   MessageInterface::ShowMessage(">>>>>=======================================\n");
   MessageInterface::ShowMessage("%s\n", header.c_str());
   MessageInterface::ShowMessage("   theForceModel=%p\n", theForceModel);
   
   // srp force
   MessageInterface::ShowMessage
      ("   usePropOriginForSrp=%d\n", usePropOriginForSrp);
   
   // primary body list
   MessageInterface::ShowMessage("   primaryBodyListSize=%d\n", primaryBodyList.size());
   for (unsigned int i=0; i<primaryBodyList.size(); i++)
   {
      MessageInterface::ShowMessage
         ("   id=%d, body=%s, gravType=%s, dragType=%s, magfType=%s\n   potFile=%s\n"
          "   gravf=%p, dragf=%p, srpf=%p\n", i, primaryBodyList[i]->bodyName.c_str(),
          primaryBodyList[i]->gravType.c_str(), primaryBodyList[i]->dragType.c_str(),
          primaryBodyList[i]->magfType.c_str(), primaryBodyList[i]->potFilename.c_str(),
          primaryBodyList[i]->gravf, primaryBodyList[i]->dragf,
          primaryBodyList[i]->srpf);
   }
   
   // pointmass body list
   MessageInterface::ShowMessage("   pointMassBodyListSize=%d\n", pointMassBodyList.size());
   for (Integer i=0; i < (Integer)pointMassBodyList.size(); i++)
   {
      MessageInterface::ShowMessage
         ("   id=%d, body=%s, pmf=%p\n", i, pointMassBodyList[i]->bodyName.c_str(),
          pointMassBodyList[i]->pmf);
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
   primaryBodyList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
   primaryBodyList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
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
         std::string upperString = GmatStringUtil::ToUpper(firstStr);
         //VC++ error C3861: 'strcasecmp': identifier not found
         // since using std::string, use GmatStringUtil and ==
         //if (strcmpi(firstStr.c_str(),"Degree") == 0)
         if (upperString == "DEGREE")
            lineStr >> fileDegree;
         //else if (strcmpi(firstStr.c_str(),"Order") == 0)         
         else if (upperString == "ORDER")
            lineStr >> fileOrder;
      }
   }
   
   // Save as string
   primaryBodyList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
   primaryBodyList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
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
   primaryBodyList[currentBodyId]->gravDegree.Printf("%d", fileDegree);
   primaryBodyList[currentBodyId]->gravOrder.Printf("%d", fileOrder);
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
