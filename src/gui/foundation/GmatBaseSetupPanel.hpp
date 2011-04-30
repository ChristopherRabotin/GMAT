//$Id$
//------------------------------------------------------------------------------
//                           GmatBaseSetupPanel
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
// Author: Linda Jun
// Created: 2008/11/19
/**
 * This class is a generic setup panel used by objects derived from GmatBase
 */
//------------------------------------------------------------------------------

#ifndef GmatBaseSetupPanel_hpp
#define GmatBaseSetupPanel_hpp

#include "GmatPanel.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/config.h>

typedef std::map<wxString, wxSizer*> SizerMapType;
typedef std::map<wxString, int> SizerSizeType;
/**
 * Generic configuration panel for GmatBase derived objects
 * 
 * This class defines a generic object configuration panel that is used when a
 * custom panel has not been coded.  It provides access to all of the object's
 * writable parameters using text controls and comboboxes.
 */
class GmatBaseSetupPanel : public GmatPanel
{
public:
   GmatBaseSetupPanel(wxWindow *parent, const wxString &name);
   virtual ~GmatBaseSetupPanel();
protected:
   
   virtual void         Create();
   virtual void         LoadData();
   virtual void         SaveData();
   wxControl *          BuildControl(wxWindow *parent, GmatBase *theObject, Integer index, const std::string &label, wxFileConfig *config);
   void          		CreateControls(wxFlexGridSizer *mainSizer, GmatBase *theObject);
   void          		CreateControls(GmatBase *theObject, Integer index, wxStaticText **aLabel, wxControl **aControl, wxControl **aUnit, wxFileConfig *config);
   void                 LoadControl(GmatBase *theObject, const std::string &label);
   bool                 SaveControl(GmatBase *theObject, const std::string &label, bool showErrorMsgs = false);
   virtual std::string  GetParameterLabel(GmatBase *theObject, Integer index, wxFileConfig *config) const;
   virtual std::string  GetParameterUnit(GmatBase *theObject, Integer index, wxFileConfig *config) const;
   std::string  		AssignAcceleratorKey(std::string text);
   virtual SizerMapType *       CreateGroups(wxFlexGridSizer *mainSizer, wxFileConfig *config);
   virtual void         CreateProperties(wxFlexGridSizer *mainSizer, GmatBase *theObject, StringArray *propertyNames, SizerMapType *groups, wxFileConfig *config);
   virtual void         SortGroups(StringArray *groupNames, wxFileConfig *config);
   virtual void         SortProperties(StringArray *propertyNames, wxFileConfig *config);
   virtual void         NormalizeLabels( StringArray propertyNames,
                                         std::vector<wxString> propertyGroups, 
                                         std::vector<wxStaticText*> propertyDescriptors, 
                                         std::vector<wxControl*> propertyControls, 
                                         std::vector<wxControl*> propertyUnits );
   wxWindow             *FixTabOrder( wxWindow *lastControl, wxSizer *sizer );
   virtual bool         GetLayoutConfig(GmatBase *theObject, wxFileConfig **config);
   virtual void         RefreshProperties(GmatBase *theObject, std::string ignoreControl = "");
   virtual void         RefreshProperty(GmatBase *theObject, Integer index, wxControl *control, wxFileConfig *config);
   
   // Text control event method
   void OnTextUpdate(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   void OnBrowseButton(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnComboBoxTextChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   
   /// List of used accelerator keys
   std::vector<char> accelKeys;
   /// Mapping between text strings and the associated control
   std::map<Integer, wxControl *> controlMap;
   /// Mapping between the associated control and text strings
   std::map<wxControl *, Integer> inverseControlMap;
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
   
   static const Integer border = 3;
   /// True-false strings (just a convenience here)
   static const wxString TF_SCHEMES[2];

   /// local copy of object for verifying changes before commit/apply
   GmatBase *localObject;
   
};

#endif /* GmatBaseSetupPanel_hpp */
