//$Header$
//------------------------------------------------------------------------------
//                              ScriptEventPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/1/12
//
/**
 * Declares ScriptEventPanel class.
 */
//------------------------------------------------------------------------------

#ifndef ScriptEventPanel_hpp
#define ScriptEventPanel_hpp

#include "GmatPanel.hpp"
#include "Parameter.hpp"

class ScriptEventPanel: public GmatPanel
{
public:
   // constructors
   //ScriptEventPanel(wxWindow *parent, GmatCommand *cmd);
   ScriptEventPanel(wxWindow *parent, MissionTreeItemData *item);
   ~ScriptEventPanel();
   
   wxTextCtrl *mFileContentsTextCtrl;
   
private:
   // member data
   MissionTreeItemData *theItem;
   GmatCommand *theCommand;
   GmatCommand *mPrevCommand;
   GmatCommand *mNextCommand;
   GmatCommand *mNewCommand;
   
   wxGridSizer *mBottomSizer;
   wxBoxSizer *mPageSizer;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void OnScript(wxCommandEvent &event);
   
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
