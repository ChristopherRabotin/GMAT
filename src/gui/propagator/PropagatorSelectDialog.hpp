//$Header$
//------------------------------------------------------------------------------
//                              PropagatorSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/10/19
//
/**
 * Declares PropagatorSelectDialog class. This class shows dialog window where
 * propagator can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef PropagatorSelectDialog_hpp
#define PropagatorSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h>    // for wxArrayString

class PropagatorSelectDialog : public GmatDialog
{
public:
   
   PropagatorSelectDialog(wxWindow *parent, const wxString &propName);
   
   bool HasSelectionChanged();
   wxString GetPropagatorName();
   
private:

   bool mHasSelectionChanged;
   wxString mPropName;
   wxString mNewPropName;
   
   wxButton *mAddButton;
   wxButton *mRemoveButton;
   wxButton *mCancelButton;
   
   wxListBox *mPropagatorListBox;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif

