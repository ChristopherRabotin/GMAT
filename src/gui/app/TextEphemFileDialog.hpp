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
   bool CreateEphemFile() { return mCreateEphemFile; }
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   wxTextCtrl *mEphemFileTextCtrl;
   wxTextCtrl *mIntervalTextCtrl;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mEpochFormatComboBox;

   wxListBox *mSpacecraftListBox;
   wxListBox *mSelectedScListBox;
   
   wxButton *mAddScButton;
   wxButton *mRemoveScButton;
   wxButton *mClearScButton;
   wxButton *mEphemFileBrowseButton;
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
      ID_LISTBOX,
   };
   
private:

   wxString mEphemDirectory;
   bool mCreateEphemFile;
   
   bool CreateTextEphem();
};

#endif
