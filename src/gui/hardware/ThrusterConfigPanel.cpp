//$Id$
//------------------------------------------------------------------------------
//                            ThrusterConfigPanel
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
// Created: 2005/01/06
// Modifications:
//    2009.02.10  Linda Jun - Derived from the BurnThrusterPanel
//
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "ThrusterConfigPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURNPANEL_SAVE

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ThrusterConfigPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs ThrusterConfigPanel object.
 */
//------------------------------------------------------------------------------
ThrusterConfigPanel::ThrusterConfigPanel(wxWindow *parent, 
                                         const wxString &name)
   : BurnThrusterPanel(parent, name)
{
   theObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage
      ("ThrusterConfigPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif
   
   // To set panel object and show warning if object is NULL
   if (SetObject(theObject))
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~ThrusterConfigPanel()
//------------------------------------------------------------------------------
ThrusterConfigPanel::~ThrusterConfigPanel()
{
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::LoadData()
{
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ThrusterConfigPanel::LoadData() entered\n");
   #endif
   
   try
   {
      Integer paramID;
      paramID = theObject->GetParameterID("DutyCycle");
      dutyCycleTextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID("ThrustScaleFactor");
      scaleFactorTextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   BurnThrusterPanel::LoadData();
   
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ThrusterConfigPanel::LoadData() entered\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::SaveData()
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ThrusterConfigPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
   Real dutyCycle, scaleFactor;
   Integer paramID;
   bool dutyCycleScaleFactorChanged = false;
   
   try 
   {
      if (dutyCycleTextCtrl->IsModified() || scaleFactorTextCtrl->IsModified())
      {
         str = dutyCycleTextCtrl->GetValue();      
         CheckReal(dutyCycle, str, "DutyCycle", "Real Number");
         
         str = scaleFactorTextCtrl->GetValue();      
         CheckReal(scaleFactor, str, "ThrustScaleFactor", "Real Number");
         
         dutyCycleScaleFactorChanged = true;
      }
      
      if (!canClose)
         return;
      
      if (dutyCycleScaleFactorChanged)
      {
         paramID = theObject->GetParameterID("DutyCycle");
         theObject->SetRealParameter(paramID, dutyCycle);
      
         paramID = theObject->GetParameterID("ThrustScaleFactor");
         theObject->SetRealParameter(paramID, scaleFactor);
      }
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   if (canClose) BurnThrusterPanel::SaveData();
   
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ThrusterConfigPanel::SaveData() exiting\n");
   #endif
}
