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
    
   ParameterSelectDialog(wxWindow *parent, bool showArrayAndString = false,
                         bool showSysParams = true,
                         bool canSelectMultiVars = false,
                         bool canSelectWholeObject = false);

   bool IsParamSelected()
      { return mIsParamSelected; }
   wxString GetParamName()
      { return mParamName; }
   wxArrayString& GetParamNameArray()
      { return mParamNameArray; }
   void SetParamNameArray(const wxArrayString &paramNames);
   
protected:
   // override methods from GmatDialog
   virtual void OnOK();

   wxString mParamName;
   wxArrayString mParamNameArray;
   
   bool mIsParamSelected;
   bool mCanClose;
   bool mUseUserParam;
   bool mShowArrayAndString;
   bool mShowSysVars;
   bool mCanSelectMultiVars;
   bool mCanSelectWholeObject;
   
   wxStaticText *mCoordSysLabel;
   
   wxButton *mAddParamButton;
   wxButton *mRemoveParamButton;
   wxButton *mRemoveAllParamButton;
   wxButton *mCreateVarButton;
   
   wxComboBox *mObjectComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox; //loj: 2/4/05 Added
   
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mVarListBox;
   wxListBox *mVarSelectedListBox;
   
   wxBoxSizer *mVarBoxSizer;
   wxBoxSizer *mCoordSysSizer;
   wxBoxSizer *mParamBoxSizer;

   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handling
   void OnButtonClick(wxCommandEvent& event);   
   void OnCreateVariable(wxCommandEvent& event);
   void OnSelectUserParam(wxCommandEvent& event);
   void OnSelectProperty(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
        
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_COMBOBOX,
      ID_BUTTON,
      ADD_VAR_BUTTON,
      CREATE_VARIABLE,
      VAR_SEL_LISTBOX,
      USER_PARAM_LISTBOX,
      PROPERTY_LISTBOX,

   };
   
private:
   wxString FormParamName();
   Parameter* GetParameter(const wxString &name);
   void ShowCoordSystem();
   
};

#endif
