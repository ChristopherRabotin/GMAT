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
#include "GmatAppData.hpp"
#include "AxisSystem.hpp"
#include "SpacePoint.hpp"

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
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();

   theCoordSys = (CoordinateSystem*) theGuiInterpreter->GetCoordinateSystem(
                  coordName.c_str());

   Create();
   Show();
}

CoordSystemConfigPanel::~CoordSystemConfigPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void CoordSystemConfigPanel::Create()
{
    Setup(this);    
}

void CoordSystemConfigPanel::Setup( wxWindow *parent)
{
   mCoordPanel = new CoordPanel(this, false);

   theMiddleSizer->Add( mCoordPanel, 0, wxALIGN_CENTRE|wxALL, 5);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::LoadData()
{
   try
   {
      epochTextCtrl = mCoordPanel->GetEpochTextCtrl();

      originComboBox = mCoordPanel->GetOriginComboBox();
      typeComboBox = mCoordPanel->GetTypeComboBox();
      primaryComboBox = mCoordPanel->GetPrimaryComboBox();
      formatComboBox = mCoordPanel->GetFormatComboBox();
      secondaryComboBox = mCoordPanel->GetSecondaryComboBox();

      xComboBox = mCoordPanel->GetXComboBox();
      yComboBox = mCoordPanel->GetYComboBox();
      zComboBox = mCoordPanel->GetZComboBox();

      // get the data from the base
      wxString origin = theCoordSys->GetStringParameter("OriginName").c_str();
      originComboBox->SetValue(origin);

      AxisSystem *axes = (AxisSystem *)theCoordSys->GetRefObject(
                  Gmat::AXIS_SYSTEM, "");

      if (axes != NULL)
      {
         int sel =typeComboBox->FindString(axes->GetTypeName().c_str());
         typeComboBox->SetSelection(sel);
         mCoordPanel->EnableOptions();

         if (mCoordPanel->GetShowPrimaryBody())
         {
            SpacePoint *primaryObj = axes->GetPrimaryObject();
            if (primaryObj != NULL)
               primaryComboBox->SetValue(primaryObj->GetName().c_str());
            else
               MessageInterface::ShowMessage("Primary object was null - may not" \
                  " be implemented yet\n");
         }

         if (mCoordPanel->GetShowSecondaryBody())
         {
            SpacePoint *secondObj = axes->GetSecondaryObject();
            if (secondObj != NULL)
               secondaryComboBox->SetValue(secondObj->GetName().c_str());
            else
               MessageInterface::ShowMessage("Secondary object was null - may "\
                  "not be implemented yet\n");
         }

         if (mCoordPanel->GetShowEpoch())
         {
            A1Mjd epoch = axes->GetEpoch();

            // hard coded for now
            /// @todo get epoch format from base code
            formatComboBox->SetValue("TAIModJulian");

            wxString epochString;
            epochString.Printf("%9.9f", epoch.Get());
            epochTextCtrl->SetValue(epochString);
         }

         if (mCoordPanel->GetShowXyz())
         {
            std::string xString = axes->GetXAxis();
            std::string yString = axes->GetYAxis();
            std::string zString = axes->GetZAxis();

            xComboBox->SetValue(xString.c_str());
            yComboBox->SetValue(yString.c_str());
            zComboBox->SetValue(zString.c_str());
         }
      }
      mObject = theCoordSys;
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordSystemConfigPanel:LoadData() error occurred!\n%s\n",
            e.GetMessage().c_str());
   }
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::SaveData()
{
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
   }

   theApplyButton->Enable(true);
}
