//$Header$
//------------------------------------------------------------------------------
//                                  DocViewFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: docview.hpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding standards.
//
/**
 * Declares operations on document view.
 */
//------------------------------------------------------------------------------
#ifndef DocViewFrame_hpp
#define DocViewFrame_hpp

#include "wx/docview.h"

// Define a new frame
class DocViewFrame: public wxDocParentFrame
{
    DECLARE_CLASS(DocViewFrame)
public:
    wxMenu *editMenu;
    
    DocViewFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id,
                 const wxString& title, const wxPoint& pos,
                 const wxSize& size, const long type);
    
    void OnAbout(wxCommandEvent& event);
    
//      DECLARE_EVENT_TABLE();
//      enum
//      {
//          DOCVIEW_CUT = 1,
//          DOCVIEW_ABOUT
//      }
};

extern DocViewFrame *GetMainFrame(void);

//  #define DOCVIEW_CUT     1
//  #define DOCVIEW_ABOUT   2

extern DocViewFrame *docMainFrame;

#endif
