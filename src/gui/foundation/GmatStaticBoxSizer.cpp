//$Header$
//------------------------------------------------------------------------------
//                              GmatStaticBoxSizer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/12/05
//
/**
 * Implements GmatStaticBoxSizer class.
 */
//------------------------------------------------------------------------------

#include "GmatStaticBoxSizer.hpp"

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatStaticBoxSizer(wxWindow *parent, const wxString& label = wxEmptyString,
//                    long style = 0)
//------------------------------------------------------------------------------
/**
 * Constructs GmatStaticBoxSizer object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
#ifdef __WXMAC__
GmatStaticBoxSizer::GmatStaticBoxSizer(int orient, wxWindow *parent,
                                       const wxString& label, long style)
   : wxBoxSizer(orient)
{
   wxStaticText *labelText =
      new wxStaticText(parent, -1, label, wxDefaultPosition, wxSize(220,20), style);
   labelText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                             false, _T(""), wxFONTENCODING_SYSTEM));
   Add(labelText);
}
#else
GmatStaticBoxSizer::GmatStaticBoxSizer(int orient, wxWindow *parent,
                                       const wxString& label, long style)
   : wxStaticBoxSizer(orient, parent, label)
{
}
#endif
