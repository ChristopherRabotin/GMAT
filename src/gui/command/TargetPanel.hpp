//$Id$
//------------------------------------------------------------------------------
//                           TargetPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Waka Waktola
// Created: 2003/12/16
//
/**
 * This class contains the Target setup window.
 */
//------------------------------------------------------------------------------
#ifndef TargetPanel_hpp
#define TargetPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatCommand.hpp"
#include "GmatPanel.hpp"

class TargetPanel : public GmatPanel
{
public:
   // constructor
   TargetPanel(wxWindow *parent, GmatCommand *cmd);
   ~TargetPanel();   
   
private:
   
   GmatCommand *theCommand;
   
   wxComboBox *mSolverComboBox;
   wxComboBox *mSolverModeComboBox;
   wxComboBox *mExitModeComboBox;
   wxCheckBox *mProgressWindowCheckBox;
   wxButton   *mApplyCorrectionsButton;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Combobox event method
   void OnComboBoxChange(wxCommandEvent& event);
   
   // Button press event method
   void OnApplyButtonPress(wxCommandEvent& event);

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 51000,
      ID_COMBO,
      ID_PROGRESS_CHECKBOX,
      ID_APPLYBUTTON
   };
};

#endif // TargetPanel_hpp

