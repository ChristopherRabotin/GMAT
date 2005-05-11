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
   
   wxArrayString& GetObjectNames()
      { return mObjectNames; }
   UnsignedIntArray& GetObjectColors()
      { return mObjectIntColors; }

   void SetDistance(float dist);
   void SetDrawEqPlane(bool flag);
   void SetDrawWireFrame(bool flag);
   void SetGotoObjectName(const wxString &objName);
   void UpdateObject(const wxArrayString &objectNames,
                     const UnsignedIntArray &objectColors);
   void UpdateObjectList(const wxArrayString &objectNames,
                         const wxStringColorMap &objectColors);
   
protected:
      
   // member data
   GuiItemManager *theGuiManager;
   MdiChildTrajFrame *mTrajFrame;
   
   bool mHasUseViewPointSpecChanged;
   bool mHasUsePerspModeChanged;
   bool mHasDistanceChanged;
   bool mHasGotoObjectChanged;
   bool mHasCoordSysChanged;
   bool mHasDrawEqPlaneChanged;
   bool mHasDrawEcPlaneChanged;
   bool mHasDrawEcLineChanged;
   bool mHasDrawAxesChanged;
   bool mHasDrawWireFrameChanged;
   bool mHasEqPlaneColorChanged;
   bool mHasEcPlaneColorChanged;
   bool mHasEcLineColorChanged;
   bool mHasRotateAboutXYChanged;
   bool mHasObjectColorChanged;
   bool mHasShowObjectChanged;
   
   int   mAnimationUpdInt;
   float mDistance;
   
   wxString mGotoObjectName;
   wxString mCoordSysName;
   
   wxArrayString mObjectNames;
   UnsignedIntArray mObjectIntColors;
   int mObjectCount;
   
   wxStringColorMap mObjectColorMap;
   wxStringBoolMap  mShowObjectMap;
   
   wxTextCtrl *mDistanceTextCtrl;
   wxTextCtrl *mAnimationUpdIntTextCtrl;
   
   wxComboBox *mGotoObjectComboBox;
   wxComboBox *mCoordSysComboBox;
   
   wxListBox *mObjectListBox;
   
   wxCheckBox *mUseInitialViewPointCheckBox;
   wxCheckBox *mUsePerspModeCheckBox;
   
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mEqPlaneCheckBox;
   wxCheckBox *mEcPlaneCheckBox;
   wxCheckBox *mEcLineCheckBox;
   wxCheckBox *mDrawAxesCheckBox;
   wxCheckBox *mRotateAboutXYCheckBox;
   
   wxCheckBox *mShowObjectCheckBox;

   wxButton *mViewAnimationButton;
   //wxButton *mCreateCoordSysButton;
   wxButton *mEqPlaneColorButton;
   wxButton *mEcPlaneColorButton;
   wxButton *mEcLineColorButton;
   
   wxButton *mObjectColorButton;
   wxButton *mAddObjectButton;
   wxButton *mAddSatButton;
   wxButton *theApplyButton;
   
   wxColor mEqPlaneColor;
   wxColor mEcPlaneColor;
   wxColor mEcLineColor;
   wxColour mObjectColor;
   
   wxBoxSizer *theDialogSizer;
   wxStaticBoxSizer *mViewObjectSizer;
   
   // member functions
   virtual void Create();
   virtual void ShowData();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   void UpdateObjectComboBox();
   void UpdateObjectListBox();
   bool ShowColorDialog(wxColor &oldColor, wxButton *button);
   void ShowSpacePointOption(const wxString &name, bool show = true);
   
   // event handlers
   void OnTextChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnObjectColorButtonClick(wxCommandEvent& event);
   void OnSatColorButtonClick(wxCommandEvent& event);
//    void OnAddObjectButtonClick(wxCommandEvent& event);
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
//       ID_ADD_OBJECT_BUTTON,
      ID_EQPLANE_COLOR_BUTTON,
      ID_ECPLANE_COLOR_BUTTON,
      ID_SUNLINE_COLOR_BUTTON,
      ID_OBJECT_COLOR_BUTTON,
   };
   
};

#endif // OpenGlOptionDialog_hpp
