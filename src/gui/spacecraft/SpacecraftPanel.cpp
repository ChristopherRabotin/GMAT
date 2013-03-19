//$Id$
//------------------------------------------------------------------------------
//                            SpacecraftPanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
// Modified: 2003/12/15 by Allison Greene for event handling
/**
 * This class contains information needed to setup spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------

#include "SpacecraftPanel.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include <stdlib.h>

//#define DEBUG_SPACECRAFT_PANEL 1
//#define DEBUG_SC_PANEL_EVENT
//#define DEBUG_SPACECRAFT_CLONE_AND_COPY


//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// SpacecraftPanel(wxWindow *parent, const wxString &scName)
//------------------------------------------------------------------------------
/**
 * Constructs a SpacecraftPanel object.
 *
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 */
//------------------------------------------------------------------------------
SpacecraftPanel::SpacecraftPanel(wxWindow *parent, const wxString &scName)
    : GmatPanel(parent)
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage
      ("SpacecraftPanel::SpacecraftPanel() entered. scName='%s', theGuiInterpreter=<%p>\n",
       scName.c_str(), theGuiInterpreter);
   #endif
   
   theSpacecraft =
      (Spacecraft*)theGuiInterpreter->GetConfiguredObject(std::string(scName.c_str()));
   
   if (theSpacecraft != NULL)
   {            
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~SpacecraftPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the SpacecraftPanel object.
 *
 */
//------------------------------------------------------------------------------
SpacecraftPanel::~SpacecraftPanel()
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::~SpacecraftPanel() entered\n");
   #endif
   
   delete currentSpacecraft;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the GUI components for the SpacecraftPanel object.
 *
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::Create()
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::Create() entered\n");
   #endif
   
   SolarSystem *theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   currentSpacecraft = new Spacecraft(*theSpacecraft);
   #ifdef DEBUG_SPACECRAFT_CLONE_AND_COPY
   MessageInterface::ShowMessage(
         "SpacecraftPanel::Create() copied spacecraft %s<%p> to new spacecraft %s<%p>\n",
         theSpacecraft->GetName().c_str(), theSpacecraft,
         currentSpacecraft->GetName().c_str(), currentSpacecraft);
   #endif
   
   try
   {
      currentSpacecraft->
         SetInternalCoordSystem(theSpacecraft->GetInternalCoordSystem());    
      currentSpacecraft->
         SetRefObject(theSpacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""), 
                      Gmat::COORDINATE_SYSTEM, "");
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   // wxNotebook
   spacecraftNotebook = new
      wxNotebook( this, ID_NOTEBOOK, wxDefaultPosition,
                  wxDefaultSize, wxGROW );
   spacecraftNotebook->SetBackgroundColour(GetBackgroundColour());
   spacecraftNotebook->SetForegroundColour(GetForegroundColour());
   
   actuatorNotebook = new
      wxNotebook( spacecraftNotebook, ID_NOTEBOOK, wxDefaultPosition,
                  wxDefaultSize, wxGROW );
   actuatorNotebook->SetBackgroundColour(GetBackgroundColour());
   actuatorNotebook->SetForegroundColour(GetBackgroundColour());
   
   //wx*Panel
   sensors = NULL;
//   sensors = new wxPanel( spacecraftNotebook, -1 );
   
   theOrbitPanel = new OrbitPanel
      (this, spacecraftNotebook, currentSpacecraft, theSolarSystem);
   
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("   OrbitPanel created\n");
   #endif
   
   theAttitudePanel = new AttitudePanel
      (this, spacecraftNotebook, currentSpacecraft);
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("   AttitudePanel created\n");
   #endif
   
   theBallisticMassPanel = new BallisticsMassPanel
      (this, spacecraftNotebook, currentSpacecraft);
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("   BallisticsMassPanel created\n");
   #endif
   
   theTankPanel = new TankPanel
      (this, spacecraftNotebook, currentSpacecraft);
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("   TankPanel created\n");
   #endif
   
   theThrusterPanel = new ThrusterPanel
      (this, actuatorNotebook, currentSpacecraft);
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("   ThrusterPanel created\n");
   #endif
   #ifdef __USE_SPICE__
      theSpicePanel = new SpicePanel
         (this, spacecraftNotebook, currentSpacecraft);
   #endif
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("   SpicePanel created\n");
   #endif

   theVisualModelPanel = new VisualModelPanel
      (this, spacecraftNotebook, currentSpacecraft, theSolarSystem);
   
   // Adding panels to notebook
   actuatorNotebook->AddPage( theThrusterPanel, wxT("Thruster") );
   spacecraftNotebook->AddPage( theOrbitPanel, wxT("Orbit") );
   spacecraftNotebook->AddPage( theAttitudePanel, wxT("Attitude") );
   spacecraftNotebook->AddPage( theBallisticMassPanel, wxT("Ballistic/Mass") );
//   spacecraftNotebook->AddPage( sensors, wxT("Sensors") );
   spacecraftNotebook->AddPage( theTankPanel, wxT("Tanks") );
   #ifdef __USE_SPICE__
      spacecraftNotebook->AddPage( theSpicePanel, wxT("SPICE") );
   #endif
   spacecraftNotebook->AddPage( actuatorNotebook, wxT("Actuators") );
   spacecraftNotebook->AddPage( theVisualModelPanel , wxT("Visualization") );
   
   theMiddleSizer->Add(spacecraftNotebook, 1, wxGROW, 5);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data into the tabs on the SpacecraftPanel object.
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::LoadData()
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::LoadData() entered\n");
   #endif
   
   // Set object pointer for "Show Script"
   mObject = theSpacecraft;
   
   try
   {
      theOrbitPanel->LoadData();
      theAttitudePanel->LoadData();
      theBallisticMassPanel->LoadData();
      theTankPanel->LoadData();
      #ifdef __USE_SPICE__
         theSpicePanel->LoadData();
      #endif
      theThrusterPanel->LoadData();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
   #ifdef __WXMAC__
      // this is needed for the Mac, as the VisualModelCanvas was messing up the other tabs
      theVisualModelPanel->CanvasOn(false);
      theVisualModelPanel->CanvasOn(true);
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves modified data from the SpacecraftPanel tabs to the Spacecraft object.
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::SaveData()
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::SaveData() entered\n");
   MessageInterface::ShowMessage
      ("BallisticMassPanelChanged=%d, OrbitPanelChanged=%d, TankPanelChanged=%d, "
       "ThrusterPanelChanged=%d, AttitudePanelChanged=%d\n",
       theBallisticMassPanel->IsDataChanged(), theOrbitPanel->IsDataChanged(),
       theTankPanel->IsDataChanged(), theThrusterPanel->IsDataChanged(),
       theAttitudePanel->IsDataChanged());
   #endif
   
   canClose = true;
   
   if (theBallisticMassPanel->IsDataChanged())
   {
      theBallisticMassPanel->SaveData();
      canClose = theBallisticMassPanel->CanClosePanel();
   }

   if (theOrbitPanel->IsDataChanged())
   {
      theOrbitPanel->SaveData();
      canClose = canClose && theOrbitPanel->CanClosePanel();
   }
   
   if (theAttitudePanel->IsDataChanged())
   {
      theAttitudePanel->SaveData();
      canClose = canClose && theAttitudePanel->CanClosePanel();
   }
#ifdef __USE_SPICE__
   if (theSpicePanel->IsDataChanged())
   {
      theSpicePanel->SaveData();
      canClose = canClose && theSpicePanel->CanClosePanel();
   }
#endif
   
   if (theVisualModelPanel->IsDataChanged())
   {
      theVisualModelPanel->SaveData();
      canClose = canClose && theVisualModelPanel->CanClosePanel();
   }

   if (!canClose)
   {
      EnableUpdate(true);
      return;
   }
   
   if (theTankPanel->IsDataChanged())
      theTankPanel->SaveData();

   if (theThrusterPanel->IsDataChanged())
      theThrusterPanel->SaveData();
   
   // copy the current info into theSpacecraft
   theSpacecraft->Copy(currentSpacecraft);
   #ifdef DEBUG_SPACECRAFT_CLONE_AND_COPY
      MessageInterface::ShowMessage("Copied data from SC %s<%p> back to SC %s<%p>\n",
            currentSpacecraft->GetName().c_str(), currentSpacecraft,
            theSpacecraft->GetName().c_str(), theSpacecraft);
   #endif
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void OnPageChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the page change event
 *
 * @param <event> the event to handle
 */
//------------------------------------------------------------------------------
void SpacecraftPanel::OnPageChange(wxCommandEvent &event)
{
   theTankPanel->LoadData();
   theThrusterPanel->LoadData();
   theAttitudePanel->LoadData();
   #ifdef __USE_SPICE__
      theSpicePanel->LoadData();
   #endif
}

//------------------------------------------------------------------------------
// bool RefreshObjects(Gmat::ObjectType type = Gmat::UNKNOWN_OBJECT)
//------------------------------------------------------------------------------
/**
 * Refreshes the tabs on the panel.  This is called when the coordinate system
 * list is updated by the GuiItemManager.
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool SpacecraftPanel::RefreshObjects(Gmat::ObjectType type)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      // Only OrbitPanel needs refreshing at this time
      return theOrbitPanel->RefreshComponents();
   }
   else
   {
      return GmatPanel::RefreshObjects(type);
   }
}

