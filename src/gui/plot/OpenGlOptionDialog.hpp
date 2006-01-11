//$Header$
//------------------------------------------------------------------------------
//                              OpenGlOptionDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/12/20
//
/**
 * Declares OpenGlOptionDialog class. This dialog allows user to view and
 * select various OpenGL plot options.
 */
//------------------------------------------------------------------------------

#ifndef OpenGlOptionDialog_hpp
#define OpenGlOptionDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "gmatdefs.hpp"        // for UnsignedIntArray
#include "MdiGlPlotData.hpp"   // for MAX_BODIES, wxStringColorMap, wxStringBoolMap
#include "GuiItemManager.hpp"
#include "RgbColor.hpp"
#include "CoordinateSystem.hpp"

class MdiChildTrajFrame;

class OpenGlOptionDialog : public wxDialog
{
public:
   OpenGlOptionDialog(wxWindow *parent, const wxString &title,
                      const wxArrayString &objectNames,
                      const UnsignedIntArray &objectColors);
   ~OpenGlOptionDialog();
   
   wxArrayString& GetObjectNames()
      { return mObjectNames; }
   UnsignedIntArray& GetObjectColors()
      { return mObjectIntColors; }

   void SetDistance(float dist);
   void SetDrawXyPlane(bool flag);
   void SetDrawWireFrame(bool flag);
   void SetDrawAxes(bool flag);
   void SetDrawEarthSunLines(bool flag);
   void SetGotoObjectName(const wxString &objName);
   void SetCoordSysName(const wxString &csName);
   
   void UpdateObjectList(const wxArrayString &objectNames,
                         const wxArrayString &validCSNames,
                         const wxStringBoolMap &showObjects,
                         const wxStringColorMap &objectColors);
   
protected:
   
   // member data
   GuiItemManager *theGuiManager;
   MdiChildTrajFrame *mTrajFrame;
   
   bool mHasRotateAboutXYChanged;
   bool mHasUseViewPointSpecChanged;
   bool mHasUsePerspModeChanged;
   bool mHasDistanceChanged;
   bool mHasGotoObjectChanged;
   bool mHasCoordSysChanged;
   
   bool mHasDrawWireFrameChanged;
   bool mHasDrawAxesChanged;
   bool mHasDrawXyPlaneChanged;
   bool mHasDrawEcPlaneChanged;
   bool mHasDrawESLineChanged;
   
   bool mHasXyPlaneColorChanged;
   bool mHasEcPlaneColorChanged;
   bool mHasESLineColorChanged;
   bool mHasObjectColorChanged;
   
   bool mHasShowObjectChanged;
   bool mHasShowOrbitNormalChanged;

   int   mAnimationUpdInt;
   float mDistance;
   
   wxString mGotoObjectName;
   wxString mCoordSysName;
   
   wxArrayString mObjectNames;
   wxArrayString mValidCSNames;
   UnsignedIntArray mObjectIntColors;
   int mObjectCount;
   int mValidCSCount;
   
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mInitialShowObjectMap;
   wxStringBoolMap  mShowObjectMap;
   wxStringBoolMap  mShowOrbitNormalMap;
   
   wxTextCtrl *mDistanceTextCtrl;
   wxTextCtrl *mAnimationUpdIntTextCtrl;
   
   wxComboBox *mGotoObjectComboBox;
   wxComboBox *mCoordSysComboBox;
   
   wxListBox *mObjectListBox;
   
   wxCheckBox *mUseInitialViewDefCheckBox;
   wxCheckBox *mUsePerspModeCheckBox;
   
   wxCheckBox *mRotateAboutXYCheckBox;
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mXyPlaneCheckBox;
   wxCheckBox *mEcPlaneCheckBox;
   wxCheckBox *mAxesCheckBox;
   wxCheckBox *mESLinesCheckBox;
   
   wxCheckBox *mShowObjectCheckBox;
   wxCheckBox *mShowOrbitNormalCheckBox;

   wxButton *mViewAnimationButton;
   wxButton *mXyPlaneColorButton;
   wxButton *mEcPlaneColorButton;
   wxButton *mESLinesColorButton;
   
   wxButton *mObjectColorButton;
   wxButton *mAddObjectButton;
   wxButton *mAddSatButton;
   wxButton *theApplyButton;
   
   wxColor mXyPlaneColor;
   wxColor mEcPlaneColor;
   wxColor mESLinesColor;
   wxColour mObjectColor;
   
   wxBoxSizer *theDialogSizer;
   wxStaticBoxSizer *mViewObjectSizer;
   
   // member functions
   virtual void Create();
   virtual void ShowData();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   void UpdateCoordSysComboBox();
   void UpdateObjectComboBox();
   void UpdateObjectListBox();
   bool ShowColorDialog(wxColor &oldColor, wxButton *button);
   void ShowSpacePointOption(const wxString &name);
   
   // event handlers
   void OnTextChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnObjectColorButtonClick(wxCommandEvent& event);
   void OnSatColorButtonClick(wxCommandEvent& event);
   void OnApplyButtonClick(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   void OnSelectObject(wxCommandEvent& event);
   void OnClose(wxCloseEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_APPLY = 8120,
      ID_TEXTCTRL,
      ID_CHECKBOX,
      ID_COMBOBOX,
      ID_LISTBOX,
      ID_BUTTON,
      ID_EQPLANE_COLOR_BUTTON,
      ID_ECPLANE_COLOR_BUTTON,
      ID_SUNLINE_COLOR_BUTTON,
      ID_OBJECT_COLOR_BUTTON,
   };
   
};

#endif // OpenGlOptionDialog_hpp
