//$Id$
//------------------------------------------------------------------------------
//                              MultiPathSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2008/04/16
//
/**
 * Shows dialog for setting path for various files used in the system.
 * 
 */
//------------------------------------------------------------------------------
#ifndef MultiPathSetupPanel_hpp
#define MultiPathSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"

class MultiPathSetupPanel : public wxPanel
{
public:
   
   MultiPathSetupPanel(wxWindow *parent, const StringArray &pathNames);
   ~MultiPathSetupPanel();
   
   const wxArrayString& GetPathNames();
   bool HasDataChanged();
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   wxTextCtrl *mFileTextCtrl;
   wxListBox  *mPathListBox;
   wxButton   *mReplaceButton;
   wxButton   *mRemoveButton;
   wxButton   *mAddButton;
   
   // event handling
   void OnListBoxSelect(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);   
   void OnUpButtonClick(wxCommandEvent& event);   
   void OnDownButtonClick(wxCommandEvent& event);   
   void OnRemoveButtonClick(wxCommandEvent& event);   
   void OnBrowseButtonClick(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_LISTBOX,
      ID_UP_BUTTON,
      ID_DOWN_BUTTON,
      ID_REMOVE_BUTTON,
      ID_BROWSE_BUTTON,
   };
   
private:
   
   /// file path names
   wxArrayString mPathNames;
   bool mHasDataChanged;
};

#endif
