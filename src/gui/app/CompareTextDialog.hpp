//$Id$
//------------------------------------------------------------------------------
//                              CompareTextDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
   
   CompareTextDialog(wxWindow *parent, bool compareNumeric = false);
   ~CompareTextDialog();
   
   bool CompareFiles() { return mCompareFiles; }
   bool SaveCompareResults() { return mSaveCompareResults; }
   Real GetCompareTolerance() { return mTolerance; }
   Integer GetNumDirsToCompare() { return mNumDirsToCompare; }
   Integer GetNumFilesToCompare() { return mNumFilesToCompare; }
   wxString GetBaseDirectory() { return mBaseDirectory; }
   wxString GetBaseString() { return mBaseString; }
   wxString GetSaveFilename() { return mSaveFileName; }
   wxArrayString GetCompareDirectories() { return mCompareDirs; }
   wxArrayString GetCompareStrings() { return mCompareStrings; }
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mBaseDirTextCtrl;
   wxTextCtrl *mBaseStringTextCtrl;
   wxTextCtrl *mCompareDirTextCtrl;
   wxTextCtrl *mComparePrefixTextCtrl;
   wxTextCtrl *mNumFilesInBaseDirTextCtrl;
   wxTextCtrl *mNumFilesInCompareDirTextCtrl;
   wxTextCtrl *mNumDirsToCompareTextCtrl;
   wxTextCtrl *mNumFilesToCompareTextCtrl;
   wxTextCtrl *mToleranceTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   
   wxComboBox *mCompareDirsComboBox;
   wxCheckBox *mSaveResultCheckBox;
   
   wxButton *mBaseDirButton;
   wxButton *mBaseStringButton;
   wxButton *mCompareDirButton;
   wxButton *mComparePrefixButton;
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

   Real mTolerance;
   bool mIsNumericCompare;
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
   wxString mBaseString;
   wxString mSaveFileName;
   wxArrayString mCompareDirs;
   wxArrayString mCompareStrings;
   wxArrayString mFileNamesInBaseDir;
   wxArrayString mFileNamesInCompareDir;
   
   void UpdateFileInfo(Integer dir, bool isBaseDir);
   wxArrayString GetFilenames(const wxString &dirname, const wxString &prefix);
};

#endif
