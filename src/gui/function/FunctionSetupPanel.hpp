//$Id$
//------------------------------------------------------------------------------
//                              FunctionSetupPanel
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
// Created: 2004/12/15
//
/**
 * Declares FunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef FunctionSetupPanel_hpp
#define FunctionSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Function.hpp"

#ifdef __USE_STC_EDITOR__
#include "Editor.hpp"
#endif

class FunctionSetupPanel: public GmatPanel
{
public:
   // constructors
   FunctionSetupPanel(wxWindow *parent, const wxString &name); 
   ~FunctionSetupPanel();
   
   virtual void SetEditorModified(bool flag);
   
   wxTextCtrl *mFileContentsTextCtrl;
   
#ifdef __USE_STC_EDITOR__
   Editor* GetEditor() { return mEditor; };
#endif
   
private:
   Function *theGmatFunction;
   wxString mFullFunctionPath;
   wxString mFunctionName;
   
#ifdef __USE_STC_EDITOR__
   Editor *mEditor;
#endif
   
   bool mEnableLoad;
   bool mEnableSave;
   bool mIsNewFunction;
   wxString mFilename;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   void OnSaveAs(wxCommandEvent& event);
   
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
