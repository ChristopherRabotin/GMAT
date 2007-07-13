//$Header$
//------------------------------------------------------------------------------
//                            SpacecraftPanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
// Modified: 2003/12/15 by Allison Greene for event handling
/**
 * This class contains information needed to setup users spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------
#include "SpacecraftPanel.hpp"
#include "GmatAppData.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"
#include "RealUtilities.hpp"

#include <stdlib.h>

//#define DEBUG_SPACECRAFT_PANEL 1

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// SpacecraftPanel(wxWindow *parent, const wxString &scName)
//------------------------------------------------------------------------------
/**
 * Constructs SpacecraftPanel object.
 *
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the spacecraft GUI
 */
//------------------------------------------------------------------------------
SpacecraftPanel::SpacecraftPanel(wxWindow *parent, const wxString &scName)
    :GmatPanel(parent)
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::SpacecraftPanel() entered\n");
   #endif
   
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
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
SpacecraftPanel::~SpacecraftPanel()
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::~SpacecraftPanel() entered\n");
   #endif
   // need to delete child from list in mainFrame
   //    delete(theBallisticMassPanel);
   //    delete(theOrbitPanel);
   //    delete(currentSpacecraft);
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void SpacecraftPanel::Create()
{
   #if DEBUG_SPACECRAFT_PANEL
   MessageInterface::ShowMessage("SpacecraftPanel::Create() entered\n");
   #endif

   SolarSystem *theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   currentSpacecraft = new Spacecraft(*theSpacecraft);
   
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
   // wxNotebookSizer
   // spacecraftSizer = new wxNotebookSizer( spacecraftNotebook );
   // actuatorSizer = new wxNotebookSizer( actuatorNotebook );
   
   // wxSizer
   // wxGridSizer *theGridSizer = new wxGridSizer( 1, 0, 0 );
   
   //wx*Panel
   sensors = new wxPanel( spacecraftNotebook, -1 );
   
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
   
   // visuals = new wxPanel( mainNotebook, -1 );
   
   // Adding panels to notebook
   actuatorNotebook->AddPage( theThrusterPanel, wxT("Thruster") );
   spacecraftNotebook->AddPage( theOrbitPanel, wxT("Orbit") );
   spacecraftNotebook->AddPage( theAttitudePanel, wxT("Attitude") );
   spacecraftNotebook->AddPage( theBallisticMassPanel, wxT("Ballistic/Mass") );
   spacecraftNotebook->AddPage( sensors, wxT("Sensors") );
   spacecraftNotebook->AddPage( theTankPanel, wxT("Tanks") );
   spacecraftNotebook->AddPage( actuatorNotebook, wxT("Actuators") );
   //spacecraftNotebook->AddPage( visuals , wxT("Visualization") );
   
   // theGridSizer->Add(spacecraftNotebook, 1, wxGROW, 5);
   theMiddleSizer->Add(spacecraftNotebook, 1, wxGROW, 5);
}


//------------------------------------------------------------------------------
// void LoadData()
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
      theThrusterPanel->LoadData();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
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
   
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void OnPageChange()
//------------------------------------------------------------------------------
void SpacecraftPanel::OnPageChange(wxCommandEvent &event)
{
   theTankPanel->LoadData();
   theThrusterPanel->LoadData();
   theAttitudePanel->LoadData();
}    
