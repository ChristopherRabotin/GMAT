//$Id$
//------------------------------------------------------------------------------
//                              CompareFilesDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2005/12/22
//
/**
 * Shows dialog where various run script folder option can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef CompareFilesDialog_hpp
#define CompareFilesDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class CompareFilesDialog : public GmatDialog
{
public:
   
   CompareFilesDialog(wxWindow *parent);
   ~CompareFilesDialog();
   
   bool CompareFiles() { return mCompareFiles; }
   bool SaveCompareResults() { return mSaveCompareResults; }
   Integer GetNumDirsToCompare() { return mNumDirsToCompare; }
   Integer GetNumFilesToCompare() { return mNumFilesToCompare; }
   Real GetAbsTolerance() { return mAbsTol; }
   wxString GetBaseString() { return mBaseString; }
   wxString GetBaseDirectory() { return mBaseDirectory; }
   wxArrayString GetCompareStrings() { return mCompareStrings; }
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
   wxTextCtrl *mBaseStrTextCtrl;
   wxTextCtrl *mCompareStrTextCtrl;
   wxTextCtrl *mAbsTolTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   
   wxComboBox *mCompareDirsComboBox;
   wxCheckBox *mSaveResultCheckBox;

   wxButton *mBaseDirButton;
   wxButton *mCompareDirButton;
   wxButton *mBaseUpdateButton;
   wxButton *mCompareUpdateButton;
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
   Real mAbsTol;
   wxString mBaseString;
   wxString mBaseDirectory;
   wxArrayString mCompareStrings;
   wxArrayString mCompareDirs;
   wxString mSaveFileName;
   wxArrayString mFileNamesInBaseDir;
   wxArrayString mFileNamesInCompareDir;

   void UpdateFileInfo(Integer dir, bool isBaseDir);
   wxArrayString GetFilenamesContain(const wxString &dirname,
                                     const wxString &str);
};

#endif
