//$Header$
//------------------------------------------------------------------------------
//                              ReportPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/08/10
/**
 * Declares ReportPanel class. This class allows user to setup Report command.
 */
//------------------------------------------------------------------------------
#ifndef ReportPanel_hpp
#define ReportPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class ReportPanel: public GmatPanel
{
public:
   ReportPanel(wxWindow *parent, GmatCommand *cmd);
   ~ReportPanel();
   
protected:
   GmatCommand *theCommand;
   
   int  mNumSc;
   int  mNumImpBurn;
   int  mNumScProperty;
   int  mNumImpBurnProperty;
   int  mNumVarParams;
   bool mUseUserParam;
   bool mIsReportFileChanged;
   bool mIsParameterChanged;
   
   wxString *mSpacecraftList;
   wxString *mImpBurnList;
   wxArrayString mSpacecraftPropertyList;
   wxArrayString mImpBurnPropertyList;
   
   wxString mLastCoordSysName;
   wxArrayString mObjectTypeList;
   
   wxStaticText *mCoordSysLabel;
   
   wxComboBox *mReportFileComboBox;
   wxComboBox *mObjectTypeComboBox;
   wxComboBox *mObjectComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox;
   
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mVarListBox;
   wxListBox *mVarSelectedListBox;
   
   wxBoxSizer *mCoordSysSizer;
   wxBoxSizer *mParamBoxSizer;
   
   void OnMoveUpVariable(wxCommandEvent& event);
   void OnMoveDownVariable(wxCommandEvent& event);
   
   void OnAddVariable(wxCommandEvent& event);
   void OnRemoveVariable(wxCommandEvent& event);
   void OnClearVariable(wxCommandEvent& event);
   
   void OnSelectUserParam(wxCommandEvent& event);
   void OnSelectProperty(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCreateVariable(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      RF_TEXT = 93000,      
      UP_VAR_BUTTON,
      DOWN_VAR_BUTTON,
      
      ADD_VAR_BUTTON,
      REMOVE_VAR_BUTTON,
      CLEAR_VAR_BUTTON,
      VAR_SEL_LISTBOX,
      CREATE_VARIABLE,
      ID_COMBOBOX,
      USER_PARAM_LISTBOX,
      PROPERTY_LISTBOX,
   };

private:
   wxString GetParamName();
   Parameter* GetParameter(const wxString &name);
   void ShowCoordSystem();
};
#endif
