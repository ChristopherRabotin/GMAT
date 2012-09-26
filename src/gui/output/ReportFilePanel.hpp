//$Id$
//------------------------------------------------------------------------------
//                              ReportFilePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
   void OnCopy(wxCommandEvent& event);
   void OnRightMouseDown(wxMouseEvent& event);
   void OnSelectAll(wxCommandEvent& event);

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
   wxMenu *mPopupMenu;

   ReportFile *theReport;

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {  
      ID_BUTTON_CLOSE = 8050,
      ID_BUTTON_HELP,
	  ID_MENU_COPY,
	  ID_MENU_SELECTALL,
      ID_TEXTCTRL,
   };

};

#endif // ReportFilePanel_hpp
