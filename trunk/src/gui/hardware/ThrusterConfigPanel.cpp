//$Id$
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
                                         const wxString &name)
   : GmatPanel(parent)
{  
   thrusterName = std::string(name.c_str());
   theThruster = (Thruster*)theGuiInterpreter->GetConfiguredObject(thrusterName);
   
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
   Integer bsize = 2; // border size
    
   // Coordinate Systems 
   coordsysStaticText = new wxStaticText( this, ID_TEXT, wxT("Coordinate System"), 
                            wxDefaultPosition, wxDefaultSize, 0);
   coordsysComboBox  =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX, wxSize(150,-1));

//   // Axis 
//   ///@todo Needs to be implemented in the base code
//   axisStaticText = new wxStaticText(this, ID_TEXT,
//      wxT("Axis"), wxDefaultPosition, wxDefaultSize, 0);
//
//   wxString strs1[] =
//   {
//      wxT("Inertial"), 
//      wxT("VNB") 
//   };
//   axisComboBox = 
//      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, 
//         wxSize(150,-1), 2, strs1, wxCB_DROPDOWN|wxCB_READONLY);
//
//   // Origin
//   originStaticText = 
//      new wxStaticText(this, ID_TEXT, wxT("Origin"), wxDefaultPosition,
//         wxDefaultSize, 0);
//   originComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX,
//      wxSize(150,-1), false);
   
   // X Direction
   XStaticText = new wxStaticText( this, ID_TEXT, wxT("X Direction"),
                     wxDefaultPosition,wxDefaultSize, 0);
   XTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(80,-1), 0 );

   // Y Direction
   YStaticText = new wxStaticText( this, ID_TEXT, wxT("Y Direction"),
                     wxDefaultPosition,wxDefaultSize, 0);  
   YTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(80,-1), 0 );

   // Z Direction
   ZStaticText = new wxStaticText( this, ID_TEXT, wxT("Z Direction"),
                     wxDefaultPosition,wxDefaultSize, 0); 
   ZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                   wxDefaultPosition, wxSize(80,-1), 0 );

   // Thruster Scale Factor
   scaleFactorStaticText = new wxStaticText( this, ID_TEXT, 
                    wxT("Thrust Scale Factor"), wxDefaultPosition,wxDefaultSize, 0);
   scaleFactorTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), 
                            wxDefaultPosition, wxSize(80,-1), 0 );
                            
   //Tank
   tankStaticText = new wxStaticText(this, ID_TEXT,
                        wxT("Tank"), wxDefaultPosition, wxDefaultSize, 0);
   tankComboBox =
      theGuiManager->GetFuelTankComboBox(this, ID_COMBOBOX, wxSize(150,-1));

   // wxButton
   cCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit Thruster Coef."),
                     wxDefaultPosition, wxDefaultSize, 0 );
   kCoefButton = new wxButton( this, ID_BUTTON, wxT("Edit Impulse Coef."),
                     wxDefaultPosition, wxDefaultSize, 0 );  
                                                              
   // wx*Sizers                                           
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 2, 0, 0 );
    
   // Add to wx*Sizers
   flexGridSizer1->Add( coordsysStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( coordsysComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
//   flexGridSizer1->Add( originStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
//   flexGridSizer1->Add( originComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
//   flexGridSizer1->Add( axisStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
//   flexGridSizer1->Add( axisComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( XStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( XTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( YStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( YTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( ZStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( ZTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( scaleFactorStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( scaleFactorTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( tankStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( tankComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( cCoefButton, 0, wxALIGN_LEFT|wxALL, bsize );
   flexGridSizer1->Add( kCoefButton, 0, wxALIGN_LEFT|wxALL, bsize );
    
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
   
//   originComboBox->SetValue("Earth"); // LTR 11/2/06 - Temperarily hard coded
//   
//   axisComboBox->SetValue("Inertial"); // LTR 11/2/06 - Temperarily hard coded
   
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
   try 
   {
//    canClose = false;
      canClose = true;
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

//ltr: 10/20/06      theOkButton->Disable();
      // Coordinate System      
      if (isCoordSysChanged)
      {
         paramID = theThruster->GetParameterID("CoordinateSystem");
         theThruster->SetStringParameter(paramID, coordsysName);
         isCoordSysChanged = false;
      }    
   
      // Axis
      ///@todo Need to be implemented in base code
      
      // Origin
      ///@todo Need to be implemented in base code
      
           // X_Direction
           inputString = XTextCtrl->GetValue();
           if (GmatStringUtil::ToReal(inputString,&rvalue))
           {
                paramID = theThruster->GetParameterID("X_Direction");
                theThruster->SetRealParameter(paramID, rvalue);
           }
           else
           {
              MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                     inputString.c_str(), "X_Direction","Real Number");
         canClose = false;
//            return;
           }
           
           // Y_Direction
           inputString = YTextCtrl->GetValue();
           if (GmatStringUtil::ToReal(inputString,&rvalue))
           {
                paramID = theThruster->GetParameterID("Y_Direction");
                theThruster->SetRealParameter(paramID, rvalue);
           }
           else
           {
              MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                     inputString.c_str(), "Y_Direction","Real Number");
         canClose = false;
//            return;
           }
        
      // Z_Direction
      inputString = ZTextCtrl->GetValue();
      if (GmatStringUtil::ToReal(inputString,&rvalue))
      {
        paramID = theThruster->GetParameterID("Z_Direction");
        theThruster->SetRealParameter(paramID, rvalue);
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                inputString.c_str(), "Z_Direction","Real Number");
         canClose = false;
//         return;
      }
   
      // ThrustScaleFactor
      inputString = scaleFactorTextCtrl->GetValue();
      if ((GmatStringUtil::ToReal(inputString,&rvalue)) && (rvalue >= 0.0))
      {
        paramID = theThruster->GetParameterID("ThrustScaleFactor");
        theThruster->SetRealParameter(paramID, rvalue);
      }
      else
      {
         MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
                inputString.c_str(), "ThrustScaleFactor","Real Number >= 0.0");
         canClose = false;
//         return;
      }
   
      if (isTankChanged)
      {
          paramID = theThruster->GetParameterID("Tank");
      
          if (theThruster->TakeAction("ClearTanks", ""))
          theThruster->SetStringParameter(paramID, tankName.c_str());
      }
      
      EnableUpdate(false);
//      canClose = true;
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
         canClose = false;
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
