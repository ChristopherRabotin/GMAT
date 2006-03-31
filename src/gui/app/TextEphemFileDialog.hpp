//$Header$
//------------------------------------------------------------------------------
//                              TextEphemFileDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2006/03/29
//
/**
 * Shows dialog for setting parameters for ephemeris file generation.
 * 
 */
//------------------------------------------------------------------------------
#ifndef TextEphemFileDialog_hpp
#define TextEphemFileDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class TextEphemFileDialog : public GmatDialog
{
public:
   
   TextEphemFileDialog(wxWindow *parent);
   ~TextEphemFileDialog();
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mHeaderFileTextCtrl;
   wxTextCtrl *mDataFileTextCtrl;
   wxTextCtrl *mIntervalTextCtrl;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mEpochFormatComboBox;
   //wxCheckBox *mSeparateFilesCheckBox;

   wxListBox *mSpacecraftListBox;
   wxListBox *mSelectedScListBox;
   
   wxButton *mAddScButton;
   wxButton *mRemoveScButton;
   wxButton *mClearScButton;
   wxButton *mHeaderFileBrowseButton;
   wxButton *mDataFileBrowseButton;
   wxButton *mRunButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   //void OnCheckBoxChange(wxCommandEvent& event);   
   //void OnComboBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_LISTBOX,
   };
   
private:

   wxString mEphemDirectory;

   bool CreateTextEphem();
};

#endif
