//$Id$
//------------------------------------------------------------------------------
//                              SinglePathSetupPanel
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
#ifndef SinglePathSetupPanel_hpp
#define SinglePathSetupPanel_hpp

#include "gmatwxdefs.hpp"

class SinglePathSetupPanel : public wxPanel
{
public:
   
   SinglePathSetupPanel(wxWindow *parent, const wxString &filepath);
   ~SinglePathSetupPanel();
   
   bool HasDataChanged();
   wxString GetFullPathName();
   
protected:
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   wxTextCtrl *mFileTextCtrl;   
   
   // event handling
   void OnBrowseButtonClick(wxCommandEvent& event);   
   void OnTextChange(wxCommandEvent& event);   
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_TEXTCTRL,
      ID_BROWSE_BUTTON,
   };
   
private:
   
   /// file path and name
   wxString mFullPathName;
   bool mHasDataChanged;
};

#endif
