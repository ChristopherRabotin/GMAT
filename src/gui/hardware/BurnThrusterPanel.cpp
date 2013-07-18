//$Id$
//------------------------------------------------------------------------------
//                            BurnThrusterPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009.02.11
/**
 * This class sets up Thruster or ImpulsiveBurn parameters.
 */
//------------------------------------------------------------------------------
#include "BurnThrusterPanel.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "ThrusterCoefficientDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/variant.h>
#include <wx/config.h>

//#define DEBUG_BURNPANEL_CREATE
//#define DEBUG_BURNPANEL_LOAD
//#define DEBUG_BURNPANEL_SAVE
//#define DEBUG_BURNPANEL_SAVE_COEFS

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(BurnThrusterPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, BurnThrusterPanel::OnTextChange)
   EVT_TEXT(ID_COMBOBOX, BurnThrusterPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, BurnThrusterPanel::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, BurnThrusterPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, BurnThrusterPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// BurnThrusterPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs BurnThrusterPanel object.
 */
//------------------------------------------------------------------------------
BurnThrusterPanel::BurnThrusterPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true, true)
{
   mObjectName = name.c_str();
   theObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage
      ("BurnThrusterPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif
   
   isCoordSysChanged = false;
   isTankChanged     = false;
   isTankEmpty       = false;
   coordSysName      = "";
   tankName          = "";

   // thruster only
   areCCoefsChanged  = false;
   areKCoefsChanged  = false;
   cCoefs.clear();
   kCoefs.clear();
   cCoefNames.clear();
   kCoefNames.clear();
}


//------------------------------------------------------------------------------
// ~Burnthrusterpanel()
//------------------------------------------------------------------------------
BurnThrusterPanel::~BurnThrusterPanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", coordSysComboBox);
   theGuiManager->UnregisterComboBox("CelestialBody", originComboBox);
   theGuiManager->UnregisterComboBox("FuelTank", tankComboBox);

   if (localObject != NULL)
	 delete localObject;
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
void BurnThrusterPanel::Create()
{
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("BurnThrusterPanel::Create() entered\n");
   #endif

   localObject = NULL;
   
   Integer bsize = 2; // border size
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Burn Thruster"));
   
   //-----------------------------------------------------------------
   // coordinate system items
   //-----------------------------------------------------------------
   // Coordinate Systems 
   wxStaticText *coordSysLabel =
      new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Coordinate System"));
   coordSysComboBox  =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   coordSysComboBox->SetToolTip(pConfig->Read(_T("CoordinateSystemHint")));
   
   // Addd Local to CoordinateSystem list
   coordSysComboBox->Insert("Local", 0);
   
   // Origin
   originLabel = new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Origin"));
   originComboBox =
      theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX,
                                              wxSize(150,-1));
   originComboBox->SetToolTip(pConfig->Read(_T("OriginHint")));
   
   // Axes 
   StringArray axesLabels = theObject->GetPropertyEnumStrings("Axes");
   wxArrayString wxAxesLabels = ToWxArrayString(axesLabels);
   
   axisLabel = new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Axes"));
   
   axesComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
                     wxSize(150,-1), wxAxesLabels, wxCB_DROPDOWN|wxCB_READONLY);
   axesComboBox->SetSelection(0);
   axesComboBox->SetToolTip(pConfig->Read(_T("AxesHint")));
   
   //----- Add to sizer
   wxFlexGridSizer *coordSysSizer = new wxFlexGridSizer(3, 0, 0);
   coordSysSizer->Add(coordSysLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   coordSysSizer->Add(coordSysComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   coordSysSizer->Add(20,20);
   
   coordSysSizer->Add(originLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   coordSysSizer->Add(originComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   coordSysSizer->Add(20,20);
   
   coordSysSizer->Add(axisLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   coordSysSizer->Add(axesComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   coordSysSizer->Add(20,20);
   
   GmatStaticBoxSizer *coordSysBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Coordinate System");
   coordSysBoxSizer->Add(coordSysSizer, 0, wxALIGN_CENTER|wxALL, 0);
   
   //-----------------------------------------------------------------
   // thrust vector items
   //-----------------------------------------------------------------
   
   // ThrustDirection1
   XUnitLabel = new wxStaticText(this, ID_TEXT, wxT(""));
   XLabel = new wxStaticText(this, ID_TEXT, wxT("ThrustDirection"GUI_ACCEL_KEY"1"));
   elem1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   elem1TextCtrl->SetToolTip(pConfig->Read(_T("ThrustDirection1Hint")));
   
   // ThrustDirection2
   YUnitLabel =
      new wxStaticText(this, ID_TEXT, wxT(""));
   YLabel =
      new wxStaticText(this, ID_TEXT, wxT("ThrustDirection"GUI_ACCEL_KEY"2"),
                        wxDefaultPosition,wxDefaultSize, 0);
   elem2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                      wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   elem2TextCtrl->SetToolTip(pConfig->Read(_T("ThrustDirection2Hint")));
   
   // ThrustDirection3
   ZUnitLabel = new wxStaticText(this, ID_TEXT, wxT(""));
   ZLabel = new wxStaticText(this, ID_TEXT, wxT("ThrustDirection"GUI_ACCEL_KEY"3"));
   elem3TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   elem3TextCtrl->SetToolTip(pConfig->Read(_T("ThrustDirection3Hint")));
   
   wxStaticText *dutyCycleLabel = NULL;
   wxStaticText *scaleFactorLabel = NULL;
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      // Thruster Duty Cycle
      dutyCycleLabel =
         new wxStaticText(this, ID_TEXT, wxT("Duty "GUI_ACCEL_KEY"Cycle"));
      dutyCycleTextCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                        wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      dutyCycleTextCtrl->SetToolTip(pConfig->Read(_T("DutyCycleHint")));
      
      // Thruster Scale Factor
      scaleFactorLabel =
         new wxStaticText(this, ID_TEXT, wxT("Thrust "GUI_ACCEL_KEY"Scale Factor"));
      scaleFactorTextCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                        wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      scaleFactorTextCtrl->SetToolTip(pConfig->Read(_T("ThrustScaleFactorHint")));
   }
   
   //----- Add to sizer   
   wxFlexGridSizer *vectorSizer = new wxFlexGridSizer(3, 0, 0);
   vectorSizer->Add(XLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   vectorSizer->Add(elem1TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   vectorSizer->Add(XUnitLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   vectorSizer->Add(YLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   vectorSizer->Add(elem2TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   vectorSizer->Add(YUnitLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   vectorSizer->Add(ZLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   vectorSizer->Add(elem3TextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   vectorSizer->Add(ZUnitLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      vectorSizer->Add(dutyCycleLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      vectorSizer->Add(dutyCycleTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
      vectorSizer->Add(20,20);
      
      vectorSizer->Add(scaleFactorLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      vectorSizer->Add(scaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
      vectorSizer->Add(20,20);
   }
   
   vectorBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Thrust Vector");
   vectorBoxSizer->Add(vectorSizer, 0, wxALIGN_CENTER|wxALL, 0);
   
   //-----------------------------------------------------------------
   // mass change items
   //-----------------------------------------------------------------
   // Decrement mass
   decMassCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT(GUI_ACCEL_KEY"Decrement Mass"),
                     wxDefaultPosition, wxSize(-1, -1), bsize);
   decMassCheckBox->SetToolTip(pConfig->Read(_T("DecrementMassHint")));
   
   //Tank
   tankLabel =
      new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Tank"));
   tankComboBox =
      theGuiManager->GetFuelTankComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   tankComboBox->SetToolTip(pConfig->Read(_T("TankHint")));
   
   ispLabel = NULL;
   ispTextCtrl = NULL;
   ispUnit = NULL;
   // Isp for ImpulsiveBurn only
   if (theObject->IsOfType(Gmat::IMPULSIVE_BURN))
   {
      ispLabel =
         new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"Isp"));
      ispTextCtrl =
         new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                        wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      ispTextCtrl->SetToolTip(pConfig->Read(_T("IspHint")));
      ispUnit =
         new wxStaticText(this, ID_TEXT, wxT(" s"));
   }
   
   // Gravitational Acceleration
   gravityAccelLabel =
      new wxStaticText(this, ID_TEXT, wxT(GUI_ACCEL_KEY"GravitationalAccel"));
   gravityAccelTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), 
                     wxDefaultPosition, wxSize(150,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   gravityAccelTextCtrl->SetToolTip(pConfig->Read(_T("GravitationalAccelHint")));
   gravityAccelUnit =
      new wxStaticText(this, ID_TEXT, wxT(" m/s^2"));
   
   // Coefficients for Thruster only
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      cCoefButton = new wxButton(this, ID_BUTTON, wxT("Edit "GUI_ACCEL_KEY"Thruster Coef."));
      cCoefButton->SetToolTip(pConfig->Read(_T("EditThrusterCoefficientHint")));
      kCoefButton = new wxButton(this, ID_BUTTON, wxT("Edit "GUI_ACCEL_KEY"Impulse Coef."));
      kCoefButton->SetToolTip(pConfig->Read(_T("EditImpulseCoefficientHint")));
   }
   
   wxBoxSizer *coefSizer = new wxBoxSizer(wxHORIZONTAL);
   if (theObject->IsOfType(Gmat::THRUSTER))
   {
      coefSizer->Add(cCoefButton, 0, wxALIGN_CENTER|wxALL, 5);
      coefSizer->Add(kCoefButton, 0, wxALIGN_CENTER|wxALL, 5);
   }
   
   //----- Add to sizer   
   wxFlexGridSizer *massSizer = new wxFlexGridSizer(3, 0, 0);
   massSizer->Add(decMassCheckBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   massSizer->Add(20,20);
   massSizer->Add(20,20);
   
   massSizer->Add(tankLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   massSizer->Add(tankComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   massSizer->Add(20,20);
   
   if (theObject->IsOfType(Gmat::IMPULSIVE_BURN))
   {
      massSizer->Add(ispLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
      massSizer->Add(ispTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
      massSizer->Add(ispUnit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   }
   
   massSizer->Add(gravityAccelLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   massSizer->Add(gravityAccelTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   massSizer->Add(gravityAccelUnit, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   
   if (theObject->IsOfType(Gmat::THRUSTER))
   {      
      massSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
      massSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
      massSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   }
   
   GmatStaticBoxSizer *massBoxSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Mass Change");
   massBoxSizer->Add(massSizer, 0, wxALIGN_CENTER|wxALL, 0);
   
   //-----------------------------------------------------------------
   // add to page sizer
   //-----------------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(coordSysBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   pageSizer->Add(vectorBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   pageSizer->Add(massBoxSizer, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   
   if (theObject->IsOfType(Gmat::THRUSTER))
      pageSizer->Add(coefSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(pageSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("BurnThrusterPanel::Create() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void BurnThrusterPanel::LoadData()
{
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("BurnThrusterPanel::LoadData() entered\n");
   #endif
   
   // Set object pointer for "Show Script"
   mObject = theObject;
   bool isImpBurn = false;
   if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
      isImpBurn = true;
   
   if (isImpBurn)
   {
      thrustDir1 = "Element1";
      thrustDir2 = "Element2";
      thrustDir3 = "Element3";
   }
   else
   {
      thrustDir1 = "ThrustDirection1";
      thrustDir2 = "ThrustDirection2";
      thrustDir3 = "ThrustDirection3";
   }
   
   Integer paramID;
   
   try
   {
      paramID = theObject->GetParameterID("CoordinateSystem");
      coordSysName = theObject->GetStringParameter(paramID);
      coordSysComboBox->SetValue(coordSysName.c_str());
      
      paramID = theObject->GetParameterID("Origin");
      std::string objName = theObject->GetStringParameter(paramID);
      originComboBox->SetValue(objName.c_str());
      
      paramID = theObject->GetParameterID("Axes");
      objName = theObject->GetStringParameter(paramID);
      axesComboBox->SetValue(objName.c_str());
      
      paramID = theObject->GetParameterID(thrustDir1);
      elem1TextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID(thrustDir2);
      elem2TextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID(thrustDir3);
      elem3TextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID("DecrementMass");
      decMassCheckBox->SetValue((wxVariant(theObject->GetBooleanParameter(paramID))));
      
      paramID = theObject->GetParameterID("GravitationalAccel");
      gravityAccelTextCtrl->SetValue((wxVariant(theObject->GetRealParameter(paramID))));
      
      paramID = theObject->GetParameterID("Tank");
      StringArray tanks = theObject->GetStringArrayParameter(paramID);   
      
      if (tanks.empty())
      {
         if (theGuiManager->GetNumFuelTank() > 0)
         {
            tankComboBox->Insert("No Fuel Tank Selected", 0);
            tankComboBox->SetSelection(0);
         }
      }
      else
      {
         tankName = tanks[0];
         tankComboBox->SetValue(tankName.c_str());
         isTankEmpty = false;
      }
      
      // Disable tank combo box if decrement mass is not checked
      if (!decMassCheckBox->IsChecked())
      {
         // Tanks needed to apply nontrivial coefficients, so don't disable
         //tankLabel->Disable();
         //tankComboBox->Disable();

         // g is only used to decrement mass
         gravityAccelLabel->Disable();
         gravityAccelTextCtrl->Disable();
         gravityAccelUnit->Disable();

         if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
         {
            ispLabel->Disable();
            ispTextCtrl->Disable();
            ispUnit->Disable();
         }
      }
      else
      {
         // g is required to decrement mass
         gravityAccelLabel->Enable();
         gravityAccelTextCtrl->Enable();
         gravityAccelUnit->Enable();
      }

      if (theObject->IsOfType(Gmat::THRUSTER))
      {
         // Get the initial values for the coefficients
         Integer cParamID  = 0;
         Integer kParamID  = 0;
         Integer coefCount = Thruster::COEFFICIENT_COUNT;
         std::stringstream cStrings("");
         std::stringstream kStrings("");
         Real cVal, kVal;
         for (Integer ii = 0; ii < coefCount; ii++)
         {
            cStrings << "C" << ii + 1;
            cParamID = theObject->GetParameterID(cStrings.str());
            cVal     = theObject->GetRealParameter(cParamID);
            #ifdef DEBUG_BURNPANEL_LOAD
               MessageInterface::ShowMessage("Loading: %s =  %lf\n", cStrings.str().c_str(), cVal);
            #endif
            cCoefs.push_back(cVal);
            cCoefNames.push_back(cStrings.str());
            cStrings.str("");
         }
         for (Integer ii = 0; ii < coefCount; ii++)
         {
            kStrings << "K" << ii + 1;
            kParamID = theObject->GetParameterID(kStrings.str());
            kVal     = theObject->GetRealParameter(kParamID);
            #ifdef DEBUG_BURNPANEL_LOAD
               MessageInterface::ShowMessage("Loading: %s =  %lf\n", kStrings.str().c_str(), kVal);
            #endif
            kCoefs.push_back(kVal);
            kCoefNames.push_back(kStrings.str());
            kStrings.str("");
         }
      }

      // Update Origin and Axes
      UpdateOriginAxes();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("BurnThrusterPanel::LoadData() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void BurnThrusterPanel::SaveData()
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() entered\n");
   #endif
   // create local copy of mObject
   if (localObject != NULL)
   {
	   delete localObject;
   }
   localObject = mObject->Clone();
   SaveData(localObject);

   // if no errors, save again
   if (canClose)
	   theObject->Copy(localObject);

   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() exiting\n");
   #endif
}

   
//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void BurnThrusterPanel::SaveData(GmatBase *theObject)
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
   Real elem1, elem2, elem3, gravityAccel;
   bool realDataChanged = false;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (elem1TextCtrl->IsModified() || elem2TextCtrl->IsModified() ||
       elem3TextCtrl->IsModified() || gravityAccelTextCtrl->IsModified())
   {
      str = elem1TextCtrl->GetValue();
      CheckReal(elem1, str, thrustDir1, "Real Number");
      
      str = elem2TextCtrl->GetValue();
      CheckReal(elem2, str, thrustDir2, "Real Number");
      
      str = elem3TextCtrl->GetValue();
      CheckReal(elem3, str, thrustDir3, "Real Number");
      
      str = gravityAccelTextCtrl->GetValue();
      CheckReal(gravityAccel, str, "GravitationalAccel", "Real Number > 0", false,
                true, true);
      
      realDataChanged = true;      
   }
   
   if (!canClose)
      return;
   
   try 
   {
      Integer paramID;
      
      // Coordinate System      
      if (isCoordSysChanged)
      {
         paramID = theObject->GetParameterID("CoordinateSystem");
         theObject->SetStringParameter(paramID, coordSysName);
         isCoordSysChanged = false;
      }
      
      if (coordSysName == "Local")
      {
         // Origin
         paramID = theObject->GetParameterID("Origin");
         theObject->SetStringParameter(paramID, originComboBox->GetValue().c_str());
         
         // Axes
         paramID = theObject->GetParameterID("Axes");
         theObject->SetStringParameter(paramID, axesComboBox->GetValue().c_str());

         std::string axisValue = axesComboBox->GetValue().c_str();
         if ((axisValue == "MJ2000Eq") || (axisValue == "SpacecraftBody"))
         {
            originLabel->Disable();
            originComboBox->Disable();
         }
         else
         {
            originLabel->Enable();
            originComboBox->Enable();
         }
      }
      
      // Save ThrustDirections
      if (realDataChanged)
      {
         paramID = theObject->GetParameterID(thrustDir1);
         theObject->SetRealParameter(paramID, elem1);
         
         paramID = theObject->GetParameterID(thrustDir2);
         theObject->SetRealParameter(paramID, elem2);
         
         paramID = theObject->GetParameterID(thrustDir3);
         theObject->SetRealParameter(paramID, elem3);
         
         paramID = theObject->GetParameterID("GravitationalAccel");
         theObject->SetRealParameter(paramID, gravityAccel);
      }
      
      // Always save DecrementMass
      // @todo If some base code computation involved, have separate flag (LOJ)
      paramID = theObject->GetParameterID("DecrementMass");
      if (decMassCheckBox->IsChecked())
         theObject->SetBooleanParameter(paramID, true);
      else
         theObject->SetBooleanParameter(paramID, false);
      
      // Save Tank
      if (isTankChanged)
      {
         isTankChanged = false;
         paramID = theObject->GetParameterID("Tank");
         
         if (theObject->TakeAction("ClearTanks", ""))
            if (tankName != "")
               theObject->SetStringParameter(paramID, tankName.c_str());
      }

      if (theObject->IsOfType(Gmat::THRUSTER))
      {
         // Save C Coefficients
         if (areCCoefsChanged)
         {
            unsigned int coefSize = cCoefs.size();
            for (unsigned int i = 0; i < coefSize; i++)
            {
               #ifdef DEBUG_BURNPANEL_SAVE_COEFS
                  MessageInterface::ShowMessage("Saving %s with value %lf\n", cCoefNames[i].c_str(), cCoefs[i]);
               #endif
               paramID = theObject->GetParameterID(cCoefNames[i]);
               theObject->SetRealParameter(paramID, cCoefs[i]);
            }
         }

         // Save K Coefficients
         if (areKCoefsChanged)
         {
            unsigned int coefSize = kCoefs.size();
            for (unsigned int i = 0; i < coefSize; i++)
            {
               #ifdef DEBUG_BURNPANEL_SAVE_COEFS
                  MessageInterface::ShowMessage("Saving %s with value %lf\n", kCoefNames[i].c_str(), kCoefs[i]);
               #endif
               paramID = theObject->GetParameterID(kCoefNames[i]);
               theObject->SetRealParameter(paramID, kCoefs[i]);
            }
         }
      }
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("BurnThrusterPanel::SaveData() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnTextChange(wxCommandEvent &event)
{
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   if (decMassCheckBox->IsChecked())
   {
      // Disabling disabled so no need to enable
      //tankLabel->Enable();
      //tankComboBox->Enable();

      // g is only used to decrement mass
      gravityAccelLabel->Enable();
      gravityAccelTextCtrl->Enable();
      gravityAccelUnit->Enable();

      if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
      {
         ispLabel->Enable();
         ispTextCtrl->Enable();
         ispUnit->Enable();
      }
   }
   else
   {
      // Tanks needed to apply nontrivial coefficients, so don't disable
      //tankLabel->Disable();
      //tankComboBox->Disable();

      // g is only used to decrement mass
      gravityAccelLabel->Disable();
      gravityAccelTextCtrl->Disable();
      gravityAccelUnit->Disable();

      if (theObject->GetType() == Gmat::IMPULSIVE_BURN)
      {
         ispLabel->Disable();
         ispTextCtrl->Disable();
         ispUnit->Disable();
      }
   }
   
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnComboBoxChange(wxCommandEvent &event)
{
   if (event.GetEventObject() == coordSysComboBox)
   {
      UpdateOriginAxes();      
      isCoordSysChanged =  true;
      coordSysName = coordSysComboBox->GetStringSelection().c_str();
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == tankComboBox)
   {
      isTankChanged = true;
      tankName = tankComboBox->GetStringSelection().c_str();
      if (tankName == "No Fuel Tank Selected")
         tankName = "";
      
      // remove "No Tank Selected" once tank is selected
      int pos = tankComboBox->FindString("No Fuel Tank Selected");
      if (pos != wxNOT_FOUND)
         tankComboBox->Delete(pos);
      
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == axesComboBox)
   {
      std::string csName = coordSysComboBox->GetStringSelection().c_str();

      if (csName == "Local")
      {
         std::string axisValue = axesComboBox->GetValue().c_str();
         if ((axisValue == "MJ2000Eq") || (axisValue == "SpacecraftBody"))
         {
            originLabel->Disable();
            originComboBox->Disable();
         }
         else
         {
            originLabel->Enable();
            originComboBox->Enable();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void BurnThrusterPanel::OnButtonClick(wxCommandEvent &event)
{  
   bool isModified = false;
   if (event.GetEventObject() == cCoefButton)
   {
      ThrusterCoefficientDialog tcDlg(this, -1, "ThrusterCoefficientDialog", theObject, "C", cCoefs);
      tcDlg.ShowModal();
      isModified = tcDlg.AreCoefsSaved();
      if (isModified)
      {
         cCoefs.clear();
         cCoefs      = tcDlg.GetCoefValues();

         EnableUpdate(true);
      }
      areCCoefsChanged = areCCoefsChanged || isModified;
   }
   else if (event.GetEventObject() == kCoefButton)
   {
      ThrusterCoefficientDialog tcDlg(this, -1, "ImpulseCoefficientDialog", theObject, "K", kCoefs);
      tcDlg.ShowModal();
      isModified = tcDlg.AreCoefsSaved();
      if (isModified)
      {
         kCoefs.clear();
         kCoefs      = tcDlg.GetCoefValues();

         EnableUpdate(true);
      }
      areKCoefsChanged = areKCoefsChanged || isModified;
   }
}


//------------------------------------------------------------------------------
// void UpdateOriginAxes()
//------------------------------------------------------------------------------
void BurnThrusterPanel::UpdateOriginAxes()
{
   if (coordSysComboBox->GetValue() == "Local")
   {
      axisLabel->Enable();
      axesComboBox->Enable();

      std::string axisValue = axesComboBox->GetValue().c_str();

      if ((axisValue == "MJ2000Eq") || (axisValue == "SpacecraftBody"))
      {
         originLabel->Disable();
         originComboBox->Disable();
      }
      else
      {
         originLabel->Enable();
         originComboBox->Enable();
      }
   }
   else
   {
      originLabel->Disable();
      originComboBox->Disable();
      axisLabel->Disable();
      axesComboBox->Disable();
   }
}
