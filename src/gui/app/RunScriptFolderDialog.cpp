//$Header$
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
   : GmatDialog(parent, -1, wxString(_T("RunScriptFolderDialog")))
{
   mRunScripts = false;
   mCompareResults = false;
   mSaveCompareResults = false;
   mOutDirChanged = false;
   
   mNumScriptsToRun = numScripts;
   mNumTimesToRun = 1;
   mAbsTol = absTol;
   mCompareDir = compareDir;
   
   mReplaceString = "STK";
   
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
   
   int bsize = 2;
   
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
   wxFlexGridSizer *runSizer = new wxFlexGridSizer(2, 0, 0);
   runSizer->Add(numScriptsLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   runSizer->Add(mNumScriptsToRunTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runSizer->Add(numTimesLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   runSizer->Add(mNumTimesToRunTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
 
 #if __WXMAC__
   wxStaticText *title1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Run Scripts"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
                                             
   wxBoxSizer *runBoxSizer = new wxBoxSizer( wxVERTICAL );
   
   runBoxSizer->Add(title1StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   runBoxSizer->Add(runSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   runBoxSizer->Add(currOutDir1, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runBoxSizer->Add(currOutDir2, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runBoxSizer->Add(mCurrOutDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runBoxSizer->Add(mChangeCurrOutDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
#else  
   wxStaticBoxSizer *runStaticSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Run Scripts");
   
   runStaticSizer->Add(runSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   runStaticSizer->Add(currOutDir1, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runStaticSizer->Add(currOutDir2, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runStaticSizer->Add(mCurrOutDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   runStaticSizer->Add(mChangeCurrOutDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
#endif

   //------------------------------------------------------
   // compare results
   //------------------------------------------------------
   mCompareCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Compare Results"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *tolLabel =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance to be used in flagging:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mAbsTolTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("1"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *replaceLabel =
      new wxStaticText(this, ID_TEXT, wxT("Compare with filenames contain:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mReplaceTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(mReplaceString),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *dirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Directory to compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);

   //---------- sizer
   wxFlexGridSizer *compareSizer = new wxFlexGridSizer(2, 0, 0);
   
   compareSizer->Add(mCompareCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(tolLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mAbsTolTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   compareSizer->Add(replaceLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mReplaceTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);

   mCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mDirBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   mSaveResultCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Save Compare Results to File"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *saveFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("File Name to Save:"),
                       wxDefaultPosition, wxDefaultSize, 0);

   mSaveFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(mCompareDir + "/CompareResults.txt"),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mSaveBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);

 #if __WXMAC__
   wxStaticText *title2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Compare Results"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title2StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
                                             
   wxBoxSizer *compareBoxSizer = new wxBoxSizer( wxVERTICAL );
   
   compareBoxSizer->Add(title2StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareBoxSizer->Add(compareSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareBoxSizer->Add(dirLabel, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize+2);
   compareBoxSizer->Add(mCompareDirTextCtrl, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize+2);
   compareBoxSizer->Add(mDirBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   compareBoxSizer->Add(20,3, 0, wxALIGN_CENTRE|wxALL, bsize);
   compareBoxSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareBoxSizer->Add(saveFileLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareBoxSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   compareBoxSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
#else   
   wxStaticBoxSizer *compareStaticSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Compare Results");

   compareStaticSizer->Add(compareSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareStaticSizer->Add(dirLabel, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize+2);
   compareStaticSizer->Add(mCompareDirTextCtrl, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize+2);
   compareStaticSizer->Add(mDirBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   compareStaticSizer->Add(20,3, 0, wxALIGN_CENTRE|wxALL, bsize);
   compareStaticSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareStaticSizer->Add(saveFileLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareStaticSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   compareStaticSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
#endif
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
 #if __WXMAC__
   pageBoxSizer->Add(runBoxSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareBoxSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
 #else  
   pageBoxSizer->Add(runStaticSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareStaticSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
 #endif  
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
   mCurrOutDir = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCurrOutDirTextCtrl->SetValue(mCurrOutDir);
   mCompareDirTextCtrl->SetValue(mCompareDir);
   mSaveResultCheckBox->Disable();
   mSaveFileTextCtrl->Disable();
   mSaveBrowseButton->Disable();
   
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
   
   mNumScriptsToRun = numScriptsToRun;
   mNumTimesToRun = numTimesToRun;
   
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
   if (event.GetEventObject() == mChangeCurrOutDirButton)
   {
      //wxDirDialog dialog(this, "Select a new output directory", wxGetCwd());
      wxDirDialog dialog(this, "Select a new output directory", mCompareDir);
   
      if (dialog.ShowModal() == wxID_OK)
      {
         mCurrOutDir = dialog.GetPath();
         mCurrOutDirTextCtrl->SetValue(mCurrOutDir);
         mSaveFileTextCtrl->SetValue(mCurrOutDir + "/CompareResults.txt");
         mOutDirChanged = true;
         
         #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
         MessageInterface::ShowMessage
            ("RunScriptFolderDialog::OnButtonClick() mCurrOutDir=%s\n",
             dirname.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mDirBrowseButton)
   {
      //wxDirDialog dialog(this, "Select a directory to compare", wxGetCwd());
      wxDirDialog dialog(this, "Select a directory to compare", mCompareDir);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString dirname = dialog.GetPath();
         mCompareDirTextCtrl->SetValue(dirname);
         
         #if DEBUG_RUN_SCRIPT_FOLDER_DIALOG
         MessageInterface::ShowMessage
            ("RunScriptFolderDialog::OnButtonClick() dirname=%s\n",
             mCurrOutDir.c_str());
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
            ("RunScriptFolderDialog::OnButtonClick() savefile=%s\n", filename.c_str());
      }
      
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void RunScriptFolderDialog::OnCheckBoxChange(wxCommandEvent& event)
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

