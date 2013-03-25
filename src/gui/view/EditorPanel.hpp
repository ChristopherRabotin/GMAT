//$Id$
//------------------------------------------------------------------------------
//                              EditorPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2012 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2009/01/15
//
/**
 * Declares EditorPanel class.
 */
//------------------------------------------------------------------------------

#ifndef EditorPanel_hpp
#define EditorPanel_hpp

#include "GmatSavePanel.hpp"
#include "Editor.hpp"
#include <wx/fontdlg.h>

class EditorPanel: public GmatSavePanel
{
public:
   // default constructor
   EditorPanel(wxWindow *parent, const wxString &name, bool activeScript = false);
   // destructor
   ~EditorPanel();
   
   // method to retrieve the Editor
   Editor* GetEditor() { return mEditor; };
   void ClickButton( bool run = false );
   
private:
   Editor   *mEditor;
   
   // methods inherited from GmatSavePanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnTextOverMaxLen(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 9000,
      ID_SYNC_BUTTON,
   };
};

#endif
