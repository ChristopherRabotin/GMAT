//$Header:
//------------------------------------------------------------------------------
//                              CoordSystemConfigPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/10/26
// Updated: 2005/03/11 by A. Greene to implement Load/Save Data
/**
 * This class contains the Coordinate System Setup Panel.
 */
//------------------------------------------------------------------------------
#include "CoordSystemConfigPanel.hpp"

// gui includes
#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "AxisSystem.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CoordSystemConfigPanel, GmatPanel)
    EVT_TEXT(ID_TEXTCTRL, CoordSystemConfigPanel::OnTextUpdate)
    EVT_COMBOBOX(ID_COMBO, CoordSystemConfigPanel::OnComboUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CoordSystemConfigPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CoordSystemConfigPanel::CoordSystemConfigPanel(wxWindow *parent,
                                               const wxString &coordName)
   : GmatPanel(parent)
{
   theCoordSys = (CoordinateSystem*) theGuiInterpreter->GetCoordinateSystem(
                  coordName.c_str());
   mEpochFormat = "TAIModJulian";
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~CoordSystemConfigPanel()
//------------------------------------------------------------------------------
CoordSystemConfigPanel::~CoordSystemConfigPanel()
{
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::Create()
{
    Setup(this);    
}


//------------------------------------------------------------------------------
// void Setup( wxWindow *parent)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::Setup( wxWindow *parent)
{
   std::string coordSysName = theCoordSys->GetName();

   // if CoordinateSystem is non-default, allow user to edit
   if (theGuiInterpreter->IsDefaultCoordinateSystem(coordSysName))
      mCoordPanel = new CoordPanel(this, false);
   else
      mCoordPanel = new CoordPanel(this, true); //loj: 6/2/05 allow user to edit
   
   theMiddleSizer->Add( mCoordPanel, 0, wxALIGN_CENTRE|wxALL, 3);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::LoadData()
{
   epochTextCtrl = mCoordPanel->GetEpochTextCtrl();
   intervalTextCtrl = mCoordPanel->GetIntervalTextCtrl();

   originComboBox = mCoordPanel->GetOriginComboBox();
   typeComboBox = mCoordPanel->GetTypeComboBox();
   primaryComboBox = mCoordPanel->GetPrimaryComboBox();
   formatComboBox = mCoordPanel->GetFormatComboBox();
   secondaryComboBox = mCoordPanel->GetSecondaryComboBox();
   
   xComboBox = mCoordPanel->GetXComboBox();
   yComboBox = mCoordPanel->GetYComboBox();
   zComboBox = mCoordPanel->GetZComboBox();
   
   //mCoordPanel->SetDefaultAxis();
   
   // get the data from the base
   wxString origin = theCoordSys->GetStringParameter("Origin").c_str();
   originComboBox->SetValue(origin);
   
   AxisSystem *axis =
      (AxisSystem *)theCoordSys->GetRefObject(Gmat::AXIS_SYSTEM, "");
   
   if (axis != NULL)
   {
      mCoordPanel->ShowAxisData(axis);
      mEpochFormat = wxString(axis->GetEpochFormat().c_str());
   }
   else
   {
      MessageInterface::ShowMessage
         ("CoordSystemConfigPanel::LoadData() the AxisSystem of %s is NULL\n",
          theCoordSys->GetName().c_str());
   }
   
   mObject = theCoordSys;
   
   theApplyButton->Enable(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::SaveData()
{
   canClose = false;
   
   wxString originName = originComboBox->GetValue().Trim();   
   AxisSystem *axis = mCoordPanel->CreateAxis();
   
   if (axis != NULL)
   {
      theCoordSys->SetStringParameter("Origin", std::string(originName.c_str()));
      AxisSystem *oldAxis =
         (AxisSystem *)theCoordSys->GetRefObject(Gmat::AXIS_SYSTEM, "");
      
      // delete old axis and set new axis
      delete oldAxis;
      theCoordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
      
      canClose = true;
   }
}


//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnTextUpdate(wxCommandEvent& event)
{  
   theApplyButton->Enable(true);
}


//------------------------------------------------------------------------------
// void OnComboUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnComboUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == originComboBox)
   {
   }
   else if (event.GetEventObject() == typeComboBox)
   {
      mCoordPanel->EnableOptions();
   }
   else if (event.GetEventObject() == formatComboBox)
   {
      mCoordPanel->ChangeEpoch(mEpochFormat);
   }
   
   theApplyButton->Enable(true);
}
