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
// Original File: /samples/docvwmdi/docview.hpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding standards.
//
/**
 * Defines operations on MDI document view.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"

#include "MdiDocViewFrame.hpp"

#include "wx/toolbar.h"

// bitmap buttons for the toolbar
//#include "bitmaps/open.xpm"


#ifndef __WXMSW__
#include "bitmaps/open.xpm"
#endif

/*
* This is the top-level window of the application.
*/

IMPLEMENT_CLASS(MdiDocViewFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(MdiDocViewFrame, wxDocMDIParentFrame)
//      EVT_MENU(DOCVIEW_ABOUT, MdiDocViewFrame::OnAbout)
   EVT_CLOSE(MdiDocViewFrame::OnClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiDocViewFrame(wxDocManager *manager, wxFrame *frame, 
//              const wxString& title, const wxPoint& pos,
//              const wxSize& size, const long type)
//------------------------------------------------------------------------------
MdiDocViewFrame::MdiDocViewFrame(wxDocManager *manager, wxFrame *frame, 
                                 const wxString& title, const wxPoint& pos,
                                 const wxSize& size, const long type)
    : wxDocMDIParentFrame(manager, frame, -1, title, pos, size, type,
                          _T("MdiDocViewFrame"))
{
    editMenu = (wxMenu *) NULL;
    
    // status bar
    CreateStatusBar();
    
    // tool bar
    wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    wxBitmap tbBitmaps[1];

    tbBitmaps[0] = wxBITMAP(open);
    
    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(wxID_OPEN, tbBitmaps[0], wxNullBitmap, FALSE, -1, -1,
                     (wxObject*)NULL, _T("Open"), _T("Open a script file"));
    
    toolBar->Realize();

}

//  //------------------------------------------------------------------------------
//  // void OnAbout(wxCommandEvent& WXUNUSED(event) )
//  //------------------------------------------------------------------------------
//  void MdiDocViewFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
//  {
//      (void)wxMessageBox(_T("Author: Julian Smart\n"),
//                         _T("About DocView"));
//  }

//------------------------------------------------------------------------------
// MdiDocViewFrame *GetMdiMainFrame(void)
//------------------------------------------------------------------------------
MdiDocViewFrame *GetMdiMainFrame(void)
{
    return mdiDocMainFrame;
}

void MdiDocViewFrame::OnClose(wxCloseEvent& event)
{
   // check if window is dirty?
  
   // remove from list of frames
   GmatAppData::GetMainFrame()->scriptMdiShown = false;

   event.Skip();
}
