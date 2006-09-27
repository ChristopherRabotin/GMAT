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
   theFuelTank = (FuelTank*)theGuiInterpreter->GetObject(tankName);
    
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
                            
   // wxCheckBox
   pressureRegulatedCheckBox = new wxCheckBox( this, ID_CHECKBOX, 
                               wxT("Pressure Regulated"),wxDefaultPosition, 
                               wxDefaultSize, 0 );
                            
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
   flexGridSizer1->Add( 0, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add(pressureRegulatedCheckBox, 0, wxALIGN_CENTER|wxALL, bsize );
   flexGridSizer1->Add( 0, 0, wxALIGN_CENTRE|wxALL, bsize);

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
   
   paramID = theFuelTank->GetParameterID("PressureRegulated");
   pressureRegulatedCheckBox->SetValue(theFuelTank->GetBooleanParameter(paramID));
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void TankConfigPanel::SaveData()
{
   canClose = false;
   if (!theApplyButton->IsEnabled())
      return;
       
   if (theFuelTank == NULL)
      return; 
       
   Integer paramID;
   try
   {
      Real rvalue;
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                        theFuelTank->GetName() + "\" is not an allowed value.  "
                        "The allowed values are: [ %s ]."; 
      
      //loj: 9/26/06 theOkButton->Disable();            
      
      // Temperature
      paramID = theFuelTank->GetParameterID("Temperature");
      inputString = temperatureTextCtrl->GetValue(); 
      if (!GmatStringUtil::ToDouble(inputString,&rvalue)) 
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Temperature","Real Number");
         return;
      }
      theFuelTank->SetRealParameter(paramID, rvalue);

      // Reference Temperature
      paramID = theFuelTank->GetParameterID("RefTemperature");
      inputString = refTemperatureTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue)) 
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Reference Temperature","Real Number");
         return;
      }
      theFuelTank->SetRealParameter(paramID, rvalue); 

      // Fuel Mass 
      paramID = theFuelTank->GetParameterID("FuelMass");
      inputString = fuelMassTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue) || rvalue < 0) 
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Fuel Mass","Real Number >= 0");
         return;
      }
      theFuelTank->SetRealParameter(paramID, rvalue); 

      // Fuel Density 
      paramID = theFuelTank->GetParameterID("FuelDensity");
      inputString = fuelDensityTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue) || rvalue < 0) 
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Fuel Density","Real Number >= 0");
         return;
      }
      theFuelTank->SetRealParameter(paramID, rvalue); 

      // Pressure 
      paramID = theFuelTank->GetParameterID("Pressure");
      inputString = pressureTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue) || rvalue < 0) 
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Pressure","Real Number >= 0");
         return;
      }
      theFuelTank->SetRealParameter(paramID, rvalue); 

      // Volume 
      paramID = theFuelTank->GetParameterID("Volume");
      inputString = volumeTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue)) 
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Volume","Real Number >= 0");
         return;
      }
      theFuelTank->SetRealParameter(paramID, rvalue); 
      
      // Pressure Regulated
      paramID = theFuelTank->GetParameterID("PressureRegulated");
      theFuelTank->SetBooleanParameter(paramID, 
                                       pressureRegulatedCheckBox->GetValue());

      EnableUpdate(false);
      canClose = true;
   }
   catch (BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetMessage());
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



