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

class GmatColorPanel: public wxPanel
{
public:
   GmatColorPanel(wxWindow *parent, GmatPanel *parentGmatPanel,
                  SpacePoint *clonedSpacePoint);
   ~GmatColorPanel();
   void SaveData();
   void LoadData();
   
   bool HasColorChanged() { return colorChanged; }
   
private:
   
   bool colorChanged;
   GmatPanel  *theParentGmatPanel;
   SpacePoint *theClonedSpacePoint;
   
   // Color pickers
   wxColourPickerCtrl *mOrbitColorCtrl;
   wxColourPickerCtrl *mTargetColorCtrl;
   wxColour mOrbitColor;
   wxColour mTargetColor;
   
   void Create();
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnColorPickerChange(wxColourPickerEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_COLOR_CTRL = 30200
   };
};
#endif

