//$Header$
//------------------------------------------------------------------------------
//                              CompareTextDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2006/06/27
//
/**
 * Shows dialog for comparing text line by line.
 */
//------------------------------------------------------------------------------

#include "CompareTextDialog.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>
#include <wx/filename.h>

//#define DEBUG_COMPARE_TEXT_DIALOG 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CompareTextDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON, CompareTextDialog::OnButtonClick)
   EVT_CHECKBOX(ID_CHECKBOX, CompareTextDialog::OnCheckBoxChange)
   EVT_COMBOBOX(ID_COMBOBOX, CompareTextDialog::OnComboBoxChange)
   EVT_TEXT_ENTER(ID_TEXTCTRL, CompareTextDialog::OnTextEnterPress)
END_EVENT_TABLE()

   
//------------------------------------------------------------------------------
// CompareTextDialog(wxWindow *parent)
//------------------------------------------------------------------------------
CompareTextDialog::CompareTextDialog(wxWindow *parent)
   : GmatDialog(parent, -1, wxString(_T("CompareTextDialog")))
{
   mCompareFiles = false;
   mSaveCompareResults = false;
   mHasDir1 = false;
   mHasDir2 = false;
   mHasDir3 = false;
   
   mNumFilesToCompare = 0;
   mNumDirsToCompare = 1;
   mBaseDirectory = "";

   mCompareDirs.Add("");
   mCompareDirs.Add("");
   mCompareDirs.Add("");
   
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~CompareTextDialog()
//------------------------------------------------------------------------------
CompareTextDialog::~CompareTextDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void CompareTextDialog::Create()
{
   #if DEBUG_COMPARE_TEXT_DIALOG
   MessageInterface::ShowMessage("CompareTextDialog::Create() entered.\n");
   #endif
   
   int bsize = 2;
   
   //------------------------------------------------------
   // compare directory
   //------------------------------------------------------
   wxStaticText *baseDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Base Directory:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mBaseDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mBaseDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,20), 0);
   
   wxStaticText *numFilesBaseDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files (.txt, .report, .data, .script):"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesInBaseDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
      
   wxFlexGridSizer *baseDirGridSizer = new wxFlexGridSizer(2, 0, 0);
   baseDirGridSizer->Add(baseDirLabel, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirGridSizer->Add(20,20, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirGridSizer->Add(mBaseDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirGridSizer->Add(mBaseDirButton, 0, wxALIGN_CENTER|wxALL, bsize);
 
   wxFlexGridSizer *baseFileGridSizer = new wxFlexGridSizer(3, 0, 0);
   baseFileGridSizer->Add(numFilesBaseDirLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   baseFileGridSizer->Add(mNumFilesInBaseDirTextCtrl, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   
   
 #if __WXMAC__
   wxStaticText *title1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Base Directory"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    false, _T(""), wxFONTENCODING_SYSTEM));
                                             
   wxBoxSizer *baseDirSizer = new wxBoxSizer( wxVERTICAL );
   baseDirSizer->Add(title1StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   
#else  
   wxStaticBoxSizer *baseDirSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Base Directory");
#endif

   baseDirSizer->Add(baseDirGridSizer, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   baseDirSizer->Add(baseFileGridSizer, 0, wxALIGN_RIGHT|wxALL|wxGROW, bsize);

   
 
   //------------------------------------------------------
   // compare file names
   //------------------------------------------------------
   wxString dirArray[] = {"Directory1", "Directory2", "Directory3"};
   
   mCompareDirsComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT("Compare Directories"),
                     wxDefaultPosition, wxDefaultSize, 3, dirArray,
                     wxCB_READONLY);
   mCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(320,20), 0);
   
   mCompareDirButton =
      new wxButton(this, ID_BUTTON, wxT("Browse"),
                    wxDefaultPosition, wxSize(60,20), 0);
   
   wxStaticText *numFilesInCompareDirLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files (.txt, .report, .data, .script):"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesInCompareDirTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   //---------- sizer
   wxFlexGridSizer *compareDirGridSizer = new wxFlexGridSizer(2, 0, 0);   
   compareDirGridSizer->Add(mCompareDirsComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirGridSizer->Add(20, 20, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirGridSizer->Add(mCompareDirTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   compareDirGridSizer->Add(mCompareDirButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   wxFlexGridSizer *compareFileGridSizer = new wxFlexGridSizer(3, 0, 0);   
   compareFileGridSizer->Add(numFilesInCompareDirLabel, 0,
                             wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   compareFileGridSizer->Add(mNumFilesInCompareDirTextCtrl, 0,
                             wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   

#if __WXMAC__
   wxStaticText *title2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Compare Directories"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title2StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    false, _T(""), wxFONTENCODING_SYSTEM));
   
   wxBoxSizer *compareDirsSizer = new wxBoxSizer( wxVERTICAL );
   compareDirsSizer->Add(title2StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
#else
   wxStaticBoxSizer *compareDirsSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Compare Directories");
#endif
   
   compareDirsSizer->Add(compareDirGridSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   compareDirsSizer->Add(compareFileGridSizer, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
   
   //------------------------------------------------------
   // compare results
   //------------------------------------------------------
   wxStaticText *numDirsToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Directories to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumDirsToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
   wxStaticText *numFilesToCompareLabel =
      new wxStaticText(this, ID_TEXT, wxT("Number of Files to Compare:"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   mNumFilesToCompareTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT("0"),
                     wxDefaultPosition, wxSize(80,20), 0);
   
//    wxStaticText *tolLabel =
//       new wxStaticText(this, ID_TEXT, wxT("Tolerance to be Used in Flagging:"),
//                        wxDefaultPosition, wxDefaultSize, 0);
   
   
   //---------- sizer
   wxFlexGridSizer *numFilesGridSizer = new wxFlexGridSizer(2, 0, 0);
   numFilesGridSizer->Add(numDirsToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mNumDirsToCompareTextCtrl, 0,
                          wxALIGN_RIGHT|wxALL|wxGROW, bsize);
   numFilesGridSizer->Add(numFilesToCompareLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   numFilesGridSizer->Add(mNumFilesToCompareTextCtrl, 0,
                          wxALIGN_RIGHT|wxALL|wxGROW, bsize);   
//    numFilesGridSizer->Add(tolLabel, 0, wxALIGN_LEFT|wxALL, bsize);

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
                    wxDefaultPosition, wxSize(60,20), 0);

   //---------- sizer
   wxFlexGridSizer *saveGridSizer = new wxFlexGridSizer(2, 0, 0);
   saveGridSizer->Add(mSaveFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   saveGridSizer->Add(mSaveBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
#if __WXMAC__
   wxStaticText *title3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Compare"),
                        wxDefaultPosition, wxSize(220,20),
                        wxBOLD);
   title3StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    false, _T(""), wxFONTENCODING_SYSTEM));
   
   wxBoxSizer *compareSizer = new wxBoxSizer( wxVERTICAL );
   compareSizer->Add(title3StaticText, 0, wxALIGN_LEFT|wxALL|wxGROW, bsize);
#else
   wxStaticBoxSizer *compareSizer =
      new wxStaticBoxSizer(wxVERTICAL, this, "Compare");
#endif
   
   compareSizer->Add(numFilesGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(mSaveResultCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(20, 5, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(saveFileLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   compareSizer->Add(saveGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   //------------------------------------------------------
   // add to page sizer
   //------------------------------------------------------
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   pageBoxSizer->Add(baseDirSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareDirsSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);
   pageBoxSizer->Add(compareSizer, 0, wxALIGN_CENTRE|wxALL|wxGROW, bsize);

   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void CompareTextDialog::LoadData()
{
   wxString str;
   str.Printf("%d", mNumFilesToCompare);
   mNumFilesToCompareTextCtrl->SetValue(str);
   
   str.Printf("%d", mNumDirsToCompare);
   mNumDirsToCompareTextCtrl->SetValue(str);


   FileManager *fm = FileManager::Instance();
   mBaseDirectory = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[0] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[1] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirs[2] = fm->GetFullPathname(FileManager::OUTPUT_PATH).c_str();
   mCompareDirsComboBox->SetSelection(0);
   mSaveFileName = mBaseDirectory + "CompareTextResults.txt";
   mBaseDirTextCtrl->SetValue(mBaseDirectory);
   mCompareDirTextCtrl->SetValue(mCompareDirs[0]);
   mSaveFileTextCtrl->SetValue(mSaveFileName);

   // update file info in directory 1 and 2
   UpdateFileInfo(0, true);
   UpdateFileInfo(1, false);
   
   mSaveResultCheckBox->Enable();
   mSaveFileTextCtrl->Disable();
   mSaveBrowseButton->Disable();
   
   theOkButton->Enable();
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void CompareTextDialog::SaveData()
{
   long longNum;
   canClose = true;
      
   if (!mNumFilesToCompareTextCtrl->GetValue().ToLong(&longNum))
   {
      wxMessageBox("Invalid number of scripts to run entered.");
      canClose = false;
      return;
   }
   
   mNumFilesToCompare = longNum;
   
   if (!mNumDirsToCompareTextCtrl->GetValue().ToLong(&longNum))
   {
      wxMessageBox("Invalid number of scripts to run entered.");
      canClose = false;
      return;
   }
   
   mNumDirsToCompare = longNum;
   
   
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
   
   #if DEBUG_COMPARE_TEXT_DIALOG
   MessageInterface::ShowMessage
      ("CompareTextDialog::SaveData() mNumFilesToCompare=%d, "
       "mCompareFiles=%d\n   mBaseDirectory=%s, mCompareDirs[0]=%s\n",
       mNumFilesToCompare, mCompareFiles,
       mBaseDirectory.c_str(), mCompareDirs[0].c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void CompareTextDialog::ResetData()
{
   canClose = true;
   mCompareFiles = false;
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mBaseDirButton)
   {
      wxDirDialog dialog(this, "Select a base directory", mBaseDirectory);
   
      if (dialog.ShowModal() == wxID_OK)
      {
         mBaseDirectory = dialog.GetPath();
         mBaseDirTextCtrl->SetValue(mBaseDirectory);
         mSaveFileTextCtrl->SetValue(mBaseDirectory + "/CompareTextResults.txt");
         UpdateFileInfo(0, true);
         
         #if DEBUG_COMPARE_TEXT_DIALOG
         MessageInterface::ShowMessage
            ("CompareTextDialog::OnButtonClick() mBaseDirectory=%s\n",
             mBaseDirectory.c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mCompareDirButton)
   {
      int dirIndex = mCompareDirsComboBox->GetSelection();
      wxDirDialog dialog(this, "Select a compare dierctory", mCompareDirs[dirIndex]);
      
      if (dialog.ShowModal() == wxID_OK)
      {
         if (dirIndex == 0)
            mHasDir1 = true;
         else if (dirIndex == 1)
            mHasDir2 = true;
         else if (dirIndex == 2)
            mHasDir3 = true;
         
         mCompareDirs[dirIndex] = dialog.GetPath();
         mCompareDirTextCtrl->SetValue(mCompareDirs[dirIndex]);
         UpdateFileInfo(dirIndex, false);

         // update number of directories to compare
         int numDirs = 0;
         if (mHasDir1) numDirs++;
         if (mHasDir2) numDirs++;
         if (mHasDir3) numDirs++;
         mNumDirsToCompare = numDirs;
         
         wxString str;
         str.Printf("%d", mNumDirsToCompare);
         mNumDirsToCompareTextCtrl->SetValue(str);
         
         #if DEBUG_COMPARE_TEXT_DIALOG
         MessageInterface::ShowMessage
            ("CompareTextDialog::OnButtonClick() mCompareDirs[%d]=%s\n",
             dirIndex, mCompareDirs[dirIndex].c_str());
         #endif
      }
   }
   else if (event.GetEventObject() == mSaveBrowseButton)
   {
      wxString filename =
         wxFileSelector("Choose a file to save", mBaseDirectory, "", "txt",
                        "Report files (*.report)|*.report|Text files (*.txt)|*.txt",
                        wxSAVE);
      
      if (!filename.empty())
      {
         mSaveFileTextCtrl->SetValue(filename);
         
         #if DEBUG_COMPARE_TEXT_DIALOG
         MessageInterface::ShowMessage
            ("CompareTextDialog::OnButtonClick() savefile=%s\n", filename.c_str());
         #endif
      }
      
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnCheckBoxChange(wxCommandEvent& event)
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
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnComboBoxChange(wxCommandEvent& event)
{
   if (event.GetEventObject() == mCompareDirsComboBox)
   {
      int currDir = mCompareDirsComboBox->GetSelection();
      mCompareDirTextCtrl->SetValue(mCompareDirs[currDir]);
   }
}


//------------------------------------------------------------------------------
// void OnTextEnterPress(wxCommandEvent& event)
//------------------------------------------------------------------------------
void CompareTextDialog::OnTextEnterPress(wxCommandEvent& event)
{
}


//------------------------------------------------------------------------------
// void UpdateFileInfo(Integer dir)
//------------------------------------------------------------------------------
void CompareTextDialog::UpdateFileInfo(Integer dir, bool isBaseDir)
{
   if (isBaseDir)
   {
      mFileNamesInBaseDir = GetFilenames(mBaseDirectory);
      mNumFilesInBaseDir = mFileNamesInBaseDir.GetCount();
      mNumFilesInBaseDirTextCtrl->SetValue("");
      *mNumFilesInBaseDirTextCtrl << mNumFilesInBaseDir;
   }
   else
   {
      mFileNamesInCompareDir = GetFilenames(mCompareDirs[dir]);
      mNumFilesInCompareDir = mFileNamesInCompareDir.GetCount();
      mNumFilesInCompareDirTextCtrl->SetValue("");
      *mNumFilesInCompareDirTextCtrl << mNumFilesInCompareDir;
   }
   
   // number of files to compare
   mNumFilesToCompareTextCtrl->SetValue("");
   if (mNumFilesInBaseDir == 0 || mNumFilesInCompareDir == 0)
      *mNumFilesToCompareTextCtrl << 0;
   else
      *mNumFilesToCompareTextCtrl << mNumFilesInBaseDir;
   
}


//------------------------------------------------------------------------------
// wxArrayString GetFilenames(const wxString &dirname)
//------------------------------------------------------------------------------
wxArrayString CompareTextDialog::GetFilenames(const wxString &dirname)
{
   wxDir dir(dirname);
   wxString filename;
   wxString filepath;
   wxArrayString fileNames;
   
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt") ||
          filename.Contains(".data") || filename.Contains(".script"))
      {
         // if not backup files
         if (filename.Last() == 't' || filename.Last() == 'a')
         {
            filepath = dirname + "/" + filename;
            fileNames.Add(filepath);
         }
      }
      
      cont = dir.GetNext(&filename);
   }

   return fileNames;
}

