//$Id$
//------------------------------------------------------------------------------
//                              GmatStaticBoxSizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
//   wxStaticText *labelText =
   labelText =
      new wxStaticText(parent, -1, label, wxDefaultPosition, wxSize(220,20), style);
   labelText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                             false, _T(""), wxFONTENCODING_SYSTEM));
   Add(labelText);
}

void GmatStaticBoxSizer::SetLabel(const wxString& label)
{
   labelText->SetLabel(label);
}

#else
GmatStaticBoxSizer::GmatStaticBoxSizer(int orient, wxWindow *parent,
                                       const wxString& label, long style)
   : wxStaticBoxSizer(orient, parent, label)
{
}

void GmatStaticBoxSizer::SetLabel(const wxString& label)
{
   GetStaticBox()->SetLabel(label);
}

#endif
