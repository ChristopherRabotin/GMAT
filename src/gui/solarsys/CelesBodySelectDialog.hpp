//$Id$
//------------------------------------------------------------------------------
//                              CelesBodySelectDialog
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
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares CelesBodySelectDialog class. This class shows a dialog window on
 * which celestial bodies can be selected.
 * 
 */
//------------------------------------------------------------------------------
#ifndef CelesBodySelectDialog_hpp
#define CelesBodySelectDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "gmatdefs.hpp"
#include <wx/string.h> // for wxArrayString

class CelesBodySelectDialog : public GmatDialog
{
public:
    
   CelesBodySelectDialog(wxWindow *parent, wxArrayString &bodiesToExclude,
                         wxArrayString &bodiesToHide, bool showCalPoints = false);
   ~CelesBodySelectDialog();
   
   wxArrayString& GetBodyNames()
      { return mBodyNames; }
   bool IsBodySelected()
      { return mIsBodySelected; }
      
private:
   wxArrayString mBodyNames;
   wxArrayString mBodiesToExclude;
   wxArrayString mBodiesToHide;
      
   bool mIsBodySelected;
   bool mShowCalPoints;
   
   wxString mSelBodyName;
   
   wxButton *mAddBodyButton;
   wxButton *mRemoveBodyButton;
   wxButton *mClearBodyButton;
   
   wxListBox *mBodyListBox;
   wxListBox *mBodySelectedListBox;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   void ShowBodyOption(const wxString &name, bool show = true);
   
   // event handling
   void OnButton(wxCommandEvent& event);
   void OnSelectBody(wxCommandEvent& event);
   void OnListBoxDoubleClick(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE()
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_LISTBOX,
      ID_BUTTON,
   };
};

#endif
