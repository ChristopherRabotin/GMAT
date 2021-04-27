//$Id$
//------------------------------------------------------------------------------
//                              BarycenterPanel
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
// Author: Allison Greene
// Created: 2005/04/21
/**
 * This class allows the user to configure a Barycenter.
 */
//------------------------------------------------------------------------------
#ifndef BarycenterPanel_hpp
#define BarycenterPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GmatColorPanel.hpp"
#include "GuiInterpreter.hpp"
#include "Barycenter.hpp"

class BarycenterPanel: public GmatPanel
{
public:
    BarycenterPanel(wxWindow *parent, const wxString &name);
    ~BarycenterPanel();
    
   wxArrayString& GetBodyNames()
      { return mBodyNames; }
   bool IsBodySelected()
      { return mIsBodySelected; }

private:
   Barycenter *theBarycenter;
   Barycenter *theClonedBarycenter;
   GmatColorPanel *theColorPanel;
   
   static const int MAX_LIST_SIZE = 30;
   wxArrayString mBodyNames;

   bool mIsBodySelected;
   bool mIsBuiltIn;

   wxString mSelBodyName;
   wxArrayString mExcludedCelesBodyList;
   
   wxButton *addBodyButton;
   wxButton *removeBodyButton;
   wxButton *clearBodyButton;

   wxListBox *bodyListBox;
   wxListBox *bodySelectedListBox;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   void OnButton(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 6100,
      ID_LISTBOX,
      ID_BUTTON,
      ID_BUTTON_ADD,
      ID_BUTTON_REMOVE,
      ID_BODY_SEL_LISTBOX,
   };
};
#endif



