//$Id$
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
#include "GmatApp.hpp"
#include "GmatMainFrame.hpp"
#include "ViewTextFrame.hpp"
#include "GmatAppData.hpp"
#include "Moderator.hpp"
#include <wx/datetime.h>
#include "wx/splash.h"
#include "wx/image.h"

//#define DEBUG_GMATAPP

// In single window mode, don't have any child windows; use
// main window.

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. GmatApp and
// not wxApp)

IMPLEMENT_APP(GmatApp)
    
//------------------------------------------------------------------------------
// GmatApp()
//------------------------------------------------------------------------------
GmatApp::GmatApp()
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
   
   try
   {
      GmatAppData *gmatAppData = GmatAppData::Instance();
      
      // create the Moderator - GMAT executive
      theModerator = Moderator::Instance();
      
      // initialize the moderator
      if (theModerator->Initialize(true))
      {
         // get GuiInterpreter
         gmatAppData->SetGuiInterpreter(theModerator->GetGuiInterpreter());
         
         // set default size
         wxSize size = wxSize(800, 600);
         
         // for Windows
         #ifdef __WXMSW__
         size = wxSize(800, 600);
         #endif
         
         // The code above broke the Linux scaling.  This is a temporary hack to 
         // repair it.  PLEASE don't use UNIX macros to detect the Mac code!!!
         #ifdef __LINUX__
            size = wxSize(1024, 768);
         #endif
         // Mac doesn't look right, either
         #ifdef __WXMAC__
            size = wxSize(235,900);
         #endif
            
         //show the splash screen
         try
         {
            wxImage::AddHandler(new wxTIFFHandler);
            
            wxString splashFile = theModerator->GetFileName("SPLASH_FILE").c_str();
            wxBitmap *bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_TIF);
            
            new wxSplashScreen(*bitmap,
                               wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                               6000, NULL, -1, wxDefaultPosition, wxSize(100, 100),
                               wxSIMPLE_BORDER|wxSTAY_ON_TOP);
         }
         catch (BaseException &e)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
         }
         
         wxYield();
         
         theMainFrame =
            new GmatMainFrame((wxFrame *)NULL, -1,
                              _T("GMAT - General Mission Analysis Tool"),
                              wxDefaultPosition, size,
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
         
         #ifdef DEBUG_GMATAPP
         MessageInterface::ShowMessage
            ("GmatApp::OnInit() size=%dx%d\n", size.GetWidth(), size.GetHeight());
         #endif
         
         // Mac user rather smaller frame and top left corner and show it.
         // (the frames, unlike simple controls, are not shown when created
         // initially)
#ifndef __WXMAC__
         theMainFrame->Maximize();
         theMainFrame->CenterOnScreen(wxBOTH);
#endif
         
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
         
         wxLogError(wxT("The error occurred during the initialization.  GMAT will exit"));
         wxLog::FlushActive();
         status = false;
      }
      
      // success: wxApp::OnRun() will be called which will enter the main message
      // loop and the application will run. If we returned FALSE here, the
      // application would exit immediately.
      
      wxDateTime now = wxDateTime::Now();
      wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
      std::string nowStr = wxNowStr.c_str();
      
      MessageInterface::LogMessage(nowStr + "GMAT GUI successfully launched.\n");
      return status;
   }
   catch (BaseException &e)
   {
      wxDateTime now = wxDateTime::Now();
      wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
      std::string nowStr = wxNowStr.c_str();
      
      MessageInterface::LogMessage
         (nowStr + "Error encounted while launching GMAT GUI.\n\n");
      
      MessageInterface::LogMessage(e.GetFullMessage());
      return false;
   }
   catch (...)
   {
      wxDateTime now = wxDateTime::Now();
      wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
      std::string nowStr = wxNowStr.c_str();
      
      MessageInterface::LogMessage
         (nowStr + "Unknown error encounted while launching GMAT GUI.\n\n");
      return false;
   }
}


//------------------------------------------------------------------------------
// OnExit()
//------------------------------------------------------------------------------
int GmatApp::OnExit()
{
   // Moderator destructor is private, so just call Finalize()
   theModerator->Finalize();
   return 0;
}
