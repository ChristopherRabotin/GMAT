//$Header$
//------------------------------------------------------------------------------
//                              GmatPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatPanel class.
 */
//------------------------------------------------------------------------------

#ifndef GmatPanel_hpp
#define GmatPanel_hpp

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

class GmatPanel : public wxPanel
{
public:
   // constructors
   GmatPanel( wxWindow *parent, bool showScriptButton = true);

   //loj: 4/1/05 Moved from protected to public because derived classes won't
   //compile under GCC 3.4.2
   virtual void OnApply(wxCommandEvent &event);
   virtual void OnOK(wxCommandEvent &event);
   virtual void OnCancel(wxCommandEvent &event);
   virtual void OnHelp(wxCommandEvent &event);
   virtual void OnScript(wxCommandEvent &event);

protected:
   // member functions
   virtual void Create() = 0;
   virtual void Show();
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
    
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool canClose;
   bool mShowScriptButton;
   
   wxWindow *theParent;
    
   wxBoxSizer *thePanelSizer;
   wxStaticBoxSizer *theTopSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
    
   wxButton *theOkButton;
   wxButton *theApplyButton;
   wxButton *theCancelButton;
   wxButton *theHelpButton;
   wxButton *theScriptButton;
   
   GmatBase *mObject;
    
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8000,
      ID_BUTTON_APPLY,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
      ID_BUTTON_SCRIPT,
   };

};

#endif // GmatPanel_hpp
