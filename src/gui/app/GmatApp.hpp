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
// Created: 2003/08/05
//
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#ifndef GmatApp_hpp
#define GmatApp_hpp

#include "gmatwxdefs.hpp"
#include "Moderator.hpp"
#include "GmatMainFrame.hpp"
#include "GmatAppData.hpp"

#if wxUSE_PRINTING_ARCHITECTURE
// global print data, to remember settings during the session
wxPrintData *globalPrintData = (wxPrintData*) NULL;
wxPageSetupData *globalPageSetupData = (wxPageSetupData*) NULL;
#endif // wxUSE_PRINTING_ARCHITECTURE

class GmatApp : public wxApp
{
public:
   GmatApp();
   // override base class virtuals
   // ----------------------------
   
   // This one is called on application startup and is a good place for the app
   // initialization (doing it here and not in the constructor allows to have
   // an error return: if OnInit() returns false, the application terminates)
   virtual bool OnInit();
   
   int OnExit(void);
   int FilterEvent(wxEvent& event);

#ifndef INCLUDE_WX_DEBUG
   void OnAssertFailure(const wxChar *file, int line, const wxChar *func, 
         const wxChar *cond, const wxChar *msg);
#endif
   
protected:
   
private:
   
   Moderator *theModerator;
   GmatMainFrame *theMainFrame;
   std::string scriptToRun;
   wxString batchFile;
   bool showMainFrame;
   bool buildScript;
   bool runScript;
   bool runBatch;
   bool startMatlabServer;

   /// Buffer for messages based on command line parameters, so they can
   /// display after the Moderator initializes
   std::string startupMessageBuffer;
   
   bool ProcessCommandLineOptions();
   void BuildAndRunScript(bool runScript = true);
   void RunBatch();
   void WriteMessage(const std::string &msg1, const std::string &msg2,
                     const std::string &msg3 = "");
};

DECLARE_APP(GmatApp)

#endif // GmatApp_hpp
