//$Id$
//------------------------------------------------------------------------------
//                              ScriptPanel
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
// Created: 2005/03/25
//
/**
 * Declares ScriptPanel class.
 */
//------------------------------------------------------------------------------

#ifndef ScriptPanel_hpp
#define ScriptPanel_hpp

#include "GmatSavePanel.hpp"
#include <wx/fontdlg.h>

class ScriptPanel: public GmatSavePanel
{
public:
   // constructors
   ScriptPanel(wxWindow *parent, const wxString &name, bool activeScript = false);
   wxTextCtrl *mFileContentsTextCtrl;
   void ClickButton( bool run = false );
   
private:
   int  mOldLineNumber;
   int  mOldLastPos;
   bool mUserModified;
   
   // wxString mScriptFilename;
   wxColour mDefBgColor;
   wxColour mBgColor;
   
   wxTextCtrl *mLineNumberTextCtrl;
      
   // methods inherited from GmatSavePanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnTextEnterPressed(wxCommandEvent& event);
   void OnTextUpdate(wxCommandEvent& event);
   void OnTextOverMaxLen(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_SYNC_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
