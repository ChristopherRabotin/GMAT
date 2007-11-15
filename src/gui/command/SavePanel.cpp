//$Id$
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
#include "MessageInterface.hpp"

//#define DEBUG_SAVE_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SavePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_CHECKLISTBOX(ID_CHECKLISTBOX, SavePanel::OnCheckListBoxChange)
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
   theGuiManager->UnregisterCheckListBox("AllObject", mObjectCheckListBox);
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
    
   // create object label
   wxStaticText *objectLabel =
      new wxStaticText(this, ID_TEXT, wxT("Select Objects to Save"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // list of object
   mObjectCheckListBox = theGuiManager->
      GetAllObjectCheckListBox(this, ID_CHECKLISTBOX, wxSize(200,150));
   
   // create sizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   // add object label and combobox to sizer
   pageBoxSizer->Add(objectLabel, 0, wxALIGN_CENTER | wxALL, bsize);
   pageBoxSizer->Add(mObjectCheckListBox, 0, wxALIGN_CENTER | wxALL, bsize);
   
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
   
   // Set objects
   
   // Get object list from the command. Actually it can be any object type,
   // but pass SPACECRAFT since the method needs input type.
   StringArray objNames = theCommand->GetRefObjectNameArray(Gmat::SPACECRAFT);
   int item;
   
   for (UnsignedInt i=0; i<objNames.size(); i++)
   {
      #ifdef DEBUG_SAVE_PANEL
      MessageInterface::ShowMessage("   objName=<%s>\n", objNames[i].c_str());
      #endif
      
      item = mObjectCheckListBox->FindString(objNames[i].c_str());
      mObjectCheckListBox->Check(item);
   }

}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SavePanel::SaveData()
{
   // save objects
   int count = mObjectCheckListBox->GetCount();
   wxString objName;
   
   theCommand->TakeAction("Clear");
   
   for (int i=0; i<count; i++)
   {
      if (mObjectCheckListBox->IsChecked(i))
      {
         objName = mObjectCheckListBox->GetString(i);
         theCommand->SetRefObjectName(Gmat::SPACECRAFT, objName.c_str());
      }
   }
}

//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void SavePanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

