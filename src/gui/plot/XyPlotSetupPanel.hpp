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
   
   int  mNumXParams;
   int  mNumYParams;
   bool mXParamChanged;
   bool mYParamChanged;
   bool mIsColorChanged;
   bool mUseUserParam;
   
   std::string mSelYName;
   wxArrayString mObjectTypeList;
   wxArrayString mXvarWxStrings;
   wxArrayString mYvarWxStrings;
   std::map<std::string, RgbColor> mColorMap;
   
   wxStaticText *mCoordSysLabel;
   wxColour mLineColor;   
   wxComboBox *mSolverIterComboBox;   
   wxListBox *mXSelectedListBox;
   wxListBox *mYSelectedListBox;   
   wxButton *mViewXButton;
   wxButton *mViewYButton;
   wxButton *mLineColorButton;   
   wxCheckBox *showPlotCheckBox;
   wxCheckBox *showGridCheckBox;
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 92000,
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_LISTBOX,
      ID_BUTTON,
   };
   
private:
   void ShowParameterOption(const wxString &scName, bool show = true);
};
#endif
