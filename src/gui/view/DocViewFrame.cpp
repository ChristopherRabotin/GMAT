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
 * Defines operations on document view.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"

#include "DocViewFrame.hpp"
//  #include "TextDocument.hpp"
//  #include "TextEditView.hpp"


/*
* This is the top-level window of the application.
*/

IMPLEMENT_CLASS(DocViewFrame, wxDocParentFrame)
//  BEGIN_EVENT_TABLE(DocViewFrame, wxDocParentFrame)
//      EVT_MENU(DOCVIEW_ABOUT, DocViewFrame::OnAbout)
//  END_EVENT_TABLE()

//------------------------------------------------------------------------------
// DocViewFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id,
//              const wxString& title, const wxPoint& pos,
//              const wxSize& size, const long type)
//------------------------------------------------------------------------------
DocViewFrame::DocViewFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id,
                           const wxString& title, const wxPoint& pos,
                           const wxSize& size, const long type)
    : wxDocParentFrame(manager, frame, id, title, pos, size, type)
{
    // This pointer only needed if in single window mode
    editMenu = (wxMenu *) NULL;
}

//  //------------------------------------------------------------------------------
//  // void OnAbout(wxCommandEvent& WXUNUSED(event) )
//  //------------------------------------------------------------------------------
//  void DocViewFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
//  {
//      (void)wxMessageBox(_T("Author: Julian Smart\n"),
//                         _T("About DocView"));
//  }

//------------------------------------------------------------------------------
// DocViewFrame *GetMainFrame(void)
//------------------------------------------------------------------------------
DocViewFrame *GetMainFrame(void)
{
    return docMainFrame;
}

