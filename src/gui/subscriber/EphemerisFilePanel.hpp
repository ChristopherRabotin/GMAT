//$Id$
//------------------------------------------------------------------------------
//                           EphemerisFilePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen
// Created: 2010/02/02
/**
 * This class is a generic setup panel used by objects derived from GmatBase
 */
//------------------------------------------------------------------------------

#ifndef EphemerisFilePanel_hpp
#define EphemerisFilePanel_hpp

#include "GmatPanel.hpp"

/**
 * Generic configuration panel for GmatBase derived objects
 * 
 * This class defines a generic object configuration panel that is used when a
 * custom panel has not been coded.  It provides access to all of the object's
 * writable parameters using text controls and comboboxes.
 */
class EphemerisFilePanel : public GmatPanel
{
public:
   EphemerisFilePanel(wxWindow *parent, const wxString &name);
   virtual ~EphemerisFilePanel();
   
protected:

   GmatBase *clonedObj;
   
   virtual void         Create();
   virtual void         LoadData();
   virtual void         SaveData();
   wxControl *          BuildControl(wxWindow *parent, Integer index);
   void                 LoadControl(const std::string &label);
   void                 SaveControl(const std::string &label);
   
   // Text control event method
   void OnTextUpdate(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnComboBoxTextChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   void OnBrowse(wxCommandEvent& event);
   void OnCheck(wxCommandEvent& event);
   
   wxComboBox*     spacecraftComboBox;
//   wxComboBox*     stateTypeComboBox;
   wxComboBox*     coordinateSystemComboBox;
   wxCheckBox*     writeEphemerisCheckBox;
   wxComboBox*     fileFormatComboBox;
   wxTextCtrl*     fileNameTextCtrl;
   wxBitmapButton* browseButton;
   wxComboBox*     interpolatorComboBox;
   wxTextCtrl*     interpolationOrderTextCtrl;
   wxComboBox*     stepSizeComboBox;
   wxComboBox*     epochFormatComboBox;
   wxComboBox*     initialEpochComboBox;
   wxComboBox*     finalEpochComboBox;
   
   // Buffer for time conversion code
   wxString        previousEpochFormat;

   wxFileDialog *fileDialog;
   
   /// Managed wxComboBox map used by GuiItemManager
   std::map<wxString, wxComboBox*>  managedComboBoxMap;
   
   /// IDs used for event management
   enum
   {
      ID_TEXT = 55000,
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_BUTTON_BROWSE
   };
};

#endif /* EphemerisFilePanel_hpp */
