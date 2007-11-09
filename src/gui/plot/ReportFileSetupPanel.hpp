//$Id$
//------------------------------------------------------------------------------
//                              ReportFileSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Declares ReportFileSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef ReportFileSetupPanel_hpp
#define ReportFileSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "ReportFile.hpp"

class ReportFileSetupPanel: public GmatPanel
{
public:
   ReportFileSetupPanel(wxWindow *parent, const wxString &subscriberName);
   ~ReportFileSetupPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
   
protected:
   ReportFile *reportFile;

   int  mNumSc;
   int  mNumImpBurn;
   int  mNumScProperty;
   int  mNumImpBurnProperty;
   int  mNumVarParams;
   bool mUseUserParam;
   bool mParamChanged;
   
   wxString *mSpacecraftList;
   wxString *mImpBurnList;
   wxArrayString mSpacecraftPropertyList;
   wxArrayString mImpBurnPropertyList;
      
   wxString mLastCoordSysName;
   wxArrayString mObjectTypeList;
   
   wxStaticText *mCoordSysLabel;
   wxStaticText *fileStaticText;
   
   wxTextCtrl *colWidthTextCtrl;
   wxTextCtrl *precisionTextCtrl;
   wxTextCtrl *fileTextCtrl;
   
   wxButton *browseButton; 
   
   wxCheckBox *writeCheckBox;
   wxCheckBox *showHeaderCheckBox;
   wxCheckBox *leftJustifyCheckBox;
   wxCheckBox *zeroFillCheckBox;
   
   wxComboBox *mSolverIterComboBox;
   wxComboBox *mObjectTypeComboBox;
   wxComboBox *mSpacecraftComboBox;
   wxComboBox *mImpBurnComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox;
   
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mVarListBox;
   wxListBox *mVarSelectedListBox;
   
   wxBoxSizer *mCoordSysSizer;
   wxBoxSizer *mParamBoxSizer;
   
   void OnWriteCheckBoxChange(wxCommandEvent& event);
   void OnBrowseButton(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent &event);
   
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
      RF_WRITE_CHECKBOX,
        
      ID_TEXT,
      ID_TEXT_CTRL,
      ID_BROWSE_BUTTON,
        
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

      CHECKBOX,
   };

private:
   void ShowCoordSystem();
   wxString GetParamName();
   Parameter* GetParameter(const wxString &name);
   wxComboBox* GetObjectComboBox();
};
#endif
