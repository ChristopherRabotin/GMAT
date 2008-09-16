//$Header$
//------------------------------------------------------------------------------
//                              ScriptPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
   ScriptPanel(wxWindow *parent, const wxString &name);
   wxTextCtrl *mFileContentsTextCtrl;

private:
   int mOldLineNumber;
   int mOldLastPos;
   
   wxString mScriptFilename;
   wxColour mDefBgColor;
   wxColour mBgColor;
   
   wxTextCtrl *mLineNumberTextCtrl;
   
   wxButton *mBuildButton;
   wxButton *mBuildRunButton;
   //wxButton *mFontButton;

   // methods inherited from GmatSavePanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handling
   void OnTextEnterPressed(wxCommandEvent& event);
   void OnTextUpdate(wxCommandEvent& event);
   void OnTextOverMaxLen(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   //void OnFontSelect(wxCommandEvent& event);

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
