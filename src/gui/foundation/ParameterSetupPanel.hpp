//$Id$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef ParameterSetupPanel_hpp
#define ParameterSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Parameter.hpp"

class ParameterSetupPanel: public GmatPanel
{
public:
   // constructors
   ParameterSetupPanel(wxWindow *parent, const wxString &name); 
    
private:

   Parameter *mParam;
   bool mIsStringVar;
   bool mIsColorChanged;
   bool mIsExpChanged;
  
   wxColour mColor;
   wxString mVarName;

   wxStaticText *mExpStaticText;
   wxStaticText *mColorStaticText;
   
   wxTextCtrl *mVarNameTextCtrl;
   wxTextCtrl *mVarExpTextCtrl;
   
   wxButton *mColorButton;
   
   wxBoxSizer *mPageBoxSizer;
   wxStaticBoxSizer *mVarStaticBoxSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_TEXTCTRL
   };
};

#endif
