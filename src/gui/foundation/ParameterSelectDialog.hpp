//$Id$
//------------------------------------------------------------------------------
//                              ParameterSelectDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
                         const wxArrayString &objectTypeList,
                         int  showOption = GuiItemManager::SHOW_PLOTTABLE,
                         bool allowMultiSelect = false,
                         bool allowString = false,
                         bool allowWholeObject = false,
                         bool allowSysParam = true,
                         bool allowVariable = true,
                         bool allowArray = true,
                         const wxString &objectType = "Spacecraft",
                         bool createParam = true);
   ~ParameterSelectDialog();

   bool HasSelectionChanged();
   bool IsParamSelected();
   wxString GetParamName();
   void SetObjectType(const wxString &objType);
   wxArrayString& GetParamNameArray();
   void SetParamNameArray(const wxArrayString &paramNames);
   
protected:
   
   wxString mParamName;
   wxArrayString mObjectTypeList;
   wxString mObjectType;
   wxString mLastCoordSysName;
   wxArrayString mParamNameArray;
   
   bool mHasSelectionChanged;
   bool mIsParamSelected;
   bool mIsAddingMode;
   bool mCanClose;
   bool mUseUserParam;
   bool mAllowSysParam;
   bool mAllowVariable;
   bool mAllowArray;
   bool mAllowString;
   bool mAllowMultiSelect;
   bool mAllowWholeObject;
   bool mCreateParam;
   
   int  mShowOption;
   int  mNumRow;
   int  mNumCol;
   
   wxArrayInt mLastObjectSelections;
   wxArrayInt mLastPropertySelections;
   
   wxStaticText *mRowStaticText;
   wxStaticText *mColStaticText;
   
   wxTextCtrl *mRowTextCtrl;
   wxTextCtrl *mColTextCtrl;
   
   wxStaticText *mCoordSysLabel;
   
   wxButton *mUpButton;
   wxButton *mDownButton;
   wxButton *mAddButton;
   wxButton *mRemoveButton;
   wxButton *mAddAllButton;
   wxButton *mRemoveAllButton;
   
   wxComboBox *mObjectTypeComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox;
   
   wxListBox *mUserParamListBox;
   wxListBox *mObjectListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mSelectedListBox;
   
   wxCheckBox *mEntireObjectCheckBox;
   
   wxBoxSizer *mVarBoxSizer;
   wxBoxSizer *mCoordSysSizer;
   wxSizer *mParameterSizer;
   
   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // virtual methods from GmatDialog
   virtual void OnCancel(wxCommandEvent &event);
   
   // event handling
   void OnButtonClick(wxCommandEvent& event);
   void OnListBoxSelect(wxCommandEvent& event);
   void OnListBoxDoubleClick(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      TEXT_ID = 9300,
      TEXTCTRL_ID,
      COMBOBOX_ID,
      BUTTON_ID,
      LISTBOX_ID,
      CHECKBOX_ID,
   };
   
private:
   
   bool AddWholeObject();
   bool AddParameter();
   void AddParameter(const wxString &param);
   bool AddMultipleSelections();
   void AddAll();
   void RemoveParameter();
   void ShowArrayInfo(bool show);
   void ShowCoordSystem();
   void ClearProperties();
   void DeselectAllObjects();
   void DeselectObjects(wxArrayInt &newSelects, wxArrayInt &oldSelects);
   int  GetLastPropertySelection();
   
   wxString GetObjectSelection();
   wxString GetPropertySelection();
   wxString FormParameterName();
   Parameter* GetParameter(const wxString &name);
};

#endif
