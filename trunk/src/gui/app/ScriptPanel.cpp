//$Id$
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
#include "GmatStaticBoxSizer.hpp"
#include <wx/file.h>              // for wxFile
#include <wx/gdicmn.h>            // for wxColourDatabase

//#define DEBUG_SCRIPT_PANEL 1


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ScriptPanel, GmatSavePanel)
   EVT_BUTTON(ID_BUTTON_SAVE, GmatSavePanel::OnSave)
   EVT_BUTTON(ID_BUTTON_SAVE_AS, GmatSavePanel::OnSaveAs)
   EVT_BUTTON(ID_BUTTON_CLOSE, GmatSavePanel::OnClose)
   EVT_TEXT(ID_TEXTCTRL, ScriptPanel::OnTextUpdate)
   EVT_TEXT_ENTER(ID_TEXTCTRL, ScriptPanel::OnTextEnterPressed)
   EVT_TEXT_MAXLEN(ID_TEXTCTRL, ScriptPanel::OnTextOverMaxLen)
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
   mOldLineNumber = 0;
   
   wxColourDatabase db;
   //mBgColor = db.Find("WHEAT");
   //mBgColor = db.Find("LIGHT BLUE");
   //mBgColor = db.Find("LIGHT GREY");
   //mBgColor = db.Find("MEDIUM GOLDENROD");
   mBgColor = db.Find("CYAN");
   
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
   //wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
   //wxStaticBoxSizer *topSizer = new wxStaticBoxSizer( topStaticBox, wxHORIZONTAL );
   
   GmatStaticBoxSizer *topSizer =
      new GmatStaticBoxSizer( wxHORIZONTAL, this, "");
   
   wxGridSizer *bottomSizer = new wxGridSizer( 1, 0, 0 );
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   
   //------------------------------------------------------
   // for line number
   //------------------------------------------------------
   wxStaticText *lineNumberText =
      new wxStaticText(this, ID_TEXT, wxT("Go To Line Number:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mLineNumberTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(50, -1),
                     wxTE_PROCESS_ENTER);
      
#ifndef __WXMSW__
   mFileContentsTextCtrl = new
      wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                 wxDefaultPosition, wxDefaultSize,
                 wxTE_MULTILINE);
#else
   mFileContentsTextCtrl = new
      wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                 wxDefaultPosition, wxDefaultSize,
                 wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_RICH2 | wxGROW | wxHSCROLL);
#endif
   
   mFileContentsTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
   
   // wxButton
   mBuildButton =
      new wxButton(this, ID_BUTTON, "Build", wxDefaultPosition, wxDefaultSize, 0);
   mBuildRunButton =
      new wxButton(this, ID_BUTTON, "Build and Run", wxDefaultPosition, wxDefaultSize, 0);
   //mFontButton =
   //   new wxButton(this, ID_BUTTON, "Font", wxDefaultPosition, wxDefaultSize, 0);
   
   
   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   topSizer->Add(lineNumberText, 0, wxALIGN_CENTER | wxALL, bsize);
   topSizer->Add(mLineNumberTextCtrl, 0, wxALIGN_CENTER | wxALL, bsize);
   topSizer->Add(80, 20, 0, wxALIGN_LEFT | wxALL, bsize);
   topSizer->Add(mBuildButton, 0, wxALIGN_CENTER | wxALL, bsize);
   topSizer->Add(mBuildRunButton, 0, wxALIGN_CENTER | wxALL, bsize);
   //topSizer->Add(mFontButton, 0, wxALIGN_CENTER | wxALL, bsize);
   
   bottomSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL,
                     bsize);
   
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   pageSizer->Add(topSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   pageSizer->Add(bottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(pageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ScriptPanel::LoadData()
{
   #if DEBUG_SCRIPT_PANEL
   MessageInterface::ShowMessage("ScriptPanel::LoadData() entered\n");
   #endif
   
   wxFile *file = new wxFile();
   bool mFileExists = file->Exists(mScriptFilename);
      
   if (mFileExists)
      mFileContentsTextCtrl->LoadFile(mScriptFilename);
   else
      mFileContentsTextCtrl->SetValue("");
   
   theSaveAsButton->Enable(true);
   theSaveButton->Enable(false);
   GmatAppData::Instance()->GetMainFrame()->SetActiveChildDirty(false);
      
   mFileContentsTextCtrl->SetDefaultStyle(wxTextAttr(wxNullColour, *wxWHITE));
   wxTextAttr defStyle = mFileContentsTextCtrl->GetDefaultStyle();
   mDefBgColor = defStyle.GetBackgroundColour();
   
   mOldLastPos = mFileContentsTextCtrl->GetLastPosition();
   
   #if DEBUG_SCRIPT_PANEL
   MessageInterface::ShowMessage("ScriptPanel::LoadData() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ScriptPanel::SaveData()
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
   
   mFileContentsTextCtrl->SaveFile(mScriptFilename);
   theSaveButton->Enable(false);
   gmatAppData->GetMainFrame()->SetActiveChildDirty(false);
}


//------------------------------------------------------------------------------
// void OnTextEnterPressed(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnTextEnterPressed(wxCommandEvent& event)
{
   if (event.GetEventObject() == mLineNumberTextCtrl)
   {
      #if DEBUG_SCRIPT_PANEL
      MessageInterface::ShowMessage
         ("ScriptPanel::OnTextEnterPressed() mDefBgColor=%ld, mBgColor=%ld\n",
          mDefBgColor.GetPixel(), mBgColor.GetPixel());
      #endif
      
      int lineLength = 0;
      long pos = 0;
      
      // unshow old line number
      if (mOldLineNumber > 0)
      {
         lineLength = mFileContentsTextCtrl->GetLineLength(mOldLineNumber-1);
         pos = mFileContentsTextCtrl->XYToPosition(0, mOldLineNumber-1);
         
         #if DEBUG_SCRIPT_PANEL
         MessageInterface::ShowMessage
            ("===> mOldLineNumber=%d, pos=%d\n", mOldLineNumber, pos);
         #endif
         
         mFileContentsTextCtrl->
            SetStyle(pos, pos+lineLength, wxTextAttr(wxNullColour, mDefBgColor));
      }
      
      // show new line number
      long lineNumber;
      wxString str = mLineNumberTextCtrl->GetValue();
      str.ToLong(&lineNumber);
      pos = mFileContentsTextCtrl->XYToPosition(0, lineNumber-1);
      lineLength = mFileContentsTextCtrl->GetLineLength(lineNumber-1);
      
      #if DEBUG_SCRIPT_PANEL
      MessageInterface::ShowMessage("===> lineNumber=%d, pos=%d\n", lineNumber, pos);
      #endif
      
      // This causes wxEVT_COMMAND_TEXT_UPDATED event generated.
      // Notice that this event will always be sent when the text controls
      // contents changes - whether this is due to user input or comes from the
      // program itself (for example, if SetValue() is called)
      mFileContentsTextCtrl->
         SetStyle(pos, pos+lineLength, wxTextAttr(wxNullColour, mBgColor));
      
      mFileContentsTextCtrl->ShowPosition(pos);
      
      mOldLineNumber = lineNumber;
   }
}


//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == mFileContentsTextCtrl &&
       mFileContentsTextCtrl->IsModified())
   {
      int lastPos = mFileContentsTextCtrl->GetLastPosition();
      
      #if DEBUG_SCRIPT_PANEL
      MessageInterface::ShowMessage
         ("ScriptPanel::OnTextUpdate() mOldLastPos=%d, lastPos=%d\n",
          mOldLastPos, lastPos);
      #endif

      // check contents size to determine if text is modified,
      // since SetStyle() for go to line number generates EVT_TEXT
      if (lastPos != mOldLastPos)
      {
         #if DEBUG_SCRIPT_PANEL
         MessageInterface::ShowMessage("===> data modified\n");
         #endif
         
         theSaveButton->Enable(true);
         GmatAppData::Instance()->GetMainFrame()->SetActiveChildDirty(true);
         mOldLastPos = lastPos;
      }
   }
}


//------------------------------------------------------------------------------
// void OnTextOverMaxLen(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnTextOverMaxLen(wxCommandEvent& event)
{
   wxMessageBox(wxT("Text control is already filled up to the maximum length.\n"
                    "The extra input will be discarded."),
                wxT("GMAT Warning"));
}


//------------------------------------------------------------------------------
// void OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnButton(wxCommandEvent& event)
{
   GmatAppData *gmatAppData = GmatAppData::Instance();
   
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
            OnSave(event);
         }
      }
      
      if ( mFileContentsTextCtrl->GetValue() != "")
      {
         if (gmatAppData->GetMainFrame()->SetScriptFileName(mScriptFilename.c_str()))
            gmatAppData->GetMainFrame()->OnScriptBuildObject(event);
      }
      else
      {
         wxMessageDialog *msgDlg = new wxMessageDialog(this,
            "Can not build an empty file ", "Can not build...", wxOK | wxICON_INFORMATION ,
            wxDefaultPosition);
         msgDlg->ShowModal();
      }
      
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
            OnSave(event);
         }
      }
      
      if ( mFileContentsTextCtrl->GetValue() != "")
      {
         if (gmatAppData->GetMainFrame()->SetScriptFileName(mScriptFilename.c_str()))
            gmatAppData->GetMainFrame()->OnScriptBuildAndRun(event);
      }
      else
      {
         wxMessageDialog *msgDlg = new wxMessageDialog(this,
            "Can not build an empty file ", "Can not build...", wxOK | wxICON_INFORMATION ,
            wxDefaultPosition);
         msgDlg->ShowModal();
      }
      
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
