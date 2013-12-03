//$Id$
//------------------------------------------------------------------------------
//                            GmatColorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Linda Jun
// Created: 2013/11/08
/**
 * This class sets up SpacePoint orbit and target colors
 */
//------------------------------------------------------------------------------
#ifndef GmatColorPanel_hpp
#define GmatColorPanel_hpp

#include "gmatdefs.hpp"
#include "SpacePoint.hpp"
#include "gmatwxdefs.hpp"
#include <wx/clrpicker.h>           // for wxColorPickerCtrl
#include "GmatPanel.hpp"
#include "ColorTypes.hpp"           // for GmatColor::

class GmatColorPanel: public wxPanel
{
public:
   GmatColorPanel(wxWindow *parent, GmatPanel *parentGmatPanel,
                  SpacePoint *clonedSpacePoint, bool useInputObjColor = false,
                  bool overrideColor = false, bool showOrbitColorOnly = false,
                  bool showOverrideColorCheckBox = false,
                  const std::string &objName = "");
   ~GmatColorPanel();
   void SaveData();
   void LoadData();
   
   bool HasColorChanged() { return mHasColorChanged; }
   bool HasOverrideColorChanged() { return mHasOverrideColorChanged; }
   bool GetOverrideColor() { return mOverrideColor; }
   UnsignedInt GetOrbitColor() { return mOrbitIntColor; }
   UnsignedInt GetTargetColor() { return mTargetIntColor; }
   
private:
   
   bool mHasColorChanged;
   bool mHasOverrideColorChanged;
   bool mUseInputObjectColor;
   bool mOverrideColor;
   bool mShowOrbitColorOnly;
   bool mShowOverrideColorCheckBox;
   std::string mSpacePointName;
   
   UnsignedInt mDefaultOrbitColor;
   UnsignedInt mOrbitIntColor;
   UnsignedInt mTargetIntColor;
   
   GmatPanel  *theParentGmatPanel;
   SpacePoint *theClonedSpacePoint;
   
   // Color pickers
   wxStaticText *mOrbitColorLabel;
   wxStaticText *mTargetColorLabel;
   wxColourPickerCtrl *mOrbitColorCtrl;
   wxColourPickerCtrl *mTargetColorCtrl;
   wxCheckBox *mOverrideColorCheckBox;
   wxColour mOrbitColor;
   wxColour mTargetColor;
   
   void Create();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnColorPickerChange(wxColourPickerEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_COLOR_CTRL = 30200,
      ID_CHECKBOX
   };
};
#endif

