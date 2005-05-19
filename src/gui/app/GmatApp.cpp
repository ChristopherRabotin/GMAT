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
// 11/24/2003 - D. Conway, Thinking Systems, Inc.
// Changes:
//  - Added test for Unix envirnment, and set the size if the main frame in that
//    environment, so that the Linux build (and, presumably, Unix builds) would
//    appear reasonably sized.
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#include "GmatMainFrame.hpp"
#include "ViewTextFrame.hpp"
#include "GmatApp.hpp"
#include "GmatAppData.hpp"
#include "Moderator.hpp"

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/splash.h"
#include "wx/image.h"

// In single window mode, don't have any child windows; use
// main window.

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. GmatApp and
// not wxApp)

IMPLEMENT_APP(GmatApp)
    
//------------------------------------------------------------------------------
// GmatApp(void)
//------------------------------------------------------------------------------
GmatApp::GmatApp(void)
{
    theModerator = (Moderator *)NULL;
}

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
    
    // create MessageWindow and save in GmatApp for later use
    GmatAppData::theMessageWindow =
        new ViewTextFrame((wxFrame *)NULL, _T("Message Window"),
                          20, 20, 600, 350, "Permanent");
    GmatAppData::theMessageWindow->Show(false);

    // create the Moderator - GMAT executive
    theModerator = Moderator::Instance();

    // initialize the moderator
    if (theModerator->Initialize(true))
    {
        // get GuiInterpreter
        GmatAppData::SetGuiInterpreter(theModerator->GetGuiInterpreter());

        // Make default size larger for Linux
        wxSize size = ((wxUSE_UNIX != 1) ? wxDefaultSize : wxSize(800, 600));
        
        //show the splash screen
        wxImage::AddHandler(new wxTIFFHandler);
        wxBitmap *bitmap = new wxBitmap("files/splash/GMATSplashScreen.tif",
                           wxBITMAP_TYPE_TIF);

        new wxSplashScreen(*bitmap,
                wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                6000, NULL, -1, wxDefaultPosition, wxSize(100, 100),
                wxSIMPLE_BORDER|wxSTAY_ON_TOP);

        wxYield();

        theMainFrame =
            new GmatMainFrame((wxFrame *)NULL, -1,
                              _T("GMAT - Goddard Mission Analysis Tool"),
                              wxDefaultPosition, size,
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL |
                              wxMAXIMIZE);

        // and show it (the frames, unlike simple controls, are not shown when
        // created initially)
        theMainFrame->CenterOnScreen(wxBOTH);
        theMainFrame->Show(true);
        
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
        wxLogError(wxT("The error occurred during the initialization.  GMAT will exit"));
        wxLog::FlushActive();
        status = false;
    }

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    
    return status;
}

//------------------------------------------------------------------------------
// OnExit()
//------------------------------------------------------------------------------
int GmatApp::OnExit()
{
    return 0;
}
