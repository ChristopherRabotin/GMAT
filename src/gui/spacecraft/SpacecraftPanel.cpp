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
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    theSpacecraft = theGuiInterpreter->GetSpacecraft(std::string(scName.c_str()));
    
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
    SolarSystem *theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
    currentSpacecraft = new Spacecraft(*theSpacecraft);

    // Set object pointer for "Show Script"
    mObject = currentSpacecraft;

    // wxNotebook
    spacecraftNotebook = new wxNotebook( this, ID_NOTEBOOK, wxDefaultPosition,
                         wxDefaultSize, wxGROW );
    spacecraftNotebook->SetBackgroundColour(GetBackgroundColour());
//    spacecraftNotebook->SetForegroundColour(GetBackgroundColour());
    spacecraftNotebook->SetForegroundColour(GetForegroundColour());

//    actuators = new wxPanel( spacecraftNotebook, -1 );
    actuatorNotebook = new wxNotebook( spacecraftNotebook, ID_NOTEBOOK, wxDefaultPosition,
                         wxDefaultSize, wxGROW );
    actuatorNotebook->SetBackgroundColour(GetBackgroundColour());
    actuatorNotebook->SetForegroundColour(GetBackgroundColour());
    // wxNotebookSizer
//    spacecraftSizer = new wxNotebookSizer( spacecraftNotebook );
//    actuatorSizer = new wxNotebookSizer( actuatorNotebook );

    // wxSizer
//    wxGridSizer *theGridSizer = new wxGridSizer( 1, 0, 0 );

    //wx*Panel
    attitude = new wxPanel( spacecraftNotebook, -1 );
    sensors = new wxPanel( spacecraftNotebook, -1 );

    theOrbitPanel = new OrbitPanel(spacecraftNotebook, currentSpacecraft,
                        theSolarSystem, theApplyButton);
    theBallisticMassPanel = new BallisticsMassPanel(spacecraftNotebook,
                                currentSpacecraft, theApplyButton);
    theTankPanel = new TankPanel(spacecraftNotebook, currentSpacecraft,
                       theApplyButton);
    theThrusterPanel = new ThrusterPanel(actuatorNotebook, currentSpacecraft,
                       theApplyButton);
    // visuals = new wxPanel( mainNotebook, -1 );

    // Adding panels to notebook
    actuatorNotebook->AddPage( theThrusterPanel, wxT("Thruster") );
    spacecraftNotebook->AddPage( theOrbitPanel, wxT("Orbit") );
    spacecraftNotebook->AddPage( attitude, wxT("Attitude") );
    spacecraftNotebook->AddPage( theBallisticMassPanel, wxT("Ballistic/Mass") );
    spacecraftNotebook->AddPage( sensors, wxT("Sensors") );
    spacecraftNotebook->AddPage( theTankPanel, wxT("Tanks") );
    spacecraftNotebook->AddPage( actuatorNotebook, wxT("Actuators") );
    //spacecraftNotebook->AddPage( visuals , wxT("Visualization") );

//    theGridSizer->Add(spacecraftNotebook, 1, wxGROW, 5);
    theMiddleSizer->Add(spacecraftNotebook, 1, wxGROW, 5);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void SpacecraftPanel::LoadData()
{
    theOrbitPanel->LoadData();
    theBallisticMassPanel->LoadData();
    theTankPanel->LoadData();
    theThrusterPanel->LoadData();
    
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void SpacecraftPanel::SaveData()
{
    theBallisticMassPanel->SaveData();
    canClose = theBallisticMassPanel->canClose;
    enableApply=true;

    if (!canClose)
    {
      enableApply=false;
      return;
    }
      
    theOrbitPanel->SaveData();
    theTankPanel->SaveData(); 
    theThrusterPanel->SaveData();
      
    // what's wrong with this?
    // copy the current info into theSpacecraft
    delete(theSpacecraft); 
    theSpacecraft = new Spacecraft(*currentSpacecraft);
    //theSpacecraft = currentSpacecraft;
    
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

//------------------------------------------------------------------------------
// void OnPageChange()
//------------------------------------------------------------------------------
void SpacecraftPanel::OnPageChange(wxCommandEvent &event)
{
    theTankPanel->LoadData();
    theThrusterPanel->LoadData();
}    
