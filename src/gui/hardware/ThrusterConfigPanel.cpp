//$Header$
//------------------------------------------------------------------------------
//                            ThrusterConfigPanel
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
// Created: 2005/01/06
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "ThrusterConfigPanel.hpp"
#include "ThrusterCoefficientDialog.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include <wx/variant.h>

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(ThrusterConfigPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_TEXT(ID_TEXTCTRL, ThrusterConfigPanel::OnTextChange)
   EVT_COMBOBOX(ID_COMBOBOX, ThrusterConfigPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON, ThrusterConfigPanel::OnButtonClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ThrusterConfigPanel(wxWindow *parent, const wxString &thrusterName)
//------------------------------------------------------------------------------
/**
 * Constructs ThrusterConfigPanel object.
 */
//------------------------------------------------------------------------------
ThrusterConfigPanel::ThrusterConfigPanel(wxWindow *parent, 
                                  const wxString &name):GmatPanel(parent)
{  
   thrusterName = std::string(name.c_str());
    
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   
   theGuiManager = GuiItemManager::GetInstance();
   
   theThruster = (Thruster*)theGuiInterpreter->GetObject(thrusterName);
   
   isCoordSysChanged = false;
   isTankChanged = false;
   isTankEmpty = false;
   
   tankSize = 0;
   
   thrusterName = "";
   coordsysName = "";
   tankName = "";
    
   Create();
   Show();
}

//------------------------------------------------------------------------------
// ~ThrusterConfigPanel()
//------------------------------------------------------------------------------
ThrusterConfigPanel::~ThrusterConfigPanel()
{
   theGuiManager->UnregisterComboBox("CoordinateSystem", coordsysComboBox);
   theGuiManager->UnregisterComboBox("FuelTank", tankComboBox);
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
void ThrusterConfigPanel::Create()
{
    // Integer
    Integer bsize = 5; // border size
    
    // wxButton
    cCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit Thruster Coef."),
                              wxDefaultPosition, wxDefaultSize, 0 );
    kCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit Impulse Coef."),
                              wxDefaultPosition, wxDefaultSize, 0 );  
                                                              
    // wxComboBox 
    coordsysComboBox  =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(180,-1));
    tankComboBox =
      theGuiManager->GetFuelTankComboBox(this, ID_COMBOBOX, wxSize(180,-1));
   
    // wxTextCtrl
    XTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    YTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    ZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
    scaleFactorTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
                            
    //wxStaticText
    coordsysStaticText = new wxStaticText( this, ID_TEXT, wxT("Coordinate System"), 
                            wxDefaultPosition, wxDefaultSize, 0);
    tankStaticText = new wxStaticText(this, ID_TEXT,
                            wxT("Use tank"), wxDefaultPosition, wxDefaultSize, 0);
    XStaticText = new wxStaticText( this, ID_TEXT, wxT("X Direction"),
                            wxDefaultPosition,wxDefaultSize, 0);
    YStaticText = new wxStaticText( this, ID_TEXT, wxT("Y Direction"),
                            wxDefaultPosition,wxDefaultSize, 0);  
    ZStaticText = new wxStaticText( this, ID_TEXT, wxT("Z Direction"),
                            wxDefaultPosition,wxDefaultSize, 0); 
    scaleFactorStaticText = new wxStaticText( this, ID_TEXT, 
                    wxT("Thrust Scale Factor"), wxDefaultPosition,wxDefaultSize, 0);
    
    // wx*Sizers                                           
    wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
    
    // Add to wx*Sizers 
    flexGridSizer1->Add(coordsysStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
    flexGridSizer1->Add(coordsysComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(XStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(XTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(YStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(YTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(ZStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(ZTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(scaleFactorStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(scaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add(tankStaticText, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(tankComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
    flexGridSizer1->Add(cCoefButton, 0, wxALIGN_CENTRE|wxALL, bsize );
    flexGridSizer1->Add(kCoefButton, 0, wxALIGN_LEFT|wxALL, bsize );
    
    theMiddleSizer->Add(flexGridSizer1, 0, wxALIGN_CENTRE|wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::LoadData()
{ 
   if (theThruster == NULL)
      return;

   // Set object pointer for "Show Script"
   mObject = theThruster;

   Integer paramID;
      
   paramID = theThruster->GetParameterID("CoordinateSystem");
   coordsysName = theThruster->GetStringParameter(paramID);
   
   coordsysComboBox->SetValue(coordsysName.c_str());
   
   paramID = theThruster->GetParameterID("X_Direction");
   XTextCtrl->SetValue(wxVariant(theThruster->GetRealParameter(paramID)));
    
   paramID = theThruster->GetParameterID("Y_Direction");
   YTextCtrl->SetValue(wxVariant(theThruster->GetRealParameter(paramID)));
        
   paramID = theThruster->GetParameterID("Z_Direction");
   ZTextCtrl->SetValue(wxVariant(theThruster->GetRealParameter(paramID)));
   
   paramID = theThruster->GetParameterID("ThrustScaleFactor");
   scaleFactorTextCtrl->SetValue(wxVariant(theThruster->GetRealParameter(paramID)));
   
   paramID = theThruster->GetParameterID("Tank");
   StringArray tanks = theThruster->GetStringArrayParameter(paramID);   
   
   if (tankComboBox->IsEmpty())
    {
       tankComboBox->Insert(wxT("No Tanks Available"), 0);
       tankComboBox->SetSelection(0);
       isTankEmpty = true;
    }
    else if (tanks.empty())
    {
       tankComboBox->Insert(wxT("No Tank Selected"), 0);
       tankComboBox->SetSelection(0);
       isTankEmpty = false;
    }
    else
    {
       tankComboBox->SetValue(tanks[0].c_str());
       isTankEmpty = false;
    }
   
   EnableUpdate(false);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::SaveData()
{
    canClose = false;
    if (!theApplyButton->IsEnabled())
      return;
       
   if (theThruster == NULL)
      return; 

   Integer paramID;
  
   Real rvalue;
   std::string inputString;
   std::string msg = "The value of \"%s\" for field \"%s\" on object \"" +
                     theThruster->GetName() + "\" is not an allowed value.\n"
                     "The allowed values are: [ %s ].";

   theOkButton->Disable();
 
   // X_Direction
   paramID = theThruster->GetParameterID("X_Direction");
   inputString = XTextCtrl->GetValue();
   if (!GmatStringUtil::ToDouble(inputString,&rvalue))
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "X_Direction","Real Number");
      return;
   }
   theThruster->SetRealParameter(paramID, rvalue);
   
   // Y_Direction
   paramID = theThruster->GetParameterID("Y_Direction");
   inputString = YTextCtrl->GetValue();
   if (!GmatStringUtil::ToDouble(inputString,&rvalue))
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
             inputString.c_str(), "Y_Direction","Real Number");
      return;
   }
   theThruster->SetRealParameter(paramID, rvalue);
        
   try 
   {
      // Z_Direction
      paramID = theThruster->GetParameterID("Z_Direction");
      inputString = ZTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue) || rvalue < 0)
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                inputString.c_str(), "Z_Direction","Real Number >= 0");
         return;
      }
      theThruster->SetRealParameter(paramID, rvalue);
   
      // ThrustScaleFactor
      paramID = theThruster->GetParameterID("ThrustScaleFactor");
      inputString = scaleFactorTextCtrl->GetValue();
      if (!GmatStringUtil::ToDouble(inputString,&rvalue))
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                inputString.c_str(), "ThrustScaleFactor","Real Number");
         return;
      }
      theThruster->SetRealParameter(paramID, rvalue);
   
      if (isCoordSysChanged)
      {
         paramID = theThruster->GetParameterID("CoordinateSystem");
         theThruster->SetStringParameter(paramID, coordsysName);
         isCoordSysChanged = false;
      }    
   
      if (isTankChanged)
      {
          paramID = theThruster->GetParameterID("Tank");
      
          if (theThruster->TakeAction("ClearTanks", ""))
          theThruster->SetStringParameter(paramID, tankName.c_str());
      }
      
      EnableUpdate(false);
      canClose = true;
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetMessage());
   }

}

//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::OnTextChange(wxCommandEvent &event)
{
    EnableUpdate(true);
} 

//------------------------------------------------------------------------------
// void OnComboBoxChange()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::OnComboBoxChange(wxCommandEvent &event)
{
        if (event.GetEventObject() == coordsysComboBox)
   {
      isCoordSysChanged =  true;
      coordsysName = coordsysComboBox->GetStringSelection().c_str();
      
      EnableUpdate(true);
   }
   else if (event.GetEventObject() == tankComboBox)
   {
           if (!isTankEmpty)
      {
              isTankChanged = true;
         tankName = tankComboBox->GetStringSelection().c_str();
         EnableUpdate(true);
      }
   }
}    

//------------------------------------------------------------------------------
// void OnButtonClick()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::OnButtonClick(wxCommandEvent &event)
{  
    if (event.GetEventObject() == cCoefButton)
    {
       std::string type = "C";
       
       ThrusterCoefficientDialog tcDlg(this, theThruster, type);
       tcDlg.ShowModal(); 
       EnableUpdate(true);      
    } 
    else if (event.GetEventObject() == kCoefButton)
    {
       std::string type = "K";
       
       ThrusterCoefficientDialog tcDlg(this, theThruster, type);
       tcDlg.ShowModal();
       EnableUpdate(true);
    }            
}    
