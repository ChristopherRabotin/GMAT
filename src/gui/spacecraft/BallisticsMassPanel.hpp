//$Id$
//------------------------------------------------------------------------------
//                            BallisticsMassPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Allison Greene
// Created: 2004/04/01
/**
 * This class contains information needed to setup users spacecraft ballistics
 * and mass through GUI
 * 
 */
//------------------------------------------------------------------------------
#ifndef BallisticsMassPanel_hpp
#define BallisticsMassPanel_hpp

#include "gmatwxdefs.hpp"
#include "Spacecraft.hpp"
#include "GmatPanel.hpp"


class BallisticsMassPanel: public wxPanel
{
public:
   BallisticsMassPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft);
   ~BallisticsMassPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }
   
private:
   bool dataChanged;
   bool canClose;
   
   bool spadFileChanged;

   void Create();
    
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnTextChange(wxCommandEvent &event);
   void OnSPADTextChange(wxCommandEvent &event);
   void OnBrowseButton(wxCommandEvent& event);
    
   Spacecraft *theSpacecraft;

   GmatPanel *theScPanel;
   
   wxTextCtrl *dryMassTextCtrl;
   wxTextCtrl *dragCoeffTextCtrl;
   wxTextCtrl *reflectCoeffTextCtrl;
   wxTextCtrl *dragAreaTextCtrl;
   wxTextCtrl *srpAreaTextCtrl;

   wxTextCtrl *spadSrpFileTextCtrl;
   wxTextCtrl *spadSrpScaleFactorTextCtrl;

   wxBitmapButton *spadBrowseButton;

   std::string    theSpadSrpFile;
   std::string    prevSpadSrpFile;

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30100,
      ID_TEXTCTRL,
      ID_SPAD_TEXTCTRL,
      ID_SPAD_BUTTON_BROWSE,
   };
};
#endif

