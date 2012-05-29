//$Id$
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// 
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#ifndef gmatwxdefs_hpp
#define gmatwxdefs_hpp

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

// VS 2010 defines M_PI in math.h
#if _MSC_VER >= 1600
#undef M_PI
#endif

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
   // Double buffer activation needed in Linux (Patch from Tristan Moody)
   //static int GmatGLCanvasAttribs[2] = {WX_GL_DOUBLEBUFFER, 0};
   //Moved to ViewCanvas.hpp to remove compiler warning: defined but not used (LOJ: 2012.05.29)
#endif

#ifdef __WXMSW__  // Windows
   #define GUI_ACCEL_KEY "&"
   #define wxGMAT_FILTER_NUMERIC wxFILTER_NUMERIC
#else
   #define GUI_ACCEL_KEY ""
   #ifdef __WXMAC__
     #define wxGMAT_FILTER_NUMERIC wxFILTER_NONE
   #else
      #define wxGMAT_FILTER_NUMERIC wxFILTER_NUMERIC
   #endif
#endif // End of OS nits

#endif // gmatwxdefs_hpp
