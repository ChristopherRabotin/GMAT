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

class ScriptPanel: public GmatSavePanel
{
public:
   // constructors
   ScriptPanel(wxWindow *parent, const wxString &name);
//   wxMenuBar *CreateScriptMenu();

private:
   wxString mScriptFilename;
   wxStaticBoxSizer *mTopSizer;
   wxGridSizer *mBottomSizer;
   wxBoxSizer *mPageSizer;

   wxTextCtrl *mFileContentsTextCtrl;

   wxButton *mBuildButton;
   wxButton *mBuildRunButton;


   // methods inherited from GmatSavePanel
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
