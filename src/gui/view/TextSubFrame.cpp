//$Header$
//------------------------------------------------------------------------------
//                                  TextSubFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: view.cpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding standards.
//
/**
 * Defines text control class
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "TextSubFrame.hpp"


/*
* Window implementations
*/


// Define a constructor for text subwindow
//------------------------------------------------------------------------------
// TextSubFrame(wxView *v, wxFrame *frame, const wxPoint& pos,
//              const wxSize& size, const long style)
//------------------------------------------------------------------------------
TextSubFrame::TextSubFrame(wxView *v, wxFrame *frame, const wxPoint& pos,
                           const wxSize& size, const long style)
    : wxTextCtrl(frame, -1, _T(""), pos, size, style)
{
    view = v;
}
