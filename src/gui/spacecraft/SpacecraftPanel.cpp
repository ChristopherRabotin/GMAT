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

#include "gmatdefs.hpp" //put this one after GUI includes
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"
#include "RealUtilities.hpp"

#include "MessageInterface.hpp"

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
    else
    {
        // show error message
    }
}

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
void SpacecraftPanel::Create()
{
    currentSpacecraft = new Spacecraft(*theSpacecraft);
    
    spacecraftNotebook = new wxNotebook( this, ID_NOTEBOOK, wxDefaultPosition, 
                         wxSize(350,300), wxGROW );
    sizer = new wxNotebookSizer( spacecraftNotebook );
 
    SolarSystem *theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
//    MessageInterface::ShowMessage
//         ("SpacecraftPanel:: got the solar sys\n");
    theOrbitPanel = new OrbitPanel(spacecraftNotebook, currentSpacecraft, 
                                   theSolarSystem,
                                   theApplyButton);
    spacecraftNotebook->AddPage( theOrbitPanel, wxT("Orbit") );

    attitude = new wxPanel( spacecraftNotebook, -1 );
    spacecraftNotebook->AddPage( attitude, wxT("Attitude") );

    theBallisticMassPanel = 
                    new BallisticsMassPanel(spacecraftNotebook, currentSpacecraft,
                                    theApplyButton);
    spacecraftNotebook->AddPage( theBallisticMassPanel, wxT("Ballistic/Mass") );

    actuators = new wxPanel( spacecraftNotebook, -1 );
    spacecraftNotebook->AddPage( actuators, wxT("Actuators") );

    sensors = new wxPanel( spacecraftNotebook, -1 );
    spacecraftNotebook->AddPage( sensors, wxT("Sensors") );

    tanks = new wxPanel( spacecraftNotebook, -1 );
    spacecraftNotebook->AddPage( tanks, wxT("Tanks") );

//    visuals = new wxPanel( mainNotebook, -1 );
    spacecraftNotebook->AddPage( tanks, wxT("Visualization") );

    theMiddleSizer->Add(sizer, 0, wxGROW, 5);
}


void SpacecraftPanel::LoadData()
{
    theOrbitPanel->LoadData();
    theBallisticMassPanel->LoadData();
    
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}

void SpacecraftPanel::SaveData()
{
    theOrbitPanel->SaveData();
    theBallisticMassPanel->SaveData();   
     
    // what's wrong with this?
      // copy the current info into theSpacecraft
    delete(theSpacecraft);
    theSpacecraft = new Spacecraft(*currentSpacecraft);
    
    // explicitly disable apply button
    // it is turned on in each of the panels
    theApplyButton->Disable();
}
