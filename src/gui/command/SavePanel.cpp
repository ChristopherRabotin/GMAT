//$Header$
//------------------------------------------------------------------------------
//                              SavePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/14
//
/**
 * This class contains the Save Setup window.
 */
//------------------------------------------------------------------------------

#include "SavePanel.hpp"
//#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SavePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_COMBOBOX(ID_COMBOBOX, SavePanel::OnComboBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// SavePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs SavePanel object.
 *
 * @param <parent> input parent.
 */
//------------------------------------------------------------------------------
SavePanel::SavePanel(wxWindow *parent, GmatCommand *cmd)
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
// ~SavePanel()
//------------------------------------------------------------------------------
SavePanel::~SavePanel()
{
   theGuiManager->UnregisterComboBox("Spacecraft", mObjectComboBox);   
}


//---------------------------------
// protected methods
//---------------------------------

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel for the Maneuver Command
 */
//------------------------------------------------------------------------------
void SavePanel::Create()
{
   StringArray items;
   int bsize = 3;
    
   // create sizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);

   // create object label
   wxStaticText *objectLabel =
      new wxStaticText(this, ID_TEXT, wxT("Object"), wxDefaultPosition,
                       wxDefaultSize, 0);

   // list of object
   //loj: 10/14/04 currently only Spacecraft is avaiable
   mObjectComboBox =
      theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX, wxSize(150,-1));
   
   // add object label and combobox to sizer
   pageBoxSizer->Add(objectLabel, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(mObjectComboBox, 0, wxALIGN_CENTER | wxALL, bsize);
   
   // add to middle sizer
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);     

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SavePanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theCommand;

   // load data from the core engine
    
   // object
   wxString objName = theCommand->GetRefObjectName(Gmat::SPACECRAFT).c_str();
   mObjectComboBox->SetStringSelection(objName);
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SavePanel::SaveData()
{
   std::string newObjName = mObjectComboBox->GetStringSelection().c_str();
   
   theCommand->SetRefObjectName(Gmat::SPACECRAFT, newObjName);
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SavePanel::OnComboBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

