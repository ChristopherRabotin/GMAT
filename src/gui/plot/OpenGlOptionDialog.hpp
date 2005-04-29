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
#include "MdiGlPlotData.hpp"   // for MAX_BODIES
#include "GuiItemManager.hpp"
#include "CoordinateSystem.hpp"

class MdiChildTrajFrame;

class OpenGlOptionDialog : public wxDialog
{
public:
   OpenGlOptionDialog(wxWindow *parent, const wxString &title,
                      const wxArrayString &bodyNames,
                      const UnsignedIntArray &bodyColors);
   
   wxArrayString& GetBodyNames()
      { return mBodyNames; }
   UnsignedIntArray& GetBodyColors()
      { return mBodyIntColors; }

   void SetDistance(float dist);
   void SetDrawEqPlane(bool flag);
   void SetDrawWireFrame(bool flag);
   void SetGotoStdBody(int bodyId);
   
protected:
   
   struct ObjectType
   {
      wxString     name;
      wxColor      color;
      wxCheckBox   *checkBox;
      wxButton     *colorButton;
      
      ObjectType(const wxString &nameStr = "BodyName" ,
                 const wxColor wxcolor = wxColor("TAN"))
      {
         name = nameStr;
         color = wxcolor;
         checkBox = NULL;
         colorButton = NULL;
      }
   };
   
   // member data
   GuiItemManager *theGuiManager;
   MdiChildTrajFrame *mTrajFrame;
   
   bool mHasUseViewPointSpecChanged;
   bool mHasUsePerspModeChanged;
   bool mHasDistanceChanged;
   bool mHasGotoBodyChanged;
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

   int   mAnimationUpdInt;
   float mDistance;
   
   wxString mGotoBodyName;
   wxString mCoordSysName;
   
   wxArrayString mBodyNames;
   UnsignedIntArray mBodyIntColors;
   ObjectType mViewBodies[GmatPlot::MAX_BODIES];
   int mBodyCount;
   
   wxTextCtrl *mDistanceTextCtrl;
   wxTextCtrl *mAnimationUpdIntTextCtrl;
   
   wxComboBox *mGotoBodyComboBox;
   wxComboBox *mCoordSysComboBox;
   
   wxCheckBox *mUseInitialViewPointCheckBox;
   wxCheckBox *mUsePerspModeCheckBox;
   
   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mEqPlaneCheckBox;
   wxCheckBox *mEcPlaneCheckBox;
   wxCheckBox *mEcLineCheckBox;
   wxCheckBox *mDrawAxesCheckBox;
   wxCheckBox *mRotateAboutXYCheckBox;

   wxButton *mViewAnimationButton;
   wxButton *mCreateCoordSysButton;
   wxButton *mEqPlaneColorButton;
   wxButton *mEcPlaneColorButton;
   wxButton *mEcLineColorButton;
   
   wxButton *mAddBodyButton;
   wxButton *mAddSatButton;
   wxButton *theApplyButton;
   
   wxColor mEqPlaneColor;
   wxColor mEcPlaneColor;
   wxColor mEcLineColor;
   
   wxBoxSizer *theDialogSizer;
   
   // member functions
   virtual void Create();
   virtual void ShowData();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   bool ShowColorDialog(wxColor &oldColor, wxButton *button);
   
   // event handlers
   void OnTextChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnBodyColorButtonClick(wxCommandEvent& event);
   void OnSatColorButtonClick(wxCommandEvent& event);
   void OnAddBodyButtonClick(wxCommandEvent& event);
   void OnApplyButtonClick(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   
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
      ID_BUTTON,
      ID_ADD_BODY_BUTTON,
      ID_EQPLANE_COLOR_BUTTON,
      ID_ECPLANE_COLOR_BUTTON,
      ID_SUNLINE_COLOR_BUTTON,
      ID_BODY_COLOR_BUTTON,
      ID_BODY_COLOR_BUTTON1,
      ID_BODY_COLOR_BUTTON2,
      ID_BODY_COLOR_BUTTON3,
      ID_BODY_COLOR_BUTTON4,
      ID_BODY_COLOR_BUTTON5,
      ID_BODY_COLOR_BUTTON6,
      ID_BODY_COLOR_BUTTON7,
      ID_BODY_COLOR_BUTTON8,
      ID_BODY_COLOR_BUTTON9,
      ID_BODY_COLOR_BUTTON10,
      ID_BODY_COLOR_BUTTON12,
      ID_BODY_COLOR_BUTTON13,
      ID_BODY_COLOR_BUTTON14,
      ID_BODY_COLOR_BUTTON15,
      ID_BODY_COLOR_BUTTON16,
      ID_BODY_COLOR_BUTTON17,
      ID_BODY_COLOR_BUTTON18,
      ID_BODY_COLOR_BUTTON19,
   };
   
};

#endif // OpenGlOptionDialog_hpp
