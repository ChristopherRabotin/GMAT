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
 * Declares GmatStaticBoxSizer class.
 */
//------------------------------------------------------------------------------

#ifndef GmatStaticBoxSizer_hpp
#define GmatStaticBoxSizer_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>

#ifdef __WXMAC__
class GmatStaticBoxSizer : public wxBoxSizer
 #else
 class GmatStaticBoxSizer : public wxStaticBoxSizer
 #endif
{
public:
   
   GmatStaticBoxSizer(int orient, wxWindow *parent,
                      const wxString& label = wxEmptyString, long style = 0);
   
protected:
   
   wxWindow *theParent;
   
};

#endif // GmatStaticBoxSizer_hpp
