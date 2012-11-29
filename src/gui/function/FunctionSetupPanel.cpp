//$Id$
//------------------------------------------------------------------------------
//                              FunctionSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/12/15
//
/**
 * Implements FunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "FunctionSetupPanel.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "FileUtil.hpp"           // for DoesFileExist()
#include "MessageInterface.hpp"
#include <wx/filename.h>          // for wxFileName::

//#define DEBUG_FUNCTIONPANEL_LOAD
//#define DEBUG_FUNCTIONPANEL_SAVE
//#define DEBUG_FUNCTIONPANEL_BUTTON

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FunctionSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_OK, FunctionSetupPanel::OnButton)
   EVT_BUTTON(ID_BUTTON_APPLY, FunctionSetupPanel::OnButton)
   EVT_BUTTON(ID_BUTTON_CANCEL, FunctionSetupPanel::OnButton)
   EVT_TEXT(ID_TEXTCTRL, FunctionSetupPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// FunctionSetupPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 *
 * @note We may have to consider deriving this class from the GmatSavePanel
 */
//------------------------------------------------------------------------------
FunctionSetupPanel::FunctionSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent, true)
{
   #ifdef DEBUG_FUNCTIONPANEL_LOAD
   MessageInterface::ShowMessage
      ("In FunctionSetupPanel() constructor, function name='%s'\n", name.c_str());
   #endif
   
   mEnableLoad = false;
   mEnableSave = false;
   mIsNewFunction = true;
   mEditorModified = false;
   mFunctionName = name;
   mFileContentsTextCtrl = NULL;
   
   #ifdef __USE_STC_EDITOR__
   mEditor = NULL;
   #endif
   
   theGmatFunction = (Function *)
      theGuiInterpreter->GetConfiguredObject(name.c_str());
   
   Create();
   Show();
}


//------------------------------------------------------------------------------
// ~FunctionSetupPanel()
//------------------------------------------------------------------------------
/**
 * A destructor.
 */
//------------------------------------------------------------------------------
FunctionSetupPanel::~FunctionSetupPanel()
{
   #ifdef __USE_STC_EDITOR__
   if (mEditor)
   {
      delete mEditor;
      mEditor = NULL;
   }
   #endif
}


//------------------------------------------------------------------------------
// void SetEditorModified(bool isModified)
//------------------------------------------------------------------------------
void FunctionSetupPanel::SetEditorModified(bool isModified)
{
   EnableUpdate(isModified);
   theOkButton->Enable(isModified); // note: theOKButton is the Save button
   mEditorModified = isModified;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void FunctionSetupPanel::Create()
{
   int bsize = 3; // border size
   
   //------------------------------------------------------
   // Create file contents
   //------------------------------------------------------
   
#ifdef __USE_STC_EDITOR__
   mEditor = new Editor(this, true, -1, wxDefaultPosition, wxSize(700,400));
#else
   mFileContentsTextCtrl = 
      new wxTextCtrl( this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, 
         wxSize(700,400), wxTE_MULTILINE | wxGROW | wxTE_DONTWRAP);
   mFileContentsTextCtrl->SetFont( GmatAppData::Instance()->GetFont() );
#endif
   
   //------------------------------------------------------
   // Add to sizer
   //------------------------------------------------------
   wxGridSizer *textSizer = new wxGridSizer( 1, 0, 0 );
#ifdef __USE_STC_EDITOR__
   textSizer->Add(mEditor, 0, wxGROW | wxALIGN_CENTER | wxALL, bsize);
#else
   textSizer->Add(mFileContentsTextCtrl, 0, wxGROW | wxALIGN_CENTER | wxALL, 
                  bsize);
#endif
   
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(textSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   theMiddleSizer->Add(pageSizer, 1, wxGROW | wxALIGN_CENTER | wxALL, bsize);
   
   // Change the label of OK, Apply and Cancel button
   theOkButton->SetLabel("Save");
   theApplyButton->SetLabel("Save As");
   theCancelButton->SetLabel("Close");
   
   theOkButton->Disable();
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void FunctionSetupPanel::LoadData()
{
   // Set the pointer for the "Show Script" button
   mObject = theGmatFunction;
   
   // If it is a new function just return
   if (theGmatFunction->IsNewFunction())
   {
      #ifdef DEBUG_FUNCTIONPANEL_LOAD
      MessageInterface::ShowMessage
         ("FunctionSetupPanel::LoadData() This is new function, so just returning\n");
      #endif
      return;
   }
   
   mFullFunctionPath = (theGmatFunction->GetStringParameter("FunctionPath")).c_str();
   
   #ifdef DEBUG_FUNCTIONPANEL_LOAD
   MessageInterface::ShowMessage
      ("FunctionSetupPanel::LoadData() mFullFunctionPath='%s'\n",
       mFullFunctionPath.c_str());
   #endif
   
   if (wxFileName::FileExists(mFullFunctionPath))
   {
      #ifdef __USE_STC_EDITOR__
      mEditor->LoadFile(mFullFunctionPath);
      #else
      mFileContentsTextCtrl->LoadFile(mFullFunctionPath);
      #endif
      mEnableSave = false;
      mIsNewFunction = false;
      EnableUpdate(false);
      theOkButton->Disable();
   }   
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void FunctionSetupPanel::SaveData()
{
   std::string pathname = theGmatFunction->GetStringParameter("FunctionPath");

   #ifdef DEBUG_FUNCTIONPANEL_SAVE
   MessageInterface::ShowMessage
      ("FunctionSetupPanel::SaveData() path='%s'\n", pathname.c_str());
   #endif
   
   if (pathname == "")
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "FunctionSetupPanel::SaveData()\n"
         "A function path was not specified.");
      return;
   }
   
   // save file contents
   #ifdef __USE_STC_EDITOR__
      mEditor->SaveFile(pathname.c_str());
      #ifdef DEBUG_FUNCTIONPANEL_SAVE
      MessageInterface::ShowMessage("   contents saved to '%s'\n", pathname.c_str());
      #endif
   #else
      mFileContentsTextCtrl->SaveFile(pathname.c_str());
      mFileContentsTextCtrl->SetModified(false);
      #ifdef DEBUG_FUNCTIONPANEL_SAVE
      MessageInterface::ShowMessage("   contents saved to '%s'\n", pathname.c_str());
      #endif
   #endif

   // Do we want to set titile to function path and name later?
   //(GmatAppData::Instance()->GetMainFrame()->GetActiveChild())->SetTitle(pathname);
   EnableUpdate(false);
   theOkButton->Disable();
}


//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FunctionSetupPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == mFileContentsTextCtrl)
   {
      mEnableSave = true;
      EnableUpdate(true);
      theOkButton->Enable();
   }
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void FunctionSetupPanel::OnButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
void FunctionSetupPanel::OnButton(wxCommandEvent& event)
{
   // This is Save button which was renamed from OK button
   if (event.GetEventObject() == theOkButton)
   {
      #ifdef DEBUG_FUNCTIONPANEL_BUTTON
      MessageInterface::ShowMessage
         ("FunctionSetupPanel::OnButton() entered, it is Save Button\n");
      #endif
      
      if (mIsNewFunction)
         OnSaveAs(event);
      else
         SaveData();
   }
   // This is SaveAs button which was renamed from Apply button
   else if (event.GetEventObject() == theApplyButton)
   {
      #ifdef DEBUG_FUNCTIONPANEL_BUTTON
      MessageInterface::ShowMessage
         ("FunctionSetupPanel::OnButton() entered, it is SaveAs Button\n");
      #endif
      
      OnSaveAs(event);
   }
   // This is Close button which was renamed from Cancel button
   else if (event.GetEventObject() == theCancelButton)
   {
      #ifdef DEBUG_FUNCTIONPANEL_BUTTON
      MessageInterface::ShowMessage
         ("FunctionSetupPanel::OnButton() entered, it is Close Button, canClose=%d\n",
          canClose);
      #endif
      
      GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
   }
}


//------------------------------------------------------------------------------
// void OnSaveAs()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void FunctionSetupPanel::OnSaveAs(wxCommandEvent &event)
{
   #ifdef DEBUG_FUNCTIONPANEL_SAVE
   MessageInterface::ShowMessage
      ("FunctionSetupPanel::OnSaveAs() entered, mIsNewFunction=%d\n", mIsNewFunction);
   #endif
   
   wxString defaultFile;
   defaultFile = mFunctionName + ".gmf";
   
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(defaultFile),
         _T("GmatFunction files (*.gmf)|*.gmf|"\
            "Text files (*.txt, *.text)|*.txt;*.text|"\
            "All files (*.*)|*.*"), wxSAVE);
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString path = dialog.GetPath().c_str();
      
      if (GmatFileUtil::DoesFileExist(path.c_str()))
      {
         if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"), 
                          _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxNO)
            return;
      }
      
      #ifdef DEBUG_FUNCTIONPANEL_SAVE
      MessageInterface::ShowMessage("   path='%s'\n", path.c_str());
      #endif
      
      theGmatFunction->SetStringParameter("FunctionPath", path.c_str());
      mFilename = path;
      SaveData();
      mIsNewFunction = false;
      theGmatFunction->SetNewFunction(false);
      theOkButton->Disable();
   }
   
   #ifdef DEBUG_FUNCTIONPANEL_SAVE
   MessageInterface::ShowMessage("FunctionSetupPanel::OnSaveAs() exiting\n");
   #endif
}

