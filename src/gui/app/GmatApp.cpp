//$Header$
//------------------------------------------------------------------------------
//                              GmatApp
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
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#include "GmatMainFrame.hpp"
#include "GmatApp.hpp"
#include "GmatAppData.hpp"
#include "Moderator.hpp"

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
    bool status = false;
    
    // create the Moderator - GMAT executive
    theModerator = Moderator::Instance();

    // initialize the moderator
    if (theModerator->Initialize())
    {
        // get GuiInterpreter
        GmatAppData::SetGuiInterpreter(theModerator->GetGuiInterpreter());
   
        // create the main application window
        GmatMainFrame *mainFrame =
            new GmatMainFrame(_T("GMAT - Goddard Mission Analysis Tool"),
                              wxDefaultPosition, wxDefaultSize,
                              wxDEFAULT_FRAME_STYLE);

        // and show it (the frames, unlike simple controls, are not shown when
        // created initially)
        mainFrame->Show(true);
        status = true;
    }
    else
    {
        // show error messages
        {
            wxBusyCursor bc;
            wxLogWarning(wxT("The Moderator failed to initialize."));
            
            // and if ~wxBusyCursor doesn't do it, then call it manually
            wxYield();
        }

        //loj: How do I change the title?
        wxLogError(wxT("The error occurred during the initialization. The GMAT will exit"));
        wxLog::FlushActive();
        status = false;
    }
   
    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
   
    return status;
}
