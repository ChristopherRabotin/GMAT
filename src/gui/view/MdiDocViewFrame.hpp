//$Header$
//------------------------------------------------------------------------------
//                                  MdiDocViewFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: samples/docvwmdi/docview.hpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding standards.
//
/**
 * Declares operations on MDI document view.
 */
//------------------------------------------------------------------------------
#ifndef MdiDocViewFrame_hpp
#define MdiDocViewFrame_hpp

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"

#include "GmatAppData.hpp"

// Define a new frame
class MdiDocViewFrame: public wxDocMDIParentFrame
{
    DECLARE_CLASS(MdiDocViewFrame)
public:
    wxMenu *editMenu;
    
    MdiDocViewFrame(wxDocManager *manager, wxFrame *frame,
                    const wxString& title, const wxPoint& pos,
                    const wxSize& size, const long type);
    
    void OnAbout(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
        
    DECLARE_EVENT_TABLE();
//      enum
//      {
//          DOCVIEW_CUT = 1,
//          DOCVIEW_ABOUT
//      }
};

extern MdiDocViewFrame *GetMdiMainFrame(void);

//  #define DOCVIEW_CUT     1
//  #define DOCVIEW_ABOUT   2

extern MdiDocViewFrame *mdiDocMainFrame;

#endif
