//$Header$
//------------------------------------------------------------------------------
//                              ManeuverPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2003/12/01
//
/**
 * This class contains the Maneuver Setup window.
 */
//------------------------------------------------------------------------------

#include "ManeuverPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_MANEUVER_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ManeuverPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_BURN_COMBOBOX, ManeuverPanel::OnBurnComboBoxChange)
   EVT_COMBOBOX(ID_SAT_COMBOBOX, ManeuverPanel::OnSatComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ManeuverPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs ManeuverPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the maneuver dialog box
 */
//------------------------------------------------------------------------------
ManeuverPanel::ManeuverPanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)

{
   theCommand = cmd;

   if (theCommand != NULL)
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~ManeuverPanel()
//------------------------------------------------------------------------------
ManeuverPanel::~ManeuverPanel()
{
   theGuiManager->UnregisterComboBox("Burn", burnCB);
   theGuiManager->UnregisterComboBox("Spacecraft", satCB);
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnBurnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ManeuverPanel::OnBurnComboBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnSatComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ManeuverPanel::OnSatComboBoxChange(wxCommandEvent& event)
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
void ManeuverPanel::Create()
{
   #if DEBUG_MANEUVER_PANEL
   MessageInterface::ShowMessage("ManeuverPanel::Create() command=%s\n",
                                 theCommand->GetTypeName().c_str());
   #endif
   
   StringArray items;
   
   // create sizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *burnSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *spacecraftSizer = new wxBoxSizer(wxHORIZONTAL);
   
   //----------------------------------------------------------------------
   // Burns
   //----------------------------------------------------------------------
   // create burn label
   wxStaticText *burnLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0);
   
   #if DEBUG_MANEUVER_PANEL
   MessageInterface::ShowMessage
      ("ManeuverPanel::Create() Calling theGuiManager->GetImpBurnComboBox()\n");
   #endif
   
   // create burn combo box
   burnCB = theGuiManager->GetImpBurnComboBox(this, ID_BURN_COMBOBOX, wxSize(150,-1));
   
   // add burn label and combobox to burn sizer
   burnSizer->Add(burnLabel, 0, wxALIGN_CENTER | wxALL, 5);
   burnSizer->Add(burnCB, 0, wxALIGN_CENTER | wxALL, 5);

   //----------------------------------------------------------------------
   // Spacecraft
   //----------------------------------------------------------------------
   // create spacecraft label
   wxStaticText *spacecraftLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("To"), wxDefaultPosition, wxDefaultSize, 0);

   #if DEBUG_MANEUVER_PANEL
   MessageInterface::ShowMessage
      ("ManeuverPanel::Create() Calling theGuiManager->GetSpacecraftComboBox()\n");
   #endif
   
   // create spacecraft combo box
   satCB = theGuiManager->GetSpacecraftComboBox(this, ID_SAT_COMBOBOX, wxSize(150,-1));
   
   // add spacecraft label and combobox to spacecraft sizer
   spacecraftSizer->Add(spacecraftLabel, 0, wxALIGN_CENTER | wxALL, 5);
   spacecraftSizer->Add(satCB, 0, wxALIGN_CENTER | wxALL, 5);
   
   // add items to page sizer
   pageBoxSizer->Add(burnSizer, 0, wxGROW | wxALIGN_LEFT | wxALL, 5);
   pageBoxSizer->Add(spacecraftSizer, 0, wxGROW | wxALIGN_RIGHT | wxALL, 5);
   
   // add to middle sizer
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);     
    
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ManeuverPanel::LoadData()
{
   // load data from the core engine
   Integer id;
   int index;
    
   // Set the pointer for the "Show Script" button
   mObject = theCommand;

   // default values
   //burnCB->SetSelection(0);
   //satCB->SetSelection(0);

   // burn 
   id = theCommand->GetParameterID("Burn");
   std::string burn = theCommand->GetStringParameter(id);
   //StringArray burnList = theCommand->GetStringArrayParameter(id);
   //MessageInterface::ShowMessage("ManeuverPanel::LoadData() burn=%s\n", burn.c_str());
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

   if (burn == "")
   {
      burnCB->SetValue("Select a burn");
   }
    
   // spacecraft
   id = theCommand->GetParameterID("Spacecraft");
   std::string sat = theCommand->GetStringParameter(id);
   //MessageInterface::ShowMessage("ManeuverPanel::LoadData() sat=%s\n", sat.c_str());
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

   if (sat == "")
   {
      satCB->SetValue("Select a spacecraft");
   }

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ManeuverPanel::SaveData()
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
