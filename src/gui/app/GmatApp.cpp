//$Header$
//------------------------------------------------------------------------------
//                              GmatMainApp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2003/08/08
// Copyright: (c) 2003 NASA/GSFC. All rights reserved.
//
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#include "GmatMainFrame.hpp"
#include "GmatApp.hpp"

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. GmatApp and
// not wxApp)
IMPLEMENT_APP(GmatApp)
   

//------------------------------------------------------------------------------
// OnInit()
//------------------------------------------------------------------------------
/**
 * The execution of Main program starts here.
 */
//------------------------------------------------------------------------------
bool GmatApp::OnInit()
{
   // create the main application window
   GmatMainFrame *mainFrame = new GmatMainFrame(_T("GMAT - Goddard Mission Analysis Tool"),
                        wxDefaultPosition, wxDefaultSize,
                        wxDEFAULT_FRAME_STYLE);

   // and show it (the frames, unlike simple controls, are not shown when
   // created initially)
   mainFrame->Show(true);

   // success: wxApp::OnRun() will be called which will enter the main message
   // loop and the application will run. If we returned FALSE here, the
   // application would exit immediately.
   return true;
}
