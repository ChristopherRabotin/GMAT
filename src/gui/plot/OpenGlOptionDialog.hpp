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
   bool HasChangeMade()
      { return mHasChangeMade; }

   void SetDistance(float dist);
   void SetDrawEquPlane(bool flag);
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
   MdiChildTrajFrame *mTrajFrame;
   
   bool mHasChangeMade;
   bool mIsDistanceChanged;
   bool mIsGotoBodyChanged;
   bool mDrawEquPlaneChanged;
   bool mDrawWireFrameChanged;
   bool mIsEquPlaneColorChanged;
   
   bool mDrawEquPlane;
   bool mDrawWireFrame;
   
   float mDistance;
   wxString mGotoBodyName;
   
   wxArrayString mBodyNames;
   UnsignedIntArray mBodyIntColors;
   ObjectType mViewBodies[GmatPlot::MAX_BODIES];
   int mBodyCount;
   
   wxTextCtrl *mDistanceTextCtrl;

   wxComboBox *mGotoBodyComboBox;
   wxComboBox *mCoordSysComboBox;

   wxCheckBox *mWireFrameCheckBox;
   wxCheckBox *mEquPlaneCheckBox;
   wxCheckBox *mEarthSunLineCheckBox;
   
   wxButton *mEquPlaneColorButton;
   wxButton *mEarthSunLineColorButton;

   wxButton *mAddBodyButton;
   wxButton *mAddSatButton;
   wxButton *theApplyButton;
   
   wxColor mEquPlaneColor;
   wxColor mSunLineColor;
   
   wxBoxSizer *theDialogSizer;
   
   // member functions
   virtual void Create();
   virtual void ShowData();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   // event handlers
   void OnTextChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);
   void OnBodyColorButtonClick(wxCommandEvent& event);
   void OnSatColorButtonClick(wxCommandEvent& event);
   void OnAddBodyButtonClick(wxCommandEvent& event);
   void OnApplyButtonClick(wxCommandEvent& event);
   
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
      ID_ADD_BODY_BUTTON,
      ID_EQUPLANE_COLOR_BUTTON,
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
