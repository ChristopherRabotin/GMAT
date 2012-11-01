//$Id$
//------------------------------------------------------------------------------
//                              UniversePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#ifndef UniversePanel_hpp
#define UniversePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "SolarSystem.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <map>

class UniversePanel: public GmatPanel
{
public:
   UniversePanel(wxWindow *parent);
   ~UniversePanel();
   
   virtual void OnScript(wxCommandEvent &event);
    
   
private:
   StringArray mAllFileTypes;
   StringArray mFileTypesInUse;
   
   std::map<wxString, wxString> mFileTypeNameMap;
   
   bool        mHasFileTypesInUseChanged;
   bool        mHasFileNameChanged;
   bool        mHasLSKFileNameChanged;
   bool        mHasTextModified;
   
   wxComboBox *mFileTypeComboBox;
   
   wxTextCtrl *mIntervalTextCtrl;
   wxTextCtrl *mFileNameTextCtrl;
   wxTextCtrl *mLSKFileNameTextCtrl;
   
   wxBitmapButton *mBrowseButton;
   wxBitmapButton *mLSKBrowseButton;

   wxCheckBox     *mOverrideCheckBox;

   GmatStaticBoxSizer *mPageSizer;
   
   SolarSystem  *theSolarSystem;
   
   wxStaticText *lskNameLabel;
   wxStaticText *fileNameLabel;

   std::map<wxString, wxString> mPreviousFileTypeNameMap;
   wxString      previousLskFile;


   void OnBrowseButton(wxCommandEvent& event);
   void OnLSKBrowseButton(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnTextCtrlChange(wxCommandEvent& event);

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 6100,
      ID_COMBOBOX,
      ID_TEXT_CTRL,
      ID_BUTTON_BROWSE,
      ID_LSK_BUTTON_BROWSE,
      ID_CHECKBOX,
   };
};
#endif
