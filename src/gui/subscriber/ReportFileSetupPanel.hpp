//$Id$
//------------------------------------------------------------------------------
//                              ReportFileSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Declares ReportFileSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef ReportFileSetupPanel_hpp
#define ReportFileSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "ReportFile.hpp"

class ReportFileSetupPanel: public GmatPanel
{
public:
   ReportFileSetupPanel(wxWindow *parent, const wxString &subscriberName);
   ~ReportFileSetupPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
   
protected:
   ReportFile *reportFile;
   
   int  mNumParameters;
   bool mHasParameterChanged;
   bool mHasBoolDataChanged;
   
   wxArrayString mObjectTypeList;
   wxArrayString mReportWxStrings;
   
   wxTextCtrl *colWidthTextCtrl;
   wxTextCtrl *precisionTextCtrl;
   wxTextCtrl *mFileTextCtrl;   
   wxCheckBox *writeCheckBox;
   wxCheckBox *showHeaderCheckBox;
   wxCheckBox *leftJustifyCheckBox;
   wxCheckBox *zeroFillCheckBox;   
   wxComboBox *mSolverIterComboBox;   
   wxListBox  *mSelectedListBox;
   wxButton   *mBrowseButton; 
   wxButton   *mViewButton;   
   
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnButtonClick(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent &event);   
   void OnComboBoxChange(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 93000,
      ID_CHECKBOX,
      ID_TEXT_CTRL,
      ID_BUTTON,
      ID_COMBOBOX,
      ID_LISTBOX,
   };

private:
};
#endif
