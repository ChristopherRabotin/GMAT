//$Id$
//------------------------------------------------------------------------------
//                              EndFiniteBurnPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
// Author: Linda Jun
// Created: 2006/07/14
//
/**
 * This class contains the EndFiniteBurn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef EndFiniteBurnPanel_hpp
#define EndFiniteBurnPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GmatCommand.hpp"

class EndFiniteBurnPanel : public GmatPanel
{
public:
   // constructors
   EndFiniteBurnPanel(wxWindow *parent, GmatCommand *cmd);
   ~EndFiniteBurnPanel();
   
protected:
   // member data
   GmatCommand *theCommand;
   wxArrayString mObjectTypeList;
   wxArrayString mSpacecraftList;
   
   wxComboBox *mFiniteBurnComboBox;
   wxTextCtrl *mSatTextCtrl;
   
   wxArrayString ToWxArrayString(const StringArray &array);
   wxString ToWxString(const wxArrayString &names);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnButtonClicked(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextUpdate(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 80000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBOBOX,
   };
};

#endif // EndFiniteBurnPanel_hpp
