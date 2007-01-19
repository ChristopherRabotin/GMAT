//$Header$
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
   theCoordSys =
      (CoordinateSystem*) theGuiInterpreter->GetObject(coordName.c_str());
   mEpochFormat = "A1ModJulian";
   mOriginChanged = false;
   mObjRefChanged = false;
   
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
   
   // get the data from the base
   wxString origin = theCoordSys->GetStringParameter("Origin").c_str();
   originComboBox->SetValue(origin);
   
   AxisSystem *axis =
      (AxisSystem *)theCoordSys->GetRefObject(Gmat::AXIS_SYSTEM, "");
   
   if (axis != NULL)
   {
      mCoordPanel->ShowAxisData(axis);
      mEpochFormat = wxString(axis->GetEpochFormat().c_str());
      
      if (mEpochFormat == "")
        mEpochFormat = "A1ModJulian";
   }
   else
   {
      MessageInterface::ShowMessage
         ("CoordSystemConfigPanel::LoadData() the AxisSystem of %s is NULL\n",
          theCoordSys->GetName().c_str());
   }
   
   mObject = theCoordSys;
   
   //theApplyButton->Enable(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::SaveData()
{
   canClose = true;
   std::string originName = originComboBox->GetValue().Trim().c_str();

   //-------------------------------------------------------
   // set new origin
   //-------------------------------------------------------
   if (mOriginChanged)
   { 
      //MessageInterface::ShowMessage
      //   ("===> originName = %s\n", originName.c_str());
      
      mOriginChanged = false;
      theCoordSys->SetStringParameter("Origin", originName);
      
      // set coordinate system origin
      SpacePoint *origin =
         (SpacePoint*)theGuiInterpreter->GetObject(originName);
      theCoordSys->SetOrigin(origin);
      
      // set Earth as J000Body if NULL
      if (origin->GetJ2000Body() == NULL)
      {
         SpacePoint *j2000body =
            (SpacePoint*)theGuiInterpreter->GetObject("Earth");
         origin->SetJ2000Body(j2000body);
      }
   }
   
   //-------------------------------------------------------
   // set new axis system
   //-------------------------------------------------------
   if (mObjRefChanged)
   {
      AxisSystem *axis = mCoordPanel->CreateAxis();
      
      if (axis != NULL)
      {
         AxisSystem *oldAxis =
            (AxisSystem *)theCoordSys->GetRefObject(Gmat::AXIS_SYSTEM, "");
         
         canClose = mCoordPanel->SaveData(theCoordSys->GetName(), axis, mEpochFormat);
         
         // delete old axis and set new axis
         delete oldAxis;
         theCoordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
      }
      else
      {
         MessageInterface::ShowMessage
            ("CoordSystemConfigPanel::SaveData() Cannot create AxisSystem.\n");
         canClose = false;
      }
   }
}


//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnTextUpdate(wxCommandEvent& event)
{
   mObjRefChanged = true;
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnComboUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnComboUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == originComboBox)
   {
      mOriginChanged = true;
   }
   else if (event.GetEventObject() == typeComboBox)
   {
      mCoordPanel->EnableOptions();
      mObjRefChanged = true;
   }
   else if (event.GetEventObject() == formatComboBox)
   {
      mCoordPanel->ChangeEpoch(mEpochFormat);
   }
   else if (event.GetEventObject() == primaryComboBox ||
            event.GetEventObject() == secondaryComboBox)
   {
      mObjRefChanged = true;
   }
   else if (event.GetEventObject() == xComboBox ||
            event.GetEventObject() == yComboBox ||
            event.GetEventObject() == zComboBox)
   {
      mObjRefChanged = true;
   }
   
   EnableUpdate(true);
}
