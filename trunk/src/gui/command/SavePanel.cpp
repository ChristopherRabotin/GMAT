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

//#define DEBUG_SAVEPANEL_LOAD
//#define DEBUG_SAVEPANEL_SAVE

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
      new wxStaticText(this, ID_TEXT, wxT("Please Select Objects to Save"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // list of object
   mObjectCheckListBox = theGuiManager->
      GetAllObjectCheckListBox(this, ID_CHECKLISTBOX, wxSize(300,200));
   
   // create sizers
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   // add object label and combobox to sizer
   pageBoxSizer->Add(objectLabel, 0, wxALIGN_CENTER|wxALL, bsize);
   pageBoxSizer->Add(mObjectCheckListBox, 0, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   
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
   
   // Get object list from the command. It can be any object type.
   StringArray objNames = theCommand->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
   
   for (UnsignedInt i=0; i<objNames.size(); i++)
   {
      #ifdef DEBUG_SAVEPANEL_LOAD
      MessageInterface::ShowMessage("   objName=<%s>\n", objNames[i].c_str());
      #endif
      
      wxString objName = objNames[i].c_str();
      for (UnsignedInt j=0; j<mObjectCheckListBox->GetCount(); j++)
      {
         wxString objStr = mObjectCheckListBox->GetString(j);
         if (objStr.BeforeFirst(' ') == objName)
            mObjectCheckListBox->Check(j);
      }
   }
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SavePanel::SaveData()
{
   int count = mObjectCheckListBox->GetCount();
   
   int checkedCount = 0;
   canClose = true;
   
   //-----------------------------------------------------------------
   // check for number of objects checked
   //-----------------------------------------------------------------
   for (int i=0; i<count; i++)
      if (mObjectCheckListBox->IsChecked(i))
         checkedCount++;
   
   if (checkedCount == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Please select one or more objects to save.");
      canClose = false;
      return;
   }
   
   #ifdef DEBUG_SAVEPANEL_SAVE
   MessageInterface::ShowMessage
      ("SavePanel::SaveData() number of checked object=%d\n", checkedCount);
   #endif
   
   //-----------------------------------------------------------------
   // save values to base, base code should do any range checking
   //-----------------------------------------------------------------
   
   wxString objStr, objName;
   
   theCommand->TakeAction("Clear");
   
   for (int i=0; i<count; i++)
   {
      if (mObjectCheckListBox->IsChecked(i))
      {
         objStr = mObjectCheckListBox->GetString(i);
         objName = objStr.BeforeFirst(' ');
         
         #ifdef DEBUG_SAVEPANEL_SAVE
         MessageInterface::ShowMessage
            ("objStr=<%s>, objName=<%s>\n", objStr.c_str(), objName.c_str());
         #endif
         
         theCommand->SetRefObjectName(Gmat::UNKNOWN_OBJECT, objName.c_str());         
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

