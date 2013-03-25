//$Id$
//------------------------------------------------------------------------------
//                              EditorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2012 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2009/01/05
//
/**
 * Implements EditorPanel class.
 */
//------------------------------------------------------------------------------

#include "EditorPanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "ResourceTree.hpp"
#include "MessageInterface.hpp"
#include <wx/file.h>              // for wxFile
#include <wx/gdicmn.h>            // for wxColourDatabase

// To add build and build&run at top of the panel
//#define __ADD_BUILD_AT_TOP__

//#define DEBUG_EDITORPANEL
//#define DEBUG_EDITORPANEL_CREATE
//#define DEBUG_EDITORPANEL_LOAD
//#define DEBUG_EDITORPANEL_CLOSE

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(EditorPanel, GmatSavePanel)
   EVT_BUTTON(ID_BUTTON_SAVE, GmatSavePanel::OnSave)
   EVT_BUTTON(ID_BUTTON_SAVE_AS, GmatSavePanel::OnSaveAs)
   EVT_BUTTON(ID_BUTTON_CLOSE, GmatSavePanel::OnClosePanel)
   EVT_BUTTON(ID_SYNC_BUTTON, EditorPanel::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EditorPanel(wxWindow *parent, const wxString &name, bool isActive)
//------------------------------------------------------------------------------
/**
 * Constructs EditorPanel
 *
 * @param parent	Parent window (frame) of the panel 
 * @param name		Filename of the script
 * @param isActive	Is this script the active script
 *
 * @note There is no parameter free constructor for EditorPanel
 *
 */
//------------------------------------------------------------------------------
EditorPanel::EditorPanel(wxWindow *parent, const wxString &name, bool isActive)
   : GmatSavePanel(parent, false, name, true, isActive)
{
   #ifdef DEBUG_EDITORPANEL
   MessageInterface::ShowMessage
      ("EditorPanel::EditorPanel() entered, name='%s', isActive=%d\n", name.c_str(),
       isActive);
   #endif
   
   mScriptFilename = name;
   mEditor = NULL;
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~EditorPanel()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
EditorPanel::~EditorPanel()
{
   #ifdef DEBUG_EDITORPANEL
   MessageInterface::ShowMessage
      ("EditorPanel::~EditorPanel() mEditor=<%p>\n", mEditor);
   #endif

   if (mEditor)
   {
      delete mEditor;
      mEditor = NULL;
   }
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates and initializes the controls for the EditorPanel
 */
void EditorPanel::Create()
{
   //------------------------------------------------------
   // for editor
   //------------------------------------------------------
   mEditor = new Editor(this, true);
   mEditor->SetFocus();
   
   #ifdef DEBUG_EDITORPANEL_CREATE
   MessageInterface::ShowMessage
      ("EditorPanel::Create() new Editor <%p> created\n", mEditor);
   #endif

   //------------------------------------------------------
   // for build and build & run
   //------------------------------------------------------
   mSaveSyncButton =
      new wxButton(this, ID_SYNC_BUTTON, "Save,Sync", wxDefaultPosition, wxDefaultSize, 0);
   mSaveSyncRunButton =
      new wxButton(this, ID_SYNC_BUTTON, "Save,Sync,Run", wxDefaultPosition, wxDefaultSize, 0);
   
   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   int bsize = 2; // border size
   
   #ifdef __ADD_BUILD_AT_TOP__
   wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL);
   topSizer->Add(mSaveSyncButton, 0, wxALIGN_CENTER | wxALL, bsize);
   topSizer->Add(mSaveSyncRunButton, 0, wxALIGN_CENTER | wxALL, bsize);
   wxGridSizer *bottomSizer = new wxGridSizer( 1, 0, 0 );
   bottomSizer->Add(mEditor, 0, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   #else
   theButtonSizer->Insert(0, mSaveSyncButton, 0, wxALIGN_LEFT | wxALL, bsize);
   theButtonSizer->Insert(1, mSaveSyncRunButton, 0, wxALIGN_LEFT | wxALL, bsize);
   theButtonSizer->Insert(2, 50, 20);
   #endif
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);

   #ifdef __ADD_BUILD_AT_TOP__
   pageSizer->Add(topSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   pageSizer->Add(bottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   #else
   pageSizer->Add(mEditor, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   #endif

   theMiddleSizer->Add(pageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the controls with data (from filename specified in constructor)
 */
void EditorPanel::LoadData()
{
   #ifdef DEBUG_EDITORPANEL_LOAD
   MessageInterface::ShowMessage("EditorPanel::LoadData() entered\n");
   #endif
   
   wxFile *file = new wxFile();
   bool mFileExists = file->Exists(mScriptFilename);
   
   if (mFileExists)
      mEditor->LoadFile(mScriptFilename);
   
   #ifdef DEBUG_EDITORPANEL_LOAD
   MessageInterface::ShowMessage
      ("   '%s' %sexist\n", mScriptFilename.c_str(), mFileExists ? "" : "does not ");
   #endif
   
   theSaveAsButton->Enable(true);
   theSaveButton->Enable(true);
   GmatAppData::Instance()->GetMainFrame()->SetActiveChildDirty(false);
   mEditorModified = false;
   hasFileLoaded = true;
   SetEditorModified(false);
   
   delete file;
   
   #ifdef DEBUG_EDITORPANEL_LOAD
   MessageInterface::ShowMessage("EditorPanel::LoadData() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the controls to file (from filename specified in constructor)
 */
void EditorPanel::SaveData()
{
   GmatAppData *gmatAppData = GmatAppData::Instance();

   if (mScriptFilename != mFilename)
   {
      // add new script to tree
      gmatAppData->GetResourceTree()->AddScriptItem(mFilename);

      // rename this child window
      gmatAppData->GetMainFrame()->RenameActiveChild(mFilename);
      mScriptFilename = mFilename;
   }

   mEditor->SaveFile(mScriptFilename);
   gmatAppData->GetMainFrame()->SetActiveChildDirty(false);
}


//------------------------------------------------------------------------------
// void OnTextOverMaxLen(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for when User tried to enter more text into the control than 
 * the limit
 *
 * @param event	command event originated by the control
 */
void EditorPanel::OnTextOverMaxLen(wxCommandEvent& event)
{
   wxMessageBox(wxT("Text control is already filled up to the maximum length.\n"
                    "The extra input will be discarded."),
                wxT("GMAT Warning"));
}


//------------------------------------------------------------------------------
// void ClickButton( bool run )
//------------------------------------------------------------------------------
void EditorPanel::ClickButton( bool run )
{
	wxCommandEvent event;
	if (run)
		event.SetEventObject(mSaveSyncRunButton);
	else
		event.SetEventObject(mSaveSyncButton);
	OnButton(event);
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for when User clicks a button
 *
 * @param event	command event originated by the button
 */
void EditorPanel::OnButton(wxCommandEvent& event)
{
   if (mEditor->GetText() == "")
   {
      wxMessageDialog *msgDlg = new wxMessageDialog
         (this, "Can not build an empty file ", "Can not build...",
          wxOK | wxICON_INFORMATION, wxDefaultPosition);
      msgDlg->ShowModal();

      delete msgDlg;
      return;
   }
   
   // Handle Save,Sync or Save,Sync,Run
   if (event.GetEventObject() == mSaveSyncButton ||
       event.GetEventObject() == mSaveSyncRunButton)
   {
      MakeScriptActive(event, mEditor->IsModified());
   }
}

