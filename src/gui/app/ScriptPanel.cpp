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

BEGIN_EVENT_TABLE(ScriptPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, ScriptPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ScriptPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ScriptPanel::ScriptPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, false)
{
   mFilename = name;
   mFileExists = false;
   mEnableSave = false;

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
   mBottomSizer = new wxGridSizer( 1, 0, 0 );
//   mPageSizer = new wxFlexGridSizer( 0, 3, 0, 0 );
   mPageSizer = new wxBoxSizer(wxVERTICAL);

   //wxStaticText
   mFileContentsTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxGROW);

   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   mBottomSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL,
                     bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mBottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ScriptPanel::LoadData()
{
   wxFile *file = new wxFile();
   mFileExists = file->Exists(mFilename);

   if (mFileExists)
      mFileContentsTextCtrl->LoadFile(mFilename);
   else
      mFileContentsTextCtrl->SetValue("");

   theApplyButton->Enable(false);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ScriptPanel::SaveData()
{
   if (mEnableSave)
   {
//      if (mFileExists)
//      {
      mFileContentsTextCtrl->SaveFile(mFilename);
      mEnableSave = false;
      mFileExists = true;
//      }
   }
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ScriptPanel::OnTextUpdate(wxCommandEvent& event)
{
   theApplyButton->Enable(true);
   mEnableSave = true;
}
