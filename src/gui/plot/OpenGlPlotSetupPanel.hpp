//$Header$
//------------------------------------------------------------------------------
//                              OpenGlPlotSetupPanel
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
 * Declares OpenGlPlotSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef OpenGlPlotSetupPanel_hpp
#define OpenGlPlotSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "OpenGlPlot.hpp"
#include "RgbColor.hpp"

class OpenGlPlotSetupPanel: public GmatPanel
{
public:
   OpenGlPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
   //Subscriber *mSubscriber; //loj: 9/28/04 commented out
   OpenGlPlot *mOpenGlPlot;
   
   bool mIsScChanged;
   bool mIsColorChanged;
   bool mIsCoordSysChanged;
   int mScCount;
   
   std::string mSelScName;
   std::map<std::string, RgbColor> mOrbitColorMap;
   std::map<std::string, RgbColor> mTargetColorMap;
   
   wxColour mScOrbitColor;
   wxColour mScTargetColor;
   
   wxListBox *mSpacecraftListBox;
   wxListBox *mSelectedObjListBox;
   wxListBox *mCelesObjectListBox;
   
   wxButton *addScButton;
   wxButton *removeScButton;
   wxButton *clearScButton;
   wxButton *mScOrbitColorButton;
   wxButton *mScTargetColorButton;
   
   wxCheckBox *mPlotCheckBox;
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mTargetStatusCheckBox;
   wxCheckBox *mEclipticPlaneCheckBox;
   wxCheckBox *mEquatorialPlaneCheckBox;
   wxCheckBox *mOverlapCheckBox;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mViewPointRefComboBox;
   wxComboBox *mViewPointVecComboBox;
   wxComboBox *mViewDirectionComboBox;

   wxStaticText *mTargetColorLabel;
   
   wxTextCtrl *mViewScaleFactorTextCtrl;
   wxTextCtrl *mViewPoint1TextCtrl;
   wxTextCtrl *mViewPoint2TextCtrl;
   wxTextCtrl *mViewPoint3TextCtrl;
   wxTextCtrl *mViewDir1TextCtrl;
   wxTextCtrl *mViewDir2TextCtrl;
   wxTextCtrl *mViewDir3TextCtrl;
   
   wxFlexGridSizer *mFlexGridSizer;
   wxFlexGridSizer *mPlotOptionSizer;
   wxBoxSizer *mScOptionBoxSizer;
   wxBoxSizer *mViewPointVecSizer;
   wxBoxSizer *mViewDirVecSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // event handler
   void OnAddSpacecraft(wxCommandEvent& event);
   void OnRemoveSpacecraft(wxCommandEvent& event);
   void OnClearSpacecraft(wxCommandEvent& event);
   void OnSelectAvailObject(wxCommandEvent& event);
   void OnSelectSpacecraft(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnOrbitColorClick(wxCommandEvent& event);
   void OnTargetColorClick(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event); //loj: 1/27/05 Added
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 93000,
      ID_COMBOBOX,
      ID_LISTBOX,
      SC_SEL_LISTBOX,
      CHECKBOX,
      ADD_SC_BUTTON,
      REMOVE_SC_BUTTON,
      CLEAR_SC_BUTTON,
      SC_ORBIT_COLOR_BUTTON,
      SC_TARGET_COLOR_BUTTON
   };
   
private:
   void ShowSpacecraftOption(const wxString &name, bool show = true);
   
};
#endif
