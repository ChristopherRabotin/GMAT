//$Id$
//------------------------------------------------------------------------------
//                              RunScriptFolderDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2005/12/22
//
/**
 * Shows dialog where various run script folder option can be selected.
 */
//------------------------------------------------------------------------------

#include "RunScriptFolderDialog.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"

//#define DEBUG_RUN_SCRIPT_FOLDER_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(RunScriptFolderDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, RunScriptFolderDialog::OnButtonClick)
   EVT_CHECKBOX(ID_CHECKBOX, RunScriptFolderDialog::OnCheckBoxChange)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// RunScriptFolderDialog(wxWindow *parent, int numScripts, Real absTol,
//                       const wxString &compareStr)
//------------------------------------------------------------------------------
RunScriptFolderDialog::RunScriptFolderDialog(wxWindow *parent, int numScripts,
                                             Real absTol, const wxString &compareDir)
   : GmatDialog(parent, -1, "RunScriptFolderDialog", NULL, wxDefaultPosition,
                wxSize(100, 100))
{
   mRunScripts = false;
   mCompareResults = false;
   mSaveCompareResults = false;
   mOutDirChanged = false;
   mCreateRunFolder = false;
   
   mNumScriptsToRun = numScripts;
   mNumTimesToRun = 1;
   mAbsTol = absTol;
   mCompareDir = compareDir;
   
   mReplaceString = "GMAT";
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~RunScriptFolderDialog()
//------------------------------------------------------------------------------
RunScriptFolderDialog::~RunScriptFolderDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void RunScriptFolderDialog::Create()
{
   #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
   MessageInterface::ShowMessage("RunScriptFolderDialog::Create() entered.\n");
   #endif
   
   int bsize = 1;
   
   //------------------------------------------------------
   // save scripts
   //------------------------------------------------------
   mRunFromSavedCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT(" Save scripts to new folder and run from it"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *saveScriptsDir =
      new wxStaticText(this, ID_TEXT, wxT("Directory to save scripts:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mSaveScriptsDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mChangeSaveScriptsDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   wxBoxSizer *saveScriptsDirSizer = new wxBoxSizer(wxHORIZONTAL);
   saveScriptsDirSizer->Add(saveScriptsDir, 0, wxALIGN_CENTRE|wxALL, bsize);
   saveScriptsDirSizer->Add(mChangeSaveScriptsDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   wxBoxSizer *saveScriptsSizer = new wxBoxSizer(wxVERTICAL);
   saveScriptsSizer->Add(mRunFromSavedCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   saveScriptsSizer->Add(saveScriptsDirSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   saveScriptsSizer->Add(mSaveScriptsDirTextCtrl, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   
   //------------------------------------------------------
   // run scripts
   //------------------------------------------------------
   wxStaticText *numScriptsLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of scripts to run:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumScriptsToRunTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("1"),
                     wxDefaultPosition, wxSize(80,20), 0);

   wxStaticText *numTimesLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of times to run each script:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mCreateRunFolderCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT(" Create RUN folder"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   mNumTimesToRunTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("1"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *currOutDir1 =
      new wxStaticText(this, ID_TEXT, wxT("Current GMAT output directory:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *currOutDir2 =
      new wxStaticText(this, ID_TEXT, wxT("(ReportFile will use this path if "
                                          "it doesn't contain path)"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mCurrOutDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mChangeCurrOutDirButton =
      new wxButton(this, ID_BUTTON, wxT("Change for this Run"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   //---------- sizer
   wxFlexGridSizer *runSizer = new wxFlexGridSizer(4, 0, 0);
   runSizer->Add(numScriptsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   runSizer->Add(mNumScriptsToRunTextCtrl, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runSizer->Add(5, 20, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runSizer->Add(20, 20, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runSizer->Add(numTimesLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   runSizer->Add(mNumTimesToRunTextCtrl, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runSizer->Add(5, 20, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runSizer->Add(mCreateRunFolderCheckBox, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   
   GmatStaticBoxSizer *runStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Run Scripts");
   
   runStaticSizer->Add(saveScriptsSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   runStaticSizer->Add(20, 3, 0, wxALIGN_LEFT|wxALL, bsize);
   runStaticSizer->Add(runSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   runStaticSizer->Add(currOutDir1, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runStaticSizer->Add(currOutDir2, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runStaticSizer->Add(mCurrOutDirTextCtrl, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   runStaticSizer->Add(mChangeCurrOutDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //------------------------------------------------------
   // compare results
   //------------------------------------------------------
   mCompareCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Compare results"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *tolLabel =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance to be used in flagging:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mAbsTolTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("1"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *replaceLabel =
      new wxStaticText(this, ID_TEXT, wxT("Compare files by replacing \"GMAT\" with:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mReplaceTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(mReplaceString),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *compDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Directory to compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   //---------- sizer
   wxFlexGridSizer *compareSizer = new wxFlexGridSizer(2, 0, 0);
   
   compareSizer->Add(mCompareCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(tolLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mAbsTolTextCtrl, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   compareSizer->Add(replaceLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mReplaceTextCtrl, 0, wxALIGN_RIGHT|wxGROW|wxALL, bsize);
   
   mCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mDirBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   mSaveResultCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Save compare results to file"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *saveFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("Filename to save:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mSaveFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mSaveBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   GmatStaticBoxSizer *compareStaticSizer =
      new GmatStaticBoxSizer(wxVERTICAL, this, "Compare Results");
   
   wxBoxSizer *compDirSizer = new wxBoxSizer(wxHORIZONTAL);
   compDirSizer->Add(compDirLabel, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   compDirSizer->Add(mDirBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   
   wxBoxSizer *saveDirSizer = new wxBoxSizer(wxHORIZONTAL);
   saveDirSizer->Add(saveFileLabel, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   saveDirSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   compareStaticSizer->Add(compareSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   compareStaticSizer->Add(compDirSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   compareStaticSizer->Add(mCompareDirTextCtrl, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize+2);
   compareStaticSizer->Add(20, 3, 0, wxALIGN_CENTRE|wxALL, bsize);
   compareStaticSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareStaticSizer->Add(saveDirSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   compareStaticSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize+2);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   pageBoxSizer->Add(runStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   pageBoxSizer->Add(compareStaticSizer, 0, wxALIGN_CENTRE|wxGROW|wxALL, bsize);
   
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void RunScriptFolderDialog::LoadData()
{
   wxString str;
   str.Printf("%d", mNumScriptsToRun);
   mNumScriptsToRunTextCtrl->SetValue(str);
   
   str.Printf("%g", mAbsTol);
   mAbsTolTextCtrl->SetValue(str);
   
   FileManager *fm = FileManager::Instance();
   wxString sep = fm->GetPathSeparator().c_str();
   
   mCurrOutDir = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mSaveScriptsDirTextCtrl->SetValue(mCurrOutDir + "AutoSave");
   mCurrOutDirTextCtrl->SetValue(mCurrOutDir);
   mCompareDirTextCtrl->SetValue(mCompareDir);
   mSaveFileTextCtrl->SetValue(mCompareDir + sep + "CompareNumericResults.txt");
   
   mSaveResultCheckBox->Disable();
   mSaveFileTextCtrl->Disable();
   mSaveBrowseButton->Disable();
   mSaveScriptsDirTextCtrl->Disable();
   mChangeSaveScriptsDirButton->Disable();
   theOkButton->Enable();
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void RunScriptFolderDialog::SaveData()
{
   long numScriptsToRun;
   long numTimesToRun;
   canClose = true;
      
   if (!mNumScriptsToRunTextCtrl->GetValue().ToLong(&numScriptsToRun))
   {
      wxMessageBox("Invalid number of scripts to run entered.");
      canClose = false;
      return;
   }
   
   if (!mNumTimesToRunTextCtrl->GetValue().ToLong(&numTimesToRun))
   {
      wxMessageBox("Invalid number of scripts to run entered.");
      canClose = false;
      return;
   }
   
   if (!mAbsTolTextCtrl->GetValue().ToDouble(&mAbsTol))
   {
      wxMessageBox("Invalid tolerance entered.");
      canClose = false;
      return;
   }
   
   if (mCurrOutDirTextCtrl->GetValue() == "")
   {
      wxMessageBox("Please enter output directory.");
      canClose = false;
      return;
   }
   
   mRunFromSavedScripts = mRunFromSavedCheckBox->GetValue();

   if (mRunFromSavedScripts)
      mOutDirChanged = true;
   
   if (mRunFromSavedScripts &&
       mSaveScriptsDirTextCtrl->GetValue() == "")
   {
      wxMessageBox("Please enter directory to save scripts.");
      canClose = false;
      return;
   }
   
   mCreateRunFolder = mCreateRunFolderCheckBox->GetValue();
   
   mNumScriptsToRun = numScriptsToRun;
   mNumTimesToRun = numTimesToRun;
   
   mSaveScriptsDir = mSaveScriptsDirTextCtrl->GetValue();
   mCurrOutDir = mCurrOutDirTextCtrl->GetValue();
   mReplaceString = mReplaceTextCtrl->GetValue();
   mCompareDir = mCompareDirTextCtrl->GetValue();
   mSaveFilename = mSaveFileTextCtrl->GetValue();
   
   mRunScripts = true;
   if (mNumScriptsToRun <= 0)
      mRunScripts = false;
   
   mCompareResults = mCompareCheckBox->GetValue();
   mSaveCompareResults = mSaveResultCheckBox->GetValue();
   
   #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
   MessageInterface::ShowMessage
      ("RunScriptFolderDialog::SaveData() mNumScriptsToRun=%d, mNumTimesToRun=%d\n"
       "mCompareResults=%d, mAbsTol=%e\n   mCompareDir=%s, mReplaceString=%s\n",
       mNumScriptsToRun, mNumTimesToRun, mCompareResults, mAbsTol,
       mCompareDir.c_str(), mReplaceString.c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void RunScriptFolderDialog::ResetData()
{
   canClose = true;
   mRunScripts = false;
   mCompareResults = false;
   mOutDirChanged = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void RunScriptFolderDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mChangeSaveScriptsDirButton)
   {
      wxDirDialog dialog(this, "Select a directory to save scripts", mCompareDir);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         mSaveScriptsDir = dialog.GetPath();
         mSaveScriptsDirTextCtrl->SetValue(mSaveScriptsDir);
         
         #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
         MessageInterface::ShowMessage
            ("RunScriptFolderDialog::OnButtonClick() mSaveScriptsDir=%s\n",
             mSaveScriptsDir.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mChangeCurrOutDirButton)
   {
      wxDirDialog dialog(this, "Select a new output directory", mCompareDir);
   
      if (dialog.ShowModal() == wxID_OK)
      {
         FileManager *fm = FileManager::Instance();
         wxString sep = fm->GetPathSeparator().c_str();
         
         mCurrOutDir = dialog.GetPath();
         mCurrOutDirTextCtrl->SetValue(mCurrOutDir);
         mSaveFileTextCtrl->SetValue(mCurrOutDir + sep + "CompareNumericResults.txt");
         mOutDirChanged = true;
         
         #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
         MessageInterface::ShowMessage
            ("RunScriptFolderDialog::OnButtonClick() mCurrOutDir=%s\n",
             mCurrOutDir.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mDirBrowseButton)
   {
      wxDirDialog dialog(this, "Select a directory to compare", mCompareDir);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString dirname = dialog.GetPath();
         mCompareDirTextCtrl->SetValue(dirname);
         
         #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
         MessageInterface::ShowMessage
            ("RunScriptFolderDialog::OnButtonClick() dirname=%s\n",
             dirname.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mSaveBrowseButton)
   {
      wxString filename =
         wxFileSelector("Choose a file to save", "", "", "txt",
                        "Report files (*.report)|*.report|Text files (*.txt)|*.txt",
                        wxSAVE);
      
      if (!filename.empty())
      {
         mSaveFileTextCtrl->SetValue(filename);
         MessageInterface::ShowMessage
            ("RunScriptFolderDialog::OnButtonClick() savefile=%s\n",
             filename.c_str());
      }
      
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void RunScriptFolderDialog::OnCheckBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mRunFromSavedCheckBox)
   {
      if (mRunFromSavedCheckBox->IsChecked())
      {
         mSaveScriptsDirTextCtrl->Enable();
         mChangeSaveScriptsDirButton->Enable();
      }
      else
      {
         mSaveScriptsDirTextCtrl->Disable();
         mChangeSaveScriptsDirButton->Disable();
      }
   }
   else if (event.GetEventObject() == mCompareCheckBox)
   {
      if (mCompareCheckBox->IsChecked())
      {
         mSaveResultCheckBox->Enable();
         mSaveFileTextCtrl->Enable();
         mSaveBrowseButton->Enable();
      }
      else
      {
         mSaveResultCheckBox->Disable();
         mSaveFileTextCtrl->Disable();
         mSaveBrowseButton->Disable();
      }
   }
}

