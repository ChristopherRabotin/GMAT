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

#include "GmatPanel.hpp"

class ScriptPanel: public GmatPanel
{
public:
   // constructors
   ScriptPanel(wxWindow *parent, const wxString &name);
    
private:
   bool mFileExists;
   bool mEnableSave;

   wxString mFilename;
   wxGridSizer *mBottomSizer;
   wxBoxSizer *mPageSizer;

   wxTextCtrl *mFileContentsTextCtrl;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);

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
