//$Id$
//------------------------------------------------------------------------------
//                              CelestialBodyPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan (original author: Allison Greene 2005.07.06)
// Created: 2009.01.26
//
/**
 * Implements CelestialBodyPanel class.
 */
//------------------------------------------------------------------------------

#include "CelestialBodyPanel.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include <stdlib.h>


//#define DEBUG_PARAM_PANEL 1
//#define DEBUG_CELESBODY_SAVE 1

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CelestialBodyPanel, GmatPanel)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// CelestialBodyPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Creates the panel (default constructor).
 *
 * @param <parent>   pointer to the parent window
 * @param <name>     name for the panel
 */
//------------------------------------------------------------------------------
CelestialBodyPanel::CelestialBodyPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true)
{
   origCelestialBody = (CelestialBody*)
      theGuiInterpreter->GetConfiguredObject(name.c_str());
           
   bodyName = name.c_str();
   
   if (origCelestialBody)
   {
      isUserDefined = origCelestialBody->IsUserDefined();
            
      Create();
      Show();
   }
}

//------------------------------------------------------------------------------
// ~CelestialBodyPanel()
//------------------------------------------------------------------------------
/**
 * The destructor.
 */
//------------------------------------------------------------------------------
CelestialBodyPanel::~CelestialBodyPanel()
{
   delete theCelestialBody;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and arranges the widgets for the panel.
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::Create()
{
   // use a clone here
   theCelestialBody = (CelestialBody*) (origCelestialBody->Clone());

   try
   {
      cbNotebook = new wxNotebook(this, ID_NOTEBOOK, wxDefaultPosition, 
                   wxDefaultSize, wxGROW);
      cbNotebook->SetBackgroundColour(GetBackgroundColour());
      cbNotebook->SetForegroundColour(GetForegroundColour());
      
      // create properties, orbit, and orientation panels
      properties       = new CelestialBodyPropertiesPanel(this, cbNotebook, theCelestialBody);
      orbit            = new CelestialBodyOrbitPanel(this, cbNotebook, theCelestialBody);
      orientation      = new CelestialBodyOrientationPanel(this, cbNotebook, theCelestialBody);
      
      // add panels to notebook
      cbNotebook->AddPage(properties, wxT("Properties"));
      cbNotebook->AddPage(orbit, wxT("Orbit"));
      cbNotebook->AddPage(orientation, wxT("Orientation"));
      
      theMiddleSizer->Add(cbNotebook, 1, wxGROW, 1);  // 3?
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Error creating the CelestialBodyPanel: %s\n",
                                   (be.GetFullMessage()).c_str());
   }
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the Celestial Body object onto the widgets.
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::LoadData()
{
   try
   {
      properties->LoadData();
      orbit->LoadData();
      orientation->LoadData();
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CelestialBodyPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }

   mObject = origCelestialBody;
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves data from the widgets back to the celestial body object.
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::SaveData()
{
   #if DEBUG_CELESBODY_SAVE
      MessageInterface::ShowMessage("in CBPanel, origBody = %p, theBody = %p\n",
            origCelestialBody, theCelestialBody);
   #endif
   
   canClose = true;
   
   if (properties->IsDataChanged())
   {
      properties->SaveData();
      canClose = canClose && properties->CanClosePanel();
   }
   if (orbit->IsDataChanged())
   {
      orbit->SaveData();
      canClose = canClose && orbit->CanClosePanel();
   }
   if (orientation->IsDataChanged())
   {
      orientation->SaveData();
      canClose = canClose && orientation->CanClosePanel();
   }
   
   if (!canClose)   // why do this???? spacecraft panel did this ....
   {
      EnableUpdate(true);
      return;
   }
   // copy the current info into origCelestialBody
   origCelestialBody->Copy(theCelestialBody);
   EnableUpdate(false);

}

//------------------------------------------------------------------------------
// void OnPageChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user change the page.
 *
 * @param <event>  the handled event
 */
//------------------------------------------------------------------------------
void CelestialBodyPanel::OnPageChange(wxCommandEvent &event)
{
   properties->LoadData();
   orbit->LoadData();
   orientation->LoadData();
}

