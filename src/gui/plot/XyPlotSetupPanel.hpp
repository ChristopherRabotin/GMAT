//$Header$
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
#include "XyPlot.hpp"
#include "RgbColor.hpp"

class XyPlotSetupPanel: public GmatPanel
{
public:
   XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
   Subscriber *mSubscriber;
   XyPlot *mXyPlot;
   
   int  mNumXParams;
   int  mNumYParams;
   bool mXParamChanged;
   bool mYParamChanged;
   bool mIsColorChanged;
   
   std::string mSelYName;
   std::map<std::string, RgbColor> mColorMap;

   wxColour mLineColor;
   
   wxListBox *mParamListBox;
   wxListBox *mXSelectedListBox;
   wxListBox *mYSelectedListBox;

   wxButton *mAddXButton;
   wxButton *mAddYButton;
   wxButton *mLineColorButton;

   wxCheckBox *showPlotCheckBox;
   wxCheckBox *showGridCheckBox;
   wxCheckBox *targetStatusCheckBox;

   wxComboBox *scComboBox;
   
   wxFlexGridSizer *mFlexGridSizer;
   wxBoxSizer *mParamOptionBoxSizer;

   void OnAddX(wxCommandEvent& event);
   void OnAddY(wxCommandEvent& event);
   void OnRemoveX(wxCommandEvent& event);
   void OnRemoveY(wxCommandEvent& event);
   void OnClearY(wxCommandEvent& event);
   void OnSelectParam(wxCommandEvent& event);
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
      PARAM_LISTBOX,
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
   
};
#endif
