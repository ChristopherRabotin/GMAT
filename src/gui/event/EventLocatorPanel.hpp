//$Id$
//------------------------------------------------------------------------------
//                            EventLocatorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// Author: Wendy Shoan
// Created: 2015.03.31
/**
 * This class contains information needed to setup event locator data.
 *
 */
//------------------------------------------------------------------------------
#ifndef EventLocatorPanel_hpp
#define EventLocatorPanel_hpp

#include "gmatdefs.hpp"
#include "gmatwxdefs.hpp"
#include "SolarSystem.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"

class EventLocatorPanel: public GmatPanel
{
public:
   EventLocatorPanel(wxWindow *parent, const wxString &name);
   ~EventLocatorPanel();

   void         LoadData();
   void         SaveData();
   virtual void SaveData(GmatBase *forObject);

   bool         IsDataChanged() { return dataChanged; }
   bool         CanClosePanel() { return canClose; }

private:

   // local copy of object for verifying changes before commit/apply
   GmatBase *localObject;
   GmatBase *theObject;

   bool     isEclipse;  // true for Eclipse, false for Contact

   bool     dataChanged;

   Real     theEpoch; // A1Mjd

   // Spacecraft for EclipseLocator, Target for ContactLocator
   bool     isSCTargetChanged;
   bool     isBodyListChanged;
   bool     isEclipseTypesChanged;
   bool     isObserverListChanged;
   bool     isFileNameChanged;
   bool     isFileNameTextChanged;
   bool     isWriteReportChanged;
   bool     isRunModeChanged;

   bool     isEntireIntervalChanged;
   bool     isEpochFormatChanged;
   bool     isInitialEpochChanged;
   bool     isFinalEpochChanged;
   bool     isInitialEpochTextChanged;
   bool     isFinalEpochTextChanged;

   bool     isLightTimeDelayChanged;
   bool     isStellarAberrationChanged;
   bool     isLightTimeDirectionChanged;
   bool     isStepSizeChanged;

   std::string fromEpochFormat;
   std::string theInitEpochStr;
   std::string theFinalEpochStr;
   std::string taiMjdInitialEpochStr;
   std::string taiMjdFinalEpochStr;

   std::string errMsgFormat;

   std::string scTargetStr;
   std::string runModeStr;
   std::string lightTimeDirectionStr;

   StringArray bodiesList;
   StringArray eclipseTypesList;
   StringArray observersList;

   SolarSystem    *ss;

   void        Create();
   wxString    ToString(Real rval);
   void        ResetChangedFlags();

   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnTextChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnBrowseButton(wxCommandEvent &event);
   void OnCheckBoxChange(wxCommandEvent &event);
   void OnCheckListBoxChange(wxCommandEvent& event);
   void OnCheckListBoxSelect(wxCommandEvent& event);


   // Widgets
   // static text (labels)
   wxStaticText *scTargetTxt;
   wxStaticText *bodiesTxt;
   wxStaticText *eclipseTypesTxt;
   wxStaticText *observersTxt;
   wxStaticText *fileNameTxt;
   wxStaticText *runModeTxt;
   wxStaticText *epochFormatTxt;
   wxStaticText *initialEpochTxt;
   wxStaticText *finalEpochTxt;
   wxStaticText *lightTimeDirectionTxt;
   wxStaticText *stepSizeTxt;
   wxStaticText *stepSizeUnitsTxt;

   // Text boxes
   wxTextCtrl   *fileNameTxtCtrl;
   wxTextCtrl   *initialEpochTxtCtrl;
   wxTextCtrl   *finalEpochTxtCtrl;
   wxTextCtrl   *stepSizeTxtCtrl;
   // Combo boxes
   wxComboBox   *scTargetComboBox;
   wxComboBox   *runModeComboBox;
   wxComboBox   *epochFormatComboBox;
   wxComboBox   *lightTimeDirectionComboBox;
   // Browse button
   wxButton     *fileNameBrowseButton;
   // Check Boxes
   wxCheckBox   *writeReportCheckBox;
   wxCheckBox   *entireIntervalCheckBox;
   wxCheckBox   *lightTimeDelayCheckBox;
   wxCheckBox   *stellarAberrationCheckBox;
   // Checklist Boxes
   wxCheckListBox *bodiesCheckListBox;
   wxCheckListBox *eclipseTypesCheckListBox;
   wxCheckListBox *observersCheckListBox;

   // wxString arrays
   wxString       *wxPossibleDir;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 31280, // ?
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_RUNMODE_COMBOBOX,
      ID_BUTTON_BROWSE,
      ID_CHECKBOX,
      ID_CHECKLISTBOX_BODIES,
      ID_CHECKLISTBOX_ECLIPSE,
      ID_CHECKLISTBOX_OBSERVER
   };
};
#endif

