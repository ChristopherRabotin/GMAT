//$Id$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
/**
 * Declares XyPlotSetupPanel class. This class allows user to setup XY Plot.
 */
//------------------------------------------------------------------------------
#ifndef XyPlotSetupPanel_hpp
#define XyPlotSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "TsPlot.hpp"
#include "RgbColor.hpp"

class XyPlotSetupPanel: public GmatPanel
{
public:
   XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   ~XyPlotSetupPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
protected:
   TsPlot *mXyPlot;
   wxString mLastCoordSysName;
   
   int  mNumSc;
   int  mNumImpBurn;
   int  mNumScProperty;
   int  mNumImpBurnProperty;
   int  mNumXParams;
   int  mNumYParams;
   bool mXParamChanged;
   bool mYParamChanged;
   bool mIsColorChanged;
   bool mUseUserParam;
   
   wxString *mSpacecraftList;
   wxString *mImpBurnList;
   wxArrayString mSpacecraftPropertyList;
   wxArrayString mImpBurnPropertyList;

   std::string mSelYName;
   wxArrayString mObjectTypeList;
   std::map<std::string, RgbColor> mColorMap;
   
   wxStaticText *mCoordSysLabel;
   
   wxColour mLineColor;
   
   wxComboBox *mSolverIterComboBox;
   wxComboBox *mObjectTypeComboBox;
   wxComboBox *mSpacecraftComboBox;
   wxComboBox *mImpBurnComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mCentralBodyComboBox;
   
   wxListBox *mUserParamListBox;
   wxListBox *mPropertyListBox;
   wxListBox *mXSelectedListBox;
   wxListBox *mYSelectedListBox;
   
   wxButton *mAddXButton;
   wxButton *mAddYButton;
   wxButton *mLineColorButton;
   
   wxCheckBox *showPlotCheckBox;
   wxCheckBox *showGridCheckBox;
   
   wxFlexGridSizer *mFlexGridSizer;
   wxBoxSizer *mParamOptionBoxSizer;
   wxBoxSizer *mCoordSysSizer;
   wxBoxSizer *mParamBoxSizer;
   
   void OnAddX(wxCommandEvent& event);
   void OnAddY(wxCommandEvent& event);
   void OnRemoveX(wxCommandEvent& event);
   void OnRemoveY(wxCommandEvent& event);
   void OnClearY(wxCommandEvent& event);
   void OnSelectUserParam(wxCommandEvent& event);
   void OnSelectProperty(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnSelectY(wxCommandEvent& event);
   void OnCreateVariable(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnLineColorClick(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      TEXTCTRL = 92000,
      ID_TEXT,
      ID_COMBOBOX,
      USER_PARAM_LISTBOX,
      PROPERTY_LISTBOX,
      X_SEL_LISTBOX,
      Y_SEL_LISTBOX,
      ADD_X,
      ADD_Y,
      REMOVE_X,
      REMOVE_Y,
      CLEAR_Y,
      CREATE_VARIABLE,
      CHECKBOX,
      LINE_COLOR_BUTTON,
   };
   
private:
   void ShowParameterOption(const wxString &scName, bool show = true);
   void ShowCoordSystem();
   wxString GetParamName();
   Parameter* GetParameter(const wxString &name);
   wxComboBox* GetObjectComboBox();
};
#endif
