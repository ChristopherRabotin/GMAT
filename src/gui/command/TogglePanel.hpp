//$Id$
//------------------------------------------------------------------------------
//                              TogglePanel
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
// Author: Linda Jun
// Created: 2004/10/20
//
/**
 * This class contains the Toggle setup window.
 */
//------------------------------------------------------------------------------

#ifndef TogglePanel_hpp
#define TogglePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"
#include <map>

class TogglePanel : public GmatPanel
{
public:
   // constructors
   TogglePanel(wxWindow *parent, GmatCommand *cmd, bool forXyPlotOnly = false,
               bool showToggleState = false);
   ~TogglePanel();

   virtual bool TakeAction(const wxString &action);
   
protected:
   // member data
   GmatCommand *theCommand;
   wxString     mCmdTypeName;
   bool         isForXyPlotOnly;
   bool         mShowToggleState;
   
   wxCheckListBox *mSubsCheckListBox;
   wxRadioButton *mOnRadioButton;
   wxRadioButton *mOffRadioButton;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnComboBoxChange(wxCommandEvent& event);
   void OnRadioButtonChange(wxCommandEvent& event);
   void OnCheckListBoxChange(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,        
      ID_CHECKLISTBOX,
      ID_RADIOBUTTON
   };
};

#endif // TogglePanel_hpp
