//$Id$
//------------------------------------------------------------------------------
//                              RunScriptFolderDialog
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
#ifndef RunScriptFolderDialog_hpp
#define RunScriptFolderDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class RunScriptFolderDialog : public GmatDialog
{
public:
   
   RunScriptFolderDialog(wxWindow *parent, int numScripts, Real absTol,
                         const wxString &compareDir);
   ~RunScriptFolderDialog();
   
   wxString GetFilterString(bool &exclude);
   
   bool RunScripts() { return mRunScripts; }
   bool RunFromSavedScripts() { return mRunFromSavedScripts; }
   bool CompareResults() { return mCompareResults; }
   bool SaveCompareResults() { return mSaveCompareResults; }
   bool HasOutDirChanged() { return mOutDirChanged; }
   bool CreateRunFolder() { return mCreateRunFolder; }
   Real GetAbsTolerance() { return mAbsTol; }
   Integer  GetStartingScriptNumber() { return mNumStartingScript; }
   Integer  GetNumScriptsToRun() { return mNumScriptsToRun; }
   Integer  GetNumTimesToRun() { return mNumTimesToRun; }
   wxString GetReplaceString() { return mReplaceString; }
   wxString GetCompareDirectory() { return mCompareDir; }
   wxString GetSaveFilename() { return mSaveFilename; }
   wxString GetCurrentOutDir() { return mCurrOutDir; }
   wxString GetSaveScriptsDir() { return mSaveScriptsDir; }
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mStartingScriptTextCtrl;
   wxTextCtrl *mNumScriptsToRunTextCtrl;
   wxTextCtrl *mFilterStringTextCtrl;
   wxTextCtrl *mNumTimesToRunTextCtrl;
   wxTextCtrl *mCompareDirTextCtrl;
   wxTextCtrl *mReplaceTextCtrl;
   wxTextCtrl *mAbsTolTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   wxTextCtrl *mCurrOutDirTextCtrl;
   wxTextCtrl *mSaveScriptsDirTextCtrl;
   
   wxCheckBox *mRunFromSavedCheckBox;
   wxCheckBox *mExcludeScriptsCheckBox;
   wxCheckBox *mCreateRunFolderCheckBox;
   wxCheckBox *mCompareCheckBox;
   wxCheckBox *mSaveResultCheckBox;
   
   wxButton *mDirBrowseButton;
   wxButton *mSaveBrowseButton;
   wxButton *mChangeCurrOutDirButton;
   wxButton *mChangeSaveScriptsDirButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   void OnCheckBoxChange(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_CHECKBOX,
   };
   
private:

   bool mRunScripts;
   bool mRunFromSavedScripts;
   bool mCompareResults;
   bool mSaveCompareResults;
   bool mOutDirChanged;
   bool mExcludeScripts;
   bool mCreateRunFolder;
   Integer mNumStartingScript;
   Integer mNumScriptsToRun;
   Integer mNumTimesToRun;
   Real mAbsTol;
   wxString mFilterString;
   wxString mReplaceString;
   wxString mCurrOutDir;
   wxString mSaveScriptsDir;
   wxString mCompareDir;
   wxString mSaveFilename;
};

#endif
