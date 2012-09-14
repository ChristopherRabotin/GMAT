//$Id$
//------------------------------------------------------------------------------
//                            WelcomePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Thomas Grubb
// Created: 2010/09/09
/**
 * This class contains welcome information
 */
//------------------------------------------------------------------------------
#ifndef WelcomePanel_hpp
#define WelcomePanel_hpp

// This panel is derived from wxFrame
// 
#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/hyperlink.h>

class WelcomePanel: public wxFrame
{
public:
   WelcomePanel(wxFrame *frame, const wxString& title, int x, int y, int w, int h);
   ~WelcomePanel();
   void OnOpenRecentScript(wxHyperlinkEvent &event);
   void OnOpenHelpLink(wxHyperlinkEvent &event);
   void OnOpenSampleScript(wxHyperlinkEvent &event);
   void OnShowWelcomePanelClicked(wxCommandEvent &event);
   wxFlexGridSizer *FillGroup(wxFileConfig *config, wxString INIGroup,
                              wxString INIIconGroup, int maxCols,
                              wxWindowID id, bool isFileList, bool isReversed=false);
   
protected:
   wxBitmap LoadBitmap( wxString filename, int width, int height );

private: 
   // methods inherited from GmatPanel
   virtual void Create();
   void OnExit(wxCommandEvent& event);
   
   Integer bsize;
   
   // Event Handling
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {
      ID_CHECKBOX = 39040,
      ID_URL,
      ID_FILE,
      ID_BUTTON_RECENT,
	  ID_HELP
   };
};

#endif // WelcomePanel_hpp
