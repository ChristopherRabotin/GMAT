//$Header$
//------------------------------------------------------------------------------
//                              ParameterSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterSelectDialog_hpp
#define ParameterSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class ParameterSelectDialog : public GmatDialog
{
public:
    
   ParameterSelectDialog(wxWindow *parent);

   wxString GetParamName()
      { return mParamName; }
   bool IsParamSelected()
      { return mIsParamSelected; }

protected:
   // override methods from GmatDialog
   virtual void OnOK();

private:
   wxString mParamName;
   bool mIsParamSelected;
   bool mCanClose;
   bool mUseUserParam;
   
   wxButton *mAddParamButton;
   wxButton *mCreateParamButton;
   wxComboBox *mObjectComboBox;
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mParamSelectedListBox;
      
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnButton(wxCommandEvent& event);
   void OnListSelect(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
        
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_COMBOBOX,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif
