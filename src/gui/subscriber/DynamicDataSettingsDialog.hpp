//$Id$
//------------------------------------------------------------------------------
//                         DynamicDataSettingsDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2018/02/20, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Declares DynamicDataSettingsDialog methods
*/
//------------------------------------------------------------------------------
#ifndef DynamicDataSettingsDialog_hpp
#define DynamicDataSettingsDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include <wx/string.h> // for wxArrayString
#include <wx/clrpicker.h> // for wxColorPickerCtrl

#include "DynamicDataDisplay.hpp"
#include "DynamicDataStruct.hpp"

class DynamicDataSettingsDialog : public GmatDialog
{
public:
   DynamicDataSettingsDialog(wxWindow *parent, DDD currDataStruct);

   DDD GetParamSettings();
   bool WasDataSaved();

protected:
   DynamicDataDisplay *currDisplay;
   DDD newParamSettings;
   DDD currParamSettings;

   bool mWasDataSaved;

   wxTextCtrl *mParamNameTextCtrl;
   wxTextCtrl *mWarnLowerBoundTextCtrl;
   wxTextCtrl *mWarnUpperBoundTextCtrl;
   wxTextCtrl *mCritLowerBoundTextCtrl;
   wxTextCtrl *mCritUpperBoundTextCtrl;
   wxButton *mSelectButton;
   wxColourPickerCtrl *mParamTextColorCtrl;
   wxColourPickerCtrl *mParamBackgroundColorCtrl;
   wxArrayString mObjectTypeList;

   // abstract methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();

   void OnSelect(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   enum
   {
      ID_TEXT = 9200,
      ID_DISPLAYTEXTCTRL,
      ID_COLOR,
      ID_BUTTON_SELECT
   };
};
#endif