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
 * 
 */
//------------------------------------------------------------------------------
#ifndef CompareTextDialog_hpp
#define CompareTextDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class CompareTextDialog : public GmatDialog
{
public:
   
   CompareTextDialog(wxWindow *parent);
   ~CompareTextDialog();
   
   bool CompareFiles() { return mCompareFiles; }
   bool SaveCompareResults() { return mSaveCompareResults; }
   Integer GetNumDirsToCompare() { return mNumDirsToCompare; }
   Integer GetNumFilesToCompare() { return mNumFilesToCompare; }
   wxString GetBaseDirectory() { return mBaseDirectory; }
   wxArrayString GetCompareDirectories() { return mCompareDirs; }
   wxString GetSaveFilename() { return mSaveFileName; }
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mBaseDirTextCtrl;
   wxTextCtrl *mCompareDirTextCtrl;
   wxTextCtrl *mNumFilesInBaseDirTextCtrl;
   wxTextCtrl *mNumFilesInCompareDirTextCtrl;
   wxTextCtrl *mNumDirsToCompareTextCtrl;
   wxTextCtrl *mNumFilesToCompareTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   
   wxComboBox *mCompareDirsComboBox;
   wxCheckBox *mSaveResultCheckBox;

   wxButton *mBaseDirButton;
   wxButton *mCompareDirButton;
   wxButton *mSaveBrowseButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   void OnCheckBoxChange(wxCommandEvent& event);   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextEnterPress(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
      ID_CHECKBOX,
   };
   
private:

   bool mCompareFiles;
   bool mSaveCompareResults;
   bool mHasDir1;
   bool mHasDir2;
   bool mHasDir3;
   Integer mNumFilesInBaseDir;
   Integer mNumFilesInCompareDir;
   Integer mNumFilesToCompare;
   Integer mNumDirsToCompare;
   wxString mBaseDirectory;
   wxArrayString mCompareDirs;
   wxString mSaveFileName;
   wxArrayString mFileNamesInBaseDir;
   wxArrayString mFileNamesInCompareDir;

   void UpdateFileInfo(Integer dir, bool isBaseDir);
   wxArrayString GetFilenames(const wxString &dirname);
};

#endif
