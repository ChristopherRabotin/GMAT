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
   ~OpenGlPlotSetupPanel();
   
protected:
   OpenGlPlot *mOpenGlPlot;
   
   bool mHasSpChanged;
   bool mHasColorChanged;
   bool mHasCoordSysChanged;
   bool mHasViewInfoChanged;
   bool mHasViewUpInfoChanged;
   int  mScCount;
   int  mNonScCount;
   
   std::string mSelSpName;
   std::map<std::string, RgbColor> mOrbitColorMap;
   std::map<std::string, RgbColor> mTargetColorMap;
   
   wxArrayString mExcludedScList;
   
   wxColour mOrbitColor;
   wxColour mTargetColor;
   
   wxListBox *mSpacecraftListBox;
   wxListBox *mCelesObjectListBox;
   wxListBox *mSelectedScListBox;
   wxListBox *mSelectedObjListBox;
   
   wxButton *addScButton;
   wxButton *removeScButton;
   wxButton *clearScButton;
   wxButton *mOrbitColorButton;
   wxButton *mTargetColorButton;
   
   wxCheckBox *mPlotCheckBox;
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mTargetStatusCheckBox;
   wxCheckBox *mEclipticPlaneCheckBox;
   wxCheckBox *mEquatorialPlaneCheckBox;
   wxCheckBox *mOverlapCheckBox;
   wxCheckBox *mUseViewPointInfoCheckBox;
   wxCheckBox *mPerspectiveModeCheckBox;
   wxCheckBox *mUseFixedFovCheckBox;
   
   wxComboBox *mCoordSysComboBox;
   wxComboBox *mViewPointRefComboBox;
   wxComboBox *mViewPointVectorComboBox;
   wxComboBox *mViewDirectionComboBox;
   wxComboBox *mViewUpCsComboBox;
   wxComboBox *mViewUpAxisComboBox;

   wxStaticText *mTargetColorLabel;
   wxStaticText *mFovLabel;
   
   wxTextCtrl *mFixedFovTextCtrl;
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
   
   wxFlexGridSizer *mObjectGridSizer;
   wxFlexGridSizer *mViewDefSizer;
   wxBoxSizer *mScOptionBoxSizer;
   wxBoxSizer *mViewPointRefSizer;
   wxBoxSizer *mViewPointVectorSizer;
   wxBoxSizer *mViewDirVectorSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   // event handler
   void OnAddSpacePoint(wxCommandEvent& event);
   void OnRemoveSpacePoint(wxCommandEvent& event);
   void OnClearSpacePoint(wxCommandEvent& event);
   void OnSelectAvailObject(wxCommandEvent& event);
   void OnSelectSpacecraft(wxCommandEvent& event);
   void OnSelectOtherObject(wxCommandEvent& event);
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
      OBJ_SEL_LISTBOX,
      CHECKBOX,
      ADD_SP_BUTTON,
      REMOVE_SP_BUTTON,
      CLEAR_SP_BUTTON,
      ORBIT_COLOR_BUTTON,
      TARGET_COLOR_BUTTON
   };
   
private:
   void ShowSpacePointOption(const wxString &name,
                             bool show = true, bool isSc = true,
                             UnsignedInt color = GmatColor::RED32);
   
};
#endif
