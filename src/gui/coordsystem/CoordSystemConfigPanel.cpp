//$Id$
//------------------------------------------------------------------------------
//                              CoordSystemConfigPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Waka Waktola
// Created: 2004/10/26
// Updated: 2005/03/11 by A. Greene to implement Load/Save Data
/**
 * This class contains the Coordinate System Setup Panel.
 */
//------------------------------------------------------------------------------
#include "CoordSystemConfigPanel.hpp"

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "AxisSystem.hpp"
#include "DateUtil.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_COORD_PANEL_LOAD 1
//#define DEBUG_COORD_PANEL_SAVE 1
//#define DEBUG_COORD_PANEL_TEXT
//#define DEBUG_COORD_PANEL_XYZ

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
 * Default constructor.
 */
//------------------------------------------------------------------------------
CoordSystemConfigPanel::CoordSystemConfigPanel(wxWindow *parent,
                                               const wxString &coordName)
   : GmatPanel(parent)
{
   theCoordSys =
      (CoordinateSystem*) theGuiInterpreter->GetConfiguredObject(coordName.c_str());
   mEpochFormat = "A1ModJulian";
   mOriginChanged = false;
   mObjRefChanged = false;
   mEpochChanged  = false;
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~CoordSystemConfigPanel()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
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
/**
 * Creates the panel.
 */
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::Create()
{
   std::string coordSysName = theCoordSys->GetName();

   // if CoordinateSystem is non-default, allow user to edit
//   if (theGuiInterpreter->IsDefaultCoordinateSystem(coordSysName))
   if (theCoordSys->IsBuiltIn())
      mCoordPanel = new CoordPanel(this, false);
   else
      mCoordPanel = new CoordPanel(this, true);
   
   theMiddleSizer->Add( mCoordPanel, 0, wxALIGN_CENTRE|wxALL, 3);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data onto the panel.
 */
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::LoadData()
{
   #if DEBUG_COORD_PANEL_LOAD
   MessageInterface::ShowMessage("CoordSystemConfigPanel::LoadData() entered\n");
   #endif
   
   // Set the pointer for the "Show Script" button
   mObject = theCoordSys;
   
   epochTextCtrl = mCoordPanel->GetEpochTextCtrl();
   
   originComboBox        = mCoordPanel->GetOriginComboBox();
   typeComboBox          = mCoordPanel->GetTypeComboBox();
   primaryComboBox       = mCoordPanel->GetPrimaryComboBox();
   secondaryComboBox     = mCoordPanel->GetSecondaryComboBox();
   refObjectComboBox     = mCoordPanel->GetReferenceObjectComboBox();
   constraintCSComboBox  = mCoordPanel->GetConstraintCSComboBox();
   
   xComboBox = mCoordPanel->GetXComboBox();
   yComboBox = mCoordPanel->GetYComboBox();
   zComboBox = mCoordPanel->GetZComboBox();

   // get the data from the base
   try
   {
      std::string str = theCoordSys->GetStringParameter("Origin");
      
      #if DEBUG_COORD_PANEL_LOAD
      MessageInterface::ShowMessage("   setting Origin to %s\n", str.c_str());
      #endif
      
      originComboBox->SetValue(str.c_str());
      
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
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel to the coordinate system object.
 */
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::SaveData()
{
   canClose = true;
   std::string originName = originComboBox->GetValue().Trim().c_str();
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      //-------------------------------------------------------
      // check/set new epoch
      //-------------------------------------------------------
      if (mEpochChanged)
      {
         std::string savedEpoch = (mCoordPanel->epochValue).c_str();
         Real epoch;
         std::string str = mCoordPanel->GetEpochTextCtrl()->GetValue().c_str();
         #if DEBUG_COORD_PANEL_SAVE
         MessageInterface::ShowMessage("Epoch data has been changed!!!!\n");
         MessageInterface::ShowMessage
            ("CoordSystemConfigPanel::SaveData() epoch value = %s\n",
             str.c_str());
         #endif
         bool isValid = CheckReal(epoch, str, "Epoch", "Real Number >= 0");
         #if DEBUG_COORD_PANEL_SAVE
         MessageInterface::ShowMessage
            ("CoordSystemConfigPanel::SaveData() isValid = %s, and epoch real value = %12.10f\n",
             (isValid? "true" : "false"), epoch);
         #endif

         // check range here too
         if (isValid)
            isValid = CheckRealRange(str, epoch, "Epoch", DateUtil::EARLIEST_VALID_MJD_VALUE, DateUtil::LATEST_VALID_MJD_VALUE, true, true, true, true);
         if (!isValid)
         {
            canClose = false;
         }
      }

      if (!canClose)
         return;

      //-------------------------------------------------------
      // set new axis system
      //-------------------------------------------------------
      if (mObjRefChanged)
      {
         AxisSystem *oldAxis = (AxisSystem*) theCoordSys->GetRefObject(Gmat::AXIS_SYSTEM, "");
         AxisSystem *axis = mCoordPanel->CreateAxis();
         #ifdef DEBUG_COORD_PANEL_XYZ
            MessageInterface::ShowMessage("CoordSystemConfigPanel:: CreateAxis returned %s\n",
                  (axis? "not NULL" : "NULL"));
         #endif
         
         if (axis != NULL)
         {
            #ifdef DEBUG_COORD_PANEL_XYZ
               MessageInterface::ShowMessage("CoordSystemConfigPanel:: executing axis not equal to NULL part ...\n");
            #endif
            canClose = mCoordPanel->SaveData(theCoordSys->GetName(), axis, mEpochFormat);
            
            if (canClose)
            {
               try
               {
                  // only set these if there was no error creating or initializing the coordinate system
                  theCoordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
                  theCoordSys->Initialize();
               }
               catch (BaseException &be)
               {
                  // reset the CS to have the axis system it started with
                  theCoordSys->SetRefObject(oldAxis, Gmat::AXIS_SYSTEM, "");
                  theCoordSys->Initialize();
                  throw;
               }
            }
         }
         else
         {
            #ifdef DEBUG_COORD_PANEL_XYZ
               MessageInterface::ShowMessage("CoordSystemConfigPanel:: executing axis EQUAL to NULL part ...\n");
            #endif
            MessageInterface::ShowMessage
               ("CoordSystemConfigPanel::SaveData() Cannot create AxisSystem.\n");
            canClose = false;
         }
      }

      //-------------------------------------------------------
      // set new origin - needs to be done after setting of the axis system
      //-------------------------------------------------------
      if (mOriginChanged)
      {
         #if DEBUG_COORD_PANEL_SAVE
         MessageInterface::ShowMessage
            ("CoordSystemConfigPanel::SaveData() originName = %s\n",
             originName.c_str());
         #endif

         // set coordinate system origin
         SpacePoint *origin =
            (SpacePoint*)theGuiInterpreter->GetConfiguredObject(originName);
         theCoordSys->SetStringParameter("Origin", originName);
         theCoordSys->SetOrigin(origin);

         mOriginChanged = false;

         // set Earth as J000Body if NULL
         if (origin->GetJ2000Body() == NULL)
         {
            SpacePoint *j2000body =
               (SpacePoint*)theGuiInterpreter->GetConfiguredObject("Earth");
            origin->SetJ2000Body(j2000body);
         }
      }
      ResetFlags();

   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
}

//------------------------------------------------------------------------------
// void ResetFlags()
//------------------------------------------------------------------------------
/**
 * resets modified flags
 */
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::ResetFlags()
{
   mOriginChanged = false;
   mObjRefChanged = false;
   mEpochChanged  = false;
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for event triggered when text is updated.
 *
 * @param event   command event to be handled
 */
//------------------------------------------------------------------------------
void CoordSystemConfigPanel::OnTextUpdate(wxCommandEvent& event)
{
   mObjRefChanged = true;
   EnableUpdate(true);
   if (mCoordPanel->GetEpochTextCtrl()->IsModified() )
   {
      mEpochChanged = true;
      #ifdef DEBUG_COORD_PANEL_TEXT
         MessageInterface::ShowMessage("Text has been updated and epoch has been changed!!!\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void OnComboUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for event triggered when a combo box is updated.
 *
 * @param event   command event to be handled
 */
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
   else if (event.GetEventObject() == primaryComboBox ||
            event.GetEventObject() == secondaryComboBox)
   {
      mObjRefChanged = true;
   }
   else if (event.GetEventObject() == refObjectComboBox)
   {
      mObjRefChanged = true;
   }
   else if (event.GetEventObject() == constraintCSComboBox)
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
