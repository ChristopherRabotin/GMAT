//$Id$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
// Modifications:
//    2009.02.10  Linda Jun - Derived from the BurnThrusterPanel
//
/**
 * This class sets up the ImpulsiveBurn parameters
 */
//------------------------------------------------------------------------------

#include "ImpulsiveBurnSetupPanel.hpp"
#include "MessageInterface.hpp"
#include <wx/config.h>


//#define DEBUG_BURNPANEL_LOAD

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ImpulsiveBurnSetupPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs ImpulsiveBurnSetupPanel object.
 *
 * @param <parent> input parent.
 * @param <name> input burn name.
 */
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::ImpulsiveBurnSetupPanel(wxWindow *parent,
                                                 const wxString &name)
   : BurnThrusterPanel(parent, name)
{
   theObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage
      ("ImpulsiveBurnSetupPanel() constructor entered, theObject=<%p>'%s'\n",
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
// ~ImpulsiveBurnSetupPanel()
//------------------------------------------------------------------------------
ImpulsiveBurnSetupPanel::~ImpulsiveBurnSetupPanel()
{
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::Create()
{
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::Create() entered\n");
   #endif
   
   BurnThrusterPanel::Create();

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Impulsive Burn"));
   //-----------------------------------------------------------------
   // thrust vector items
   //-----------------------------------------------------------------
   XLabel->SetLabel(wxT("Element"GUI_ACCEL_KEY"1"));
   YLabel->SetLabel(wxT("Element"GUI_ACCEL_KEY"2"));
   ZLabel->SetLabel(wxT("Element"GUI_ACCEL_KEY"3"));
   XUnitLabel->SetLabel(wxT("km/s"));
   YUnitLabel->SetLabel(wxT("km/s"));
   ZUnitLabel->SetLabel(wxT("km/s"));
   vectorBoxSizer->SetLabel(wxT("Delta-V Vector"));
   elem1TextCtrl->SetToolTip(pConfig->Read(_T("Element1Hint")));
   elem2TextCtrl->SetToolTip(pConfig->Read(_T("Element2Hint")));
   elem3TextCtrl->SetToolTip(pConfig->Read(_T("Element3Hint")));
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::Create() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::LoadData()
{
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::LoadData() entered\n");
   #endif
   
   try
   {
      Integer paramID;
      paramID = theObject->GetParameterID("Isp");
      ispTextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   BurnThrusterPanel::LoadData();
   
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::LoadData() entered\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData(GmatBase *theObject)
//------------------------------------------------------------------------------
void ImpulsiveBurnSetupPanel::SaveData(GmatBase *theObject)
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
   Real isp;
   Integer paramID;
   bool ispChanged = false;
   
   try 
   {
      if (ispTextCtrl->IsModified())
      {
         str = ispTextCtrl->GetValue();      
         CheckReal(isp, str, "Isp", "Real Number >= 0", false, true, true, true);
         ispChanged = true;
      }
      
      if (!canClose)
         return;
      
      if (ispChanged)
      {
         paramID = theObject->GetParameterID("Isp");
         theObject->SetRealParameter(paramID, isp);
      }

      BurnThrusterPanel::SaveData(theObject);
      
      // Validation checks mass depletion settings
      theObject->Validate();
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ImpulsiveBurnSetupPanel::SaveData() exiting\n");
   #endif
}
