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
// wxMenuBar *CreateScriptMenu()
//------------------------------------------------------------------------------
/**
 * Adds items to the script menu.
 *
 * @return script Menu bar.
 */
//------------------------------------------------------------------------------
wxMenuBar *ScriptPanel::CreateScriptMenu()
{
    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_NEW, _T("&New"));
    file_menu->Enable(wxID_NEW, false);
    file_menu->Append(wxID_OPEN, _T("&Open"));
    file_menu->Enable(wxID_OPEN, false);
//    file_menu->Append(MENU_FILE_NEW_SCRIPT, _T("&New"));
//    file_menu->Append(MENU_FILE_OPEN_SCRIPT, _T("&Open"));
    file_menu->Append(ID_BUTTON_CLOSE, _T("&Close"));
    file_menu->Enable(ID_BUTTON_CLOSE, false);
    file_menu->Append(ID_BUTTON_SAVE, _T("&Save"));
    file_menu->Enable(ID_BUTTON_SAVE, false);
    file_menu->Append(ID_BUTTON_SAVE_AS, _T("Save &As"));
    file_menu->Enable(ID_BUTTON_SAVE_AS, false);

    wxMenu *editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, _T("&Undo"));
    editMenu->Enable(wxID_UNDO, false);
    editMenu->Append(wxID_REDO, _T("&Redo"));
    editMenu->Enable(wxID_REDO, false);
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, _T("Cu&t"));
    editMenu->Enable(wxID_CUT, false);
    editMenu->Append(wxID_COPY, _T("&Copy"));
    editMenu->Enable(wxID_COPY, false);
    editMenu->Append(wxID_PASTE, _T("&Paste"));
    editMenu->Enable(wxID_PASTE, false);

    wxMenu *scriptMenu = (wxMenu *) NULL;

    scriptMenu = new wxMenu;
    scriptMenu->Append(GmatScript::MENU_SCRIPT_BUILD_OBJECT,
          _T("&Build Object"));
    scriptMenu->Append(GmatScript::MENU_SCRIPT_BUILD_AND_RUN,
          _T("&Build and Run"));
    scriptMenu->Append(GmatScript::MENU_SCRIPT_RUN, _T("&Run"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(editMenu, _T("&Edit"));
    menu_bar->Append(scriptMenu, _T("&Script"));

    return menu_bar;
}


