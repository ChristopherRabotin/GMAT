//$Id$
//------------------------------------------------------------------------------
//                              ReportPanel
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
// Created: 2006/08/10
/**
 * Declares ReportPanel class. This class allows user to setup Report command.
 */
//------------------------------------------------------------------------------
#ifndef ReportPanel_hpp
#define ReportPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class ReportPanel: public GmatPanel
{
public:
   ReportPanel(wxWindow *parent, GmatCommand *cmd);
   ~ReportPanel();
   
protected:
   GmatCommand *theCommand;
   
   int  mNumParameters;
   bool mHasReportFileChanged;
   bool mHasParameterChanged;
   
   wxArrayString mObjectTypeList;
   wxArrayString mReportWxStrings;
   
   wxComboBox *mReportFileComboBox;   
   wxListBox  *mSelectedListBox;
   wxButton   *mViewButton;   
   
   void OnButtonClick(wxCommandEvent& event);
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
      ID_BUTTON,
      ID_LISTBOX,
      ID_COMBOBOX,
   };

private:
   
};
#endif
