//$Header$
//------------------------------------------------------------------------------
//                              FunctionSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/12/15
//
/**
 * Implements FunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "FunctionSetupPanel.hpp"
#include "MessageInterface.hpp"
#include <wx/filename.h>          // for wxFileName::


//#define DEBUG_PARAM_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FunctionSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   
   EVT_TEXT(ID_TEXTCTRL, FunctionSetupPanel::OnTextUpdate)
   EVT_BUTTON(ID_BUTTON, FunctionSetupPanel::OnButton)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// FunctionSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
FunctionSetupPanel::FunctionSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true)
{
   mEnableLoad = false;
   mEnableSave = false;

   theGmatFunction = (GmatFunction *)
            theGuiInterpreter->GetConfiguredObject(std::string(name.c_str()));

   Create();
   Show();
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void FunctionSetupPanel::Create()
{
   int bsize = 3; // border size
   
   wxStaticBox *topStaticBox = new wxStaticBox(this, -1, wxT(""));

    // create sizers
   mMiddleSizer = new wxBoxSizer(wxHORIZONTAL);
   mBottomSizer = new wxGridSizer( 1, 0, 0 );
//   mPageSizer = new wxFlexGridSizer( 0, 3, 0, 0 );
   mPageSizer = new wxBoxSizer(wxVERTICAL);
   mTopSizer = new wxStaticBoxSizer(topStaticBox, wxHORIZONTAL);
   wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);

   //wxStaticText
   wxStaticText *fileStaticText =
      new wxStaticText(this, ID_TEXT, wxT("File: "), wxDefaultPosition, 
         wxDefaultSize, 0);

   // wxTextCtrl
   mFileNameTextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(250,20), 0);

   // create buttons
   mBrowseButton =
      new wxButton(this, ID_BUTTON, "Browse", wxDefaultPosition, wxDefaultSize, 0);

   mLoadButton =
      new wxButton(this, ID_BUTTON, "Load", wxDefaultPosition, wxDefaultSize, 0);
   mLoadButton->Enable(mEnableLoad);

   mSaveButton =
      new wxButton(this, ID_BUTTON, "Save", wxDefaultPosition, wxDefaultSize, 0);
   mSaveButton->Enable(mEnableSave);

   // wxTextCtrl
   mFileContentsTextCtrl = 
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
         wxDefaultSize, wxTE_MULTILINE | wxGROW);
   mFileContentsTextCtrl->SetFont( GmatAppData::GetFont() );

   //------------------------------------------------------
   // add to sizer
   //------------------------------------------------------
   fileSizer->Add(fileStaticText, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mFileNameTextCtrl, 0, wxALIGN_CENTER  | wxALL, bsize);
   fileSizer->Add(mBrowseButton, 0, wxALIGN_CENTER  | wxALL, bsize);
   mTopSizer->Add(fileSizer, 0, wxALIGN_CENTER | wxALL, bsize);

   mMiddleSizer->Add(mLoadButton, 0, wxALIGN_CENTER | wxALL, bsize);
   mMiddleSizer->Add(mSaveButton, 0, wxALIGN_CENTER | wxALL, bsize);

   mBottomSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL, 
                     bsize);

   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   mPageSizer->Add(mTopSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   mPageSizer->Add(mMiddleSizer, 0, wxALIGN_CENTER | wxALL, bsize);
   mPageSizer->Add(mBottomSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(mPageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void FunctionSetupPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theGmatFunction;

   int pathId = theGmatFunction->GetParameterID("FunctionPath");
   std::string path = theGmatFunction->GetStringParameter(pathId);

   if (path == "")
      path = theGmatFunction->GetName() + ".script";

   mFileNameTextCtrl->SetValue(path.c_str());
   
   wxString filename = mFileNameTextCtrl->GetValue();

   if (filename != "")
   {
        // need to add default path...
   }

   if (wxFileName::FileExists(filename))
   {
       mFileContentsTextCtrl->LoadFile(filename);
       mEnableSave = false;
   }

}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void FunctionSetupPanel::SaveData()
{
   wxString filename = mFileNameTextCtrl->GetValue();

   if (filename == "")
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "FunctionSetupPanel::SaveData()\n"
         "A function path was not specified.");
      return;
   }

   // save file path to base
   int pathId = theGmatFunction->GetParameterID("FunctionPath");
   theGmatFunction->SetStringParameter(pathId,
         mFileNameTextCtrl->GetValue().c_str());

   // was file edited and not saved?
   if (mEnableSave)
   {
      wxMessageDialog *msgDlg = new wxMessageDialog(this,
         "Save function file?", "Save...", wxYES_NO | wxICON_QUESTION ,
         wxDefaultPosition);
      int result = msgDlg->ShowModal();

      if (result == wxID_YES)
         mFileContentsTextCtrl->SaveFile(filename);
   }
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FunctionSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == mFileNameTextCtrl)
   {
      mEnableLoad = true;
      EnableUpdate(true);

      if (mFileNameTextCtrl->GetValue() == "")
      {
         mEnableSave = false;
         mEnableLoad = false;
      }
   }
   else if (event.GetEventObject() == mFileContentsTextCtrl)
   {
      mEnableSave = true;
      EnableUpdate(true);
   }

   mLoadButton->Enable(mEnableLoad);
   mSaveButton->Enable(mEnableSave);
}

//---------------------------------
// private methods
//---------------------------------
void FunctionSetupPanel::OnButton(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBrowseButton)
   {
      // ag: should all files be included in thid dialog?
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
            _T("Text files (*.txt, *.text)|*.txt;*.text|"\
               "Script files (*.script, *.m)|*.script;*.m|"\
               "GMAT Function files (*.gmf)|*.gmf"));

      if (dialog.ShowModal() == wxID_OK)
      {
         wxString filename;

         filename = dialog.GetPath().c_str();
         mFileNameTextCtrl->SetValue(filename);
      }
   }
   else if (event.GetEventObject() == mLoadButton)
   {
      wxString filename = mFileNameTextCtrl->GetValue();

      if (filename != "")
      {
         mFileContentsTextCtrl->LoadFile(filename);
         mEnableSave = false;
      }
   }
   else if (event.GetEventObject() == mSaveButton)
   {
      wxString filename = mFileNameTextCtrl->GetValue();

      if (filename != "")
      {
         mFileContentsTextCtrl->SaveFile(filename);
         mEnableSave = false;
      }
   }
   else
   {
      //Error - unknown object
   }

   mLoadButton->Enable(mEnableLoad);
   mSaveButton->Enable(mEnableSave);
}



