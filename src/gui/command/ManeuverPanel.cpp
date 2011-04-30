//$Id$
//------------------------------------------------------------------------------
//                              ManeuverPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
   theGuiManager->UnregisterComboBox("ImpulsiveBurn", burnCB);
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
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnSatComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ManeuverPanel::OnSatComboBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
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
   Integer bsize = 5; // border size
   
   // create sizers
   wxFlexGridSizer *pageFlexGridSizer = new wxFlexGridSizer( 2, 0, 0 );
   
   //----------------------------------------------------------------------
   // Burns
   //----------------------------------------------------------------------
   // create burn label
   wxStaticText *burnLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("Burn"), wxDefaultPosition, wxDefaultSize, 0);
   
   #if DEBUG_MANEUVER_PANEL
   MessageInterface::ShowMessage
      ("ManeuverPanel::Create() Calling theGuiManager->GetImpBurnComboBox()\n");
   #endif
   
   // create burn combo box
   burnCB = theGuiManager->GetImpBurnComboBox(this, ID_BURN_COMBOBOX, wxSize(150,-1));
   
   //----------------------------------------------------------------------
   // Spacecraft
   //----------------------------------------------------------------------
   // create spacecraft label
   wxStaticText *spacecraftLabel =
      new wxStaticText(this, ID_TEXT,
                       wxT("Spacecraft"), wxDefaultPosition, wxDefaultSize, 0);

   #if DEBUG_MANEUVER_PANEL
   MessageInterface::ShowMessage
      ("ManeuverPanel::Create() Calling theGuiManager->GetSpacecraftComboBox()\n");
   #endif
   
   // create spacecraft combo box
   satCB = theGuiManager->GetSpacecraftComboBox(this, ID_SAT_COMBOBOX, wxSize(150,-1));
   
   // add burn label and combobox to burn sizer
   pageFlexGridSizer->Add( burnLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   pageFlexGridSizer->Add( burnCB, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
      
   // add spacecraft label and combobox to spacecraft sizer
   pageFlexGridSizer->Add( spacecraftLabel, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   pageFlexGridSizer->Add( satCB, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
    
   // add to middle sizer
   theMiddleSizer->Add(pageFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, 5);     
    
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
   StringArray burnList = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
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
   
   if (!burnCB->SetStringSelection(burn.c_str()))
   {
          burnCB->Append("");
          burnCB->SetStringSelection("");
   }
    
   // spacecraft
   id = theCommand->GetParameterID("Spacecraft");
   std::string sat = theCommand->GetStringParameter(id);
   //MessageInterface::ShowMessage("ManeuverPanel::LoadData() sat=%s\n", sat.c_str());
   StringArray satList = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
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
   
   if (!satCB->SetStringSelection(sat.c_str()))
   {
          satCB->Append("");
          satCB->SetStringSelection("");
   }
   

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ManeuverPanel::SaveData()
{
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
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
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
}
