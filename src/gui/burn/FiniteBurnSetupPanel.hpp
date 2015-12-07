//$Id$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
// Author: LaMont Ruley
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef FiniteBurnSetupPanel_hpp
#define FiniteBurnSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "FiniteBurn.hpp"

class FiniteBurnSetupPanel : public GmatPanel
{
public:
   FiniteBurnSetupPanel( wxWindow *parent, const wxString &burnName);
   ~FiniteBurnSetupPanel();

private:
   static const int MAX_PROP_ROW = 5;
   
   // member data
   FiniteBurn *theBurn;
   
   wxComboBox *mThrusterComboBox;
   
   wxString thrusterSelected;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // for event handling
   void OnComboBoxChange(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
       ID_TEXT = 81000,
       ID_COMBOBOX
   };
};

#endif // FiniteBurnSetupPanel_hpp
