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
#include "GmatMainNotebook.hpp"

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
    
    mainNotebook = new wxNotebook( this, ID_NOTEBOOK, wxDefaultPosition, 
                         wxSize(350,300), wxGROW );
    sizer = new wxNotebookSizer( mainNotebook );
 
    SolarSystem *theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
    MessageInterface::ShowMessage
         ("SpacecraftPanel:: got the solar sys\n");
    theOrbitPanel = new OrbitPanel(mainNotebook, currentSpacecraft, 
                                   theSolarSystem,
                                   theApplyButton);
    mainNotebook->AddPage( theOrbitPanel, wxT("Orbit") );

    attitude = new wxPanel( mainNotebook, -1 );
    mainNotebook->AddPage( attitude, wxT("Attitude") );

    theBallisticMassPanel = 
                    new BallisticsMassPanel(mainNotebook, currentSpacecraft,
                                    theApplyButton);
    mainNotebook->AddPage( theBallisticMassPanel, wxT("Ballistic/Mass") );

    actuators = new wxPanel( mainNotebook, -1 );
    mainNotebook->AddPage( actuators, wxT("Actuators") );

    sensors = new wxPanel( mainNotebook, -1 );
    mainNotebook->AddPage( sensors, wxT("Sensors") );

    tanks = new wxPanel( mainNotebook, -1 );
    mainNotebook->AddPage( tanks, wxT("Tanks") );

//    visuals = new wxPanel( mainNotebook, -1 );
    mainNotebook->AddPage( tanks, wxT("Visualization") );

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
