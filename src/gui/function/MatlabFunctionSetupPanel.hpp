//$Id$
//------------------------------------------------------------------------------
//                              MatlabFunctionSetupPanel
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
// Author: Allison Greene
// Created: 2005/02/11
//
/**
 * Declares MatlabFunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef MatlabFunctionSetupPanel_hpp
#define MatlabFunctionSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Function.hpp"

class MatlabFunctionSetupPanel: public GmatPanel
{
public:
   // constructors
   MatlabFunctionSetupPanel(wxWindow *parent, const wxString &name);
    
private:

   Function *theMatlabFunction;

   wxStaticBoxSizer *mTopSizer;
   wxBoxSizer *mPageSizer;
   wxTextCtrl *mPathTextCtrl;
   wxButton *mBrowseButton;
    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
