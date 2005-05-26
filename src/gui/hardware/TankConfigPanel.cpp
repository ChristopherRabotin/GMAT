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
   theFuelTank = (FuelTank*)theGuiInterpreter->GetHardware(tankName);
    
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
   // wxTextCtrl
   temperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   refTemperatureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   fuelMassTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   fuelDensityTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   pressureTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
   volumeTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    
   //wxStaticText
   temperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Temperature"),
                            wxDefaultPosition,wxDefaultSize, 0);
   refTemperatureStaticText = new wxStaticText( this, ID_TEXT, wxT("Reference Temperature"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelMassStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Mass"),
                            wxDefaultPosition,wxDefaultSize, 0);
   fuelDensityStaticText = new wxStaticText( this, ID_TEXT, wxT("Fuel Density"),
                            wxDefaultPosition,wxDefaultSize, 0);
   pressureStaticText = new wxStaticText( this, ID_TEXT, wxT("Pressure"),
                            wxDefaultPosition,wxDefaultSize, 0);
   volumeStaticText = new wxStaticText( this, ID_TEXT, wxT("Volume"),
                            wxDefaultPosition,wxDefaultSize, 0);  
   unit1StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit2StaticText = new wxStaticText( this, ID_TEXT, wxT("C"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit3StaticText = new wxStaticText( this, ID_TEXT, wxT("kg"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit4StaticText = new wxStaticText( this, ID_TEXT, wxT("kg/m^3"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit5StaticText = new wxStaticText( this, ID_TEXT, wxT("kPa"),
                            wxDefaultPosition,wxDefaultSize, 0); 
   unit6StaticText = new wxStaticText( this, ID_TEXT, wxT("m^3"),
                            wxDefaultPosition,wxDefaultSize, 0); 
                            
   Integer bsize = 5; // border size
   
   // wx*Sizers     
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );

   // Add to wx*Sizers 
   flexGridSizer1->Add(temperatureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(temperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit1StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(refTemperatureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(refTemperatureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit2StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelMassStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelMassTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit3StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelDensityStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(fuelDensityTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit4StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(pressureStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(pressureTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit5StaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(volumeStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(volumeTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add(unit6StaticText, 0, wxALIGN_CENTER|wxALL, bsize );

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

   paramID = theFuelTank->GetParameterID("Temperature");
   temperatureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
         
   paramID = theFuelTank->GetParameterID("RefTemperature");
   refTemperatureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
    
   paramID = theFuelTank->GetParameterID("FuelMass");
   fuelMassTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));

   paramID = theFuelTank->GetParameterID("FuelDensity");
   fuelDensityTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
   
   paramID = theFuelTank->GetParameterID("Pressure");
   pressureTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
   
   paramID = theFuelTank->GetParameterID("Volume");
   volumeTextCtrl->SetValue(wxVariant(theFuelTank->GetRealParameter(paramID)));
   
   theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankConfigPanel::SaveData()
{
   if (!theApplyButton->IsEnabled())
      return;
       
   if (theFuelTank == NULL)
      return; 
       
   Integer paramID;
   
   paramID = theFuelTank->GetParameterID("Temperature");
   theFuelTank->SetRealParameter(paramID, atof(temperatureTextCtrl->GetValue()));
        
   paramID = theFuelTank->GetParameterID("RefTemperature");
   theFuelTank->SetRealParameter(paramID, atof(refTemperatureTextCtrl->GetValue()));
        
   paramID = theFuelTank->GetParameterID("FuelMass");
   theFuelTank->SetRealParameter(paramID, atof(fuelMassTextCtrl->GetValue()));
        
   paramID = theFuelTank->GetParameterID("FuelDensity");
   theFuelTank->SetRealParameter(paramID, atof(fuelDensityTextCtrl->GetValue()));
        
   paramID = theFuelTank->GetParameterID("Pressure");
   theFuelTank->SetRealParameter(paramID, atof(pressureTextCtrl->GetValue()));
        
   paramID = theFuelTank->GetParameterID("Volume");
   theFuelTank->SetRealParameter(paramID, atof(volumeTextCtrl->GetValue()));
   
   theApplyButton->Disable();
}
  

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void TankConfigPanel::OnTextChange(wxCommandEvent &event)
{
    theApplyButton->Enable();
}    
