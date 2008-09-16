//$Header$
//------------------------------------------------------------------------------
//                              ReportFilePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/04/18
//
/**
 * Declares ReportFilePanel class.
 */
//------------------------------------------------------------------------------

#ifndef ReportFilePanel_hpp
#define ReportFilePanel_hpp

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
#include "ReportFile.hpp"

class ReportFilePanel : public wxPanel
{
public:
   // constructors
   ReportFilePanel( wxWindow *parent, wxString reportName);

   void OnClose(wxCommandEvent &event);
   void OnHelp();

   wxTextCtrl *mFileContentsTextCtrl;

protected:
   // member functions
   void Create();
   void Show();
   void LoadData();

   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   wxString mReportName;
   
   wxWindow *theParent;
    
   wxBoxSizer *thePanelSizer;
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;

   wxButton *theCloseButton;
   wxButton *theHelpButton;

   ReportFile *theReport;

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {  
      ID_BUTTON_CLOSE = 8050,
      ID_BUTTON_HELP,
      ID_TEXTCTRL,
   };

};

#endif // ReportFilePanel_hpp
