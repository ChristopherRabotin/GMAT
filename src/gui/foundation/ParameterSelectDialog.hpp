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
#include "GuiItemManager.hpp"

class ParameterSelectDialog : public GmatDialog
{
public:
   
   ParameterSelectDialog(wxWindow *parent,
                         const wxString &ownerType = "Spacecraft",
                         int showOption = GuiItemManager::SHOW_PLOTTABLE,
                         bool showVariable = true,
                         bool showArray = false,
                         bool showSysParams = true,
                         bool canSelectMultiVars = false,
                         bool canSelectWholeObject = false,
                         bool createParam = true);
   ~ParameterSelectDialog();
   
   bool IsParamSelected()
      { return mIsParamSelected; }
   wxString GetParamName()
      { return mParamName; }
   wxArrayString& GetParamNameArray()
      { return mParamNameArray; }
   void SetParamNameArray(const wxArrayString &paramNames);
   
protected:
   // override methods from GmatDialog
   virtual void OnOK(wxCommandEvent &event);

   wxString mParamName;
   wxString mOwnerType;
   wxString mLastCoordSysName;
   wxArrayString mParamNameArray;
   int  mShowOption;
   bool mIsParamSelected;
   bool mCanClose;
   bool mUseUserParam;
   bool mShowVariable;
   bool mShowArray;
   bool mShowSysVars;
   bool mCanSelectMultiVars;
   bool mCanSelectWholeObject;
   bool mCreateParam;
   int  mLastUserParamSelection;
   int  mLastPropertySelection;
   
   wxStaticText *mCoordSysLabel;
   
   wxButton *mAddParamButton;
   wxButton *mRemoveParamButton;
   wxButton *mRemoveAllParamButton;
   wxButton *mCreateVarButton;
   
   wxComboBox *mObjectComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox;
   
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
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
   void OnDoubleClick(wxCommandEvent& event);
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
   void HighlightObject(wxCommandEvent& event, bool highlight);
};

#endif
