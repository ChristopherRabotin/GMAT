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
   OpenGlPlot *mOpenGlPlot;
   
   bool mHasScChanged;
   bool mHasColorChanged;
   bool mHasCoordSysChanged;
   bool mHasViewInfoChanged;
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
   wxCheckBox *mUseViewPointInfoCheckBox;
   wxCheckBox *mPerspectiveModeCheckBox;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mViewPointRefComboBox;
   wxComboBox *mViewPointVectorComboBox;
   wxComboBox *mViewDirectionComboBox;

   wxStaticText *mTargetColorLabel;
   
   wxTextCtrl *mViewScaleFactorTextCtrl;
   wxTextCtrl *mViewPointRef1TextCtrl;
   wxTextCtrl *mViewPointRef2TextCtrl;
   wxTextCtrl *mViewPointRef3TextCtrl;
   wxTextCtrl *mViewPointVec1TextCtrl;
   wxTextCtrl *mViewPointVec2TextCtrl;
   wxTextCtrl *mViewPointVec3TextCtrl;
   wxTextCtrl *mViewDir1TextCtrl;
   wxTextCtrl *mViewDir2TextCtrl;
   wxTextCtrl *mViewDir3TextCtrl;
   
   wxFlexGridSizer *mFlexGridSizer;
   wxFlexGridSizer *mPlotOptionSizer;
   wxBoxSizer *mScOptionBoxSizer;
   wxBoxSizer *mViewPointRefSizer;
   wxBoxSizer *mViewPointVectorSizer;
   wxBoxSizer *mViewDirVectorSizer;
   
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
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   
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
