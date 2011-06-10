//$Id$
//------------------------------------------------------------------------------
//                              AboutDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2007/08/22
//
/**
 * Declares AboutDialog class.
 */
//------------------------------------------------------------------------------

#ifndef AboutDialog_hpp
#define AboutDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/button.h>
#include <wx/hyperlink.h>

class AboutDialog : public wxDialog
{
public:
   
   // constructors
   AboutDialog(wxWindow *parent, wxWindowID id, const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_DIALOG_STYLE);
   
   void OnOK(wxCommandEvent &event);
   void OnHyperLinkClick(wxHyperlinkEvent &event);
   
protected:
   
   wxButton *theOkButton;
   wxHyperlinkCtrl *theLicenseLink;
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8100,
      ID_HYPERLINK,
   };

};

#endif // AboutDialog_hpp
