//$Header$
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/08
//
/**
 * This header file contains wxWindows definitions.  Include this file if you
 * use wxWindows.
 */
//------------------------------------------------------------------------------
#ifndef gmatwxdefs_h
#define gmatwxdefs_h

#ifdef _MSC_VER
#pragma warning( disable : 4267 )  // Disable warning messages 4267 
#endif                             // (conversion from 'size_t' to 'int', 
                                   // possible loss of data)

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef wxUSE_UNIX
#define wxUSE_UNIX = 0
#endif
    
#ifdef _MSC_VER
#pragma warning( default : 4267 )  // Reset warning messages 4267
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef wxUSE_GLCANVAS
#define wxUSE_GLCANVAS 1
#endif

#ifdef wxUSE_GLCANVAS
#  ifdef __WXMAC__
#    ifdef __DARWIN__
#      include <OpenGL/glu.h>
#    else
#      include <glu.h>
#    endif
#  else
#    include <GL/glu.h>
#  endif
#  include "wx/glcanvas.h"
#endif

#endif // gmatwxdefs_h
