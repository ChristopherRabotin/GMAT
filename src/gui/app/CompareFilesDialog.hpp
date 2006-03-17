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
   Integer GetNumFilesToCompare() { return mNumFilesToCompare; }
   Real GetAbsTolerance() { return mAbsTol; }
   wxString GetReplaceStringFrom() { return mFromString; }
   wxString GetReplaceStringTo() { return mToString; }
   wxString GetDirectory1() { return mDirectory1; }
   wxString GetDirectory2() { return mDirectory2; }
   wxString GetSaveFilename() { return mSaveFileName; }
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mDir1TextCtrl;
   wxTextCtrl *mDir2TextCtrl;
   wxTextCtrl *mNumFilesFromDir1TextCtrl;
   wxTextCtrl *mNumFilesFromDir2TextCtrl;
   wxTextCtrl *mNumFilesToCompareTextCtrl;
   wxTextCtrl *mNumFilesContainFromTextCtrl;
   wxTextCtrl *mNumFilesContainToTextCtrl;
   wxTextCtrl *mReplaceFromTextCtrl;
   wxTextCtrl *mReplaceToTextCtrl;
   wxTextCtrl *mAbsTolTextCtrl;
   wxTextCtrl *mSaveFileTextCtrl;
   
   wxCheckBox *mSaveResultCheckBox;

   wxButton *mDirectory1Button;
   wxButton *mDirectory2Button;
   wxButton *mUpdate1Button;
   wxButton *mUpdate2Button;
   wxButton *mSaveBrowseButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   void OnCheckBoxChange(wxCommandEvent& event);   
   void OnTextEnterPress(wxCommandEvent& event);   
   
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

   bool mCompareFiles;
   bool mSaveCompareResults;
   Integer mNumFilesInDir1;
   Integer mNumFilesInDir2;
   Integer mNumFilesToCompare;
   Real mAbsTol;
   wxString mFromString;
   wxString mToString;
   wxString mDirectory1;
   wxString mDirectory2;
   wxString mSaveFileName;
   wxArrayString mFileNamesInDir1;
   wxArrayString mFileNamesInDir2;

   void UpdateFileInfo(Integer dir);
   wxArrayString GetFilenamesContain(const wxString &dirname,
                                     const wxString &str);
};

#endif
