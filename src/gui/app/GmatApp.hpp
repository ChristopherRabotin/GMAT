//$Header$
//------------------------------------------------------------------------------
//                              GmatApp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2003/08/05
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#ifndef GmatApp_hpp
#define GmatApp_hpp

#include "gmatwxdefs.h"


//  // ----------------------------------------------------------------------------
//  // resources
//  // ----------------------------------------------------------------------------

//  // the application icon (under Windows and OS/2 it is in resources)
//  #if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
//      #include "mondrian.xpm"
//  #endif


class GmatApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

#endif // GmatApp_hpp
