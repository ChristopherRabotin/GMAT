//$Header Exp $
//------------------------------------------------------------------------------
//                              ManeuverSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2003/12/01
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#include "ManeuverSetupPanel.hpp"
//#include "GmatAppData.hpp"
//#include "gmatdefs.hpp" //put this one after GUI includes
//#include "GuiInterpreter.hpp"
//#include "Command.hpp"
//#include "MessageInterface.hpp"
//#include "RealUtilities.hpp"

//#include <stdlib.h>

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ManeuverSetupPanel, wxPanel)
    EVT_BUTTON(ID_BUTTON_OK, ManeuverSetupPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, ManeuverSetupPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, ManeuverSetupPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, ManeuverSetupPanel::OnScript)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ManeuverSetupPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs ManeuverSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the maneuver dialog box
 */
//------------------------------------------------------------------------------
ManeuverSetupPanel::ManeuverSetupPanel(wxWindow *parent, Command *cmd)
    :wxPanel(parent)
{
    theCommand = cmd;
    
    CreateManeuver(this );
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void CreateImpulsive(wxWindow *parent, Command *cmd)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 *
 * @note Creates the panel for the Impulsive notebook page
 * @note Use impulsivePanel as the parent of all the objects
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::CreateManeuver(wxWindow *parent)
{
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();

    //loj: 2/9/04 theCommand = theGuiInterpreter->CreateCommand("Maneuver", "Maneuver1");
    
//    theCommand = theGuiInterpreter->GetCommand(std::string(maneuverName.c_str()));

//    if (theCommand != NULL)
//    {
        wxFlexGridSizer *impulsiveSizer = new wxFlexGridSizer(3, 1, 0, 0 );
//        impulsiveSizer->AddGrowableCol(0);
//        impulsiveSizer->AddGrowableCol(1);
//        impulsiveSizer->AddGrowableRow(0);
        impulsiveSizer->AddGrowableRow(1);

        // create script button
        wxFlexGridSizer *topSizer = new wxFlexGridSizer(1, 2, 0, 0);
        topSizer->AddGrowableCol(0);
        
        scriptButton = new wxButton(parent, ID_BUTTON_SCRIPT, "Create Script", 
            wxDefaultPosition, wxDefaultSize, 0);
    
        // button not functional, yet 
        scriptButton->Disable();
        
        // create spacers
/*        wxBoxSizer *spacerSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText *spacer1 = new wxStaticText(parent, ID_TEXT,
            wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        wxStaticText *spacer2 = new wxStaticText(parent, ID_TEXT,
            wxT(""), wxDefaultPosition, wxDefaultSize, 0);
        wxStaticText *spacer3 = new wxStaticText(parent, ID_TEXT,
            wxT(""), wxDefaultPosition, wxDefaultSize, 0);

        spacerSizer->Add(spacer1, 0, wxALIGN_CENTER | wxALL, 5);
        spacerSizer->Add(spacer2, 0, wxALIGN_CENTER | wxALL, 5);
        spacerSizer->Add(spacer3, 0, wxALIGN_CENTER | wxALL, 5);
*/  
        // add items to top sizer
        topSizer->Add( 200, 20, 0, wxALIGN_CENTRE|wxALL, 5);
        topSizer->Add(scriptButton, 0, wxALIGN_RIGHT | wxALL, 5);

        // create burn label and combo box
        wxBoxSizer *burnSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText *item44 = new wxStaticText(parent, ID_TEXT,
            wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
        
        // list of burns
        StringArray &listBurns = theGuiInterpreter->GetListOfConfiguredItems(Gmat::BURN);
        int listSize = listBurns.size();
        if (listSize > 0)  // check to see if any burns exist
        {
            wxString choices[listSize];
            for (int i=0; i<listSize; i++)
                choices[i] = wxString(listBurns[i].c_str());
            // combo box for avaliable burns 
            burnCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), listSize, choices, wxCB_DROPDOWN);
        }
        else
        {
            // no burns available 
            wxString strs9[] =
            {
                wxT("No Burns Available"), 
            }; 
            burnCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), 1, strs9, wxCB_DROPDOWN);
        }               

        burnSizer->Add(item44, 0, wxALIGN_CENTER | wxALL, 5);
        burnSizer->Add(burnCB, 0, wxALIGN_CENTER | wxALL, 5);

        // create spacecraft label and combo box
        wxBoxSizer *spacecraftSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText *item41 = new wxStaticText(parent, ID_TEXT,
            wxT("To"), wxDefaultPosition, wxDefaultSize, 0);
        // list of spacecrafts
        StringArray &listSat = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
        listSize = listSat.size();
        if (listSize > 0)  // check to see if any spacecrafts exist
        {
            wxString choices[listSize];
            for (int i=0; i<listSize; i++)
                choices[i] = wxString(listSat[i].c_str());
            // combo box for avaliable spacecrafts 
            satCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), listSize, choices, wxCB_DROPDOWN);
        }
        else
        {
            // combo box for avaliable spacecrafts 
            wxString strs9[] =
            {
                wxT("No Spacecrafts Available"), 
            }; 
            satCB = new wxComboBox(parent, ID_COMBO, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), 1, strs9, wxCB_DROPDOWN);
        }               
        spacecraftSizer->Add(item41, 0, wxALIGN_CENTER | wxALL, 5);
        spacecraftSizer->Add(satCB, 0, wxALIGN_CENTER | wxALL, 5);
    
        // add items to middle sizer
        wxGridSizer *middleSizer = new wxGridSizer(1, 2, 0, 0);
        middleSizer->Add(burnSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
        middleSizer->Add(spacecraftSizer, 0, wxGROW | wxALIGN_RIGHT | wxALL, 5);
        
        // create buttons
        wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        okButton = new wxButton(parent, ID_BUTTON_OK, "OK", 
            wxDefaultPosition, wxDefaultSize, 0);
        applyButton = new wxButton(parent, ID_BUTTON_APPLY, "Apply", 
            wxDefaultPosition, wxDefaultSize, 0);
        cancelButton = new wxButton(parent, ID_BUTTON_CANCEL, "Cancel", 
            wxDefaultPosition, wxDefaultSize, 0);
        helpButton = new wxButton(parent, ID_BUTTON_HELP, "Help", 
            wxDefaultPosition, wxDefaultSize, 0);
        
        // adds the buttons to button sizer    
        buttonSizer->Add(okButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
        buttonSizer->Add(applyButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
        buttonSizer->Add(cancelButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);
        buttonSizer->Add(helpButton, 0, wxGROW | wxALIGN_CENTER | wxALL, 5);

        // add items to panel sizer
        impulsiveSizer->Add(topSizer, 0, wxGROW | wxALL, 5);
        impulsiveSizer->Add(middleSizer, 0, wxGROW | wxALL, 5);
        impulsiveSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        impulsiveSizer->Fit(parent);

        // tells the enclosing window to adjust to the size of the sizer
        impulsiveSizer->SetSizeHints(parent);
        parent->SetSizer(impulsiveSizer);
        parent->SetAutoLayout( TRUE );

        // default values for now
        burnCB->SetSelection(0);
        satCB->SetSelection(0);

//    }  // theCommand->GetTypeName()
//    else
//    {
       // show error message
//    }

}

//------------------------------------------------------------------------------
// void OnOk()
//------------------------------------------------------------------------------
/**
 * 
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnOK()
{
    OnApply();
    
    // Close page from main notebook    
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();

}

//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * @note Does not save entered data and destroys the notebook
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnCancel()
{
    // Close page from main notebook
    GmatMainNotebook *gmatMainNotebook = GmatAppData::GetMainNotebook();
    gmatMainNotebook->ClosePage();
}

//------------------------------------------------------------------------------
// void OnApply()
//------------------------------------------------------------------------------
/**
 * @note Saves the data to theCommand
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnApply()
{
    // will implement later
    //loj:
    // LaMont,
    // Use theCommand->GetParameterID("Spacecraft") and call SetStringParameter()
    // to set spacecraft name.
    // Do the same for the "Burn"
}

//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * @note Writes data in script form
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnScript()
{
    // will implement later
}

