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

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ManeuverSetupPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_COMBOBOX(ID_BURN_COMBOBOX, ManeuverSetupPanel::OnBurnComboBoxChange)
    EVT_COMBOBOX(ID_SAT_COMBOBOX, ManeuverSetupPanel::OnSatComboBoxChange)
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
ManeuverSetupPanel::ManeuverSetupPanel(wxWindow *parent, GmatCommand *cmd)
    :GmatPanel(parent)

{
    theCommand = cmd;

    Create();
    Show();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnBurnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnBurnComboBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnSatComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ManeuverSetupPanel::OnSatComboBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 *
 * @note Creates the panel for the Maneuver Command
 */
//------------------------------------------------------------------------------
void ManeuverSetupPanel::Create()
{
    unsigned int i, count;
    StringArray items;

    if (theCommand != NULL)
    {
    
        // create sizers
        wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer *burnSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer *spacecraftSizer = new wxBoxSizer(wxHORIZONTAL);

        //----------------------------------------------------------------------
        // Burns
        //----------------------------------------------------------------------
        // create burn label
        wxStaticText *burnLabel = new wxStaticText(this, ID_TEXT,
            wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
        
        // list of burns
        items = theGuiInterpreter->GetListOfConfiguredItems(Gmat::BURN);
        count = items.size();
        wxString *burnList = new wxString[count];

        // create burn combo box
        if (count > 0)  // check to see if any burns exist
        {
            for (i=0; i<count; i++)
                burnList[i] = items[i].c_str();

            // combo box for avaliable burns 
            burnCB = new wxComboBox(this, ID_BURN_COMBOBOX, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), count, burnList, wxCB_DROPDOWN);
        }
        else            // no burns available
        {
            wxString strs9[] =
            {
                wxT("No burn available"), 
            }; 
            burnCB = new wxComboBox(this, ID_BURN_COMBOBOX, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), 1, strs9, wxCB_DROPDOWN);
        }               

        // add burn label and combobox to burn sizer
        burnSizer->Add(burnLabel, 0, wxALIGN_CENTER | wxALL, 5);
        burnSizer->Add(burnCB, 0, wxALIGN_CENTER | wxALL, 5);

        //----------------------------------------------------------------------
        // Spacecraft
        //----------------------------------------------------------------------
        // create spacecraft label
        wxStaticText *spacecraftLabel = new wxStaticText(this, ID_TEXT,
            wxT("To"), wxDefaultPosition, wxDefaultSize, 0);

        // list of spacecraft
        items = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
        count = items.size();
        wxString *spacecraftList = new wxString[count];

        // create spacecraft combo box
        if (count > 0)  // check to see if any spacecrafts exist
        {
            for (i=0; i<count; i++)
                spacecraftList[i] = items[i].c_str();
            // combo box for avaliable spacecrafts 
            satCB = new wxComboBox(this, ID_SAT_COMBOBOX, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), count, spacecraftList, wxCB_DROPDOWN);
        }
        else
        {
            // combo box for avaliable spacecrafts 
            wxString strs9[] =
            {
                wxT("No spacecraft available"), 
            }; 
            satCB = new wxComboBox(this, ID_SAT_COMBOBOX, wxT(""), wxDefaultPosition, 
                wxSize(150,-1), 1, strs9, wxCB_DROPDOWN);
        }
                       
        // add spacecraft label and combobox to spacecraft sizer
        spacecraftSizer->Add(spacecraftLabel, 0, wxALIGN_CENTER | wxALL, 5);
        spacecraftSizer->Add(satCB, 0, wxALIGN_CENTER | wxALL, 5);
    
        // add items to page sizer
        pageBoxSizer->Add(burnSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
        pageBoxSizer->Add(spacecraftSizer, 0, wxGROW | wxALIGN_RIGHT | wxALL, 5);

        // add to middle sizer
        theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);     

    }  // theCommand != NULL
    else
    {
       // show error message
    }
    
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ManeuverSetupPanel::LoadData()
{
    // load data from the core engine
    Integer id;
    int index;
    
    // default values
    burnCB->SetSelection(0);
    satCB->SetSelection(0);

    // burn 
    id = theCommand->GetParameterID("Burn");
    std::string burn = theCommand->GetStringParameter(id);
//    StringArray burnList = theCommand->GetStringArrayParameter(id);
    StringArray burnList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::BURN);
    index = 0;
    for (StringArray::iterator iter = burnList.begin(); 
        iter != burnList.end(); ++iter) 
    {
        if (burn == *iter) 
            burnCB->SetSelection(index);
        else
            ++index;
    }

    // spacecraft
    id = theCommand->GetParameterID("Spacecraft");
    std::string sat = theCommand->GetStringParameter(id);
    StringArray satList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    index = 0;
    for (StringArray::iterator iter = satList.begin(); 
        iter != satList.end(); ++iter) 
    {
        if (sat == *iter) 
            satCB->SetSelection(index);
        else
            ++index;
    }
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ManeuverSetupPanel::SaveData()
{
    // save data to core engine
    Integer id;
    wxString elemString;

    // save burn
    wxString burnString = burnCB->GetStringSelection();
    id = theCommand->GetParameterID("Burn");
    std::string burn = std::string (burnString.c_str());
    theCommand->SetStringParameter(id, burn);

    // save spacecraft
    wxString satString = satCB->GetStringSelection();
    id = theCommand->GetParameterID("Spacecraft");
    std::string spacecraft = std::string (satString.c_str());
    theCommand->SetStringParameter(id, spacecraft);

}



