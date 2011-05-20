//$Id$
//------------------------------------------------------------------------------
//                            TankConfigPanel
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
// Author: Waka Waktola
// Created: 2004/11/19
// Modified: 
//    2010.03.26 Thomas Grubb - Modified extensively
//    2009.05.27 Linda Jun - To derive from GmatBaseSetupPanel
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "TankConfigPanel.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>

//====================================================================
#ifndef __USE_OLD_TANK_PANEL__
//====================================================================

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TankConfigPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs TankConfigPanel object.
 */
//------------------------------------------------------------------------------
TankConfigPanel::TankConfigPanel(wxWindow *parent, const wxString &name)
   : GmatBaseSetupPanel(parent, name)
{           
}


//------------------------------------------------------------------------------
// ~TankConfigPanel()
//------------------------------------------------------------------------------
TankConfigPanel::~TankConfigPanel()
{
}


//====================================================================
#else
//====================================================================


#include "StringUtil.hpp"
#include <wx/variant.h>

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(TankConfigPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, TankConfigPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, TankConfigPanel::OnComboBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, TankConfigPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TankConfigPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs TankConfigPanel object.
 */
//------------------------------------------------------------------------------
TankConfigPanel::TankConfigPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{           
   std::string tankName = std::string(name.c_str());
   theFuelTank = (FuelTank*)theGuiInterpreter->GetConfiguredObject(tankName);
   
   if (theFuelTank)
   {
      Create();
      Show();
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot find the FuelTank object named " + tankName);
   }
}

//------------------------------------------------------------------------------
// ~TankConfigPanel()
//------------------------------------------------------------------------------
TankConfigPanel::~TankConfigPanel()
{
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
void TankConfigPanel::Create()
{
   // Border size
   int minLabelSize;
   Integer bsize = 2;
   Integer labelSizeProportion = 0;
   Integer ctrlSizeProportion = 1;
   Integer unitSizeProportion = 0;
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Fuel Tank"));

   //-----------------------------------------------------------------
   // Create controls in tab order
   //-----------------------------------------------------------------
   // Volume
   wxStaticText *volumeLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Volume")); 
   volumeTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   volumeTextCtrl->SetToolTip(pConfig->Read(_T("VolumeHint")));
   wxStaticText *volumeUnit =
      new wxStaticText( this, ID_TEXT, wxT("m^3"));
   
   // Pressure Model
   wxStaticText *pressureModelLabel =
      new wxStaticText( this, ID_TEXT, wxT("Pressure "GUI_ACCEL_KEY"Model"));
   StringArray pressModelList =
       theFuelTank->GetPropertyEnumStrings(FuelTank::PRESSURE_MODEL);
   wxArrayString wxPressModelLabels = ToWxArrayString(pressModelList);
   pressureModelComboBox = 
      new wxComboBox( this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxSize(120,-1),
                      wxPressModelLabels, wxCB_DROPDOWN|wxCB_READONLY);
   pressureModelComboBox->SetToolTip(pConfig->Read(_T("PressureModelHint")));
   
   // Fuel Mass
   wxStaticText *fuelMassLabel =
      new wxStaticText( this, ID_TEXT, wxT("F"GUI_ACCEL_KEY"uel Mass"));
   fuelMassTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   fuelMassTextCtrl->SetToolTip(pConfig->Read(_T("MassHint")));
   wxStaticText *fuelMassUnit =
      new wxStaticText( this, ID_TEXT, wxT("kg"));
   
   // Allow Negative Fuel Mass
   allowNegativeFuelMassCheckBox =
      new wxCheckBox( this, ID_CHECKBOX, wxT("Allow "GUI_ACCEL_KEY"Negative Fuel Mass"));
   allowNegativeFuelMassCheckBox->SetToolTip(pConfig->Read(_T("AllowNegFuelMassHint")));
   
   // Fuel Density
   wxStaticText *fuelDensityLabel =
      new wxStaticText( this, ID_TEXT, wxT("Fuel "GUI_ACCEL_KEY"Density"));
   fuelDensityTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   fuelDensityTextCtrl->SetToolTip(pConfig->Read(_T("DensityHint")));
   wxStaticText *fuelDensityUnit =
      new wxStaticText( this, ID_TEXT, wxT("kg/m^3"));
   
   // Temperature
   wxStaticText *temperatureLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Temperature"));
   temperatureTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   temperatureTextCtrl->SetToolTip(pConfig->Read(_T("TemperatureHint")));
   wxStaticText *temperatureUnit =
      new wxStaticText( this, ID_TEXT, wxT("C"));
   
   // Reference Temperature
   wxStaticText *refTemperatureLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Reference Temperature"));
   refTemperatureTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   refTemperatureTextCtrl->SetToolTip(pConfig->Read(_T("ReferenceTemperatureHint")));
   wxStaticText *refTemperatureUnit =
      new wxStaticText( this, ID_TEXT, wxT("C"));
   
   // Pressure
   wxStaticText *pressureLabel =
      new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Pressure"));
   pressureTextCtrl =
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC) );
   pressureTextCtrl->SetToolTip(pConfig->Read(_T("PressureHint")));
   wxStaticText *pressureUnit =
      new wxStaticText( this, ID_TEXT, wxT("kPa"));

   // set the min width for one of the labels for each GmatStaticBoxSizer
   minLabelSize = volumeLabel->GetBestSize().x;
   minLabelSize = (minLabelSize < pressureModelLabel->GetBestSize().x) ? pressureModelLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < fuelMassLabel->GetBestSize().x) ? fuelMassLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < fuelDensityLabel->GetBestSize().x) ? fuelDensityLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < temperatureLabel->GetBestSize().x) ? temperatureLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < refTemperatureLabel->GetBestSize().x) ? refTemperatureLabel->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < pressureLabel->GetBestSize().x) ? pressureLabel->GetBestSize().x : minLabelSize;

   volumeLabel->SetMinSize(wxSize(minLabelSize, volumeLabel->GetMinHeight()));
   fuelMassLabel->SetMinSize(wxSize(minLabelSize, fuelMassLabel->GetMinHeight()));

   // set the min width for one of the UNIT labels for each GmatStaticBoxSizer
   minLabelSize = volumeUnit->GetBestSize().x;
   minLabelSize = (minLabelSize < fuelMassUnit->GetBestSize().x) ? fuelMassUnit->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < fuelDensityUnit->GetBestSize().x) ? fuelDensityUnit->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < temperatureUnit->GetBestSize().x) ? temperatureUnit->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < refTemperatureUnit->GetBestSize().x) ? refTemperatureUnit->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < pressureUnit->GetBestSize().x) ? pressureUnit->GetBestSize().x : minLabelSize;

   volumeUnit->SetMinSize(wxSize(minLabelSize, volumeUnit->GetMinHeight()));
   fuelMassUnit->SetMinSize(wxSize(minLabelSize, fuelMassUnit->GetMinHeight()));

   //-----------------------------------------------------------------
   // Add to fuel properties sizer
   //-----------------------------------------------------------------
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
   //flexGridSizer1->AddGrowableCol(1);
   
   flexGridSizer1->Add(fuelMassLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(fuelMassTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer1->Add(fuelMassUnit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   flexGridSizer1->Add(0, 0, 0);
   flexGridSizer1->Add(allowNegativeFuelMassCheckBox, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer1->Add(0, 0, 0);
   
   flexGridSizer1->Add(fuelDensityLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(fuelDensityTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer1->Add(fuelDensityUnit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);

   flexGridSizer1->Add(temperatureLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(temperatureTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer1->Add(temperatureUnit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   flexGridSizer1->Add(refTemperatureLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(refTemperatureTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer1->Add(refTemperatureUnit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   flexGridSizer1->Add(pressureLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(pressureTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer1->Add(pressureUnit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   // create the Fuel Properties group box
   fuelPropertiesSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Fuel Properties" );
   fuelPropertiesSizer->Add(flexGridSizer1, 0, wxEXPAND|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Add to tank properties sizer
   //-----------------------------------------------------------------
   wxFlexGridSizer *flexGridSizer2 = new wxFlexGridSizer( 3, 0, 0 );
   //flexGridSizer2->AddGrowableCol(1);
   
   flexGridSizer2->Add(volumeLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(volumeTextCtrl, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(volumeUnit, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   
   flexGridSizer2->Add(pressureModelLabel, labelSizeProportion, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer2->Add(pressureModelComboBox, ctrlSizeProportion, wxGROW|wxALL, bsize);
   flexGridSizer2->Add(0, unitSizeProportion, wxALIGN_LEFT|wxALL, bsize);

   // create the Tank Properties group box
   tankPropertiesSizer = new GmatStaticBoxSizer( wxVERTICAL, this, "Tank Properties" );
   tankPropertiesSizer->Add(flexGridSizer2, 0, wxEXPAND|wxALL, bsize);

   //-----------------------------------------------------------------
   // Now put tank & fuel properties sizers into the middle sizer
   //-----------------------------------------------------------------
   theMiddleSizer->Add(tankPropertiesSizer, 0, wxEXPAND|wxALL, bsize);
   theMiddleSizer->Add(fuelPropertiesSizer, 1, wxEXPAND|wxALL, bsize);
   theMiddleSizer->SetSizeHints(this);

}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TankConfigPanel::LoadData()
{ 
   if (theFuelTank == NULL)
      return; 
   
   // Set object pointer for "Show Script"
   mObject = theFuelTank;

   try
   {
      fuelMassTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(FuelTank::FUEL_MASS)));
      allowNegativeFuelMassCheckBox->SetValue(theFuelTank->GetBooleanParameter(FuelTank::ALLOW_NEGATIVE_FUEL_MASS));
      pressureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(FuelTank::PRESSURE)));
      temperatureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(FuelTank::TEMPERATURE)));
      refTemperatureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(FuelTank::REFERENCE_TEMPERATURE)));
      volumeTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(FuelTank::VOLUME)));
      fuelDensityTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(FuelTank::FUEL_DENSITY)));
      pressureModelComboBox->SetValue(theFuelTank->GetStringParameter(FuelTank::PRESSURE_MODEL).c_str());
      
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankConfigPanel::SaveData()
{
   canClose = true;
   
   Real fuelMass, pressure, temp, refTemp, volume, fuelDensity;
   std::string inputString;
   
   //-----------------------------------------------------------------
   // validate user input for non-Real value
   //-----------------------------------------------------------------
   // Fuel Mass 
   inputString = fuelMassTextCtrl->GetValue();
   CheckReal(fuelMass, inputString, "FuelMass", "Real Number >= 0.0", false, true, true, true);
   
   // Pressure 
   inputString = pressureTextCtrl->GetValue();
   CheckReal(pressure, inputString, "Pressure", "Real Number >= 0.0", false, true, true, true);
   
   // Temperature
   inputString = temperatureTextCtrl->GetValue(); 
   CheckReal(temp, inputString, "Temperature", "Real Number");
   
   // Reference Temperature
   inputString = refTemperatureTextCtrl->GetValue();
   CheckReal(refTemp, inputString, "RefTemperature", "Real Number");
   
   // Volume
   inputString = volumeTextCtrl->GetValue();
   CheckReal(volume, inputString, "Volume", "Real Number >= 0.0", false, true, true, true);
   
   // Fuel Density 
   inputString = fuelDensityTextCtrl->GetValue();
   CheckReal(fuelDensity, inputString, "FuelDensity", "Real Number > 0.0", false, true, true, false);
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // Fuel Mass
      theFuelTank->SetRealParameter(FuelTank::FUEL_MASS, fuelMass);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Allow Negative Fuel Mass
      theFuelTank->SetBooleanParameter(FuelTank::ALLOW_NEGATIVE_FUEL_MASS, allowNegativeFuelMassCheckBox->GetValue());
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Pressure 
       theFuelTank->SetRealParameter(FuelTank::PRESSURE, pressure);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Temperature
       theFuelTank->SetRealParameter(FuelTank::TEMPERATURE, temp);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Reference Temperature
       theFuelTank->SetRealParameter(FuelTank::REFERENCE_TEMPERATURE, refTemp);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Volume 
       theFuelTank->SetRealParameter(FuelTank::VOLUME, volume);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Fuel Density 
       theFuelTank->SetRealParameter(FuelTank::FUEL_DENSITY, fuelDensity);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   try
   {
      // Pressure Model
      std::string modelName = pressureModelComboBox->GetValue().c_str();
      theFuelTank->SetStringParameter(FuelTank::PRESSURE_MODEL, modelName);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void TankConfigPanel::OnTextChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}    


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void TankConfigPanel::OnComboBoxChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}    

//====================================================================
#endif
//====================================================================
