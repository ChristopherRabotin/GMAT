//$Header$
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2003/08/08
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
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

#ifdef _MSC_VER
#pragma warning( default : 4267 )  // Reset warning messages 4267
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#endif // gmatwxdefs_h
