//$Header$
//------------------------------------------------------------------------------
//                              ScriptPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/03/25
//
/**
 * Implements ScriptPanel class.
 */
//------------------------------------------------------------------------------

#include "ScriptPanel.hpp"
#include "MessageInterface.hpp"
#include <wx/file.h> // for wxFile

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ScriptPanel, GmatSavePanel)
   EVT_BUTTON(ID_BUTTON_SAVE, GmatSavePanel::OnSave)
   EVT_BUTTON(ID_BUTTON_SAVE_AS, GmatSavePanel::OnSaveAs)
   EVT_BUTTON(ID_BUTTON_CLOSE, GmatSavePanel::OnClose)

   EVT_TEXT(ID_TEXTCTRL, ScriptPanel::OnTextUpdate)
   EVT_BUTTON(ID_BUTTON, ScriptPanel::OnButton)

END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ScriptPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ScriptPanel::ScriptPanel(wxWindow *parent, const wxString &name)
   : GmatSavePanel(parent, false, name)
{
   mScriptFilename = name;

   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ScriptPanel::Create()
{
   int bsize = 3; // border size

    // create sizers
   wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
   mTopSizer = new wxStaticBoxSizer( topStaticBox, wxHORIZONTAL );
   mBottomSizer = new wxGridSizer( 1, 0, 0 );
//   mPageSizer = new wxFlexGridSizer( 0, 3, 0, 0 );
   mPageSizer = new wxBoxSizer(wxVERTICAL);

   //wxStaticText
   mFileContentsTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxGROW | wxHSCROLL);
                            
   // 5/24/05 - arg: for demo change font size to HUGE
   mFileContentsTextCtrl->SetFont( GmatAppData::GetFont() );

   // wxButton
   mBuildButton =
      new wxButton(this, ID_BUTTON, "Build", wxDefaultPosition, wxDefaultSize, 0);
   mBuildRunButton =
      new wxButton(this, ID_BUTTON, "Build and Run", wxDefaultPosition, wxDefaultSize, 0);
//   mFontButton =
//      new wxButton(this, ID_BUTTON, "Font", wxDefaultPosition, wxDefaultSize, 0);


   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   mTopSizer->Add(mBuildButton, 0, wxALIGN_RIGHT | wxALL, bsize);
   mTopSizer->Add(mBuildRunButton, 0, wxALIGN_RIGHT | wxALL, bsize);
//   mTopSizer->Add(mFontButton, 0, wxALIGN_RIGHT | wxALL, bsize);

   mBottomSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL,
                     bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mTopSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   mPageSizer->Add(mBottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ScriptPanel::LoadData()
{
   wxFile *file = new wxFile();
   bool mFileExists = file->Exists(mScriptFilename);

   if (mFileExists)
      mFileContentsTextCtrl->LoadFile(mScriptFilename);
   else
      mFileContentsTextCtrl->SetValue("");

   theSaveAsButton->Enable(true);
   theSaveButton->Enable(false);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ScriptPanel::SaveData()
{
   if (mScriptFilename != mFilename)
   {
      // add new script to tree
      GmatAppData::GetResourceTree()->AddScriptItem(mFilename);
      // rename this child window
      GmatAppData::GetMainFrame()->RenameChild(mScriptFilename, mFilename);
      mScriptFilename = mFilename;
   }

   mFileContentsTextCtrl->SaveFile(mScriptFilename);
   theSaveButton->Enable(false);
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnTextUpdate(wxCommandEvent& event)
{
   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBuildButton)
   {
      if (theSaveButton->IsEnabled())
      {
         // prompt user to save
         wxMessageDialog *msgDlg = new wxMessageDialog(this,
            "Would you like to save changes?", "Save...", wxYES_NO | wxICON_QUESTION ,
            wxDefaultPosition);
         int result = msgDlg->ShowModal();

         if (result == wxID_YES)
         {
            SaveData();
         }
      }
      GmatAppData::GetMainFrame()->OnScriptBuildObject(event);
   }
   else if (event.GetEventObject() == mBuildRunButton)
   {
      if (theSaveButton->IsEnabled())
      {
         // prompt user to save
         wxMessageDialog *msgDlg = new wxMessageDialog(this,
            "Would you like to save changes?", "Save...", wxYES_NO | wxICON_QUESTION ,
            wxDefaultPosition);
         int result = msgDlg->ShowModal();

         if (result == wxID_YES)
         {
            SaveData();
         }
      }
      GmatAppData::GetMainFrame()->OnScriptBuildAndRun(event);
   }
//   else if (event.GetEventObject() == mFontButton)
//   {
//      OnFontSelect(event);
//   }

}

//------------------------------------------------------------------------------
// void OnFontSelect(wxCommandEvent& event)
//------------------------------------------------------------------------------
//void ScriptPanel::OnFontSelect(wxCommandEvent& event)
//{
//  wxFontData data;
//  data.SetInitialFont(mFileContentsTextCtrl->GetFont());
////  data.SetColour(canvasTextColour);
//
//  wxFontDialog dialog(this, &data);
//  if (dialog.ShowModal() == wxID_OK)
//  {
//    wxFontData retData = dialog.GetFontData();
//    wxFont newFont = retData.GetChosenFont();
//
//    mFileContentsTextCtrl->SetFont(newFont);
//  }
//}
