//$Id$
//------------------------------------------------------------------------------
//                              MinimizePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Allison Greene
// Created: 2006/09/20
/**
 * This class contains the Minimize command setup window.
 */
//------------------------------------------------------------------------------

#ifndef MINIMIZEPANEL_HPP_
#define MINIMIZEPANEL_HPP_

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "Minimize.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"

class MinimizePanel : public GmatPanel
{
public:
   MinimizePanel(wxWindow *parent, GmatCommand *cmd);
   ~MinimizePanel(); 
    
private:             
   GuiItemManager *theGuiManager;

   wxString  solverName;
   wxString  variableName;
   Parameter *minParam;
   bool mVarNameChanged;
   
   wxTextCtrl *mVariableTextCtrl;
   
   wxButton *mChooseButton;
   
   wxComboBox *mSolverComboBox;
   
   wxArrayString mObjectTypeList;
   Minimize *mMinimizeCommand;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   void ShowGoalSetup();
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);    
   void OnSolverSelection(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
  
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 53000,
      ID_TEXTCTRL,
      ID_BUTTON,
      ID_COMBO,
      ID_GRID,
   };
};

#endif /*MINIMIZEPANEL_HPP_*/

