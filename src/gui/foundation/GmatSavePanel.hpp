//$Header$
//------------------------------------------------------------------------------
//                              GmatSavePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatSavePanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatSavePanel_hpp
#define GmatSavePanel_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include "wx/notebook.h"
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatSavePanel : public wxPanel
{
public:
   // constructors
   GmatSavePanel( wxWindow *parent, bool showScriptButton = true,
                  wxString filename = "");

   //loj: 4/1/05 Moved from protected to public because derived classes won't
   //compile under GCC 3.4.2
   virtual void OnSave(wxCommandEvent &event);
   virtual void OnSaveAs(wxCommandEvent &event);
   virtual void OnClose(wxCommandEvent &event);
   virtual void OnHelp(wxCommandEvent &event);
   virtual void OnScript(wxCommandEvent &event);

protected:
   // member functions
   virtual void Create() = 0;
   virtual void Show();
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
    
   bool FileExists(std::string scriptFilename);
      
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool canClose;
   bool mShowScriptButton;
   wxString mFilename;
   
   wxWindow *theParent;
    
   wxBoxSizer *thePanelSizer;
   wxStaticBoxSizer *theTopSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
    
   wxButton *theSaveButton;
   wxButton *theSaveAsButton;
   wxButton *theCloseButton;
   wxButton *theHelpButton;
   wxButton *theScriptButton;
   
   GmatBase *mObject;
    
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_SAVE = 8050,
      ID_BUTTON_SAVE_AS,
      ID_BUTTON_CLOSE,
      ID_BUTTON_HELP,
      ID_BUTTON_SCRIPT,
   };

};

#endif // GmatSavePanel_hpp
