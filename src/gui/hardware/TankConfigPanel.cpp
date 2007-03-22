//$Header$
//------------------------------------------------------------------------------
//                            TankConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Waka Waktola
// Created: 2004/11/19
/**
 * This class contains information needed to setup users spacecraft tank 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "TankConfigPanel.hpp"
#include "MessageInterface.hpp"
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
   EVT_CHECKBOX(ID_CHECKBOX, TankConfigPanel::OnCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// TankConfigPanel(wxWindow *parent, Spacecraft *spacecraft, wxButton *theApplyButton)
//------------------------------------------------------------------------------
/**
 * Constructs TankConfigPanel object.
 */
//------------------------------------------------------------------------------
TankConfigPanel::TankConfigPanel(wxWindow *parent, const wxString &name):GmatPanel(parent)
{           
   tankName = std::string(name.c_str());
    
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theFuelTank = (FuelTank*)theGuiInterpreter->GetConfiguredObject(tankName);
    
   Create();
   Show();
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
   // Integer
   Integer bsize = 2; // border size

   // Fuel Mass
   fuelMassStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Mass"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                          wxDefaultPosition, wxSize(100,-1), 0 );
   unit3StaticText = new wxStaticText( this, ID_TEXT, wxT("kg"),
                         wxDefaultPosition,wxDefaultSize, 0); 

   // Pressure
   pressureStaticText = new wxStaticText( this, ID_TEXT, wxT("Pressure"),
                            wxDefaultPosition,wxDefaultSize, 0);
   pressureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                          wxDefaultPosition, wxSize(100,-1), 0 );
   unit5StaticText = new wxStaticText( this, ID_TEXT, wxT("kPa"),
                         wxDefaultPosition,wxDefaultSize, 0); 

   // Temperature
   temperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Temperature"),
                               wxDefaultPosition,wxDefaultSize, 0);
   temperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                             wxDefaultPosition, wxSize(100,-1), 0 );
   unit1StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                         wxDefaultPosition,wxDefaultSize, 0); 

   // Reference Temperature
   refTemperatureStaticText = 
      new wxStaticText( this, ID_TEXT, wxT("Reference Temperature"),
          wxDefaultPosition,wxDefaultSize, 0);
   refTemperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                                wxDefaultPosition, wxSize(100,-1), 0 );
   unit2StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                         wxDefaultPosition,wxDefaultSize, 0); 

   // Volume
   volumeStaticText = new wxStaticText( this, ID_TEXT, wxT("Volume"),
                          wxDefaultPosition,wxDefaultSize, 0);  
   volumeTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                        wxDefaultPosition, wxSize(100,-1), 0 );
   unit6StaticText = new wxStaticText( this, ID_TEXT, wxT("m^3"),
                         wxDefaultPosition,wxDefaultSize, 0); 
                            
   // Fuel Density
   fuelDensityStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Density"),
                               wxDefaultPosition,wxDefaultSize, 0);
   fuelDensityTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                             wxDefaultPosition, wxSize(100,-1), 0 );
   unit4StaticText = new wxStaticText( this, ID_TEXT, wxT("kg/m^3"),
                         wxDefaultPosition,wxDefaultSize, 0); 

   // Pressure Regulated
   pressureRegulatedCheckBox = 
      new wxCheckBox( this, ID_CHECKBOX, wxT("Pressure Regulated"),
          wxDefaultPosition, wxDefaultSize, 0 );
   
   // wx*Sizers     
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );

   // Add to wx*Sizers 
   flexGridSizer1->Add(fuelMassStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(fuelMassTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add(unit3StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(pressureStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(pressureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add(unit5StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(temperatureStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(temperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add(unit1StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(refTemperatureStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(refTemperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add(unit2StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(volumeStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(volumeTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add(unit6StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(fuelDensityStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(fuelDensityTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   flexGridSizer1->Add(unit4StaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(0, 0, wxALIGN_LEFT|wxALL, bsize);
   flexGridSizer1->Add(pressureRegulatedCheckBox, 0, wxALIGN_RIGHT|wxALL, bsize);
   flexGridSizer1->Add(0, 0, wxALIGN_LEFT|wxALL, bsize);

   theMiddleSizer->Add(flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void TankConfigPanel::LoadData()
{ 
   if (theFuelTank == NULL)
      return; 
       
   Integer paramID;
   
   // Set object pointer for "Show Script"
   mObject = theFuelTank;

   paramID = theFuelTank->GetParameterID("FuelMass");
   fuelMassTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));

   paramID = theFuelTank->GetParameterID("Pressure");
   pressureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
   
   paramID = theFuelTank->GetParameterID("Temperature");
   temperatureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
         
   paramID = theFuelTank->GetParameterID("RefTemperature");
   refTemperatureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
    
   paramID = theFuelTank->GetParameterID("Volume");
   volumeTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
   
   paramID = theFuelTank->GetParameterID("FuelDensity");
   fuelDensityTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
   
   paramID = theFuelTank->GetParameterID("PressureRegulated");
   pressureRegulatedCheckBox->SetValue(theFuelTank->GetBooleanParameter(paramID));
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankConfigPanel::SaveData()
{
   try
   {
//   canClose = false;
      canClose = true;
   
      if (!theApplyButton->IsEnabled())
         return;
       
      if (theFuelTank == NULL)
         return; 
       
      Integer paramID;
      Real rvalue;
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                        theFuelTank->GetName() + "\" is not an allowed value.\n"
                        "The allowed values are: [ %s ]."; 
            
      // Fuel Mass 
      inputString = fuelMassTextCtrl->GetValue();
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && (rvalue >= 0.0))
      {
         paramID = theFuelTank->GetParameterID("FuelMass");
         theFuelTank->SetRealParameter(paramID, rvalue); 
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Fuel Mass","Real Number >= 0.0");
         canClose = false;
      }

      // Pressure 
      inputString = pressureTextCtrl->GetValue();
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && (rvalue >= 0.0))
      {
         paramID = theFuelTank->GetParameterID("Pressure");
         theFuelTank->SetRealParameter(paramID, rvalue); 
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Pressure","Real Number >= 0.0");
         canClose = false;
      }

      // Temperature
      inputString = temperatureTextCtrl->GetValue(); 
      if (GmatStringUtil::ToReal(inputString,&rvalue)) 
      {
         paramID = theFuelTank->GetParameterID("Temperature");
         theFuelTank->SetRealParameter(paramID, rvalue);
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Temperature","Real Number");
         canClose = false;
      }

      // Reference Temperature
      inputString = refTemperatureTextCtrl->GetValue();
      if (GmatStringUtil::ToReal(inputString,&rvalue)) 
      {
         paramID = theFuelTank->GetParameterID("RefTemperature");
         theFuelTank->SetRealParameter(paramID, rvalue); 
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Reference Temperature","Real Number");
         canClose = false;
      }

      // Volume 
      inputString = volumeTextCtrl->GetValue();
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && (rvalue >= 0.0))
      {
         paramID = theFuelTank->GetParameterID("Volume");
         theFuelTank->SetRealParameter(paramID, rvalue); 
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Volume","Real Number >= 0.0");
         canClose = false;
      }
      
      // Fuel Density 
      inputString = fuelDensityTextCtrl->GetValue();
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && (rvalue >= 0.0))
      {
         paramID = theFuelTank->GetParameterID("FuelDensity");
         theFuelTank->SetRealParameter(paramID, rvalue); 
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Fuel Density","Real Number >= 0.0");
         canClose = false;
      }

      // Pressure Regulated
      paramID = theFuelTank->GetParameterID("PressureRegulated");
      theFuelTank->SetBooleanParameter(paramID, 
         pressureRegulatedCheckBox->GetValue());

      EnableUpdate(false);
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
         canClose = false;
   }
   
   
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
void TankConfigPanel::OnCheckBoxChange(wxCommandEvent &event)
{
    EnableUpdate(true);
}    



