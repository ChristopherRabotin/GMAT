//$Id$
//------------------------------------------------------------------------------
//                              EditorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
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
   EVT_BUTTON(ID_BUTTON, EditorPanel::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// EditorPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
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
 * A destructor.
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
// void Create()
//------------------------------------------------------------------------------
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
   mBuildButton =
      new wxButton(this, ID_BUTTON, "Save,Sync", wxDefaultPosition, wxDefaultSize, 0);
   mBuildRunButton =
      new wxButton(this, ID_BUTTON, "Save,Sync,Run", wxDefaultPosition, wxDefaultSize, 0);

   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   int bsize = 2; // border size
   
   #ifdef __ADD_BUILD_AT_TOP__
   wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL);
   topSizer->Add(mBuildButton, 0, wxALIGN_CENTER | wxALL, bsize);
   topSizer->Add(mBuildRunButton, 0, wxALIGN_CENTER | wxALL, bsize);
   wxGridSizer *bottomSizer = new wxGridSizer( 1, 0, 0 );
   bottomSizer->Add(mEditor, 0, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   #else
   theButtonSizer->Insert(0, mBuildButton, 0, wxALIGN_LEFT | wxALL, bsize);
   theButtonSizer->Insert(1, mBuildRunButton, 0, wxALIGN_LEFT | wxALL, bsize);
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
void EditorPanel::OnTextOverMaxLen(wxCommandEvent& event)
{
   wxMessageBox(wxT("Text control is already filled up to the maximum length.\n"
                    "The extra input will be discarded."),
                wxT("GMAT Warning"));
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
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

   GmatAppData *gmatAppData = GmatAppData::Instance();
   bool continueBuild = true;
   
   if (event.GetEventObject() == mBuildButton ||
       event.GetEventObject() == mBuildRunButton)
   {
      // If this is not an active script, prompt the user for setting active
      if (!mIsScriptActive)
      {
         wxMessageDialog *msgDlg = new wxMessageDialog
            (this,"Are you sure you want to make this script active?", "Save active...",
             wxYES_NO | wxICON_QUESTION, wxDefaultPosition);
         int result = msgDlg->ShowModal();
         
         if (result == wxID_YES)
            continueBuild = true;
         else
            continueBuild = false;
      }
      
      if (mEditor->IsModified())
      {
         //=======================================
         #ifdef __PROMPT_USER_ON_MODIFIED__
         //=======================================
         
         // prompt user to save
         wxMessageDialog *msgDlg = new wxMessageDialog(this,
            "Would you like to save changes?", "Save...", wxYES_NO | wxICON_QUESTION ,
            wxDefaultPosition);
         int result = msgDlg->ShowModal();
         
         if (result == wxID_YES)
            OnSave(event);
         
         //=======================================
         #else
         //=======================================
         
         OnSave(event);
         
         //=======================================
         #endif
         //=======================================
      }
   }
   
   // If continue building, set script file name and build script
   if (continueBuild)
   {
      if (event.GetEventObject() == mBuildButton)
      {
         if (gmatAppData->GetMainFrame()->SetScriptFileName(mScriptFilename.c_str()))
            gmatAppData->GetMainFrame()->OnScriptBuildObject(event);
      }
      else if (event.GetEventObject() == mBuildRunButton)
      {
         if (gmatAppData->GetMainFrame()->SetScriptFileName(mScriptFilename.c_str()))
            gmatAppData->GetMainFrame()->OnScriptBuildAndRun(event);
      }
   }
}

