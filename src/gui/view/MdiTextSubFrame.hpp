//$Header$
//------------------------------------------------------------------------------
//                                  MdiTextSubFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: /samples/docvwmdi/view.hpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding standards.
//
/**
 * Declares text control class
 */
//------------------------------------------------------------------------------
#ifndef MdiTextSubFrame_hpp
#define MdiTextSubFrame_hpp

#include "gmatwxdefs.hpp"

//#include "wx/docview.h"

class MdiTextSubFrame: public wxTextCtrl
{
public:
    MdiTextSubFrame(wxView *v, wxMDIChildFrame *frame, const wxPoint& pos,
                    const wxSize& size, const long style);
    
    wxView *view;
};

#endif
