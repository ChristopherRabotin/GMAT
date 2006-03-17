//$Header$
//------------------------------------------------------------------------------
//                              CompareFilesDialog
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

#include "CompareFilesDialog.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>
#include <wx/filename.h>

//#define DEBUG_COMPARE_FILES_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CompareFilesDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, CompareFilesDialog::OnButtonClick)
   EVT_CHECKBOX(ID_CHECKBOX, CompareFilesDialog::OnCheckBoxChange)
   EVT_TEXT_ENTER(ID_TEXTCTRL, CompareFilesDialog::OnTextEnterPress)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// CompareFilesDialog(wxWindow *parent)
//------------------------------------------------------------------------------
CompareFilesDialog::CompareFilesDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("CompareFilesDialog")))
{
   mCompareFiles = false;
   mSaveCompareResults = false;
   
   mNumFilesToCompare = 0;
   mAbsTol = 1.0e-4;
   mDirectory1 = "";

   mFromString = "GMAT";
   mToString = "STK";
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~CompareFilesDialog()
//------------------------------------------------------------------------------
CompareFilesDialog::~CompareFilesDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void CompareFilesDialog::Create()
{
   #if DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage("CompareFilesDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   
   //------------------------------------------------------
   // compare directory
   //------------------------------------------------------
   wxStaticText *dir1Label =
      new wxStaticText(this, ID_TEXT, wxT("Directory 1:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mDirectory1Button =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *dir2Label =
      new wxStaticText(this, ID_TEXT, wxT("Directory 2:"),
                       wxDefaultPosition, wxDefaultSize, 0);

   mDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mDirectory2Button =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);
   
 
 #if __WXMAC__
   wxStaticText *title1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Compare Directory"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    false, _T(""), wxFONTENCODING_SYSTEM));
                                             
   wxBoxSizer *dirBoxSizer = new wxBoxSizer( wxVERTICAL );
   
   dirBoxSizer->Add(title1StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   dirBoxSizer->Add(dir1Label, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirBoxSizer->Add(mDir1TextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirBoxSizer->Add(mDirectory1Button, 0, wxALIGN_CENTER|wxALL, bsize);
   dirBoxSizer->Add(dir2Label, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirBoxSizer->Add(mDir2TextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirBoxSizer->Add(mDirectory2Button, 0, wxALIGN_CENTER|wxALL, bsize);
#else  
   wxStaticBoxSizer *dirStaticSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Compare Directory");
   
   dirStaticSizer->Add(dir1Label, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirStaticSizer->Add(mDir1TextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirStaticSizer->Add(mDirectory1Button, 0, wxALIGN_CENTER|wxALL, bsize);
   dirStaticSizer->Add(dir2Label, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirStaticSizer->Add(mDir2TextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   dirStaticSizer->Add(mDirectory2Button, 0, wxALIGN_CENTER|wxALL, bsize);
#endif

   //------------------------------------------------------
   // compare file names
   //------------------------------------------------------
   wxStaticText *replaceFromLabel =
      new wxStaticText(this, ID_TEXT, wxT("Compare File Names Contain:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mReplaceFromTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(mFromString),
                     wxDefaultPosition, wxSize(80,20), wxTE_PROCESS_ENTER);
   
   wxStaticText *fromDirLabel =
      new wxStaticText(this, ID_TEXT, wxT(" From Directory 1"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *numFilesFromDir1Label =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesFromDir1TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   mUpdate1Button =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxSize(50,20), 0);
   
   wxStaticText *numFilesFromDir2Label =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesFromDir2TextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   mUpdate2Button =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxSize(50,20), 0);
   
   wxStaticText *replaceToLabel =
      new wxStaticText(this, ID_TEXT, wxT("With File Names Contain:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mReplaceToTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(mToString),
                     wxDefaultPosition, wxSize(80,20), wxTE_PROCESS_ENTER);
   
   wxStaticText *toDirLabel =
      new wxStaticText(this, ID_TEXT, wxT(" From Directory 2"),
                       wxDefaultPosition, wxDefaultSize, 0);


   //---------- sizer
   wxFlexGridSizer *filesSizer = new wxFlexGridSizer(3, 0, 0);
   filesSizer->Add(replaceFromLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   filesSizer->Add(mReplaceFromTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   filesSizer->Add(fromDirLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   
   filesSizer->Add(numFilesFromDir1Label, 0, wxALIGN_LEFT|wxALL, bsize);
   filesSizer->Add(mNumFilesFromDir1TextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   //filesSizer->Add(20, 20, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   filesSizer->Add(mUpdate1Button, 0, wxALIGN_LEFT|wxALL, bsize);
   
   filesSizer->Add(replaceToLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   filesSizer->Add(mReplaceToTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   filesSizer->Add(toDirLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   
   filesSizer->Add(numFilesFromDir2Label, 0, wxALIGN_LEFT|wxALL, bsize);
   filesSizer->Add(mNumFilesFromDir2TextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   //filesSizer->Add(20, 20, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   filesSizer->Add(mUpdate2Button, 0, wxALIGN_LEFT|wxALL, bsize);
   

 #if __WXMAC__
   wxStaticText *title2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Compare Files"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title2StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    false, _T(""), wxFONTENCODING_SYSTEM));
   
   wxBoxSizer *filesBoxSizer = new wxBoxSizer( wxVERTICAL );
   
   filesBoxSizer->Add(title2StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   filesBoxSizer->Add(filesSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
#else   
   wxStaticBoxSizer *filesStaticSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Compare Files");

   filesStaticSizer->Add(filesSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
#endif
   
   //------------------------------------------------------
   // compare results
   //------------------------------------------------------
   wxStaticText *numFilesToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *tolLabel =
      new wxStaticText(this, ID_TEXT, wxT("Tolerance to be Used in Flagging:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mAbsTolTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("1"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   //---------- sizer
   wxFlexGridSizer *compareSizer = new wxFlexGridSizer(2, 0, 0);
   compareSizer->Add(numFilesToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mNumFilesToCompareTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   
   compareSizer->Add(tolLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mAbsTolTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);

   mSaveResultCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxT("Save Compare Results to File"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   
   wxStaticText *saveFileLabel =
      new wxStaticText(this, ID_TEXT, wxT("File Name to Save:"),
                       wxDefaultPosition, wxDefaultSize, 0);

   mSaveFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mSaveBrowseButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxDefaultSize, 0);

 #if __WXMAC__
   wxStaticText *title3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Compare"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title3StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    false, _T(""), wxFONTENCODING_SYSTEM));
   
   wxBoxSizer *compareBoxSizer = new wxBoxSizer( wxVERTICAL );
   
   compareBoxSizer->Add(title3StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareBoxSizer->Add(compareSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareBoxSizer->Add(20,5, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareBoxSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareBoxSizer->Add(saveFileLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareBoxSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   compareBoxSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
#else   
   wxStaticBoxSizer *compareStaticSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Compare");

   compareStaticSizer->Add(compareSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareStaticSizer->Add(20,5, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
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
   pageBoxSizer->Add(dirBoxSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(filesBoxSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareBoxSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
 #else  
   pageBoxSizer->Add(dirStaticSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(filesStaticSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareStaticSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
 #endif  
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CompareFilesDialog::LoadData()
{
   wxString str;
   str.Printf("%d", mNumFilesToCompare);
   mNumFilesToCompareTextCtrl->SetValue(str);

   str.Printf("%g", mAbsTol);
   mAbsTolTextCtrl->SetValue(str);

   FileManager *fm = FileManager::Instance();
   mDirectory1 = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mDirectory2 = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mSaveFileName = mDirectory1 + "CompareResults.txt";
   mDir1TextCtrl->SetValue(mDirectory1);
   mDir2TextCtrl->SetValue(mDirectory2);
   mSaveFileTextCtrl->SetValue(mSaveFileName);

   // update file info in directory 1 and 2
   UpdateFileInfo(1);
   UpdateFileInfo(2);
   
   mSaveResultCheckBox->Enable();
   mSaveFileTextCtrl->Disable();
   mSaveBrowseButton->Disable();
   
   theOkButton->Enable();
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CompareFilesDialog::SaveData()
{
   long numFilesToCompare;
   canClose = true;
      
//    // update file info in directory 1 and 2
//    mFromString = mReplaceFromTextCtrl->GetValue();
//    mToString = mReplaceToTextCtrl->GetValue();
//    UpdateFileInfo(1);
//    UpdateFileInfo(2);
   
   if (!mNumFilesToCompareTextCtrl->GetValue().ToLong(&numFilesToCompare))
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
   
   mNumFilesToCompare = numFilesToCompare;
   
   //mFromString = mReplaceFromTextCtrl->GetValue();
   //mToString = mReplaceToTextCtrl->GetValue();
   mSaveFileName = mSaveFileTextCtrl->GetValue();

   mCompareFiles = true;
   if (mNumFilesToCompare <= 0)
   {
      wxMessageBox(wxT("There are no specific report files to compare.\n"
                       "Please check file names to compare."),
                   wxT("GMAT Warning"));
      canClose = false;
      mCompareFiles = false;
   }
   
   mSaveCompareResults = mSaveResultCheckBox->GetValue();
   
   #if DEBUG_COMPARE_FILES_DIALOG
   MessageInterface::ShowMessage
      ("CompareFilesDialog::SaveData() mNumFilesToCompare=%d, "
       "mCompareFiles=%d, mAbsTol=%e\n   mDirectory1=%s, mDirectory2=%s, "
       "mFromString=%s, mToString=%s\n", mNumFilesToCompare, mCompareFiles,
       mAbsTol, mDirectory1.c_str(), mDirectory2.c_str(), mFromString.c_str(),
       mToString.c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CompareFilesDialog::ResetData()
{
   canClose = true;
   mCompareFiles = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mDirectory1Button)
   {
      wxDirDialog dialog(this, "Select a first directory", mDirectory1);
   
      if (dialog.ShowModal() == wxID_OK)
      {
         mDirectory1 = dialog.GetPath();
         mDir1TextCtrl->SetValue(mDirectory1);
         mSaveFileTextCtrl->SetValue(mDirectory1 + "/CompareResults.txt");
         UpdateFileInfo(1);
         
         #if DEBUG_COMPARE_FILES_DIALOG
         MessageInterface::ShowMessage
            ("CompareFilesDialog::OnButtonClick() mDirectory1=%s\n",
             mDirectory1.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mDirectory2Button)
   {
      wxDirDialog dialog(this, "Select a second dierctory", mDirectory2);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         mDirectory2 = dialog.GetPath();
         mDir2TextCtrl->SetValue(mDirectory2);
         UpdateFileInfo(2);
         
         #if DEBUG_COMPARE_FILES_DIALOG
         MessageInterface::ShowMessage
            ("CompareFilesDialog::OnButtonClick() mDirectory2=%s\n",
             mDirectory2.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mUpdate1Button)
   {
      // update file info in directory 1
      mFromString = mReplaceFromTextCtrl->GetValue();
      UpdateFileInfo(1);
   }
   else if (event.GetEventObject() == mUpdate2Button)
   {
      // update file info in directory 2
      mToString = mReplaceToTextCtrl->GetValue();
      UpdateFileInfo(2);
   }
   else if (event.GetEventObject() == mSaveBrowseButton)
   {
      wxString filename =
         wxFileSelector("Choose a file to save", mDirectory1, "", "txt",
                        "Report files (*.report)|*.report|Text files (*.txt)|*.txt",
                        wxSAVE);
      
      if (!filename.empty())
      {
         mSaveFileTextCtrl->SetValue(filename);
         
         #if DEBUG_COMPARE_FILES_DIALOG
         MessageInterface::ShowMessage
            ("CompareFilesDialog::OnButtonClick() savefile=%s\n", filename.c_str());
         #endif
      }
      
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnCheckBoxChange(wxCommandEvent& event)
{
   if (mSaveResultCheckBox->IsChecked())
   {
      mSaveFileTextCtrl->Enable();
      mSaveBrowseButton->Enable();
   }
   else
   {
      mSaveFileTextCtrl->Disable();
      mSaveBrowseButton->Disable();
   }
}


//------------------------------------------------------------------------------
// void OnTextEnterPress(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareFilesDialog::OnTextEnterPress(wxCommandEvent& event)
{
   if (event.GetEventObject() == mReplaceFromTextCtrl)
   {
      mFromString = mReplaceFromTextCtrl->GetValue();
      UpdateFileInfo(1);
   }
   else if (event.GetEventObject() == mReplaceToTextCtrl)
   {
      mToString = mReplaceToTextCtrl->GetValue();
      UpdateFileInfo(2);
   }
}


//------------------------------------------------------------------------------
// void UpdateFileInfo(Integer dir)
//------------------------------------------------------------------------------
void CompareFilesDialog::UpdateFileInfo(Integer dir)
{
   if (dir == 1)
   {
      mFileNamesInDir1 = GetFilenamesContain(mDirectory1, mFromString);
      mNumFilesInDir1 = mFileNamesInDir1.GetCount();
      mNumFilesFromDir1TextCtrl->SetValue("");
      *mNumFilesFromDir1TextCtrl << mNumFilesInDir1;
   }
   else
   {
      mFileNamesInDir2 = GetFilenamesContain(mDirectory2, mToString);
      mNumFilesInDir2 = mFileNamesInDir2.GetCount();
      mNumFilesFromDir2TextCtrl->SetValue("");
      *mNumFilesFromDir2TextCtrl << mNumFilesInDir2;
   }
   
   // number of files to compare
   mNumFilesToCompareTextCtrl->SetValue("");
   if (mNumFilesInDir1 == 0 || mNumFilesInDir2 == 0)
      *mNumFilesToCompareTextCtrl << 0;
   else
      *mNumFilesToCompareTextCtrl << mNumFilesInDir1;
   
}


//------------------------------------------------------------------------------
// wxArrayString GetFilenamesContain(const wxString &dirname,
//                                   const wxString &str)
//------------------------------------------------------------------------------
wxArrayString CompareFilesDialog::GetFilenamesContain(const wxString &dirname,
                                                      const wxString &str)
{
   wxDir dir(dirname);
   wxString filename;
   wxString filepath;
   wxArrayString fileNames;
   
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt"))
      {
         // if not backup files
         if (filename.Last() == 't')
         {
            if (filename.Contains(str))
            {
               filepath = dirname + "/" + filename;
               fileNames.Add(filepath);
            }
         }
      }
      
      cont = dir.GetNext(&filename);
   }

   return fileNames;
}

