//$Header$
//------------------------------------------------------------------------------
//                              ParameterMultiSelectDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterMultiSelectDialog class. This class shows dialog window where a
 * user parameter can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ParameterMultiSelectDialog_hpp
#define ParameterMultiSelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"

class ParameterMultiSelectDialog : public GmatDialog
{
public:
    
   ParameterMultiSelectDialog(wxWindow *parent, wxArrayString &paramNames,
                              bool showArray = false,
                              bool showSysVars = true);

   wxArrayString GetParamNames()
      { return mParamNames; }
   bool IsParamSelected()
      { return mIsParamSelected; }

protected:
   // override methods from GmatDialog
   virtual void OnOK();

private:
   wxArrayString mParamNames;

   bool mIsParamSelected;
   bool mCanClose;
   bool mUseUserParam;
   bool mShowArray;
   bool mShowSysVars;
   
   wxButton *mAddParamButton;
   wxButton *mRemoveParamButton;
   wxButton *mRemoveAllParamButton;
   wxButton *mCreateParamButton;
   wxComboBox *mObjectComboBox;
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mParamSelectedListBox;
   wxStaticBox *systemParamStaticBox;
   wxStaticText *objectStaticText;
   wxStaticText *propertyStaticText;
   wxStaticBoxSizer *systemParamBoxSizer;
      
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   wxString GetNewParam();
   Parameter* CreateParameter(const wxString &name);

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
