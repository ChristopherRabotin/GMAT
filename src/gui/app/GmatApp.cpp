//$Id$
//------------------------------------------------------------------------------
//                              GmatApp
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
// 11/24/2003 - D. Conway, Thinking Systems, Inc.
// Changes:
//  - Added test for Unix envirnment, and set the size if the main frame in that
//    environment, so that the Linux build (and, presumably, Unix builds) would
//    appear reasonably sized.
//
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
#include <wx/splash.h>
#include <wx/image.h>
#include <wx/config.h>

#include "MessageInterface.hpp"
#include "PlotInterface.hpp"
#include "GuiMessageReceiver.hpp"
#include "GuiPlotReceiver.hpp"
#include "GuiInterpreter.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"          // for ToIntegerArray()

//#define DEBUG_GMATAPP
//#define DEBUG_CMD_LINE

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
   GuiMessageReceiver *theMessageReceiver = GuiMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(theMessageReceiver);
   
   GuiPlotReceiver *thePlotReceiver = GuiPlotReceiver::Instance();
   PlotInterface::SetPlotReceiver(thePlotReceiver);
   
   theModerator = (Moderator *)NULL;
   scriptToRun = "";
   showMainFrame = true;
   buildScript = false;
   runScript = false;
   runBatch = false;
   startMatlabServer = false;
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
   
   // Moved from TrajPlotCanvas.cpp (loj: 2009.01.08)
   wxInitAllImageHandlers();
   
   // set application name
   SetAppName("GMAT");
   
#if wxUSE_PRINTING_ARCHITECTURE
   // initialize print data and setup
   globalPrintData = new wxPrintData;
   globalPageSetupData = new wxPageSetupDialogData;
#endif // wxUSE_PRINTING_ARCHITECTURE
   
   try
   {
      GmatAppData *gmatAppData = GmatAppData::Instance();
      FileManager *fm = FileManager::Instance();
      std::string startupFile = fm->GetFullStartupFilePath();
      
      // continue work on this (loj: 2008.12.04)
      //@todo: add all files contains gmat_startup_file in
      // startup up directory, and show user to select one
      //---------------------------------------------------------
      #ifdef __GET_STARTUP_FILE_FROM_USER__
      //---------------------------------------------------------
      bool readOtherStartupFile = false;
      
      wxArrayString choices;
      choices.Add(startupFile);
      choices.Add("Read other startup file");
      wxString msg = "Please select GMAT startup file to read";
      int result =
         wxGetSingleChoiceIndex(msg, "GMAT Startup File", 
                                choices, NULL, -1, -1, true, 150, 200);
      if (result == 1)
         readOtherStartupFile = true;
      
      // reading other startup file, save current directory and set it back
      if (readOtherStartupFile)
      {
         wxString filename = 
            ::wxFileSelector("Choose GMAT startup file", "",
                             "gmat_startup_file.txt", "txt", "*.*");
         if (filename != "")
            startupFile = filename;
      }
      //---------------------------------------------------------      
      #endif
      //---------------------------------------------------------
      
      // create the Moderator - GMAT executive
      theModerator = Moderator::Instance();
      
      // initialize the moderator
      if (theModerator->Initialize(startupFile, true))
      {
         GuiInterpreter *guiInterp = GuiInterpreter::Instance();
         theModerator->SetUiInterpreter(guiInterp);
         theModerator->SetInterpreterMapAndSS(guiInterp);
         guiInterp->BuildCreatableObjectMaps();
         
         // get GuiInterpreter
         gmatAppData->SetGuiInterpreter(
               (GuiInterpreter *)theModerator->GetUiInterpreter());
         
         // set default size
         wxSize size = wxSize(800, 600);
         wxPoint position = wxDefaultPosition;
         
         // for Windows
         #ifdef __WXMSW__
         // Get MainFrame position and size from the config file
         wxConfigBase *pConfig = GmatAppData::Instance()->GetPersonalizationConfig();         
         wxString upperLeftStr, windowSizeStr;
         Integer windowX = -99, windowY = -99;
         Integer windowW = -99, windowH = -99;
         IntegerArray intValues;
         bool upperLeftOk = false;
         bool windowSizeOk = false;
         try
         {
            if (pConfig->Read("/MainFrame/UpperLeft", &upperLeftStr))
            {
               intValues = GmatStringUtil::ToIntegerArray(upperLeftStr.c_str());
               if (intValues.size() == 2)
               {
                  windowX = intValues[0];
                  windowY = intValues[1];
                  upperLeftOk = true;
               }
            }
            if (pConfig->Read("/MainFrame/Size", &windowSizeStr))
            {
               intValues = GmatStringUtil::ToIntegerArray(windowSizeStr.c_str());
               if (intValues.size() == 2)
               {
                  windowW = intValues[0];
                  windowH = intValues[1];
                  windowSizeOk = true;
               }
            }
            #ifdef DEBUG_CONFIG_FILE
            MessageInterface::ShowMessage
               ("   Initial screen pos,  Y = %4d, Y = %4d\n", windowX, windowY);
            MessageInterface::ShowMessage
               ("   Initial window size, W = %4d, H = %4d\n", windowW, windowH);
            #endif
         }
         catch (BaseException &be)
         {
            MessageInterface::ShowMessage("**** %s\n", be.GetFullMessage().c_str());
         }
         
         if (upperLeftOk && windowSizeOk)
         {
            // if upper left is not negative (not maximized),
            // then set new position and size
            if (windowX > 0 && windowY > 0)
            {
               position = wxPoint(windowX, windowY);
               size = wxSize(windowW, windowH);
            }
         }
         
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
            
         
         if (!ProcessCommandLineOptions())
            return false;
         
         if (!showMainFrame)
            return false;
         
         
         if (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI)
         {
            // Initializes all available image handlers.
            ::wxInitAllImageHandlers();
            
            //show the splash screen
            wxString splashFile = theModerator->GetFileName("SPLASH_FILE").c_str();
            if (GmatFileUtil::DoesFileExist(splashFile.c_str()))
            {
               std::string ext = GmatFileUtil::ParseFileExtension(splashFile.c_str());
               std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
               wxBitmap *bitmap = NULL;
               if ((ext == "TIF") || (ext == "TIFF"))
               {
                  wxImage::AddHandler(new wxTIFFHandler);
                  bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_TIF);
               }
               else if ((ext == "JPG") || (ext == "JPEG"))
               {
                  wxImage::AddHandler(new wxJPEGHandler);
                  bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_JPEG);
               }
               else if (ext == "PNG")
               {
                  wxImage::AddHandler(new wxPNGHandler);
                  bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_PNG);
               }
               else
               {
                  MessageInterface::ShowMessage
                    ("*** WARNING *** Can't load SPLASH_FILE from '%s'\n", splashFile.c_str());
               }
                  // Changed to time out in 4 sec (LOJ: 2009.10.07)
               if (bitmap != NULL)
               {
                  long splashStyle;
                  #ifdef __WXMAC__
                     splashStyle = wxSIMPLE_BORDER|wxSTAY_ON_TOP;
                  #else
                     splashStyle = wxSIMPLE_BORDER|wxSTAY_ON_TOP|wxFRAME_SHAPED;
                  #endif
                   wxRegion region(*bitmap, *wxGREEN);
                   wxSplashScreen *splash = new wxSplashScreen(*bitmap,
                                        wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                                        4000, NULL, -1, wxDefaultPosition, wxSize(100, 100),
                                        splashStyle);
                   splash->SetShape(region);
               }
            }
            else
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** Can't load SPLASH_FILE from '%s'\n", splashFile.c_str());
            }
         }
         
         wxYield();
         
         theMainFrame =
            new GmatMainFrame((wxFrame *)NULL, -1,
                              _T("GMAT - General Mission Analysis Tool"),
                              position, size,
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
         
         WriteMessage("GMAT GUI successfully launched.\n", "");
         
         // Show any errors occured during initialization
         std::string savedMsg = MessageInterface::GetQueuedMessage();
         if (savedMsg != "")
            MessageInterface::ShowMessage(savedMsg);
         
         #ifdef DEBUG_GMATAPP
         MessageInterface::ShowMessage
            ("GmatApp::OnInit() size=%dx%d\n", size.GetWidth(), size.GetHeight());
         #endif
         
         // Mac user rather smaller frame and top left corner and show it.
         // (the frames, unlike simple controls, are not shown when created
         // initially)
         #ifndef __WXMAC__
         #ifdef __WXMSW__
         // if upper left is negative, then maximize
         if (windowX < 0)
         {
            theMainFrame->Maximize();
            theMainFrame->CenterOnScreen(wxBOTH);
         }
         #else
         theMainFrame->Maximize();
         theMainFrame->CenterOnScreen(wxBOTH);
         #endif
         #endif
         
         if (startMatlabServer)
            theMainFrame->StartMatlabServer();
         
         if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
            theMainFrame->Show(false);
         else
         {
            theMainFrame->Show(true);
            theMainFrame->ManageMissionTree();
         }
         
         #ifdef DEBUG_CMD_LINE
         MessageInterface::ShowMessage
            ("  buildScript=%d, runScript=%d\n", buildScript, runScript);
         #endif
         
         if (buildScript && runScript)
         {
            BuildAndRunScript(true);
         }
         else if (buildScript)
         {
            BuildAndRunScript(false);
         }
         else if (runBatch)
         {
            RunBatch();
         }
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

      if (startupMessageBuffer.length() > 0)
         MessageInterface::ShowMessage("%s\n", startupMessageBuffer.c_str());
      
      return status;
   }
   catch (BaseException &e)
   {
      WriteMessage("Following error encountered while launching GMAT GUI.\n",
                        e.GetFullMessage() + "\n\n");
      return false;
   }
   catch (...)
   {
      WriteMessage("Unknown error encountered while launching GMAT GUI.\n\n", "");
      //Re-throw the exception so OS can give a debug opportunity.
      throw;
      //return false;
   }
}


//------------------------------------------------------------------------------
// OnExit()
//------------------------------------------------------------------------------
int GmatApp::OnExit()
{
   wxSafeYield();
   // Moderator destructor is private, so just call Finalize()
   theModerator->Finalize();
   
#if wxUSE_PRINTING_ARCHITECTURE
   // delete global print data and setup
   if (globalPrintData)
      delete globalPrintData;
   if (globalPageSetupData)
      delete globalPageSetupData;
#endif // wxUSE_PRINTING_ARCHITECTURE
   
   WriteMessage("GMAT GUI exiting.\n", "");
   return 0;
}


//------------------------------------------------------------------------------
// int FilterEvent(wxEvent& event)
//------------------------------------------------------------------------------
/**
 * Keyboard events go to the component that currently has focus and do not propagate
 * to the parent.
 *
 * This function is called early in event-processing, so we can catch key events
 * globally and do things like F3 for find next.
 */
//------------------------------------------------------------------------------
int GmatApp::FilterEvent(wxEvent& event)
{
   if (theMainFrame)
   {
      if (event.GetEventType() == wxEVT_KEY_DOWN)
      {
         // Find Next
         if (((wxKeyEvent&)event).GetKeyCode() == WXK_F3)
         {
            theMainFrame->OnFindNext( (wxCommandEvent&)event );
            return 1;
         }
         
         // Find and Replace
         if (((wxKeyEvent&)event).GetKeyCode() == 'H' &&     
             ((wxKeyEvent&)event).GetModifiers() == wxMOD_CONTROL)
         {
            theMainFrame->OnReplaceNext( (wxCommandEvent&)event );
            return 1;
         }
      }
   }
   
   return -1;
}


#ifndef INCLUDE_WX_DEBUG
//------------------------------------------------------------------------------
// void OnAssertFailure(const wxChar *file, int line, const wxChar *func, 
//         const wxChar *cond, const wxChar *msg)
//------------------------------------------------------------------------------
/**
 * Overrides the wxApp::OnAssertFailure method, hiding wxWidgets assertion 
 * failures.  
 *
 * To see the wx assertions, define the macro INCLUDE_WX_DEBUG in the 
 * preprocessor.
 *
 * @param  file The name of the source file where the assert occurred
 * @param  line The line number in this file where the assert occurred
 * @param  func The name of the function where the assert occurred, may be 
 *              empty if the compiler doesn't support C99 __FUNCTION__
 * @param  cond The condition of the failed assert in text form
 * @param  msg  The message specified as argument to wxASSERT_MSG or 
 *              wxFAIL_MSG, will be NULL if just wxASSERT or wxFAIL was used
 */
//------------------------------------------------------------------------------
void GmatApp::OnAssertFailure(const wxChar *file, int line, const wxChar *func, 
         const wxChar *cond, const wxChar *msg)
{
}
#endif


//------------------------------------------------------------------------------
// bool ProcessCommandLineOptions()
//------------------------------------------------------------------------------
/// @note Should this be moved to the Moderator so the options are available to
/// all user interfaces?
bool GmatApp::ProcessCommandLineOptions()
{
   #ifdef DEBUG_CMD_LINE
   MessageInterface::ShowMessage("GmatApp::ProcessCommandLineOptions() entered\n");
   #endif
   
   bool retval = true;

   std::string gmatUsage;
   std::string gmatText =
         "If no arguments are specified, GMAT opens with the full GUI and with the default\n"
         "mission loaded.  If a GMAT script name is provided for 'scriptname', GMAT will\n"
         "open with the specified script loaded.  GMAT is started whether or not any of the\n"
         "listed options are used.\n";
   #ifdef __WXMAC__
      gmatUsage =
            "Usage: open GMAT.app [--args [options] [scriptname]]\n\n";
      gmatText +=
            "The \"--args\" must appear before any specified options or a scriptname.\n";
   #else
      gmatUsage =
            "Usage: gmat [options] [scriptname]\n\n";
   #endif
   std::string gmatHelp = gmatUsage + gmatText + "\n";
   gmatHelp +=
         "-h, \t--help        \t\tDisplay command line usage information in the Message Window\n"
         "-m, \t--minimize    \t\tOpen GMAT with a minimized interface\n"
         "-r, \t--run         \t\tAutomatically run the specified script after loading\n"
         "    \t              \t\t[has no effect if no script is specified]\n"
         "-v, \t--version     \t\tDisplay version information in the Message Window\n"
         "-x, \t--exit        \t\tExit GMAT after running the specified script\n"
         "    \t              \t\t[has no effect if no script is specified]\n";
   
   wxString commandLineOptions = wxT(gmatHelp.c_str());

   #ifdef DEBUG_CMD_LINE
   MessageInterface::ShowMessage("argc = %d\n", argc);
   #endif
   
   // Handle any command line arguments
   if (argc > 1)
   {
      for (int i = 1; i < argc; ++i)
      {
         std::string arg = argv[i];
         #ifdef DEBUG_CMD_LINE
         MessageInterface::ShowMessage("arg = %s\n", arg.c_str());
         #endif
         if ((arg == "--version") || (arg == "-v"))
         {
            wxString buildDate;
            buildDate.Printf("Build Date: %s %s\n\n", __DATE__, __TIME__);
            startupMessageBuffer += buildDate;
            #ifdef DEBUG_CMD_LINE
            MessageInterface::ShowMessage(buildDate.c_str());
            #endif
         }
         #ifdef __ADD_GMAT_SERVER__
         else if (arg == "--start-server")
         {
            startMatlabServer = true;
         }
         #endif
         else if ((arg == "--run") || (arg == "-r"))
         {
            if (argc < 3)
            {
               MessageInterface::ShowMessage
                  ("Please enter script file name to run\n");
            }
            else
            {
               buildScript = true;
               runScript = true;
            }
         }
         else if ((arg == "--help") || (arg == "-h"))
         {
            startupMessageBuffer += commandLineOptions;
            //MessageInterface::ShowMessage(commandLineOptions.c_str());
         }
         else if ((arg == "--exit") || (arg == "-x"))
         {
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::EXIT_AFTER_RUN);
         }
         else if ((arg == "--minimize") || (arg == "-m"))
         {
            GmatGlobal::Instance()->SetGuiMode(GmatGlobal::MINIMIZED_GUI);
         }
         else if ((arg == "--nits") || (arg == "-n"))
         {
            // NITS Support
            MessageInterface::ShowMessage("GMAT is running as a NITS client\n");
            // Check for NITS plugin by checking for a NITS connection command
            StringArray commands = 
               theModerator->GetListOfFactoryItems(Gmat::COMMAND);
            if (find(commands.begin(), commands.end(), "SendMessage") == commands.end())
            {
               wxBusyCursor bc;
               wxLogError(wxT("GMAT was started as a NITS client, but the "
                  "NITS plugin was not loaded.\n"
                  "The error occurred during the initialization.  GMAT will exit."));
               wxLog::FlushActive();
               retval = false;
            }
            else
            {
               scriptToRun = "NITS_Config.script";
               // Replace single quotes
               scriptToRun = GmatStringUtil::Replace(scriptToRun, "'", "");
               runScript = true;
               GmatGlobal::Instance()->SetNitsClient(true);
               GmatGlobal::Instance()->SetRunMode(GmatGlobal::EXIT_AFTER_RUN);
               
               // When this changes from throw-away prototype mode to NITS compatible
               // mode, we'll want to do a NITS handshake here to register as a NITS
               // client.  That mode is not yet ready for use, though.  Here's a start on 
               // how to do that, via a static method in the command that does not 
               // (and cannot) yet exist:
               //SendMessage::ShakeHands();
               
               ++i;
               #ifdef DEBUG_CMD_LINE
                  MessageInterface::ShowMessage("%s\n", scriptToRun.c_str());
               #endif
            }
         }
         else
         {
            #ifdef DEBUG_CMD_LINE
            MessageInterface::ShowMessage("Checking if arg is script name, arg=<%s>\n", arg.c_str());
            #endif
            
            bool isArgValid = false;
            // Remove single quotes before checking
            std::string tempfile = GmatStringUtil::Replace(arg, "'", "");
            
            #ifdef DEBUG_CMD_LINE
            MessageInterface::ShowMessage("tempfile=<%s>\n", tempfile.c_str());
            #endif
            
            // Check for file type association with GMAT on Windows
            // since argv[0] contains the file name to run
            // 2013.03.11: Changed to check for the script file so that script
            // name can appear anywhere in the argument list, for example --run can
            // appear after script name such as, 'BplaneTarget.script' -- run
            // So removed #ifdef __WIN32__
            //#ifdef __WIN32__
            if (GmatFileUtil::DoesFileExist(tempfile))
            {
               // Set this as script to run
               scriptToRun = tempfile;
               buildScript = true;
               isArgValid = true;
               #ifdef DEBUG_CMD_LINE
               MessageInterface::ShowMessage("scriptToRun=<%s>\n", scriptToRun.c_str());
               #endif
            }
            //#endif
            //@todo Implement this for Mac and Linux?
            
            if (!isArgValid)
            {
               MessageInterface::ShowMessage("The option \"%s\" is not valid.\n", arg.c_str());
               MessageInterface::ShowMessage(commandLineOptions.c_str());
               break;
            }
         }
      }
   }
   
   #ifdef DEBUG_CMD_LINE
   MessageInterface::ShowMessage
      ("GmatApp::ProcessCommandLineOptions() returning %d, runScript=%d\n", retval, runScript);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// void BuildAndRunScript(bool runScript)
//------------------------------------------------------------------------------
void GmatApp::BuildAndRunScript(bool runScript)
{
   #ifdef DEBUG_CMD_LINE
   MessageInterface::ShowMessage
      ("GmatApp::BuildAndRunScript() entered, runScript=%d, scriptToRun='%s'\n",
       runScript, scriptToRun.c_str());
   #endif
   
   if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
      theMainFrame->Iconize(true);
   
   // Build script
   bool builtOk = false;
   try
   {
      #ifdef DEBUG_CMD_LINE
      MessageInterface::ShowMessage("   Building script ...\n");
      #endif
      wxSafeYield();
      builtOk = theMainFrame->BuildScript(scriptToRun.c_str(), true);
   }
   catch (BaseException &e)
   {
      WriteMessage("Following error encountered while building the script\n   ",
                   scriptToRun + "\n   ", e.GetFullMessage());
   }
   catch (...)
   {
      WriteMessage("Unknown error encountered while building the script\n   ",
                   scriptToRun + "\n");
   }
   
   // Run script
   Integer runStatus = 0;
   if (builtOk && runScript)
   {
      try
      {
         #ifdef DEBUG_CMD_LINE
         MessageInterface::ShowMessage("   Running script ...\n");
         #endif
         wxSafeYield();
         runStatus = theMainFrame->RunCurrentScript();
         if (runStatus != 1)
         {
            WriteMessage("Failed to run the script\n   ", scriptToRun + "\n");
            MessageInterface::LogMessage("   Run status is %d\n", runStatus);
         }
      }
      catch (BaseException &e)
      {
         WriteMessage("Following error encountered while running the script\n   ",
                      scriptToRun + "\n   ", e.GetFullMessage());
      }
      catch (...)
      {
         WriteMessage("Unknown error encountered while running the script\n   ",
                      scriptToRun + "\n");
      }
   }
   else
   {
      WriteMessage("Failed to build the script\n   ", scriptToRun + "\n");
   }
   
   wxSafeYield();
   // Close GMAT on option
   if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::EXIT_AFTER_RUN)
   {
      //Set auto exit mode to GmatMainFrame
      theMainFrame->SetAutoExitAfterRun(true);
      theMainFrame->Close();
      wxSafeYield();
      #ifdef __LINUX__
      // Linux needs this to complete shutdown
      MessageInterface::ShowMessage("\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void RunBatch()
//------------------------------------------------------------------------------
void GmatApp::RunBatch()
{
}


//------------------------------------------------------------------------------
// void WriteMessage(const std::string &msg1,const std::string &msg2, ... )
//------------------------------------------------------------------------------
void GmatApp::WriteMessage(const std::string &msg1, const std::string &msg2,
                           const std::string &msg3)
{
   wxDateTime now = wxDateTime::Now();
   wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
   std::string nowStr = wxNowStr.c_str();
   
   MessageInterface::LogMessage(nowStr + msg1 + msg2 + msg3);
}

