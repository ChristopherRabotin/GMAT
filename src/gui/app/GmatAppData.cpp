//$Id$
//------------------------------------------------------------------------------
//                              GmatAppData
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
// Created: 2003/10/29
// Modified: 
//    2010.03.01 Thomas Grubb 
//       - Open configuration file (GMAT.ini)
//
/**
 * defines application data.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>

#if !defined __CONSOLE_APP__
#include "ViewTextFrame.hpp"
#include "ResourceTree.hpp"
#include "MissionTree.hpp"
#include "OutputTree.hpp"
#include "GmatMainFrame.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#endif

//#define DEBUG_SET_ICON

GmatAppData* GmatAppData::theGmatAppData = NULL;

//------------------------------------------------------------------------------
// GmatAppData* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the GmatAppData class.
 */
//------------------------------------------------------------------------------
GmatAppData* GmatAppData::Instance()
{
   if (!theGmatAppData)
      theGmatAppData = new GmatAppData;
   
   return theGmatAppData;
}


//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter* GmatAppData::GetGuiInterpreter()
{
   return theGuiInterpreter;
}


//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
//------------------------------------------------------------------------------
void GmatAppData::SetGuiInterpreter(GuiInterpreter *guiInterp)
{
   theGuiInterpreter = guiInterp;
}


#if !defined __CONSOLE_APP__

//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *mainFrame)
//------------------------------------------------------------------------------
void GmatAppData::SetMainFrame(GmatMainFrame *mainFrame)
{
   theMainFrame = mainFrame;
}


//------------------------------------------------------------------------------
// GmatMainFrame* GetMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame* GmatAppData::GetMainFrame()
{
   return theMainFrame;
}


//------------------------------------------------------------------------------
// void SetResourceTree(ResourceTree *resourceTree)
//------------------------------------------------------------------------------
void GmatAppData::SetResourceTree(ResourceTree *resourceTree)
{
   theResourceTree = resourceTree;
}


//------------------------------------------------------------------------------
// ResourceTree* GetResourceTree()
//------------------------------------------------------------------------------
ResourceTree* GmatAppData::GetResourceTree()
{
   return theResourceTree;
}


//------------------------------------------------------------------------------
// wxConfigBase* GetPersonalizationConfig()
//------------------------------------------------------------------------------
wxConfigBase* GmatAppData::GetPersonalizationConfig()
{
   if (thePersonalizationConfig == NULL)
   {
	  std::string pfile = FileManager::Instance()->GetFullPathname(FileManager::PERSONALIZATION_FILE);
	  // fix crash for invalid filename
	  if (!FileManager::Instance()->DoesDirectoryExist(pfile))
	  {
		  MessageInterface::PopupMessage( Gmat::WARNING_, "Invalid personalization file specified: \n%s\n",
                                 pfile.c_str() );
		  pfile = "";
	  }
      thePersonalizationConfig = new wxFileConfig(wxEmptyString, wxEmptyString, pfile.c_str(),
              wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);
   }
   return thePersonalizationConfig;
}


//------------------------------------------------------------------------------
// void SetMissionTree(MissionTree *missionTree)
//------------------------------------------------------------------------------
void GmatAppData::SetMissionTree(MissionTree *missionTree)
{
   theMissionTree = missionTree;
}


//------------------------------------------------------------------------------
// MissionTree* GetMissionTree()
//------------------------------------------------------------------------------
MissionTree* GmatAppData::GetMissionTree()
{
   return theMissionTree;
}


//------------------------------------------------------------------------------
// void SetOutputTree(OutputTree *outputTree)
//------------------------------------------------------------------------------
void GmatAppData::SetOutputTree(OutputTree *outputTree)
{
   theOutputTree = outputTree;
}


//------------------------------------------------------------------------------
// OutputTree* GetOutputTree()
//------------------------------------------------------------------------------
OutputTree* GmatAppData::GetOutputTree()
{
   return theOutputTree;
}


//------------------------------------------------------------------------------
// void SetMessageWindow(ViewTextFrame *msgFrame)
//------------------------------------------------------------------------------
void GmatAppData::SetMessageWindow(ViewTextFrame *frame)
{
   theMessageWindow = frame;
}


//------------------------------------------------------------------------------
// ViewTextFrame* GetMessageWindow()
//------------------------------------------------------------------------------
ViewTextFrame* GmatAppData::GetMessageWindow()
{
   return theMessageWindow;
}


//------------------------------------------------------------------------------
// void SetCompareWindow(ViewTextFrame *frame)
//------------------------------------------------------------------------------
void GmatAppData::SetCompareWindow(ViewTextFrame *frame)
{
   theCompareWindow = frame;
}


//------------------------------------------------------------------------------
// ViewTextFrame* GetCompareWindow()
//------------------------------------------------------------------------------
ViewTextFrame* GmatAppData::GetCompareWindow()
{
   return theCompareWindow;
}


//------------------------------------------------------------------------------
// void SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
//------------------------------------------------------------------------------
void GmatAppData::SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
{
   theMessageTextCtrl = msgTextCtrl;
}


//------------------------------------------------------------------------------
// wxTextCtrl* GetMessageTextCtrl()
//------------------------------------------------------------------------------
wxTextCtrl* GmatAppData::GetMessageTextCtrl()
{
   return theMessageTextCtrl;
}


//------------------------------------------------------------------------------
// void SetFont(wxFont font)
//------------------------------------------------------------------------------
void GmatAppData::SetFont(wxFont font)
{
   theFont = font;
}


//------------------------------------------------------------------------------
// wxFont GetFont()
//------------------------------------------------------------------------------
wxFont GmatAppData::GetFont()
{
   return theFont;
}


//------------------------------------------------------------------------------
// void SetTempScriptName(const wxString &tempName)
//------------------------------------------------------------------------------
void GmatAppData::SetTempScriptName(const wxString &tempName)
{
   theTempScriptName = tempName;
}


//------------------------------------------------------------------------------
// wxString GetTempScriptName()
//------------------------------------------------------------------------------
wxString GmatAppData::GetTempScriptName()
{
   return theTempScriptName;
}


//------------------------------------------------------------------------------
// bool SetIcon(wxTopLevelWindow *topWindow, const std::string &calledFrom)
//------------------------------------------------------------------------------
/**
 * Sets icon to Dialog, MdiChildFrame, MdiParentFrame, etc
 *
 * @param  topWindow  The top level window of icon to be set
 *
 * @return true If icon was successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatAppData::SetIcon(wxTopLevelWindow *topWindow, const std::string &calledFrom)
{
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage
      ("GmatAppData::SetIcon() entered, called from '%s' named '%s'\n",
       calledFrom.c_str(), topWindow->GetName().c_str());
   #endif
   
   bool retval = true;
   
   if (theIconFile == "")
   {
      // Set icon if icon file is in the start up file
      FileManager *fm = FileManager::Instance();
      theIconFile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      #ifdef DEBUG_SET_ICON
      MessageInterface::ShowMessage("   theIconFile = '%s'\n", theIconFile.c_str());
      #endif
   }
   
   try
   {
      if (GmatFileUtil::DoesFileExist(theIconFile.c_str()))
      {
         #if defined __WXMSW__
            topWindow->SetIcon(wxIcon(theIconFile, wxBITMAP_TYPE_ICO));
         #elif defined __WXGTK__
            topWindow->SetIcon(wxIcon(theIconFile, wxBITMAP_TYPE_XPM));
         #elif defined __WXMAC__
            topWindow->SetIcon(wxIcon(theIconFile, wxBITMAP_TYPE_PICT_RESOURCE));
         #endif
      }
      else
      {
         if (theIconFile.Len() > 1)
         {
            if (theIconFile[0] == '.')
            {
               FileManager *fm = FileManager::Instance();
               // Used bin directory from the FileManager(for GMT-4408 LOJ: 2014.04.09)
               //wxString absIconDir = fm->GetWorkingDirectory().c_str();
               wxString absIconDir = fm->GetBinDirectory().c_str();
               wxString pathSep = fm->GetPathSeparator().c_str();
               wxString absIconFile = absIconDir + pathSep + theIconFile;
               MessageInterface::ShowMessage
                  ("*** WARNING *** The icon file \"%s\" does not exist for window '%s' named '%s', "
                   "so trying with abs path \"%s\"\n", theIconFile.c_str(), calledFrom.c_str(),
                   topWindow->GetName().c_str(), absIconFile.c_str());
               if (GmatFileUtil::DoesFileExist(absIconFile.c_str()))
               {
                  MessageInterface::ShowMessage
                     ("*** WARNING *** The icon file \"%s\" does not exist for window '%s' named '%s'\n",
                      absIconFile.c_str(), calledFrom.c_str(), topWindow->GetName().c_str());
                  retval = false;
               }
            }
         }
         else
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** The icon file \"%s\" does not exist for window '%s' named '%s'\n",
                theIconFile.c_str(), calledFrom.c_str(), topWindow->GetName().c_str());
            retval = false;
         }
      }
   }
   catch (GmatBaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** error setting icon for window '%s' named '%s'\n   The message is '%s'\n",
          calledFrom.c_str(), topWindow->GetName().c_str(), e.GetFullMessage().c_str());
      retval = false;
   }
   
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage("GmatAppData::SetIcon() returning %d\n", retval);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void SetIconFile()
//------------------------------------------------------------------------------
/**
 * Sets icon file from the GMAT startup file. This method needs to be called
 * after startup file is read.
 */
//------------------------------------------------------------------------------
void GmatAppData::SetIconFile()
{
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage("GmatAppData::SetIconFile() entered\n");
   #endif
   
   // Set icon file from the startup file
   FileManager *fm = FileManager::Instance();
   theIconFile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
   
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage
      ("GmatAppData::SetIconFile() leaving, theIconFile = '%s'\n", theIconFile.c_str());
   #endif
}

//------------------------------------------------------------------------------
// wxString GetIconFile()
//------------------------------------------------------------------------------
wxString GmatAppData::GetIconFile()
{
   return theIconFile;
}

//------------------------------------------------------------------------------
// GmatAppData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
GmatAppData::GmatAppData()
{
#if !defined __CONSOLE_APP__
   theMainFrame = NULL;
   theResourceTree = NULL;
   theMissionTree = NULL;
   theOutputTree = NULL;
   theMessageWindow = NULL;
   theCompareWindow = NULL;
   theMessageTextCtrl = NULL;
   theTempScriptName = "$gmattempscript$.script";
   thePersonalizationConfig = NULL;
   
   #ifdef __USE_EDITOR__
   thePageSetupDialogData = NULL;
   #endif
   
   // Set font
   theFont = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL);
   
   // Set the global wx config, read from local directory (GMAT.ini)
   wxFileConfig *pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, "GMAT.ini", 
           wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);
   wxConfigBase::Set(pConfig);
      
#endif
}


//------------------------------------------------------------------------------
// ~GmatAppData()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatAppData::~GmatAppData()
{
}

#endif

