//$Id$
//------------------------------------------------------------------------------
//                              GmatStaticBoxSizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/12/05
//
/**
 * Declares GmatStaticBoxSizer class.
 */
//------------------------------------------------------------------------------

#ifndef GmatStaticBoxSizer_hpp
#define GmatStaticBoxSizer_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>

#if wxCHECK_VERSION(3, 0, 0) || !defined( __WXMAC__)
   class GmatStaticBoxSizer : public wxStaticBoxSizer
#else
   class GmatStaticBoxSizer : public wxBoxSizer
#endif
{
public:
   
   GmatStaticBoxSizer(int orient, wxWindow *parent,
                      const wxString& label = wxEmptyString, long style = 0);
   
   void SetLabel(const wxString& label);

protected:
   
//   wxWindow     *theParent;
   wxStaticText *labelText;
   
};

#endif // GmatStaticBoxSizer_hpp
