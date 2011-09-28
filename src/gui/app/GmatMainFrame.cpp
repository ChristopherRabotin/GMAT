//$Id$
//------------------------------------------------------------------------------
//                              GmatMainFrame
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
// Modified: 2003/08/28 - Allison Greene
//         : Updated to include full menubar and splitter window.
//
//         : 2003/09/18 - Allison Greene
//         : Updated to include tool bar.
//
//         : 2003/10/03 - Allison Greene
//         : Updated to include tabs for right side of window and left side.
//    2010.03.16 Thomas Grubb 
//      - Modified code to use new GroundStationPanel class instead of default panel
//
/**
 * This class provides the main frame for GMAT.
 */
//------------------------------------------------------------------------------
#include <wx/fileconf.h>
#include "gmatwxrcs.hpp"
#include "GmatMainFrame.hpp"
#include "GmatAppData.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiTsPlotData.hpp"
#include "GroundStationPanel.hpp"
#include "GmatNotebook.hpp"
#include "GmatMenuBar.hpp"
#include "GmatToolBar.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatMdiChildFrame.hpp"
#include "MdiChildViewFrame.hpp"
// panels
#include "GmatBaseSetupPanel.hpp"
#include "SpacecraftPanel.hpp"
#include "TankConfigPanel.hpp"
#include "ThrusterConfigPanel.hpp"
#include "UniversePanel.hpp"
#include "PropagationConfigPanel.hpp"
#include "PropagatePanel.hpp"
#include "ImpulsiveBurnSetupPanel.hpp"
#include "FiniteBurnSetupPanel.hpp"
#include "DCSetupPanel.hpp"
#include "SQPSetupPanel.hpp"
#include "SolverSetupPanel.hpp"
#include "ManeuverPanel.hpp"
#include "BeginFiniteBurnPanel.hpp"
#include "EndFiniteBurnPanel.hpp"
#include "XyPlotSetupPanel.hpp"
#include "OrbitViewPanel.hpp"
#include "ReportFileSetupPanel.hpp"
#include "EphemerisFilePanel.hpp"
#include "SubscriberSetupPanel.hpp"
#include "MessageInterface.hpp"
#include "SolverGoalsPanel.hpp"
#include "SolverVariablesPanel.hpp"
#include "TargetPanel.hpp"
#include "OptimizePanel.hpp"
#include "AchievePanel.hpp"
#include "VaryPanel.hpp"
#include "MinimizePanel.hpp"
#include "NonlinearConstraintPanel.hpp"
#include "SavePanel.hpp"
#include "ReportPanel.hpp"
#include "TogglePanel.hpp"
#include "ParameterCreateDialog.hpp"
#include "ConditionPanel.hpp"
#include "ForPanel.hpp"
#include "DoWhilePanel.hpp"
#include "FormationSetupPanel.hpp"
#include "CallFunctionPanel.hpp"
#include "CoordSystemConfigPanel.hpp"
#include "FunctionSetupPanel.hpp"
#include "MatlabFunctionSetupPanel.hpp"
#include "AssignmentPanel.hpp"
#include "ScriptEventPanel.hpp"
#include "ScriptPanel.hpp"
#ifdef __USE_STC_EDITOR__
#include "EditorPanel.hpp"
#include "EditorPrintout.hpp"
#endif
#include "ReportFilePanel.hpp"
#include "BarycenterPanel.hpp"
#include "LibrationPointPanel.hpp"
#include "CelestialBodyPanel.hpp"
#include "CompareReportPanel.hpp"
#include "GmatCommandPanel.hpp"
#include "UndockedMissionPanel.hpp"
// dialogs
#include "CompareFilesDialog.hpp"
#include "CompareTextDialog.hpp"
#include "TextEphemFileDialog.hpp"
#include "AboutDialog.hpp"
#include "SetPathDialog.hpp"
#include "WelcomePanel.hpp"
// email
#ifdef __ENABLE_EMAIL__
#include "wx/net/msg.h"               // for wxMailMessage
#include "wx/net/email.h"             // for wxEmail
#endif

#include "FileManager.hpp"
#include "FileUtil.hpp"               // for Compare()
#include "GmatOpenGLSupport.hpp"      // for ScreenShotSave


#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/progdlg.h>
#include <wx/utils.h>     // for ::wxLaunchDefaultBrowser()
#include "ddesetup.hpp"   // for IPC_SERVICE, IPC_TOPIC


// If we want to show GL option dialog from tool bar
//#define __SHOW_GL_OPTION_DIALOG__

#ifdef __SHOW_GL_OPTION_DIALOG__
#include "bitmaps/animation_options.xpm"
#endif

#define __USE_CHILD_BEST_SIZE__

// If Sleep in not defined (on unix boxes)
#ifndef Sleep
#ifndef __WXMSW__
#include <unistd.h>
#define Sleep(t) usleep((t))
#endif
#endif


//#define DEBUG_MAINFRAME
//#define DEBUG_MAINFRAME_CLOSE
//#define DEBUG_MAINFRAME_SAVE
//#define DEBUG_FILE_COMPARE
//#define DEBUG_SERVER
//#define DEBUG_CREATE_CHILD
//#define DEBUG_CHILD_OPEN
//#define DEBUG_CHILD_WINDOW
//#define DEBUG_REMOVE_CHILD
//#define DEBUG_OPEN_SCRIPT
//#define DEBUG_REFRESH_SCRIPT
//#define DEBUG_INTERPRET
//#define DEBUG_RUN
//#define DEBUG_SIZE
//#define DBGLVL_MENUBAR 1
//#define DEBUG_PENDING_EVENTS
//#define DEBUG_DOCK_UNDOCK
//#define DEBUG_CONFIG_DATA

using namespace GmatMenu;

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatMainFrame, wxFrame)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatMainFrame, wxMDIParentFrame)
   EVT_MENU (MENU_EMPTY_PROJECT, GmatMainFrame::OnProjectNew)
   EVT_MENU (MENU_LOAD_DEFAULT_MISSION, GmatMainFrame::OnLoadDefaultMission)
   EVT_MENU (MENU_FILE_SAVE_SCRIPT, GmatMainFrame::OnSaveScript)
   EVT_MENU (MENU_FILE_SAVE_SCRIPT_AS, GmatMainFrame::OnSaveScriptAs)
   EVT_MENU (MENU_FILE_PRINT_SETUP, GmatMainFrame::OnPrintSetup)
   EVT_MENU (MENU_FILE_PRINT, GmatMainFrame::OnPrint)
   EVT_MENU (MENU_PROJECT_EXIT, GmatMainFrame::OnProjectExit)
   EVT_MENU (MENU_PREFERENCES_FONT, GmatMainFrame::OnFont)
   EVT_MENU (TOOL_RUN, GmatMainFrame::OnRun)
   EVT_MENU (TOOL_PAUSE, GmatMainFrame::OnPause)
   EVT_MENU (TOOL_STOP, GmatMainFrame::OnStop)
   EVT_MENU (TOOL_CLOSE_CHILDREN, GmatMainFrame::OnCloseAll)
   EVT_MENU (TOOL_CLOSE_CURRENT, GmatMainFrame::OnCloseActive)

   EVT_MENU (TOOL_SCREENSHOT, GmatMainFrame::OnScreenshot)

   EVT_MENU (MENU_HELP_ABOUT, GmatMainFrame::OnHelpAbout)
   EVT_MENU (MENU_HELP_WELCOME, GmatMainFrame::OnHelpWelcome)
   EVT_MENU (MENU_HELP_CONTENTS, GmatMainFrame::OnHelpContents)
   EVT_MENU (MENU_HELP_ONLINE, GmatMainFrame::OnHelpOnline)
   EVT_MENU (MENU_HELP_TUTORIAL, GmatMainFrame::OnHelpTutorial)
   EVT_MENU (MENU_HELP_FORUM, GmatMainFrame::OnHelpForum)
   EVT_MENU (MENU_HELP_ISSUE, GmatMainFrame::OnHelpIssue)
   EVT_MENU (MENU_HELP_FEEDBACK, GmatMainFrame::OnHelpFeedback)

   EVT_MENU (MENU_FILE_NEW_SCRIPT, GmatMainFrame::OnNewScript)
   EVT_MENU (MENU_FILE_OPEN_SCRIPT, GmatMainFrame::OnOpenScript)
   EVT_MENU (MENU_FILE_OPEN_RECENT_SCRIPT1, GmatMainFrame::OnOpenRecentScript1)
   EVT_MENU (MENU_FILE_OPEN_RECENT_SCRIPT2, GmatMainFrame::OnOpenRecentScript2)
   EVT_MENU (MENU_FILE_OPEN_RECENT_SCRIPT3, GmatMainFrame::OnOpenRecentScript3)
   EVT_MENU (MENU_FILE_OPEN_RECENT_SCRIPT4, GmatMainFrame::OnOpenRecentScript4)
   EVT_MENU (MENU_FILE_OPEN_RECENT_SCRIPT5, GmatMainFrame::OnOpenRecentScript5)

   EVT_MENU (MENU_SET_PATH_AND_LOG, GmatMainFrame::OnSetPath)

   EVT_MENU (MENU_EDIT_UNDO, GmatMainFrame::OnUndo)
   EVT_MENU (MENU_EDIT_REDO, GmatMainFrame::OnRedo)
   EVT_MENU (MENU_EDIT_COPY, GmatMainFrame::OnCopy)
   EVT_MENU (MENU_EDIT_CUT, GmatMainFrame::OnCut)
   EVT_MENU (MENU_EDIT_PASTE, GmatMainFrame::OnPaste)
   EVT_MENU (MENU_EDIT_COMMENT, GmatMainFrame::OnComment)
   EVT_MENU (MENU_EDIT_UNCOMMENT, GmatMainFrame::OnUncomment)
   EVT_MENU (MENU_EDIT_SELECT_ALL, GmatMainFrame::OnSelectAll)

   EVT_MENU (MENU_EDIT_FIND, GmatMainFrame::OnFind)
   EVT_MENU (MENU_EDIT_FIND_NEXT, GmatMainFrame::OnFindNext)
   EVT_MENU (MENU_EDIT_REPLACE, GmatMainFrame::OnReplace)
   EVT_MENU (MENU_EDIT_REPLACE_NEXT, GmatMainFrame::OnReplaceNext)
   EVT_MENU (MENU_EDIT_GOTO_LINE, GmatMainFrame::OnGoToLine)
   EVT_MENU (MENU_EDIT_LINE_NUMBER, GmatMainFrame::OnLineNumber)
   EVT_MENU (MENU_EDIT_INDENT_MORE, GmatMainFrame::OnIndentMore)
   EVT_MENU (MENU_EDIT_INDENT_LESS, GmatMainFrame::OnIndentLess)
   
   EVT_MENU (MENU_MATLAB_OPEN, GmatMainFrame::OnOpenMatlab)
   EVT_MENU (MENU_MATLAB_CLOSE, GmatMainFrame::OnCloseMatlab)
   EVT_MENU (MENU_MATLAB_SERVER_START, GmatMainFrame::OnMatlabServerStart)
   EVT_MENU (MENU_MATLAB_SERVER_STOP, GmatMainFrame::OnMatlabServerStop)
   
   EVT_MENU (MENU_TOOLS_FILE_COMPARE_NUMERIC, GmatMainFrame::OnFileCompareNumeric)
   EVT_MENU (MENU_TOOLS_FILE_COMPARE_TEXT, GmatMainFrame::OnFileCompareText)
   EVT_MENU (MENU_TOOLS_GEN_TEXT_EPHEM_FILE, GmatMainFrame::OnGenerateTextEphemFile)

   EVT_SASH_DRAGGED (ID_SASH_WINDOW, GmatMainFrame::OnSashDrag)
   EVT_SASH_DRAGGED (ID_MSG_SASH_WINDOW, GmatMainFrame::OnMsgSashDrag)

   EVT_SIZE (GmatMainFrame::OnMainFrameSize)
   EVT_CLOSE (GmatMainFrame::OnClose)
   EVT_SET_FOCUS (GmatMainFrame::OnSetFocus)
   EVT_KEY_DOWN (GmatMainFrame::OnKeyDown)
   
   EVT_MENU (MENU_SCRIPT_BUILD_OBJECT, GmatMainFrame::OnScriptBuildObject)
   EVT_MENU (MENU_SCRIPT_BUILD_AND_RUN, GmatMainFrame::OnScriptBuildAndRun)
   EVT_MENU (MENU_SCRIPT_RUN, GmatMainFrame::OnScriptRun)

   EVT_MENU_RANGE (TOOL_ANIMATION_PLAY, TOOL_ANIMATION_OPTIONS, GmatMainFrame::OnAnimation)

END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
//               long style)
//------------------------------------------------------------------------------
/**
 * Constructs GmatMainFrame object.

 *
 * @param <title> input title.
 * @param <pos> input position.
 * @param <size> input size.
 * @param <style> input style.
 *
 * @note Creates the menu bar, tool bar, status bar, splitter window, and notebooks
 *       for the right hand side and the left hand side.
 */
//------------------------------------------------------------------------------
GmatMainFrame::GmatMainFrame(wxWindow *parent,  const wxWindowID id,
                             const wxString& title, const wxPoint& pos,
                             const wxSize& size, long style)
   : wxMDIParentFrame(parent, id, title, pos, size, style)
{
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() this=<%p> entered\n", this);
   #endif

   theMessageWin = NULL;
   theMainWin = NULL;
   mWelcomePanel = NULL;

   // set the script name
   mTempScriptName = "$gmattempscript$.script";
   mScriptFilename = mTempScriptName;
   mAnimationFrameInc = 1;
   mAnimationEnabled = true;
   mInterpretFailed = false;
   mExitWithoutConfirm = false;
   mUndockedMissionTreePresized = false;
   mRunStatus = 0;
   
   viewSubframe = (MdiChildViewFrame *)NULL;
   tsSubframe = (MdiChildTsFrame *)NULL;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   theGuiInterpreter = gmatAppData->GetGuiInterpreter();
   gmatAppData->SetTempScriptName(mTempScriptName.c_str());

   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::GmatMainFrame() theGuiInterpreter=%p\n", theGuiInterpreter);
   #endif

   //-----------------------------------------------------------------
   // Create menu bar
   //-----------------------------------------------------------------
#if wxUSE_MENUS
   // create a menu bar
   // pass Window menu if Windows
   #ifdef __WXMSW__
   theMenuBar = new GmatMenuBar(GmatTree::UNKNOWN_ITEM, GetWindowMenu());
   #else
   theMenuBar = new GmatMenuBar(GmatTree::UNKNOWN_ITEM, NULL);
   #endif

   #if DBGLVL_MENUBAR
   MessageInterface::ShowMessage
      ("GmatMainFrame::GmatMainFrame() theMenuBar created: %p\n", theMenuBar);
   #endif

   SetMenuBar(theMenuBar);

   // Disble Edit menu, Edit menu will be enable in GmatMdiChildFrame if
   // ItemType is GmatTree::SCRIPT_FILE
   int editIndex = theMenuBar->FindMenu("Edit");
   theMenuBar->EnableTop(editIndex, false);

#endif // wxUSE_MENUS

   //-----------------------------------------------------------------
   // Create status bar
   //-----------------------------------------------------------------
#if wxUSE_STATUSBAR
   // create a status bar
   // There is a problem with status bar field 0 being blank out when
   // mouse is moved around the tool bar, so make the number of field
   // to 4 and start from field 1 (LOJ: 2009.02.12)
   theStatusBar = CreateStatusBar(4, wxBORDER);
   int widths[] = {1, 150, -3, -1};
   SetStatusWidths(4, widths);
   SetStatusText(_T("Welcome to GMAT!"), 1);
#endif // wxUSE_STATUSBAR

   //-----------------------------------------------------------------
   // Create tool bar
   //-----------------------------------------------------------------
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::GmatMainFrame() creating ToolBar...\n");
   #endif

   // Why I need to set wxTB_FLAT to show separators? (loj: 2008.11.14)
#ifdef __WXMAC__
//   theToolBar = new GmatToolBar(NULL, wxTB_FLAT);
//   theToolBar = new GmatToolBar(this);
   theToolBar = new GmatToolBar(this, wxTB_VERTICAL);
#else
   theToolBar = new GmatToolBar(this, wxTB_FLAT);
#endif
   SetToolBar(theToolBar);

   // used to store the list of open children
   theMdiChildren = new wxList();
   
   int w, h;
   GetClientSize(&w, &h);

   #ifdef DEBUG_SIZE
   MessageInterface::ShowMessage
      ("Before creating wxSashLayoutWindow\n   client size w=%d, h=%d\n", w, h);
   int winW, winH;
   GetSize(&winW, &winH);
   MessageInterface::ShowMessage("   window size w=%d, h=%d\n", winW, winH);
   #endif

   // A window w/sash for messages
   theMessageWin = new wxSashLayoutWindow(this, ID_MSG_SASH_WINDOW,
                           wxDefaultPosition, wxSize(30, 200),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
#ifdef __WXMAC__
   theMessageWin->SetDefaultSize(wxSize(w, (int) (0.25 * h)));
#else
   theMessageWin->SetDefaultSize(wxSize(w, 100));
#endif

   theMessageWin->SetMinimumSizeY(20);
   theMessageWin->SetMaximumSizeY(h-20);
   theMessageWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   theMessageWin->SetAlignment(wxLAYOUT_BOTTOM);
   theMessageWin->SetSashVisible(wxSASH_TOP, TRUE);

   // create MessageWindow TextCtrl
   // Set additional style wxTE_READONLY and wxTE_RICH to Ctrl + mouse scroll
   // wheel to decrease or increase text size(loj: 2009.02.05)
   wxTextCtrl *msgTextCtrl =
      new wxTextCtrl(theMessageWin, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                     wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH);

   msgTextCtrl->SetMaxLength(320000);
   // Added SetFocus() to automatically show the last line. (LOJ: 2009.03.04)
   // This was needed since msgTextCtrl changed to read-only.
   msgTextCtrl->SetFocus();
   gmatAppData->SetMessageTextCtrl(msgTextCtrl);

   //-----------------------------------------------------------------
   #ifdef __USE_FLOATING_MESSAGE_WINDOW__
   // @todo: Need more testing on this (loj: 2008.04.10)
   // create floating MessageWindow
   ViewTextFrame *msgWin =
      //new ViewTextFrame((wxFrame *)NULL, _T("Message Window"),
      new ViewTextFrame(this, _T("Message Window"),
                        20, 20, 600, 350, "Permanent");
   gmatAppData->SetMessageWindow(msgWin);
   msgWin->Show(true);

   // create MessageWindow TextCtrl
   // Set additional style wxTE_READONLY and wxTE_RICH to Ctrl + mouse scroll
   // wheel to decrease or increase text size on Windows(loj: 2009.02.05)
   wxTextCtrl *floatingMsgTextCtrl =
      new wxTextCtrl(msgWin, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                     wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

   msgTextCtrl->SetMaxLength(320000);
   gmatAppData->SetMessageTextCtrl(floatingMsgTextCtrl);
   #endif
   //-----------------------------------------------------------------

   // Set font style
   // Commented out because it showed in color other than black when
   // style is wxTE_RICH (loj: 2009.02.05)
   //msgTextCtrl->SetDefaultStyle(wxTextAttr(wxTELETYPE));

   // A window w/sash for gmat notebook
   theMainWin = new wxSashLayoutWindow(this, ID_SASH_WINDOW,
                           wxDefaultPosition, wxSize(200, 30),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

#ifdef __WXMAC__
   //theMainWinSetDefaultSize(wxSize(275, h));
   theMainWin->SetDefaultSize(wxSize(w, h));
#else
   //theMainWin->SetDefaultSize(wxSize(200, h));
   // Try 220 since Mission page has one more panel to show (LOJ: 2011.08.02)
   theMainWin->SetDefaultSize(wxSize(220, h));
   // 200 is too narrow for most linux themes
   #ifdef __LINUX__
      //theMainWin->SetDefaultSize(wxSize(220, h));
      theMainWin->SetDefaultSize(wxSize(240, h));
   #endif
#endif
   theMainWin->SetMinimumSizeX(20);
   theMainWin->SetMaximumSizeX(w-20);
   theMainWin->SetOrientation(wxLAYOUT_VERTICAL);
   theMainWin->SetAlignment(wxLAYOUT_LEFT);
   theMainWin->SetSashVisible(wxSASH_RIGHT, TRUE);
   
   //-----------------------------------------------------------------
   // Create Resource, Mission, and Output notebook tabs
   //-----------------------------------------------------------------
   theNotebook = new GmatNotebook(theMainWin, -1, wxDefaultPosition,
                                  wxDefaultSize, wxCLIP_CHILDREN);
   
   // Set the main frame, because there will no longer be right notebook
   gmatAppData->SetMainFrame(this);
   gmatAppData->GetResourceTree()->SetMainFrame(this);
   gmatAppData->GetMissionTree()->SetMainFrame(this);
   
   mMatlabServer = NULL;
   mRunPaused = false;
   mRunCompleted = true;
   
   // Set icon if icon file is in the start up file
   FileManager *fm = FileManager::Instance();
   try
   {
      wxString iconfile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      if (GmatFileUtil::DoesFileExist(iconfile.c_str()))
      {
         #if defined __WXMSW__
            SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_ICO));
         #elif defined __WXGTK__
            SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_XPM));
         #elif defined __WXMAC__
            SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_PICT_RESOURCE));
         #endif
      }
   }
   catch (GmatBaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
   
   // If GUI mode is normal (not minimized), show welcome panel
   if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::NORMAL_GUI)
   {
      // show welcome panel
      wxConfigBase *pConfig = GmatAppData::Instance()->GetPersonalizationConfig();
      wxCommandEvent WXUNUSED; 
      wxString showWelcomePanel;
      pConfig->Read("/Main/ShowWelcomeOnStart", &showWelcomePanel, "true");
      if (showWelcomePanel.Lower() == "true")
         OnHelpWelcome(WXUNUSED);
   }
   
   // Create HelpController from CHM file
   std::string helpFileName = fm->GetFullPathname("HELP_FILE");
   #ifdef __WXMSW__
   theHelpController = new wxCHMHelpController;
   theHelpController->Initialize(helpFileName);
   #else
   #endif
   
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() this=<%p> exiting\n", this);
   #endif
}


//------------------------------------------------------------------------------
// ~GmatMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame::~GmatMainFrame()
{
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage
      ("GmatMainFrame::~GmatMainFrame() entered. mMatlabServer=%p, theGuiInterpreter=%p\n",
       mMatlabServer, theGuiInterpreter);
   #endif
   
   theGuiInterpreter->CloseMatlabEngine();
   
   if (mMatlabServer)
      delete mMatlabServer;
   
   GmatAppData *gmatAppData = GmatAppData::Instance();

   wxConfigBase *pConfig = gmatAppData->GetPersonalizationConfig();
   pConfig->Flush(true);

   if (gmatAppData->GetMessageWindow() != NULL)
      gmatAppData->GetMessageWindow()->Close();
   
   gmatAppData->SetMainFrame(NULL);
   gmatAppData->SetMessageWindow(NULL);
   gmatAppData->SetMessageTextCtrl(NULL);
   gmatAppData->SetResourceTree(NULL);
   gmatAppData->SetMissionTree(NULL);
   gmatAppData->SetOutputTree(NULL);
   
   // Delete Help file
   #ifdef __WXMSW__
   theHelpController->Quit();
   delete theHelpController;
   #else
   #endif
   
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage("GmatMainFrame::~GmatMainFrame() exiting.\n");
   #endif
}


//------------------------------------------------------------------------------
// bool GmatMainFrame::Show(bool show)
//------------------------------------------------------------------------------
/**
 * Provides a connection point for updating panels owned by GMAT's MainFrame
 * after the MainFrame is displayed.
 *
 * @param show Indicates in the frame should be displayed
 *
 * @return The success or failure of the MainFrame display.
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::Show(bool show)
{
   bool retval = wxMDIParentFrame::Show(show);
   if (retval)
      // Only show the Welcome panel is it exists and if the MainFrame is shown
      if ((mWelcomePanel != NULL) && show)
         mWelcomePanel->Raise();

   return retval;
}


//------------------------------------------------------------------------------
// void GetActualClientSize(Integer *w, Integer *h, bool ignoreMissionTree)
//------------------------------------------------------------------------------
/**
 * Returns actual client size that plots can be positioned without offset
 */
//------------------------------------------------------------------------------
void GmatMainFrame::GetActualClientSize(Integer *w, Integer *h, bool ignoreMissionTree)
{
   Integer toolW, toolH;
   Integer msgSashWinW, msgSashWinH;
   Integer mainSashWinW, mainSashWinH;
   Integer mainClientWinW, mainClientWinH;
   Integer mtWinX, mtWinY, mtWinW;
   Integer actW, actH;
   
   theToolBar->GetSize(&toolW, &toolH);
   theMessageWin->GetSize(&msgSashWinW, &msgSashWinH);
   theMainWin->GetSize(&mainSashWinW, &mainSashWinH);
   GetClientSize(&mainClientWinW, &mainClientWinH);
   
   #ifdef DEBUG_ACTUAL_CLIENT_SIZE
   MessageInterface::ShowMessage("        ToolBar: w = %4d, h = %4d\n", toolW, toolH);
   MessageInterface::ShowMessage("     MessageWin: w = %4d, h = %4d\n", msgSashWinW, msgSashWinH);
   MessageInterface::ShowMessage("    MainSashWin: w = %4d, h = %4d\n", mainSashWinW, mainSashWinH);
   MessageInterface::ShowMessage("MainFrameClient: w = %4d, h = %4d\n", mainClientWinW, mainClientWinH);
   #endif
   
   actW = mainClientWinW;
   actH = mainSashWinH;
   
   if (!ignoreMissionTree)
      if (IsMissionTreeUndocked(mtWinX, mtWinY, mtWinW))
         actW -= mtWinW;
   
   #ifdef DEBUG_ACTUAL_CLIENT_SIZE
   MessageInterface::ShowMessage("     ActualSize: w = %4d, h = %4d\n", actW, actH);
   #endif
   
   *w = actW;
   *h = actH;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateChild(GmatTreeItemData *item, bool restore)
//------------------------------------------------------------------------------
/**
 * Adds a page to notebook based on tree item type.
 *
 * @param <item> input GmatTreeItemData.
 * @param <restore> if true the child will be restored if minimized
 *
 * @return created child or NULL If child is already open
 */
//------------------------------------------------------------------------------
GmatMdiChildFrame* GmatMainFrame::CreateChild(GmatTreeItemData *item,
                                              bool restore)
{
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateChild() this=<%p>, title='%s', name='%s', type=%d, "
       "restore=%d\n", this, item->GetTitle().c_str(), item->GetName().c_str(),
       item->GetItemType(), restore);
   #endif
   
   GmatMdiChildFrame *newChild = NULL;
   
   // if child already open, just return
   if (IsChildOpen(item, restore))
      return NULL;
   
   GmatTree::ItemType itemType = item->GetItemType();
   
   //----------------------------------------------------------------------
   // create a mdi child
   // Note: Do not change the order of ItemType in GmatTreeItemData.hpp.
   // The wrong order of itemType will not work properly.
   //----------------------------------------------------------------------
   if (itemType >= GmatTree::BEGIN_OF_RESOURCE &&
       itemType <= GmatTree::END_OF_RESOURCE)
   {
      // Added check for SCRIPT_FILE when showing error message(LOJ: 2009.03.04)
      if (item->GetTitle() == "")
      {
         if (itemType == GmatTree::SCRIPT_FILE)
         {
            item->SetTitle(item->GetName());
         }
         else
         {
            wxString name = item->GetName();
            GmatBase *obj = theGuiInterpreter->GetConfiguredObject(name.c_str());
            if (obj == NULL)
            {
               MessageInterface::ShowMessage
                  ("**** ERROR **** Cannot find object named '%s' in "
                   "GmatMainFrame::CreateChild\n", item->GetName().c_str());
               return NULL;
            }
            
            // Append object type name to title (loj: 2009.01.28)
            wxString objType = (obj->GetTypeName()).c_str();
            wxString newTitle = objType + " - " + name;
            item->SetTitle(newTitle);
         }
      }
      
      newChild = CreateNewResource(item->GetTitle(), item->GetName(), itemType);
   }
   else if (itemType >= GmatTree::BEGIN_OF_COMMAND &&
            itemType <= GmatTree::END_OF_COMMAND)
   {
      newChild = CreateNewCommand(itemType, item);
   }
   else if (itemType >= GmatTree::BEGIN_OF_CONTROL &&
            itemType <= GmatTree::END_OF_CONTROL)
   {
      newChild = CreateNewControl(item->GetTitle(), item->GetName(), itemType,
                                  item->GetCommand());
   }
   else if (itemType >= GmatTree::BEGIN_OF_OUTPUT &&
            itemType <= GmatTree::END_OF_OUTPUT)
   {
      // Create panel if Report or Compare Report
      if (itemType == GmatTree::OUTPUT_REPORT ||
          itemType == GmatTree::COMPARE_REPORT)
         newChild = CreateNewOutput(item->GetTitle(), item->GetName(), itemType);
   }
   else if (itemType == GmatTree::MISSION_TREE_UNDOCKED)
   {
      newChild = CreateUndockedMissionPanel(item->GetTitle(), item->GetName(), itemType);
   }
   else
   {
      // do nothing
      #ifdef DEBUG_CREATE_CHILD
      MessageInterface::ShowMessage
         ("GmatMainFrame::CreateChild() Invalid item=%s itemType=%d entered\n",
          item->GetTitle().c_str(), itemType);
      #endif
   }
   
   // Reposition mdi child windows (LOJ: 2011.02.01 Bug 2320 fix)
   if (newChild != NULL)
   {
      int numChildren = GetNumberOfChildOpen(false, true, true);
      #ifdef DEBUG_CHILD_WINDOW
      MessageInterface::ShowMessage
         ("   numChildren = %d, mUndockedMissionTreePresized = %d\n", numChildren,
          mUndockedMissionTreePresized);
      #endif
      
      #ifdef __WXMAC__
      if ((numChildren > 0) && !((newChild->GetItemType() == GmatTree::MISSION_TREE_UNDOCKED) && mUndockedMissionTreePresized) &&
          !((newChild->GetItemType() == GmatTree::OUTPUT_REPORT) && newChild->GetSavedConfigFlag()))
      #else
      if (numChildren > 0)
      #endif
      {
         #ifdef DEBUG_CHILD_WINDOW
         MessageInterface::ShowMessage
            ("Now repositioning the Mdi child window(s) ...................\n");
         #endif
         int toolW, toolH;
         theToolBar->GetSize(&toolW, &toolH);         
         int x = (numChildren - 1) * 20;         
         #ifdef __WXMAC__
            // reposition vertical position of first panel for Mac, so top button bar is visible
            int y = (numChildren) * 20;
            newChild->SetPosition(wxPoint(x, y));
         #else
            int clientX, clientY, clientW = -1, clientH = -1;
            GetActualClientSize(&clientW, &clientH, true);
            #ifdef DEBUG_CHILD_WINDOW
            MessageInterface::ShowMessage
               ("client window: clientW=%d, clientH=%d\n", clientW, clientH);
            #endif
            
            int y = x - toolH;
            
            // Why it doesn't position first child (0,0) to top left corner?
            // Put UndockedMissionPanel always in the top left corner
            if (newChild->GetItemType() == GmatTree::MISSION_TREE_UNDOCKED)
            {
               int mtX, mtY, mtW, mtH;
               newChild->GetPosition(&mtX, &mtY);
               newChild->GetSize(&mtW, &mtH);
               #ifdef DEBUG_CHILD_WINDOW
               MessageInterface::ShowMessage
                  ("MissionTree window: W = %d, H = %d\n", mtW, mtH);
               #endif
               
               if (!mUndockedMissionTreePresized)
               {
                  // For default size, we need to resize this child depends on
                  // the command length. Set to full mdi height for now.
                  // We don't want to hide iconized script, so reduce the height.
                  x = 0;
                  y = -toolH;
                  int height = clientH;
                  // We don't want to hide iconized children at the bottom, so reduce the height by 40.
                  height = clientH - 40;
                  newChild->SetSize(-1, height);
                  
                  // Reposition other children
                  if (numChildren > 1)
                     RepositionChildren(mtW + mtX);
               }
               else
               {
                  // Reposition other children
                  if (numChildren > 1)
                     RepositionChildren(mtW + mtX);
              }
            }
            else
            {
               // If Mission panel is opened and it is at the left,
               // place child besides it
               Integer mtX, mtY, mtW;
               if (IsMissionTreeUndocked(mtX, mtY, mtW))
               {
                  if (mtX < 20)
                  {
                     if (numChildren > 1)
                     {
                        x = (numChildren - 2) * 20 + mtW + 20;
                        y = (numChildren - 2) * 20 - toolH;
                     }
                     newChild->SetPosition(wxPoint(x, y));
                  }
               }
            }
         #endif
      }
      newChild->Show();
   }
   
   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* GetChild(const wxString &name)
//------------------------------------------------------------------------------
/**
 * Retrieves mdi child frame by name
 */
//------------------------------------------------------------------------------
GmatMdiChildFrame* GmatMainFrame::GetChild(const wxString &name)
{
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::GetChild() name=%s\n", name.c_str());
   #endif

   GmatMdiChildFrame *theChild = NULL;
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      theChild = (GmatMdiChildFrame *)node->GetData();

      #ifdef DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("   theChild=%s\n", theChild->GetName().c_str());
      #endif

      if (theChild->GetName().IsSameAs(name))
      {
         return theChild;
      }
      node = node->GetNext();
   }

   return NULL;
}


//------------------------------------------------------------------------------
// Integer GetNumberOfChildOpen(bool scriptsOnly = false, bool incPlots = false,
//            bool incScripts = false)
//------------------------------------------------------------------------------
/**
 * Returns number of children opened.
 *
 * @param  scriptsOnly  Set this to true if only scripts to be included in counting
 * @param  incPlots     Set this to true if plotcs to be included in counting
 * @param  incScripts   Set this to true if scripts to be included in counting
 */
//------------------------------------------------------------------------------
Integer GmatMainFrame::GetNumberOfChildOpen(bool scriptsOnly, bool incPlots,
                                            bool incScripts)
{
   #ifdef DEBUG_MAINFRAME_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::GetNumberOfChildOpen() scriptsOnly=%d, incPlots=%d, incScripts=%d\n",
       scriptsOnly, incPlots, incScripts);
   #endif
   
   Integer openCount = 0;
   Integer scriptOpenCount = 0;
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      GmatTree::ItemType itemType = theChild->GetItemType();

      #ifdef DEBUG_MAINFRAME_CHILD
      MessageInterface::ShowMessage
         ("   itemType=%d, title=%s\n", itemType, theChild->GetName().c_str());
      #endif
      
      if (itemType == GmatTree::SCRIPT_FILE)
      {
         scriptOpenCount++;
         if (incScripts)
            openCount++;
      }
      else if (itemType >= GmatTree::BEGIN_OF_OUTPUT && itemType <= GmatTree::END_OF_OUTPUT)
      {
         if (incPlots)
            openCount++;
      }
      else
      {
         openCount++;
      }
      
      node = node->GetNext();
      
   }
   
   #ifdef DEBUG_MAINFRAME_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::GetNumberOfChildOpen() returning %d\n", openCount);
   #endif
   
   if (scriptsOnly)
      return scriptOpenCount;
   else
      return openCount;
}


//------------------------------------------------------------------------------
// Integer GetNumberOfActivePlots()
//------------------------------------------------------------------------------
/**
 * Returns total number of plots showing in the mission.
 */
//------------------------------------------------------------------------------
Integer GmatMainFrame::GetNumberOfActivePlots()
{
   return theGuiInterpreter->GetNumberOfActivePlots();
}


//------------------------------------------------------------------------------
// bool IsMissionTreeUndocked(Integer &x, Integer &y, Integer &width)
//------------------------------------------------------------------------------
/**
 * @param x  The upper left x position of undocked mission tree returned
 * @param y  The upper left y position of undocked mission tree returned
 * @param width  The width of undocked mission tree returned
 * @return  true if MissionTree is undocked, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::IsMissionTreeUndocked(Integer &xPos, Integer &yPos, Integer &width)
{
   int x = 0, y = 0, h = 0, w = 0;
   xPos = 0, yPos = 0, width = 0;
   
   GmatMdiChildFrame *child = GetChild("Mission");
   if (child != NULL)
   {
      if (child->GetItemType() == GmatTree::MISSION_TREE_UNDOCKED)
      {
         child->GetPosition(&x, &y);
         child->GetSize(&w, &h);
         xPos = x;
         yPos = y;
         width = w;
         #ifdef DEBUG_DOCK_UNDOCK
            MessageInterface::ShowMessage("IsMissionTreeUndocked: returning true\n");
         #endif
         return true;
      }
      #ifdef DEBUG_DOCK_UNDOCK
         else
         {
            MessageInterface::ShowMessage("IsMissionTreeUndocked: child is not of type MISSION_TREE_UNDOCKED\n");
         }
      #endif
   }
   #ifdef DEBUG_DOCK_UNDOCK
      else
      {
         MessageInterface::ShowMessage("IsMissionTreeUndocked: child == NULL\n");
      }
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// void IconizeUndockedMissionTree()
//------------------------------------------------------------------------------
/**
 * Iconizes undocked mission tree if opened
 */
//------------------------------------------------------------------------------
void GmatMainFrame::IconizeUndockedMissionTree()
{
   GmatMdiChildFrame *child = GetChild("Mission");
   if (child != NULL)
      child->Iconize();
}


//------------------------------------------------------------------------------
// bool IsChildOpen(GmatTreeItemData *item, bool restore)
//------------------------------------------------------------------------------
/**
 * Determines if page should be opened.  If the page is already opened, sets that
 * page as the selected page.
 *
 * @param <item> input GmatTreeItemData.
 * @param <restore> if true the child will be restored if it was minimized
 *
 * @return true if child was already opened, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::IsChildOpen(GmatTreeItemData *item, bool restore)
{
   #ifdef DEBUG_CHILD_OPEN
   MessageInterface::ShowMessage
      ("GmatMainFrame::IsChildOpen() this=<%p> entered, title='%s'\n   name='%s'\n   "
       "theMdiChildren=<%p>\n", this, item->GetTitle().c_str(), item->GetName().c_str(),
       theMdiChildren);
   #endif

   wxString itemName = item->GetName();
   GmatTree::ItemType itemType = item->GetItemType();

   return IsChildOpen(itemName, itemType);
   
   
   #if 0
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      
      #ifdef DEBUG_CHILD_OPEN
      MessageInterface::ShowMessage
         ("   child title='%s'\n   child name='%s'\n",
          theChild->GetTitle().c_str(), item->GetName().c_str());
      #endif
      
      bool isChildAlreadyOpen = false;
      // If item is script file, compare the name which is script path and name
      if(item->GetItemType() == GmatTree::SCRIPT_FILE)
      {
         if (GmatFileUtil::IsSameFileName(item->GetName().c_str(),
                                          theChild->GetTitle().c_str()))
            isChildAlreadyOpen = true;
      }
      else if ((theChild->GetName().IsSameAs(item->GetName().c_str())) &&
               (theChild->GetItemType() == item->GetItemType()))
      {
         isChildAlreadyOpen = true;
      }
      
      if (isChildAlreadyOpen)
      {
         // move child to the front
         if (restore)
         {
            theChild->Activate();
            theChild->Restore();
         }
         
         #ifdef DEBUG_CHILD_OPEN
         MessageInterface::ShowMessage("GmatMainFrame::IsChildOpen() returning true\n");
         #endif
         return TRUE;
      }
      
      node = node->GetNext();
   }
   
   #ifdef DEBUG_CHILD_OPEN
   MessageInterface::ShowMessage("GmatMainFrame::IsChildOpen() returning false\n");
   #endif
   return false;
   #endif
}


//------------------------------------------------------------------------------
// bool IsChildOpen(const wxString &itemName, GmatTree::ItemType itemType, ...)
//------------------------------------------------------------------------------
/**
 * Determines if page should be opened.  If the page is already opened, sets that
 * page as the selected page.
 *
 * @param <itemName> The item name to use for checking
 * @param <itemType> The item type to use for checking
 * @param <restore> if true the child will be restored if it was minimized
 *
 * @return true if child was already opened, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::IsChildOpen(const wxString &itemName, GmatTree::ItemType itemType,
                                bool restore)
{
   #ifdef DEBUG_CHILD_OPEN
   MessageInterface::ShowMessage
      ("GmatMainFrame::IsChildOpen() this=<%p> entered, itemName='%s', "
       "itemType=%d\n", this, itemName.c_str(), itemType);
   #endif
   
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      
      #ifdef DEBUG_CHILD_OPEN
      MessageInterface::ShowMessage
         ("   child title='%s'\n   child name='%s'\n",
          theChild->GetTitle().c_str(), item->GetName().c_str());
      #endif
      
      bool isChildAlreadyOpen = false;
      // If item is script file, compare the name which is script path and name
      if(itemType == GmatTree::SCRIPT_FILE)
      {
         if (GmatFileUtil::IsSameFileName(itemName.c_str(),
                                          theChild->GetTitle().c_str()))
            isChildAlreadyOpen = true;
      }
      else if ((theChild->GetName().IsSameAs(itemName.c_str())) &&
               (theChild->GetItemType() == itemType))
      {
         isChildAlreadyOpen = true;
      }
      
      if (isChildAlreadyOpen)
      {
         // move child to the front
         if (restore)
         {
            theChild->Activate();
            theChild->Restore();
         }
         
         #ifdef DEBUG_CHILD_OPEN
         MessageInterface::ShowMessage("GmatMainFrame::IsChildOpen() returning true\n");
         #endif
         return true;
      }
      
      node = node->GetNext();
   }
   
   #ifdef DEBUG_CHILD_OPEN
   MessageInterface::ShowMessage("GmatMainFrame::IsChildOpen() returning false\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// bool RenameChild(GmatTreeItemData *item, wxString newName)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::RenameChild(GmatTreeItemData *item, wxString newName)
{
   wxString oldName = item->GetName();
   return RenameChild(oldName, newName);
}


//------------------------------------------------------------------------------
// bool RenameChild(const wxString &oldName, const wxString &newName)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::RenameChild(const wxString &oldName, const wxString &newName)
{
   GmatMdiChildFrame *theChild = GetChild(oldName);

   if (theChild != NULL)
   {
      if (theChild->GetName().IsSameAs(oldName))
      {
         theChild->SetTitle(newName);
         return TRUE;
      }
   }

   return FALSE;
}


//------------------------------------------------------------------------------
// bool RenameActiveChild(const wxString &newName)
//------------------------------------------------------------------------------
/**
 * Gives active child a new name.
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::RenameActiveChild(const wxString &newName)
{
    GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)GetActiveChild();

    if (theChild != NULL)
    {
       theChild->SetTitle(newName);
       theChild->SetName(newName);
       return TRUE;
    }

    return FALSE;
}


//------------------------------------------------------------------------------
// bool RemoveChild(const wxString &name, GmatTree::ItemType itemType,
//                  bool deleteChild = true)
//------------------------------------------------------------------------------
/*
 * Removes and deletes child frame from the list.
 *
 * @param <name> Name of the child frame
 * @param <itemType> Item type of the child frame
 * @param <deleteChild> Set to true if child frame is to be deleted
 *                      This flag is set to false if plot frame is deleted by
 *                      clicking X in the upper right corner.
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::RemoveChild(const wxString &name, GmatTree::ItemType itemType,
                                bool deleteChild)
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::RemoveChild() name=%s, itemType=%d, deleteChild=%d\n",
       name.c_str(), itemType, deleteChild);
   #endif

   wxNode *node = theMdiChildren->GetFirst();
   bool childRemoved = false;
   wxString childName, childTitle;
   GmatTree::ItemType childItemType;
   GmatAppData *gmatAppData = GmatAppData::Instance();

   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();

      childName = child->GetName();
      childTitle = child->GetTitle();
      childItemType = child->GetItemType();
      
      #ifdef DEBUG_REMOVE_CHILD_DETAIL
      MessageInterface::ShowMessage
         ("   childName='%s', childTitle='%s', childItemType=%d\n",
          childName.c_str(), childTitle.c_str(), childItemType);
      #endif
      
      if ((childItemType == itemType) && (childName.IsSameAs(name)))
      {
         //------------------------------------------------------
         // Notes:
         // OrbitView and XYPlot is added to theMdiChildren list
         // in this main frame and to its own list of
         // MdiGlPlot::mdiChildren and MdiTsPlot::mdiChildren.
         // These lists are used in the PlotInterface.
         // The count is decremented and object is deleted in the
         // destructors.
         //------------------------------------------------------

         #ifdef DEBUG_REMOVE_CHILD
         MessageInterface::ShowMessage
            ("   removing title: %s\n   name: %s, child=<%p>\n",
             childName.c_str(), name.c_str(), child);
         #endif
         
         // MdiChildViewFrame::OnPlotClose() and MdiChildTsrame::OnPlotClose()
         // set deleteChild to false
         if (deleteChild)
            delete child;

         delete node;
         childRemoved = true;
         break;
      }
      
      node = node->GetNext();
   }
      
   // If plot was removed, remove it from the OutputTree also
   if (childRemoved)
   {
      #ifdef DEBUG_REMOVE_CHILD
      MessageInterface::ShowMessage("   %s removed\n", name.c_str());
      #endif

      if (gmatAppData->GetOutputTree() != NULL)
      {
         #ifdef DEBUG_REMOVE_CHILD
         MessageInterface::ShowMessage("   calling GetOutputTree()->RemoveItem()\n");
         #endif

         gmatAppData->GetOutputTree()->RemoveItem(itemType, name);
      }

      // Change MissionTree node label (loj: 2007.11.15)
      gmatAppData->GetMissionTree()->ChangeNodeLabel(childName);
   }
   
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::RemoveChild() returning %d\n", childRemoved);
   #endif

   return childRemoved;
}


//------------------------------------------------------------------------------
// void RemoveOutputIfOpened(const wxString &name)
//------------------------------------------------------------------------------
/*
 * Removes the output child with given name from the list and deletes it
 * if it is opened.
 *
 */
//------------------------------------------------------------------------------
void GmatMainFrame::RemoveOutputIfOpened(const wxString &name)
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::RemoveOutputIfOpened() entered, name='%s'\n", name.c_str());
   #endif
   
   if (IsChildOpen(name, GmatTree::OUTPUT_XY_PLOT, false))
      RemoveChild(name, GmatTree::OUTPUT_XY_PLOT);
   else if (IsChildOpen(name, GmatTree::OUTPUT_ORBIT_VIEW, false))
      RemoveChild(name, GmatTree::OUTPUT_ORBIT_VIEW);
   else if (IsChildOpen(name, GmatTree::OUTPUT_GROUND_TRACK_PLOT, false))
      RemoveChild(name, GmatTree::OUTPUT_GROUND_TRACK_PLOT);
   else if (IsChildOpen(name, GmatTree::OUTPUT_REPORT, false))
      RemoveChild(name, GmatTree::OUTPUT_REPORT);
   
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::RemoveOutputIfOpened() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void CloseWelcomePanel(const wxString &name, GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
/*
 * Closes child frame of given item name and type.
 *
 * @param <name> Name of the child frame
 * @param <itemType> Item type of the child frame
 */
//------------------------------------------------------------------------------
void GmatMainFrame::CloseWelcomePanel()
{
   mWelcomePanel = NULL;
}

//------------------------------------------------------------------------------
// void CloseChild(const wxString &name, GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
/*
 * Closes child frame of given item name and type.
 *
 * @param <name> Name of the child frame
 * @param <itemType> Item type of the child frame
 */
//------------------------------------------------------------------------------
void GmatMainFrame::CloseChild(const wxString &name, GmatTree::ItemType itemType)
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CloseChild() name='%s', itemType=%d\n", name.c_str(),
       itemType);
   #endif

   wxNode *node = theMdiChildren->GetFirst();

   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();

      if ((child->GetName().IsSameAs(name.c_str())) &&
          (child->GetItemType() == itemType))
      {
         #ifdef DEBUG_REMOVE_CHILD
         MessageInterface::ShowMessage("   About to close '%s'\n", name.c_str());
         #endif
         wxCloseEvent event;
         child->OnClose(event);
         break;
      }

      node = node->GetNext();
   }

   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage("GmatMainFrame::CloseChild() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void CloseChild(GmatMdiChildFrame *child)
//------------------------------------------------------------------------------
/*
 * Closes child.
 *
 * @param <child> The child frame pointer
 */
//------------------------------------------------------------------------------
void GmatMainFrame::CloseChild(GmatMdiChildFrame *child)
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage("GmatMainFrame::CloseChild() child=%p\n", child);
   #endif

   if (child != NULL)
   {
      // Note: child->Close() will not process OnClose() correctly
      // so use OnClose(event) instead
      wxCloseEvent event;
      child->OnClose(event);
      wxSafeYield();
   }
}


//------------------------------------------------------------------------------
// void CloseActiveChild()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseActiveChild()
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage("GmatMainFrame::CloseActiveChild() entered\n");
   #endif

   GmatMdiChildFrame *child = (GmatMdiChildFrame *)GetActiveChild();
   CloseChild(child);
}


//------------------------------------------------------------------------------
// bool CloseAllChildren(bool closeScriptWindow = true, bool closePlots = true,
//                       bool closeReports = true, bool closeUndockedMissionTree = true)
//------------------------------------------------------------------------------
/*
 * Closes all mdi children frames.
 *
 * @param <closeScriptWindow> true to close script window (true)
 * @param <closePlots> true to close all plot windows (true)
 * @param <closeReports> true to close all reports from Output tab (true)
 * @return true if all frames is closed false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::CloseAllChildren(bool closeScriptWindow, bool closePlots,
                                     bool closeReports, bool closeUndockedMissionTree)
{
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage
      ("GmatMainFrame::CloseAllChildren() closeScriptWindow=%d, closePlots=%d, "
       "closeReports=%d, closeUndockedMissionTree=%d\n", closeScriptWindow, closePlots,
       closeReports, closeUndockedMissionTree );
   MessageInterface::ShowMessage
      ("   Number of children = %d\n", theMdiChildren->GetCount());
   #endif

   // Make sure the Mission Tree data is set in the personalization file
   if (closeUndockedMissionTree)
   {
      wxFileConfig *pConfig;
      pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
      Integer x = 0, y = 0, width = 0;
      if (IsMissionTreeUndocked(x, y, width))
      {
         pConfig->Write("/MissionTree/Docked", "false");
//         SaveConfigurationData("MissionTree");
      }
      else
         pConfig->Write("/MissionTree/Docked", "true");
   }
   wxString name;
   wxString title;
   GmatTree::ItemType type;
   bool canDelete;
   wxNode *node = theMdiChildren->GetFirst();
   wxCloseEvent event;
   GmatAppData *gmatAppData = GmatAppData::Instance();

   if (mWelcomePanel != NULL)
      mWelcomePanel->Close();

   wxArrayString ignoreNames;
   
   //-------------------------------------------------------
   // delete child frames
   //-------------------------------------------------------
   while (node)
   {
      #ifdef DEBUG_MAINFRAME_CLOSE
      MessageInterface::ShowMessage("   node = %p\n", node);
      #endif

      canDelete = false;
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();

      title = child->GetTitle();
      name = child->GetName();
      type = child->GetItemType();

      #ifdef DEBUG_MAINFRAME_CLOSE
      MessageInterface::ShowMessage("   name = %s, type = %d\n", name.c_str(), type);
      #endif
      
      // If name is in the ignore list, continue
      if (ignoreNames.Index(name) != wxNOT_FOUND)
      {
         node = node->GetNext();
         continue;
      }
      
      if ((type >= GmatTree::BEGIN_OF_RESOURCE && type <= GmatTree::END_OF_RESOURCE) ||
          (type >= GmatTree::BEGIN_OF_COMMAND && type <= GmatTree::END_OF_CONTROL))
      {
         // check if script file to be closed or not
         if (type == GmatTree::SCRIPT_FILE)
         {
            if (closeScriptWindow)
               canDelete = true;
         }
         else
         {
            canDelete = true;
         }
      }
      else if (type >= GmatTree::BEGIN_OF_OUTPUT && type <= GmatTree::END_OF_OUTPUT)
      {
         // delete output child except compare
         if (closePlots && type != GmatTree::COMPARE_REPORT)
         {
            gmatAppData->GetOutputTree()->UpdateOutput(true, closeReports);
            canDelete = true;
         }
      }
      else if (type == GmatTree::MISSION_TREE_UNDOCKED)
      {
         // delete output child except compare
         if (closeUndockedMissionTree)
         {
            canDelete = true;
         }
      }
      
      //--------------------------------------------------------------
      // delete children by child->OnClose()
      //--------------------------------------------------------------
      
      bool childDeleted = false;
      
      if (canDelete)
      {
         #ifdef DEBUG_MAINFRAME_CLOSE
         MessageInterface::ShowMessage("   ==> closing child = %s\n", name.c_str());
         #endif
         
         //-------------------------------------------------
         // delete child if frame can be closed
         // Note: GmatMdiChildFrame::OnClose() calls RemoveChild()
         // child->Close() will not process OnClose() correctly
         // So use OnClose(event) instead
         //-------------------------------------------------
         if (mExitWithoutConfirm)
            child->SetDirty(false);
         
         #ifdef DEBUG_MAINFRAME_CLOSE
         MessageInterface::ShowMessage("   ==> calling child->OnClose()\n");
         #endif
         
         // If it is output frame it is not needed to check for dirty
         if (type > GmatTree::BEGIN_OF_OUTPUT && type < GmatTree::END_OF_OUTPUT)
         {
            child->OnClose(event);
            childDeleted = true;
         }
         else
         {
            // Check if frame is dirty first
            if (!child->IsDirty())
            {
               child->OnClose(event);
               childDeleted = true;
            }
            else
            {
               child->OnClose(event);
               if (child->CanClose())
               {
                  childDeleted = true;
               }
               else
               {
                  canDelete = false;
                  #ifdef DEBUG_MAINFRAME_CLOSE
                  MessageInterface::ShowMessage
                     //("   ==> cannot close this child, so returning false\n");
                     ("   ==> cannot close this child, so added to ignore list\n");
                  #endif
                  // returning false does not check for the next child, so commented out (LOJ: 2010.12.29)
                  //return false;
                  ignoreNames.Add(name);
               }
            }
         }
      }
      
      //-------------------------------------------------
      // Note: The node is deleted from RemoveChild()
      //-------------------------------------------------
      wxNode *nextNode = NULL;
      if (childDeleted)
      {
         nextNode = theMdiChildren->GetFirst();
      }
      else
      {
         nextNode = node->GetNext();
         // returning false does not check for the next child, so commented out (LOJ: 2010.12.29)
         //#ifdef DEBUG_MAINFRAME_CLOSE
         //MessageInterface::ShowMessage("   ==> child was not deleted, so returning false\n");
         //#endif
         //return false;
      }

      #ifdef DEBUG_MAINFRAME_CLOSE
      MessageInterface::ShowMessage
         ("   next node = %p, canDelete = %d\n", nextNode, canDelete);
      if (nextNode)
      {
         child = (GmatMdiChildFrame *)nextNode->GetData();
         title = child->GetTitle();
         name = child->GetName();
         MessageInterface::ShowMessage
            ("   title='%s', name='%s'\n", title.c_str(), name.c_str());
      }
      #endif

      node = nextNode;

   }
   
   wxSafeYield();
   bool retval = true;
   if (!ignoreNames.IsEmpty())
      retval = false;
   
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage
      ("GmatMainFrame::CloseAllChildren() returning %s\n", retval ? "true" : "false");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void MinimizeChildren()
//------------------------------------------------------------------------------
void GmatMainFrame::MinimizeChildren()
{
   // do not need to check if script window is open
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      if (child->GetItemType() != GmatTree::OUTPUT_ORBIT_VIEW &&
          child->GetItemType() != GmatTree::OUTPUT_GROUND_TRACK_PLOT &&
          child->GetItemType() != GmatTree::OUTPUT_XY_PLOT &&
          child->GetItemType() != GmatTree::MISSION_TREE_UNDOCKED &&
          child->GetItemType() != GmatTree::COMPARE_REPORT)
         child->Iconize(TRUE);
      node = node->GetNext();
   }
}


//------------------------------------------------------------------------------
// void RepositionChildren(int xOffset)
//------------------------------------------------------------------------------
void GmatMainFrame::RepositionChildren(int xOffset)
{
   // do not need to check if script window is open
   int toolW, toolH;
   theToolBar->GetSize(&toolW, &toolH);         
   wxNode *node = theMdiChildren->GetFirst();
   int numChildren = 0;
   int x = 0, y = 0;
   
   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      GmatTree::ItemType itemType = child->GetItemType();
      bool isIconized = false;
      
      if (child->IsIconized())
         isIconized = true;
      
      #ifdef DEBUG_REPOSITION_CHILDREN
      MessageInterface::ShowMessage
         ("RepositionChildren() child='%s', isIconized=%d\n", child->GetName().c_str(), isIconized);
      #endif
      
      if (itemType == GmatTree::OUTPUT_REPORT ||
          itemType == GmatTree::OUTPUT_ORBIT_VIEW ||
          itemType == GmatTree::OUTPUT_GROUND_TRACK_PLOT ||
          itemType == GmatTree::OUTPUT_XY_PLOT)
      {
         // If plots or report, just move right
         if (!isIconized)
         {
            child->GetPosition(&x, &y);
            if (x < xOffset)
            {
               // Why y = 0 does not position to top of the client window?
               // Make some y adjustments, need to subtract toolbar height
               int toolW, toolH;
               theToolBar->GetSize(&toolW, &toolH);
               int screenW, screenH;
               GetClientSize(&screenW, &screenH);
               child->Move(wxPoint(x + xOffset, y - toolH));
            }
         }
      }
      else if (itemType != GmatTree::MISSION_TREE_UNDOCKED && !isIconized)
      {
         numChildren++;
         int x = (numChildren - 1) * 20 + xOffset;
         int y = (numChildren - 1) * 20 - toolH;
         child->Move(wxPoint(x,y));
      }
      node = node->GetNext();
   }
   
   // Need this to refresh child windows
   Refresh(false);
   Update();
}


//------------------------------------------------------------------------------
// void SetActiveChildDirty()
//------------------------------------------------------------------------------
void GmatMainFrame::SetActiveChildDirty(bool dirty)
{
   GmatMdiChildFrame *child = (GmatMdiChildFrame *)GetActiveChild();

   if (child != NULL)
      child->SetDirty(dirty);
}


//------------------------------------------------------------------------------
// void OverrideActiveChildDirty()
//------------------------------------------------------------------------------
void GmatMainFrame::OverrideActiveChildDirty(bool override)
{
   GmatMdiChildFrame *child = (GmatMdiChildFrame *)GetActiveChild();

   if (child != NULL)
      child->OverrideDirty(override);
}


//------------------------------------------------------------------------------
// void CloseCurrentProject()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseCurrentProject()
{
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage("GmatMainFrame::CloseCurrentProject() entered\n");
   #endif

   // close all windows
   CloseAllChildren(true, true, true, false);

   // update title and status bar
   wxString statusText;
   statusText.Printf("GMAT - General Mission Analysis Tool");
   SetStatusText("", 1);
   UpdateTitle();

   // clear trees, message window
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage("   clearing trees and message window\n");
   #endif

   // clear command sequence before resource (loj: 2008.07.10)
   theGuiInterpreter->ClearAllSandboxes();
   theGuiInterpreter->ClearCommandSeq();
   theGuiInterpreter->ClearResource();
   MessageInterface::ClearMessage();

   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetResourceTree()->UpdateResource(true);
   gmatAppData->GetMissionTree()->UpdateMission(true);
   gmatAppData->GetOutputTree()->UpdateOutput(true, true);

   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage("GmatMainFrame::CloseCurrentProject() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool InterpretScript(const wxString &filename, Integer scriptOpenOpt,
//                      bool closeScript, bool readBack, const wxString &savePath,
//                      bool multiScripts)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <ScriptOpenOpt> open script editor option after build script (0)
 *         0, script file to be opened on error only
 *         1, script file to be opened always
 *         2, NO script file to be opened
 * @param <closeScript> true will close opened script editor (false)
 * @param <readBack> true will read scripts, save, and read back in (false)
 * @param <newPath> new path to be used for saving scripts ("")
 * @param <multiScripts> true if running scripts from the folder (false)
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::InterpretScript(const wxString &filename, Integer scriptOpenOpt,
                                    bool closeScript, bool readBack,
                                    const wxString &savePath, bool multiScripts)
{
   #ifdef DEBUG_INTERPRET
   MessageInterface::ShowMessage
      ("GmatMainFrame::InterpretScript()\n   filename=%s\n   scriptOpenOpt=%d, "
       "closeScript=%d, readBack=%d, multiScripts=%d\n   savePath=%s\n", filename.c_str(),
       scriptOpenOpt, closeScript, readBack, multiScripts, savePath.c_str());
   #endif

   UpdateTitle(filename);

   bool success = false;
   GmatAppData *gmatAppData = GmatAppData::Instance();

   // Always refresh the gui before new scripts are read
   CloseAllChildren(closeScript, true, true, false);
   gmatAppData->GetResourceTree()->ClearResource(true);
   gmatAppData->GetMissionTree()->ClearMission();
   gmatAppData->GetOutputTree()->UpdateOutput(true, true);
   
   // Indicate active script in bold face in the ResourceTree (LOJ: 2010.12.27)
   RefreshActiveScript(filename);
   
   // let's try building the script, Moderator::InterpretScript() will
   // clear all resource and commands
   try
   {
      // If successfully interpreted, set status to true
      if (theGuiInterpreter->
          InterpretScript(filename.c_str(), readBack, savePath.c_str()))
      {
         #ifdef DEBUG_INTERPRET
         MessageInterface::ShowMessage("   Successfully interpreted the script\n");
         #endif
         success = true;
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Errors were found in the script named \"%s\".\n"
             "Please fix all errors listed in message window.\n", filename.c_str());

         // Clear command sequence before resource (loj: 2008.07.10)
         theGuiInterpreter->ClearCommandSeq();
         theGuiInterpreter->ClearResource();
      }
      
      if (success)
      {
         #ifdef DEBUG_INTERPRET
         MessageInterface::ShowMessage("   Now updating ResourceTree and MissionTree\n");
         #endif
         // Update ResourceTree and MissionTree
         gmatAppData->GetResourceTree()->UpdateResource(true);
         gmatAppData->GetMissionTree()->UpdateMission(true);
         
         #ifdef DEBUG_INTERPRET
         MessageInterface::ShowMessage("   Now restoring UndockedMissionPanel\n");
         #endif
         RestoreUndockedMissionPanel();
         
         #ifdef DEBUG_INTERPRET
         MessageInterface::ShowMessage("   Now updating GUI/Script sync status\n");
         #endif
         UpdateGuiScriptSyncStatus(1, 1);
         
         // if not running script folder, clear status
         if (!multiScripts)
            SetStatusText("", 2);
         
         // open script editor
         if (scriptOpenOpt == GmatGui::ALWAYS_OPEN_SCRIPT)
            OpenScript(false);
      }
      else
      {
         SetStatusText("Errors were Found in the Script!!", 2);

         // open script editor
         if (scriptOpenOpt == GmatGui::ALWAYS_OPEN_SCRIPT ||
             scriptOpenOpt == GmatGui::OPEN_SCRIPT_ON_ERROR)
            OpenScript();
         
         UpdateGuiScriptSyncStatus(1, 3);
         
      }
   }
   catch (BaseException &e)
   {
      wxLogError("%s", e.GetFullMessage().c_str());
      wxLog::FlushActive();
      MessageInterface::ShowMessage(e.GetFullMessage());
   }

   mInterpretFailed = !success;

   #ifdef DEBUG_INTERPRET
   MessageInterface::ShowMessage
      ("GmatMainFrame::InterpretScript() returning %d\n", success);
   #endif

   return success;
}


//------------------------------------------------------------------------------
// void BuildAndRunScript(const wxString &filename)
//------------------------------------------------------------------------------
void GmatMainFrame::BuildAndRunScript(const wxString &filename)
{
   CloseCurrentProject();
   
   // Check if file exist first
   if (wxFileName::FileExists(filename))
   {
      mScriptFilename = filename.c_str();
      
      if (InterpretScript(filename, GmatGui::DO_NOT_OPEN_SCRIPT))
         mRunStatus = RunCurrentMission();
   }
   else
   {
      wxMessageBox(wxT("The script file \"" + filename + "\" does not exist.\n"),
                   wxT("GMAT Error"));
   }
}


//------------------------------------------------------------------------------
// Integer RunCurrentMission()
//------------------------------------------------------------------------------
/*
 * Executes current mission by calling GuiInterpreter::RunMission() which
 * calls Moderator::RunMission()
 *
 * @return  1 if run was successful
 *         -2 if execution interrupted by user
 *         -3 if exception thrown during the run
 *         -4 if unknown error occurred during the run
 */
//------------------------------------------------------------------------------
Integer GmatMainFrame::RunCurrentMission()
{
   #ifdef DEBUG_RUN
   MessageInterface::ShowMessage
      ("GmatMainFrame::RunCurrentMission() mRunPaused=%d\n", mRunPaused);
   #endif
      
   Integer retval = 1;
   mAnimationEnabled = true;
   
   // We don't want to write out script error message since users can start
   // brand new mission from the GUI when script errors occur.
   // So exclude it until we revisit this. Changed the code while looking
   // at bug 1532. (LOJ: 2009.11.13)
   #if 0
   if (mInterpretFailed)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Errors were found in the script named \"%s\".\n"
          "Please fix all errors listed in message window before running "
          "the mission.\n", mScriptFilename.c_str());

      return 0;
   }
   #endif
   
   EnableMenuAndToolBar(false, true);

   wxYield();
   SetFocus();

   mRunCompleted = false;

   if (mRunPaused)
   {
      mRunPaused = false;

      MessageInterface::ShowMessage("Execution resumed.\n");
      theGuiInterpreter->ChangeRunState("Resume");
      SetStatusText("Busy", 1);
   }
   else
   {
      SetStatusText("Busy", 1);
      MinimizeChildren();
      GmatAppData::Instance()->GetMessageTextCtrl()->SetFocus();
      retval = theGuiInterpreter->RunMission();

      #ifdef DEBUG_RUN
      MessageInterface::ShowMessage("   return code from RunMission()=%d\n", retval);
      #endif

      // always stop server after run (loj: 2008.02.06) - to investigate Bug 1133
      // stop server after user interrupt (loj: 2008.03.05)
      //if (mMatlabServer)
      if (retval != 1 && mMatlabServer)
         StopMatlabServer(); // stop server if running to avoid getting callback staus
                             // when run stopped by user

      EnableMenuAndToolBar(true, true);
      SetStatusText("", 1);

      //put items in output tab
      GmatAppData::Instance()->GetOutputTree()->UpdateOutput(false, true);
   }

   return retval;
} // end RunCurrentMission()


//------------------------------------------------------------------------------
// void StopRunningMission()
//------------------------------------------------------------------------------
/*
 * Stops running mission and updates tool bar accordingly.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::StopRunningMission()
{
   wxToolBar* toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_STOP, FALSE);
   wxYield();
   
   theGuiInterpreter->ChangeRunState("Stop");
   mRunPaused = false;
   mRunCompleted = true;
   
   theMenuBar->Enable(MENU_FILE_OPEN_SCRIPT, TRUE);
   UpdateMenus(TRUE);
   toolBar->EnableTool(MENU_FILE_OPEN_SCRIPT, TRUE);
   toolBar->EnableTool(TOOL_RUN, TRUE);
}


//------------------------------------------------------------------------------
// void NotifyRunCompleted()
//------------------------------------------------------------------------------
/*
 * This is called by the Moderator when a mission run is completed.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::NotifyRunCompleted()
{
   mRunCompleted = true;
}


//------------------------------------------------------------------------------
// void ProcessPendingEvent()
//------------------------------------------------------------------------------
/*
 * Process any pending event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::ProcessPendingEvent()
{
   #ifdef DEBUG_PENDING_EVENTS
      MessageInterface::ShowMessage("GmatMainFrame::ProcessPendingEvent() entered\n");
   #endif
   wxYield();
}


//------------------------------------------------------------------------------
// void StartMatlabServer()
//------------------------------------------------------------------------------
void GmatMainFrame::StartMatlabServer()
{
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage("GmatMainFrame::StartMatlabServer() entered.\n");
   #endif
   
   if (!mMatlabServer)
   {
      // service name (DDE classes) or port number (TCP/IP based classes)
      wxString service = IPC_SERVICE;
      
      // Create a new server
      mMatlabServer = new GmatServer;
      mMatlabServer->Create(service);
      
      MessageInterface::ShowMessage("Server started.\n");
      
      #ifdef DEBUG_SERVER
      MessageInterface::ShowMessage
         ("   service='%s', mMatlabServer=%p\n", service.c_str(), mMatlabServer);
      #endif
      
      // Disable ResourceTree Matlab Server Start popup menu
      GmatAppData::Instance()->GetResourceTree()->UpdateMatlabServerItem(true);
   }
   else
   {
      MessageInterface::ShowMessage("Server has already started.\n");
   }
}


//------------------------------------------------------------------------------
// void StopMatlabServer()
//------------------------------------------------------------------------------
void GmatMainFrame::StopMatlabServer()
{
   #ifdef DEBUG_SERVER
   MessageInterface::ShowMessage
      ("GmatMainFrame::StopMatlabServer() entered. mMatlabServer=%p\n", mMatlabServer);
   #endif

   if (mMatlabServer)
   {
      mMatlabServer->Disconnect();
      delete mMatlabServer;

      MessageInterface::ShowMessage("Server terminated.\n");

      mMatlabServer = NULL;

      //==============================================================
      #ifdef __WAIT_BEFORE_RERUN__
      // Show progress bar while GMAT closes the server
      wxProgressDialog dlg(wxT("GMAT closing the server"),
                           wxT("Please wait while GMAT closes the server"), 100, this,
                           wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_SMOOTH);

      // wait for 2 seconds
      for (int i=0; i<10; i++)
      {
         dlg.Update((i+1)*10);
         Sleep(200);
      }
      #endif
      //==============================================================
      
      // Disable ResourceTree Matlab Server Stop popup menu
      GmatAppData::Instance()->GetResourceTree()->UpdateMatlabServerItem(false);
   }
   else
   {
      MessageInterface::ShowMessage("Server has not started.\n");
   }
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnClose(wxCloseEvent& event)
{
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnClose() entered. mMatlabServer=%p\n", mMatlabServer);
   #endif

   if (!mRunCompleted)
   {
      wxMessageBox(wxT("GMAT is still running the mission.\n"
                       "Please STOP the run before closing."),
                   wxT("GMAT Warning"));
      event.Veto();
      return;
   }
   
   // close all child windows first
   if (CloseAllChildren(true, true, true, true))
   {
      // if user canceled, veto the event
      if (ShowSaveMessage())
         event.Veto();
      else
         event.Skip();
   }
   else
   {
      event.Veto();
   }
   
   // stop server if running
   if (mMatlabServer)
      StopMatlabServer();
}


//------------------------------------------------------------------------------
// Integer GetToolBarHeight()
//------------------------------------------------------------------------------
Integer GmatMainFrame::GetToolBarHeight()
{
   Integer w, h;
   GetToolBar()->GetSize(&w, &h);
   return h;
}


//------------------------------------------------------------------------------
// wxToolBar* GmatMainFrame::GetMainFrameToolBar()
//------------------------------------------------------------------------------
wxToolBar* GmatMainFrame::GetMainFrameToolBar()
{
   return GetToolBar();
}


//------------------------------------------------------------------------------
// wxStatusBar* GmatMainFrame::GetMainFrameStatusBar()
//------------------------------------------------------------------------------
wxStatusBar* GmatMainFrame::GetMainFrameStatusBar()
{
   return GetStatusBar();
}


//------------------------------------------------------------------------------
// void EnableAnimation(bool enable)
//------------------------------------------------------------------------------
void GmatMainFrame::EnableAnimation(bool enable)
{
   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage("GmatMainFrame::EnableAnimation() entered.\n");
   #endif
   
   mAnimationEnabled = enable;
}

//------------------------------------------------------------------------------
// void ManageMissionTree()
//------------------------------------------------------------------------------
void GmatMainFrame::ManageMissionTree()
{
   // Check to see if the Mission Tree should be undocked
   wxFileConfig *pConfig;
   pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
   wxString isMissionTreeDocked;
   pConfig->Read("/MissionTree/Docked", &isMissionTreeDocked, "true");
   if (isMissionTreeDocked.Lower() == "false")
      theNotebook->CreateUndockedMissionPanel();

}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// bool ShowScriptOverwriteMessage()
//------------------------------------------------------------------------------
/*
 * Shows save changes to script file message.
 *
 * @return true if user chose to overwrite the script
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::ShowScriptOverwriteMessage()
{
   wxMessageDialog *msgDlg = new wxMessageDialog
      (this, "You will lose changes made in the script, do you want to overwrite "
       "the script?", "Save GUI...",
       wxYES_NO | wxCANCEL |wxICON_QUESTION, wxDefaultPosition);
   
   int result = msgDlg->ShowModal();
   if (result == wxID_YES)
   {
      SaveGuiToActiveScript();
      delete msgDlg;
      return true;
   }
   else
   {
      delete msgDlg;
      return false;
   }
}


//------------------------------------------------------------------------------
// bool ShowSaveMessage()
//------------------------------------------------------------------------------
/*
 * Shows save changes to script file message.
 *
 * @return true if user canceled from the message dialog
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::ShowSaveMessage()
{
   // prompt save, if changes were made
   if (theGuiInterpreter->HasConfigurationChanged())
   {
      wxMessageDialog *msgDlg =
         new wxMessageDialog(this,
                             "Would you like to save changes to script file?", "Save...",
                             wxYES_NO | wxCANCEL |wxICON_QUESTION, wxDefaultPosition);

      int result = msgDlg->ShowModal();
      std::string oldScriptName = mScriptFilename;
      mExitWithoutConfirm = false;

      if (result == wxID_CANCEL)
      {
         delete msgDlg;
         return true;
      }
      else if (result == wxID_NO)
      {
         // If we decided to ignore any changes made to panel later,
         // just uncomment this
         //mExitWithoutConfirm = true;
         delete msgDlg;
         return false;
      }
      else if (result == wxID_YES)
      {
         if (mScriptFilename == mTempScriptName)
         {
            SaveScriptAs();
         }
         else
         {
            SaveGuiToActiveScript();
         }
         
         delete msgDlg;
         return false;
      }
   }
   else
   {
      #ifdef __CONFIRM_EXIT__
      wxMessageDialog *msgDlg =
         new wxMessageDialog(this, "Do you really want to exit?", "Exiting...",
                             wxYES_NO |wxICON_QUESTION, wxDefaultPosition);

      int result = msgDlg->ShowModal();

      if (result == wxID_NO)
      {
         delete msgDlg;
         return false;
      }
      #endif
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool SaveScriptAs()
//------------------------------------------------------------------------------
bool GmatMainFrame::SaveScriptAs()
{
   #ifdef DEBUG_MAINFRAME_SAVE
   MessageInterface::ShowMessage
      ("GmatMainFrame::SaveScriptAs() mScriptFilename=%s\n",
       mScriptFilename.c_str());
   #endif
   
   bool scriptSaved = true;
   std::string oldScriptName = mScriptFilename;

   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
         _T("Script files (*.script, *.m)|*.script;*.m|"\
            "Text files (*.txt, *.text)|*.txt;*.text|"\
            "All files (*.*)|*.*"), wxSAVE);

   if (dialog.ShowModal() == wxID_OK)
   {
      mScriptFilename = dialog.GetPath().c_str();

      if (wxFileName::FileExists(mScriptFilename.c_str()))
      {
         #ifdef DEBUG_MAINFRAME_SAVE
         MessageInterface::ShowMessage
            ("The script file: \"%s\" exist\n", mScriptFilename.c_str());
         #endif

         if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"),
                          _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
         {
            SaveChildPositionsAndSizes();
            theGuiInterpreter->SaveScript(mScriptFilename);
         }
         else
         {
            mScriptFilename = oldScriptName;
            scriptSaved = false;
         }
      }
      else
      {
         SaveChildPositionsAndSizes();
         theGuiInterpreter->SaveScript(mScriptFilename);
      }
   }
   else
   {
      scriptSaved = false;
   }
   
   #ifdef __CLOSE_CHILDREN_AFTER_SAVE__
   if (scriptSaved)
      CloseAllChildren();
   #endif

   if (scriptSaved)
      UpdateGuiScriptSyncStatus(1, 1);
   
   return scriptSaved;
} // SaveScriptAs()


//------------------------------------------------------------------------------
// void OpenScript(bool restore)
//------------------------------------------------------------------------------
/**
 * Creates script item and opens the script in the text editor.
 *
 * @param <restore> if true the child will be restored if minimized [true]
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OpenScript(bool restore)
{
   //MessageInterface::ShowMessage("===> GmatMainFrame::OpenScript() entered\n");

   GmatTreeItemData *scriptItem =
      new GmatTreeItemData(mScriptFilename.c_str(), GmatTree::SCRIPT_FILE);
   
   CreateChild(scriptItem, restore);
}


//------------------------------------------------------------------------------
// void OpenRecentScript(size_t index)
//------------------------------------------------------------------------------
/**
 * open recent script file from the menu bar.
 *
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OpenRecentScript(size_t index, wxCommandEvent &event)
{
   wxArrayString files;
   wxString aFilename;
   wxString aKey;
   std::string s;
   
   // get the files from the personalization config
   // get the config object
   wxConfigBase *pConfig = GmatAppData::Instance()->GetPersonalizationConfig();
   pConfig->SetPath(wxT("/RecentFiles"));
   
   // read filenames from config object
   aKey = wxString::Format(wxT("%d"), (int) index);
   aFilename = pConfig->Read(aKey);
   OpenRecentScript(aFilename, event);
}


//------------------------------------------------------------------------------
// void OpenRecentScript(size_t index)
//------------------------------------------------------------------------------
/**
 * open recent script file from the menu bar.
 *
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OpenRecentScript(wxString filename, wxCommandEvent &event)
{
   // Check if file exist first
   if (!wxFileName::FileExists(filename))
   {
      wxMessageBox(wxT("The script file \"" + filename + "\" no longer exist.  "
                       "It may have been removed or renamed.\n"),
                   wxT("GMAT Error"));
      return;
   }
   
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetResourceTree()->AddScript(filename);
   
   if (gmatAppData->GetResourceTree()->WasScriptAdded())
   {
      #ifdef DEBUG_OPEN_SCRIPT
      MessageInterface::ShowMessage
         ("GmatMainFrame::OnOpenRecentScript() mInterpretFailed=%d, "
          "HasConfigurationChanged=%d\n", mInterpretFailed,
          theGuiInterpreter->HasConfigurationChanged());
      #endif
      
      wxString scriptName = gmatAppData->GetResourceTree()->GetLastScriptAdded();
      SetScriptFileName(scriptName.c_str());
      
      if (!mInterpretFailed && theGuiInterpreter->HasConfigurationChanged())
      {
          // need to save new file name because it gets overwritten in save
          std::string tmpFilename = mScriptFilename;

          // ask user to continue because changes will be lost
          if (wxMessageBox(_T("Changes will be lost.\nDo you want to save the current script?"),
             _T("Please confirm"),
             wxICON_QUESTION | wxYES_NO) == wxYES)
          {
             OnSaveScriptAs(event);
          }

          mScriptFilename = tmpFilename;
      }

      SetStatusText("", 2);
      InterpretScript(mScriptFilename.c_str(), GmatGui::OPEN_SCRIPT_ON_ERROR, true);
   }
}


//---------------------------------
// event handling
//---------------------------------

//------------------------------------------------------------------------------
// void OnProjectNew(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles New command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnProjectNew(wxCommandEvent& WXUNUSED(event))
{
   CloseCurrentProject();
}


//------------------------------------------------------------------------------
// void OnLoadDefaultMission(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles loading the default mission from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnLoadDefaultMission(wxCommandEvent& WXUNUSED(event))
{
   // if any changes were made, ask user to continue
   if (theGuiInterpreter->HasConfigurationChanged())
   {
      if (wxMessageBox(_T("Changes will be lost.\nDo you still want to continue?"),
                       _T("Please confirm"),
                       wxICON_QUESTION | wxYES_NO) != wxYES)
      {
         return;
      }
   }
   else
   {
      if (wxMessageBox(_T("Do you really want to load default mission?"),
                       _T("Please confirm"),
                       wxICON_QUESTION | wxYES_NO) != wxYES)
      {
         return;
      }
   }

   CloseCurrentProject();
   //mScriptFilename = "$gmattempscript$.script";
   mScriptFilename = mTempScriptName;
   theGuiInterpreter->LoadDefaultMission();
   mInterpretFailed = false;

   // Update trees
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetResourceTree()->UpdateResource(true);
   gmatAppData->GetMissionTree()->UpdateMission(true);
   gmatAppData->GetOutputTree()->UpdateOutput(true, true);
   RestoreUndockedMissionPanel();
   
   // Update GUI/Script sync status
   UpdateGuiScriptSyncStatus(1, 1);
   
}


//------------------------------------------------------------------------------
// void OnSaveScript(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles saving the gui to a script.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnSaveScript(wxCommandEvent& event)
{
   #ifdef DEBUG_MAINFRAME_SAVE
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnSaveScript() mInterpretFailed=%d\n", mInterpretFailed);
   #endif

   bool scriptSaved = false;
   GmatAppData *gmatAppData = GmatAppData::Instance();

   if (mScriptFilename == mTempScriptName)
   {
      scriptSaved = SaveScriptAs();
      if (scriptSaved)
      {
         gmatAppData->GetResourceTree()->AddScriptItem(mScriptFilename.c_str());
         gmatAppData->GetResourceTree()->UpdateResource(false);
      }
   }
   else
   {
      if (mInterpretFailed)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Errors were found in the script named \"%s\".\n"
             "Please fix all errors listed in message window before saving "
             "the mission.\n", mScriptFilename.c_str());
      }
      else
      {
         GuiItemManager *guiManager = GuiItemManager::GetInstance();
         if (guiManager->GetGuiStatus() == 2 && guiManager->GetActiveScriptStatus() == 2)
         {
            scriptSaved = ShowScriptOverwriteMessage();
         }
         else
         {
            SaveGuiToActiveScript();
            scriptSaved = true;
         }
      }
   }
   
   if (scriptSaved)
   {
      UpdateTitle(mScriptFilename.c_str());
      UpdateGuiScriptSyncStatus(1, 1);
      RefreshActiveScript(mScriptFilename.c_str());
      
      #ifdef __CONFIRM_SAVE__
      MessageInterface::PopupMessage
         (Gmat::INFO_, "Script saved to \"%s\"\n", mScriptFilename.c_str());
      #endif

      #ifdef __CLOSE_CHILDREN_AFTER_SAVE__
      CloseAllChildren();
      #endif

   }
} // OnSaveScript


//------------------------------------------------------------------------------
// void OnSaveScriptAs(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles saving the gui to a script.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnSaveScriptAs(wxCommandEvent& WXUNUSED(event))
{
   if (SaveScriptAs())
   {
      GmatAppData::Instance()->GetResourceTree()->AddScriptItem(mScriptFilename.c_str());
      GmatAppData::Instance()->GetResourceTree()->SetActiveScript(mScriptFilename.c_str());
      UpdateTitle(mScriptFilename.c_str());
   }
}


//------------------------------------------------------------------------------
// void OnPrintSetup(wxCommandEvent &event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnPrintSetup(wxCommandEvent &event)
{
#if wxUSE_PRINTING_ARCHITECTURE
   (*globalPageSetupData) = * globalPrintData;
   wxPageSetupDialog pageSetupDialog(this, globalPageSetupData);
   pageSetupDialog.ShowModal();
   (*globalPrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
   (*globalPageSetupData) = pageSetupDialog.GetPageSetupData();
#endif // wxUSE_PRINTING_ARCHITECTURE
}


//------------------------------------------------------------------------------
// void OnPrint(wxCommandEvent &event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnPrint(wxCommandEvent &event)
{
#if wxUSE_PRINTING_ARCHITECTURE
   #ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();

   if (editor)
   {
      wxPrintDialogData printDialogData( *globalPrintData);
      wxPrinter printer (&printDialogData);
      EditorPrintout printout (editor);
      if (!printer.Print (this, &printout, true))
      {
         if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
         {
            wxMessageBox (_("There was a problem with printing.\n\
                         Perhaps your current printer is not correctly?"),
                          _("Previewing"), wxOK);
            return;
         }
      }
      (*globalPrintData) = printer.GetPrintDialogData().GetPrintData();
   }

   #endif // __USE_STC_EDITOR__
#endif // wxUSE_PRINTING_ARCHITECTURE
}


//------------------------------------------------------------------------------
// void OnProjectExit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles exit command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnProjectExit(wxCommandEvent& WXUNUSED(event))
{
   // true is to force the frame to close
   Close(true);
}


//------------------------------------------------------------------------------
// void OnRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles run command from the tool bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnRun(wxCommandEvent& WXUNUSED(event))
{
   mRunStatus = RunCurrentMission();
}


//------------------------------------------------------------------------------
// void OnPause(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles pause command from the tool bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnPause(wxCommandEvent& WXUNUSED(event))
{
   wxToolBar* toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_PAUSE, FALSE);
   wxYield();

   theGuiInterpreter->ChangeRunState("Pause");
   MessageInterface::ShowMessage("Execution paused.\n");

   theMenuBar->Enable(MENU_FILE_OPEN_SCRIPT, FALSE);
   UpdateMenus(FALSE);
   toolBar->EnableTool(MENU_FILE_OPEN_SCRIPT, FALSE);
   toolBar->EnableTool(TOOL_RUN, TRUE);
   SetStatusText("Paused", 1);
   mRunPaused = true;
}


//------------------------------------------------------------------------------
// void OnStop(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles stop command from the tool bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
   StopRunningMission();
}


//------------------------------------------------------------------------------
// void OnCloseAll(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles closing all open children.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnCloseAll(wxCommandEvent& WXUNUSED(event))
{
   CloseAllChildren(true, true, false, false);
   wxSafeYield();

   wxToolBar* toolBar = GetToolBar();
   // enable screen capture when the simulation is run
   toolBar->EnableTool(TOOL_SCREENSHOT, false);
}


//------------------------------------------------------------------------------
// void OnCloseActive(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles closing all open children.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnCloseActive(wxCommandEvent& WXUNUSED(event))
{
   CloseActiveChild();
   wxSafeYield();
   
   if (theMdiChildren->GetCount() <= 0)
   {
      wxToolBar* toolBar = GetToolBar();
      // deactivate screen capture when not running
      toolBar->EnableTool(TOOL_SCREENSHOT, false);
   }
}


//------------------------------------------------------------------------------
// void OnHelpAbout(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles about command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpAbout(wxCommandEvent& WXUNUSED(event))
{
   AboutDialog dlg(this, -1, "About GMAT");
   dlg.ShowModal();
}


//------------------------------------------------------------------------------
// void OnHelpWelcome(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles display Welcome page command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpWelcome(wxCommandEvent& WXUNUSED(event))
{
   if (mWelcomePanel == NULL)
   {
      mWelcomePanel =
         new WelcomePanel(this, _T("Welcome to GMAT"), 20, 20, 600, 350);
   }
   mWelcomePanel->Show(true);
   // set focus to this panel so that user can close it when error occurs
   mWelcomePanel->SetFocus();  
}


//------------------------------------------------------------------------------
// void OnHelpContents(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles online help command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event))
{
   #ifdef __WXMSW__
   theHelpController->DisplayContents();
   #else
   #endif
}


//------------------------------------------------------------------------------
// void OnHelpOnline(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles online help command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpOnline(wxCommandEvent& WXUNUSED(event))
{
   wxString url = "http://gmat.sourceforge.net/docs/R2011a/html/index.html";
   ::wxLaunchDefaultBrowser(url);
}


//------------------------------------------------------------------------------
// void OnHelpTutorial(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles online help command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpTutorial(wxCommandEvent& WXUNUSED(event))
{
   wxString url = "http://gmat.sourceforge.net/docs/R2011a/help.html#N10373";
   ::wxLaunchDefaultBrowser(url);
}


//------------------------------------------------------------------------------
// void OnHelpForum(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles online help command from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpForum(wxCommandEvent& WXUNUSED(event))
{
   wxString wikiUrl = "http://gmat.ed-pages.com/forum/index.php";
   ::wxLaunchDefaultBrowser(wikiUrl);
}


//------------------------------------------------------------------------------
// void OnHelpIssue(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles report an issue command from the help menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpIssue(wxCommandEvent& WXUNUSED(event))
{
   wxString wikiUrl = "http://pows003.gsfc.nasa.gov/bugzilla/";
   ::wxLaunchDefaultBrowser(wikiUrl);
}


//------------------------------------------------------------------------------
// void OnHelpFeedback(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles provide feedback command from the help menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnHelpFeedback(wxCommandEvent& WXUNUSED(event))
{
   #ifdef __ENABLE_EMAIL__

   // Get current working directory
   wxString cwd1 = ::wxGetCwd();

   #ifdef DEBUG_FEEDBACK
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnHelpFeedback() before email, cwd = '%s'\n", cwd1.c_str());
   #endif
   
   // Should bring up email to gmat@gsfc.nasa.gov
   wxEmail *email = new wxEmail();
   wxMailMessage feedback(wxT("GMAT Feedback"),
                          wxT("gmat@gsfc.nasa.gov"),
                          wxT("Please enter your feedback here.\n"));
   
   if (!wxEmail::Send(feedback))
   {
      MessageInterface::ShowMessage("The feedback was not sent.\n");
   }
   
   wxString cwd2 = ::wxGetCwd();
   
   #ifdef DEBUG_FEEDBACK
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnHelpFeedback()  after email, cwd = '%s'\n", cwd2.c_str());
   #endif
   
   if (::wxSetWorkingDirectory(cwd1))
   {   
      cwd2 = ::wxGetCwd();
      
      #ifdef DEBUG_FEEDBACK
      MessageInterface::ShowMessage
         ("GmatMainFrame::OnHelpFeedback()  after resetting cwd, cwd = '%s'\n", cwd2.c_str());
      #endif
   }
   else
   {
      #ifdef DEBUG_FEEDBACK
      MessageInterface::ShowMessage
         ("GmatMainFrame::OnHelpFeedback()  failed resetting cwd to '%s'\n", cwd1.c_str());
      #endif
   }
   
   delete email;
   
   #else
   
   MessageInterface::PopupMessage
      (Gmat::INFO_, "Use of email is disabled.");
   
   #endif
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewResource(const wxString &title, const wxString &name
//                                      GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewResource(const wxString &title, const wxString &name,
                                 GmatTree::ItemType itemType)
{
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateNewResource() title='%s', name='%s', itemType=%d\n",
       title.c_str(), name.c_str(), itemType);
   #endif

   // if variable, then display dialog, TGG 4/2010
   switch (itemType)
   {
   case GmatTree::ARRAY:
   case GmatTree::STRING:
   case GmatTree::VARIABLE:
      {
         ParameterCreateDialog paramDlg(this, name);
         paramDlg.ShowModal();
         return NULL;
      }
   default:
      break;
   }
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, name, title, itemType);   
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
   case GmatTree::GROUND_STATION:
      sizer->Add(new GroundStationPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SPACECRAFT:
      sizer->Add(new SpacecraftPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::CELESTIAL_BODY:
   case GmatTree::CELESTIAL_BODY_STAR:
   case GmatTree::CELESTIAL_BODY_PLANET:
   case GmatTree::CELESTIAL_BODY_MOON:
   case GmatTree::CELESTIAL_BODY_COMET:
   case GmatTree::CELESTIAL_BODY_ASTEROID:
      sizer->Add(new CelestialBodyPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::FUELTANK:
      sizer->Add(new GmatBaseSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
//      sizer->Add(new TankConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::THRUSTER:
      sizer->Add(new ThrusterConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::HARDWARE:
      sizer->Add(new GmatBaseSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
//      sizer->Add(new TankConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::FORMATION:
      sizer->Add(new FormationSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SOLAR_SYSTEM:
      sizer->Add(new UniversePanel(scrolledWin), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::IMPULSIVE_BURN:
      sizer->Add(new ImpulsiveBurnSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::FINITE_BURN:
      sizer->Add(new FiniteBurnSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::PROPAGATOR:
      sizer->Add(new PropagationConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
//   case GmatTree::SPK_PROPAGATOR:
//      sizer->Add(new GmatBaseSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
//      break;
   case GmatTree::DIFF_CORR:
      sizer->Add(new DCSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SQP:
      sizer->Add(new SQPSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SOLVER:
      sizer->Add(new GmatBaseSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
//     sizer->Add(new SolverSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::REPORT_FILE:
      sizer->Add(new ReportFileSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::XY_PLOT:
      sizer->Add(new XyPlotSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::ORBIT_VIEW:
      sizer->Add(new OrbitViewPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::EPHEMERIS_FILE:
      sizer->Add(new EphemerisFilePanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::GROUND_TRACK_PLOT:
   case GmatTree::SUBSCRIBER:
      // Switched to use GmatBaseSetupPanel (LOJ: 2011.06.02)
      //sizer->Add(new SubscriberSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      sizer->Add(new GmatBaseSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::MATLAB_FUNCTION:
      sizer->Add(new MatlabFunctionSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::GMAT_FUNCTION:
      {
         FunctionSetupPanel *functPanel = new FunctionSetupPanel(scrolledWin, name);
         newChild->SetAssociatedWindow(functPanel);
         sizer->Add(functPanel, 0, wxGROW|wxALL, 0);
         #ifdef __USE_STC_EDITOR__
         newChild->SetEditor(functPanel->GetEditor());
         newChild->OverrideDirty(true);
         #else
         newChild->SetScriptTextCtrl(functPanel->mFileContentsTextCtrl);
         #endif
         break;
      }
   case GmatTree::SCRIPT_FILE:
      {
         bool activeScript = false;
         
         //if (mScriptFilename.c_str() == name)
         if (GmatFileUtil::IsSameFileName(mScriptFilename.c_str(), name.c_str()))
            activeScript = true;
         
         #ifdef __USE_STC_EDITOR__
         EditorPanel *editorPanel = new EditorPanel(scrolledWin, name, activeScript);
         sizer->Add(editorPanel, 0, wxGROW|wxALL, 0);
         newChild->SetEditor(editorPanel->GetEditor());
         newChild->SetAssociatedWindow(editorPanel);
         #else
         ScriptPanel *scriptPanel = new ScriptPanel(scrolledWin, name, activeScript);
         sizer->Add(scriptPanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(scriptPanel->mFileContentsTextCtrl);
         newChild->SetAssociatedWindow(scriptPanel);
         #endif
         break;
      }
   case GmatTree::COORD_SYSTEM:
   case GmatTree::USER_COORD_SYSTEM:
      sizer->Add(new CoordSystemConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::BARYCENTER:
      sizer->Add(new BarycenterPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::LIBRATION_POINT:
      sizer->Add(new LibrationPointPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::USER_DEFINED_OBJECT:
     sizer->Add(new GmatBaseSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
     break;
   default:
      return NULL;
   }

   scrolledWin->SetScrollRate(5, 5);
   scrolledWin->SetAutoLayout(TRUE);
   scrolledWin->SetSizer(sizer);
   sizer->Fit(scrolledWin);
   sizer->SetSizeHints(scrolledWin);

   #ifdef __USE_CHILD_BEST_SIZE__
   if (itemType != GmatTree::SCRIPT_FILE)
   {
      wxSize bestSize = newChild->GetBestSize();
      newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
   }
   else
   {
      #ifndef __WXMSW__
      wxSize bestSize = newChild->GetBestSize();
      newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
      #endif
   }
   #endif

   // list of open children
   theMdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
#ifndef __WXMSW__
   newChild->Show();
#endif

   return newChild;
}

//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewCommand(GmatTree::ItemType itemType, GmatCommand *cmd)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewCommand(GmatTree::ItemType itemType, GmatTreeItemData *item)
{
   wxString title = item->GetTitle();
   wxString name = item->GetName();
   GmatCommand *cmd = item->GetCommand();

   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateNewCommand() title=%s, name=%s, itemType=%d, cmd=<%p><%s>\n",
       title.c_str(), name.c_str(), itemType, cmd, cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif

   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, name, title, itemType);
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
   case GmatTree::PROPAGATE:
      sizer->Add(new PropagatePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::MANEUVER:
      sizer->Add(new ManeuverPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::BEGIN_FINITE_BURN:
      sizer->Add(new BeginFiniteBurnPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::END_FINITE_BURN:
      sizer->Add(new EndFiniteBurnPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::TARGET:
      sizer->Add(new TargetPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::OPTIMIZE:
      sizer->Add(new OptimizePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::ACHIEVE:
      sizer->Add(new AchievePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::VARY:
      sizer->Add(new VaryPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::OPTIMIZE_VARY:
      sizer->Add(new VaryPanel(scrolledWin, cmd, true), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SAVE:
      sizer->Add(new SavePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::REPORT:
      sizer->Add(new ReportPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::TOGGLE:
      sizer->Add(new TogglePanel(scrolledWin, cmd, false, true), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::XY_PLOT_ACTION:
      sizer->Add(new TogglePanel(scrolledWin, cmd, true, false), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::PLOT_ACTION:
      sizer->Add(new TogglePanel(scrolledWin, cmd, false, false), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::CALL_FUNCTION:
      sizer->Add(new CallFunctionPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::MINIMIZE:
      sizer->Add(new MinimizePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::NON_LINEAR_CONSTRAINT:
      sizer->Add(new NonlinearConstraintPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SCRIPT_EVENT:
   {
      ScriptEventPanel *scriptEventPanel =
         new ScriptEventPanel(scrolledWin, (MissionTreeItemData*)item);
      sizer->Add(scriptEventPanel, 0, wxGROW|wxALL, 0);
      #ifdef __USE_STC_EDITOR__
      newChild->SetEditor(scriptEventPanel->GetEditor());
      #else
      newChild->SetScriptTextCtrl(scriptEventPanel->mFileContentsTextCtrl);
      #endif
      break;
   }
   case GmatTree::ASSIGNMENT:
      sizer->Add(new AssignmentPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::OTHER_COMMAND:
      sizer->Add(new GmatCommandPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;

   default:
      #ifdef DEBUG_CREATE_CHILD
      MessageInterface::ShowMessage
         ("GmatMainFrame::CreateNewCommand() returning NULL\n");
      #endif
      return NULL;
   }

   scrolledWin->SetScrollRate(5, 5);
   scrolledWin->SetAutoLayout(TRUE);
   scrolledWin->SetSizer(sizer);
   sizer->Fit(scrolledWin);
   sizer->SetSizeHints(scrolledWin);

   #ifdef __USE_CHILD_BEST_SIZE__
   if (itemType != GmatTree::SCRIPT_EVENT)
   {
      wxSize bestSize = newChild->GetBestSize();
      newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
   }
   else
   {
      #ifndef __WXMSW__
      wxSize bestSize = newChild->GetBestSize();
      newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
      #endif
   }
   #endif

   // list of open children
   theMdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif

   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateNewCommand() returning <%p>\n", newChild);
   #endif
   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewControl(const wxString &title, const wxString &name,
//                                     GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewControl(const wxString &title, const wxString &name,
                                GmatTree::ItemType itemType, GmatCommand *cmd)
{
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, name, title, itemType);
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
   case GmatTree::IF_CONTROL:
   case GmatTree::WHILE_CONTROL:
      sizer->Add(new ConditionPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
      //case GmatTree::ELSE_IF_CONTROL:
      //sizer->Add(new ElseIfPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      //break;
   case GmatTree::FOR_CONTROL:
      sizer->Add(new ForPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
      //case GmatTree::DO_CONTROL:
      //sizer->Add(new DoWhilePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      //break;
      //case GmatTree::SWITCH_CONTROL:
      //sizer->Add(new SwitchPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      //break;
   default:
      return NULL;
   }

   scrolledWin->SetScrollRate(5, 5);
   scrolledWin->SetAutoLayout(TRUE);
   scrolledWin->SetSizer(sizer);
   sizer->Fit(scrolledWin);
   sizer->SetSizeHints(scrolledWin);

   #ifdef __USE_CHILD_BEST_SIZE__
   wxSize bestSize = newChild->GetBestSize();
   newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
   #endif

   // list of open children
   theMdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif

   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewOutput(const wxString &title, const wxString &name,
//                                    GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewOutput(const wxString &title, const wxString &name,
                               GmatTree::ItemType itemType)
{
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateNewOutput() title=%s, name=%s, itemType=%d\n",
       title.c_str(), name.c_str(), itemType);
   #endif
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild;
   wxScrolledWindow *scrolledWin;

   switch (itemType)
   {
   case GmatTree::OUTPUT_REPORT:
      {
         GmatBase *obj = (Subscriber*) theGuiInterpreter->GetConfiguredObject(name.c_str());
         if ((!obj) || !(obj->IsOfType("Subscriber")))
         {
            wxMessageBox(wxT("The object \"" + name + "\" cannot be found or is not a Subscriber - cannot obtain report file panel position and size.\n"),
                         wxT("GMAT Error"));
         }
         Subscriber *sub = (Subscriber*) obj;
         // Get the position and size of the report file panel
         Real positionX = sub->GetRealParameter(sub->GetParameterID("UpperLeft"),0);
         Real positionY = sub->GetRealParameter(sub->GetParameterID("UpperLeft"),1);
         Real width     = sub->GetRealParameter(sub->GetParameterID("Size"),0);
         Real height    = sub->GetRealParameter(sub->GetParameterID("Size"),1);

         Integer x = 0.0;
         Integer y = 0.0;
         Integer w = 0.0;
         Integer h = 0.0;
         bool isUsingSaved = false;
         // ********* replace this with cross-platform code *********
         Integer screenWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
         Integer screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
         // if position and size were not saved from an earlier run, figure out the initial values
         if (GmatMathUtil::IsEqual(positionX,0.0) && GmatMathUtil::IsEqual(positionY,0.0) &&
             GmatMathUtil::IsEqual(width,0.0)     && GmatMathUtil::IsEqual(height,0.0))
         {
            #ifdef __WXMAC__
               x = -1;   // @todo - move it so it doesn't come up in the upper left corner on top of the main frame
               y = -1;
               w = -1;
               h = -1;
            #else
               w = (Integer)((Real)screenWidth / 3.0);
               h = (Integer)((Real)screenHeight / 2.5);
               x = -1;
               y = -1;
            #endif
         }
         else
         {
            x = (Integer) (positionX * (Real) screenWidth);
            y = (Integer) (positionY * (Real) screenHeight);
            w = (Integer) (width     * (Real) screenWidth);
            h = (Integer) (height    * (Real) screenHeight);
            isUsingSaved = true;
         }
         #ifndef __WXMAC__
            Real realW = (Real)screenWidth;
            Real realH = (Real)screenHeight;
            Integer xOffset = (Integer)((realW * 0.01) + (10000.0 / realW));
            Integer yOffset = (Integer)((realH * 0.06) + (10000.0 / realH));
            if (x == -1) x = 0;
            //else x -= xOffset;
            y -= yOffset;
            #ifdef DEBUG_CHILD_PERSISTENCY
            MessageInterface::ShowMessage("   screen offset: x = %4d, y = %4d\n", xOffset, yOffset);
            MessageInterface::ShowMessage("   after offset : x = %4d, y = %4d\n", x, y);
            #endif
         #endif

         newChild = new GmatMdiChildFrame(this, name, title, itemType, -1, wxPoint(x,y), wxSize(w,h));
         scrolledWin = new wxScrolledWindow(newChild);
         ReportFilePanel *reportPanel = new ReportFilePanel(scrolledWin, name);
         sizer->Add(reportPanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(reportPanel->mFileContentsTextCtrl);
         newChild->SetSavedConfigFlag(isUsingSaved);
         break;
      }
   case GmatTree::COMPARE_REPORT:
      {
         newChild = new GmatMdiChildFrame(this, name, title, itemType); // -1, wxPoint, wxSize
         scrolledWin = new wxScrolledWindow(newChild);
         CompareReportPanel *comparePanel = new CompareReportPanel(scrolledWin, name);
         sizer->Add(comparePanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(comparePanel->GetTextCtrl());
         break;
      }
   default:
      return NULL;
   }

   scrolledWin->SetScrollRate(5, 5);
   scrolledWin->SetAutoLayout(TRUE);
   scrolledWin->SetSizer(sizer);
   sizer->Fit(scrolledWin);
   sizer->SetSizeHints(scrolledWin);

   // list of open children
   theMdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif

   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateUndockedMissionPanel(GmatTree::ItemType itemType, ...)
//------------------------------------------------------------------------------
/**
 * Creates MissionTreePage as it appears in the GmatNotebook.
 * This panel is created when user undocks the MissionPage from the GmatNotebook
 * by dragging the tab.
 */
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateUndockedMissionPanel(const wxString &title,
                                          const wxString &name,
                                          GmatTree::ItemType itemType)
{
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateUndockedMissionPanel() title=%s, name=%s, itemType=%d\n",
       title.c_str(), name.c_str(), itemType);
   #endif
   
   // Get config data from the personalization file
   Integer x = -1, y = -1, w = -1, h = -1;
   mUndockedMissionTreePresized = GetConfigurationData("MissionTree", x, y, w, h);

   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, name, title, itemType, -1, wxPoint(x, y), wxSize(w, h));
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
   case GmatTree::MISSION_TREE_UNDOCKED:
      {
         UndockedMissionPanel *mtPanel = new UndockedMissionPanel(scrolledWin, name);
         MissionTree *newMissionTree = mtPanel->GetMissionTree();
         
         // Now GMAT will work with new mission tree, so set appropriate pointers
         #ifdef DEBUG_MISSION_TREE
         MessageInterface::ShowMessage("   newMissionTree=<%p>\n", newMissionTree);
         #endif
         
         mtPanel->SetGmatNotebook(theNotebook);
         newMissionTree->SetMainFrame(this);
         newMissionTree->SetNotebook(theNotebook);
         GmatAppData::Instance()->SetMissionTree(newMissionTree);
         sizer->Add(mtPanel, 0, wxGROW|wxALL, 0);
         break;
      }
   default:
      return NULL;
   }
   
   scrolledWin->SetScrollRate(5, 5);
   scrolledWin->SetAutoLayout(TRUE);
   scrolledWin->SetSizer(sizer);
   sizer->Fit(scrolledWin);
   sizer->SetSizeHints(scrolledWin);
   
   #ifdef __USE_CHILD_BEST_SIZE__
   if (itemType != GmatTree::SCRIPT_FILE)
   {
      if (!((newChild->GetItemType() == GmatTree::MISSION_TREE_UNDOCKED) && mUndockedMissionTreePresized))
      {
         wxSize bestSize = newChild->GetBestSize();
         newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
      }
   }
   else
   {
      #ifndef __WXMSW__
      wxSize bestSize = newChild->GetBestSize();
      newChild->SetSize(bestSize.GetWidth(), bestSize.GetHeight());
      #endif
   }
   #endif
   
   // list of open children
   theMdiChildren->Append(newChild);
   
   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif

   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateUndockedMissionPanel() returning newChild=<%p>\n", newChild);
   #endif
   return newChild;
}


//------------------------------------------------------------------------------
// void RestoreUndockedMissionPanel()
//------------------------------------------------------------------------------
void GmatMainFrame::RestoreUndockedMissionPanel()
{
   if (GetNumberOfChildOpen(false, false, false) > 0)
   {
      wxNode *node = theMdiChildren->GetFirst();
      while (node)
      {
         GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
         GmatTree::ItemType itemType = theChild->GetItemType();
         if (itemType == GmatTree::MISSION_TREE_UNDOCKED)
         {
            theChild->Restore();
            break;
         }
         
         node = node->GetNext();
      }
   }
}


//------------------------------------------------------------------------------
// void OnNewScript(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles new script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnNewScript(wxCommandEvent& WXUNUSED(event))
{
   // Changed to temp file so that it can be saved using file dialog (LOJ: 2009.01.23)
   wxString name = GmatAppData::Instance()->GetTempScriptName();

   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, name, name, GmatTree::SCRIPT_FILE);
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   #ifdef __USE_STC_EDITOR__
      EditorPanel *editorPanel = new EditorPanel(scrolledWin, name);
      sizer->Add(editorPanel, 0, wxGROW|wxALL, 0);
      newChild->SetEditor(editorPanel->GetEditor());
   #else
      ScriptPanel *scriptPanel = new ScriptPanel(scrolledWin, "");
      sizer->Add(scriptPanel, 0, wxGROW|wxALL, 0);
      newChild->SetScriptTextCtrl(scriptPanel->mFileContentsTextCtrl);
   #endif

   if (newChild && scrolledWin)
   {
       scrolledWin->SetScrollRate(5, 5);
       scrolledWin->SetAutoLayout(TRUE);
       scrolledWin->SetSizer(sizer);
       sizer->Fit(scrolledWin);
       sizer->SetSizeHints(scrolledWin);

       // list of open children
       theMdiChildren->Append(newChild);

       // djc: Under linux, force the new child to display
       #ifndef __WXMSW__
          newChild->Show();
       #endif
   }
}


//------------------------------------------------------------------------------
// void OnOpenScript(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenScript(wxCommandEvent& event)
{
   GmatAppData *gmatAppData = GmatAppData::Instance();
   gmatAppData->GetResourceTree()->OnAddScript(event);
   
   if (gmatAppData->GetResourceTree()->WasScriptAdded())
   {
      #ifdef DEBUG_OPEN_SCRIPT
      MessageInterface::ShowMessage
         ("GmatMainFrame::OnOpenScript() mInterpretFailed=%d, "
          "HasConfigurationChanged=%d\n", mInterpretFailed,
          theGuiInterpreter->HasConfigurationChanged());
      #endif

      wxString scriptName = gmatAppData->GetResourceTree()->GetLastScriptAdded();
      SetScriptFileName(scriptName.c_str());
      
      if (!mInterpretFailed && theGuiInterpreter->HasConfigurationChanged())
      {
          // need to save new file name because it gets overwritten in save
          std::string tmpFilename = mScriptFilename;

          // ask user to continue because changes will be lost
          if (wxMessageBox(_T("Changes will be lost.\nDo you want to save the current script?"),
             _T("Please confirm"),
             wxICON_QUESTION | wxYES_NO) == wxYES)
          {
             OnSaveScriptAs(event);
          }

          mScriptFilename = tmpFilename;
      }

      SetStatusText("", 2);
      InterpretScript(mScriptFilename.c_str(), GmatGui::OPEN_SCRIPT_ON_ERROR, true);
   }
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript1(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenRecentScript1(wxCommandEvent& event)
{
   OpenRecentScript(0, event);
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript2(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenRecentScript2(wxCommandEvent& event)
{
   OpenRecentScript(1, event);
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript3(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenRecentScript3(wxCommandEvent& event)
{
   OpenRecentScript(2, event);
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript4(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenRecentScript4(wxCommandEvent& event)
{
   OpenRecentScript(3, event);
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript5(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenRecentScript5(wxCommandEvent& event)
{
   OpenRecentScript(4, event);
}


//------------------------------------------------------------------------------
// void OnSetPath(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles setting path used in the system from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnSetPath(wxCommandEvent& event)
{
   SetPathDialog dlg(this);
   dlg.ShowModal();
}


//------------------------------------------------------------------------------
// void OnOpenMatlab(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening matlab from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnOpenMatlab(wxCommandEvent& event)
{
   wxBeginBusyCursor();
   theGuiInterpreter->OpenMatlabEngine();
   wxEndBusyCursor();
}


//------------------------------------------------------------------------------
// void OnCloseMatlab(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles closing matlab from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnCloseMatlab(wxCommandEvent& event)
{
   theGuiInterpreter->CloseMatlabEngine();
}


//------------------------------------------------------------------------------
// void OnMatlabServerStart(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles starting server from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnMatlabServerStart(wxCommandEvent& event)
{
   StartMatlabServer();
}


//------------------------------------------------------------------------------
// void OnMatlabServerStop(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles terminating server from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnMatlabServerStop(wxCommandEvent& event)
{
   StopMatlabServer();
}


//------------------------------------------------------------------------------
// void OnFileCompareNumeric(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles comparing two files numerically
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnFileCompareNumeric(wxCommandEvent& event)
{
   CompareFilesDialog dlg(this);
   dlg.ShowModal();

   if (!dlg.CompareFiles())
      return;

   Integer numDirsToCompare = dlg.GetNumDirsToCompare();
   if (numDirsToCompare <= 0)
      return;

   Integer numFilesToCompare = dlg.GetNumFilesToCompare();
   if (numFilesToCompare <= 0)
      return;

   wxString baseDir = dlg.GetBaseDirectory();
   wxArrayString compDirs = dlg.GetCompareDirectories();
   wxString baseStr = dlg.GetBaseString();
   wxArrayString compareStrs = dlg.GetCompareStrings();
   Real absTol = dlg.GetAbsTolerance();
   bool saveCompareResults = dlg.SaveCompareResults();
   wxString saveFileName = dlg.GetSaveFilename();

   #ifdef DEBUG_FILE_COMPARE
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnFileCompare() baseDir=%s, compareStrs[0]=%s\n   "
       "compDirs[0]=%s\n", baseDir.c_str(), compareStrs[0].c_str(),
       compDirs[0].c_str());
   MessageInterface::ShowMessage
      ("   numDirsToCompare=%d, numFilesToCompare=%d\n", numDirsToCompare,
       numFilesToCompare);
   #endif

   wxTextCtrl *textCtrl = NULL;
   wxString compareStr = compareStrs[0];
   wxString dir1 = compDirs[0];

   GmatMdiChildFrame *textFrame = GetChild("CompareReport");

   if (textFrame == NULL)
   {
      GmatTreeItemData *compareItem =
         new GmatTreeItemData("CompareReport", GmatTree::COMPARE_REPORT);

      textFrame = CreateChild(compareItem);
   }

   textCtrl = textFrame->GetScriptTextCtrl();
   textCtrl->SetMaxLength(320000); // make long enough
   textFrame->Show();
   wxString msg;
   msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);
   textCtrl->AppendText(msg);

   //loj: Why Do I need to do this to show whole TextCtrl?
   // textFrame->Layout() didn't work.
   int w, h;
   textFrame->GetSize(&w, &h);
   textFrame->SetSize(w+1, h+1);

   // Get files in the base directory
   wxDir dir(baseDir);
   wxString filename;
   wxString filepath;
   wxArrayString baseFileNameArray;

   //How do I specify multiple file ext?
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt"))
      {
         if (filename.Contains(baseStr))
         {
            filepath = baseDir + "/" + filename;

            // remove any backup files
            if (filename.Last() == 't')
               baseFileNameArray.push_back(filepath.c_str());
         }
      }

      cont = dir.GetNext(&filename);
   }

   StringArray colTitles;
   wxString tempStr;
   int fileCount = 0;
   wxString baseFileName;

   // Now call compare utility
   for (UnsignedInt i=0; i<baseFileNameArray.size(); i++)
   {
      if (fileCount > numFilesToCompare)
         break;

      tempStr.Printf("%d", i+1);
      textCtrl->AppendText("==> File Compare Count: " + tempStr + "\n");

      baseFileName = baseFileNameArray[i];
      wxFileName filename(baseFileName);

      wxArrayString compareNames;

      for (int j=0; j<numDirsToCompare; j++)
      {
         compareNames.Add(filename.GetFullName());
         compareStr = compareStrs[j];
         size_t numReplaced = compareNames[j].Replace(baseStr, compareStr.c_str());

         if (numReplaced == 0)
         {
            textCtrl->AppendText
               ("***Cannot compare results. The report file doesn't contain " +
                baseStr + "\n");
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() Cannot compare results.\n"
                "The report file doesn't contain %s.\n\n", baseStr.c_str());

            fileCount++;
            continue;
         }

         if (numReplaced > 1)
         {
            textCtrl->AppendText
               ("***Cannot compare results. The report file name contains more "
                "than 1 " + baseStr + " string.\n");
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() Cannot compare results.\n"
                "The report file name contains more than 1 %s string.\n\n",
                baseStr.c_str());
            //return;
            fileCount++;
            continue;
         }
      }

      // set compare file names
      wxString filename1;
      wxString filename2;
      wxString filename3;

      if (numDirsToCompare >= 1)
         filename1 = compDirs[0] + "/" + compareNames[0];

      if (numDirsToCompare >= 2)
         filename2 = compDirs[1] + "/" + compareNames[1];

      if (numDirsToCompare >= 3)
         filename3 = compDirs[2] + "/" + compareNames[2];

      StringArray output;

      if (numDirsToCompare == 1)
         output =
            GmatFileUtil::Compare(baseFileName.c_str(), filename1.c_str(),
                                  colTitles, absTol);
      else
         output =
            GmatFileUtil::Compare(numDirsToCompare, baseFileName.c_str(), filename1.c_str(),
                                  filename2.c_str(), filename3.c_str(), colTitles, absTol);

      // append text
      for (unsigned int i=0; i<output.size(); i++)
         textCtrl->AppendText(wxString(output[i].c_str()));

      textCtrl->AppendText
         ("========================================================\n\n");

      fileCount++;
   }

   if (fileCount == 0)
   {
      textCtrl->AppendText("** There is no report file to compare.\n\n");
      MessageInterface::ShowMessage("** There is no report file to compare.\n");
   }
   else
   {
      if (saveCompareResults)
         textCtrl->SaveFile(saveFileName);
   }
}


//------------------------------------------------------------------------------
// void OnFileCompareText(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles comparing two files line by line.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnFileCompareText(wxCommandEvent& event)
{
   CompareTextDialog dlg(this);
   dlg.ShowModal();

   if (!dlg.CompareFiles())
      return;

   Integer numDirsToCompare = dlg.GetNumDirsToCompare();
   if (numDirsToCompare <= 0)
      return;

   Integer numFilesToCompare = dlg.GetNumFilesToCompare();
   if (numFilesToCompare <= 0)
      return;

   wxString baseDir = dlg.GetBaseDirectory();
   wxArrayString compDirs = dlg.GetCompareDirectories();
   bool saveCompareResults = dlg.SaveCompareResults();
   wxString saveFileName = dlg.GetSaveFilename();
   wxString basePrefix = dlg.GetBasePrefix();
   wxArrayString compPrefixes = dlg.GetComparePrefixes();

   #ifdef DEBUG_FILE_COMPARE
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnFileCompareText() baseDir=%s\n   "
       "compDirs[0]=%s\n", baseDir.c_str(), compDirs[0].c_str());
   MessageInterface::ShowMessage
      ("   basePrefix='%s', comparePrefixex[0]='%s', numDirsToCompare=%d, "
       "numFilesToCompare=%d\n", basePrefix.c_str(), compPrefixes[0].c_str(),
       numDirsToCompare, numFilesToCompare);
   #endif

   wxTextCtrl *textCtrl = NULL;
   wxString dir1 = compDirs[0];

   GmatMdiChildFrame *textFrame = GetChild("CompareReport");

   if (textFrame == NULL)
   {
      GmatTreeItemData *compareItem =
         new GmatTreeItemData("CompareReport", GmatTree::COMPARE_REPORT);

      textFrame = CreateChild(compareItem);
   }

   textCtrl = textFrame->GetScriptTextCtrl();
   textCtrl->SetMaxLength(320000); // make long enough
   textFrame->Show();
   wxString msg;
   msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);
   textCtrl->AppendText(msg);

   //loj: Why Do I need to do this to show whole TextCtrl?
   // textFrame->Layout() didn't work.
   int w, h;
   textFrame->GetSize(&w, &h);
   textFrame->SetSize(w+1, h+1);

   // Get files in the base directory
   wxDir dir(baseDir);
   wxString filename;
   wxString filepath;
   wxArrayString baseFileNameArray;
   wxArrayString noPrefixNameArray;
   size_t prefixLen = basePrefix.Len();

   //How do I specify multiple file ext?
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt") ||
          filename.Contains(".data") || filename.Contains(".script") ||
          filename.Contains(".eph"))
      {
         // if file has prefix
         if (filename.Left(prefixLen) == basePrefix)
         {
            filepath = baseDir + "/" + filename;

            // remove any backup files
            if (filename.Last() == 't' || filename.Last() == 'a' ||
                filename.Last() == 'h')
            {
               wxString noPrefixName = filename;
               noPrefixName.Replace(basePrefix, "", false);
               noPrefixNameArray.push_back(noPrefixName.c_str());
               baseFileNameArray.push_back(filepath.c_str());
            }
         }
      }

      cont = dir.GetNext(&filename);
   }

   wxString tempStr;
   int fileCount = 0;
   wxString noPrefixName, baseFileName;
   int file1DiffCount = 0;
   int file2DiffCount = 0;
   int file3DiffCount = 0;
   wxString summary;
   std::string cannotOpen;

   // Now call compare utility
   for (UnsignedInt i=0; i<baseFileNameArray.size(); i++)
   {
      if (fileCount > numFilesToCompare)
         break;

      tempStr.Printf("%d", i+1);
      textCtrl->AppendText("==> File Compare Count: " + tempStr + "\n");

      baseFileName = baseFileNameArray[i];
      noPrefixName = noPrefixNameArray[i];
      wxFileName filename(noPrefixName);
      wxArrayString compareNames;

      #ifdef DEBUG_FILE_COMPARE
      MessageInterface::ShowMessage("   baseFileName='%s'\n", baseFileName.c_str());
      MessageInterface::ShowMessage("   noPrefixName='%s'\n", noPrefixName.c_str());
      #endif

      for (int j=0; j<numDirsToCompare; j++)
      {
         compareNames.Add(filename.GetFullName());

         #ifdef DEBUG_FILE_COMPARE
         MessageInterface::ShowMessage
            ("   compareNames[%d]='%s'\n", j, compareNames[j].c_str());
         #endif
      }

      // set compare file names
      wxString filename1;
      wxString filename2;
      wxString filename3;

      if (numDirsToCompare >= 1)
         filename1 = compDirs[0] + "/" + compPrefixes[0] + compareNames[0];

      if (numDirsToCompare >= 2)
         filename2 = compDirs[1] + "/" + compPrefixes[1] + compareNames[1];

      if (numDirsToCompare >= 3)
         filename3 = compDirs[2] + "/" + compPrefixes[2] + compareNames[2];

      StringArray output;

      output =
         GmatFileUtil::CompareLines(numDirsToCompare, baseFileName.c_str(), filename1.c_str(),
                                    filename2.c_str(), filename3.c_str(), file1DiffCount,
                                    file2DiffCount, file3DiffCount);

      for (UnsignedInt i=0; i<output.size(); i++)
      {
         if (output[i].find("Cannot open") != std::string::npos)
         {
            cannotOpen = cannotOpen + output[i];
            break;
         }
      }

      wxString str;
      // for summary array
      if (file1DiffCount > 0)
      {
         str.Printf("%s: %d\n", filename1.c_str(), file1DiffCount);
         summary = summary + str;
      }

      if (file2DiffCount > 0)
      {
         str.Printf("%s: %d\n", filename2.c_str(), file2DiffCount);
         summary = summary + str;
      }

      if (file3DiffCount > 0)
      {
         str.Printf("%s: %d\n", filename3.c_str(), file3DiffCount);
         summary = summary + str;
      }

      // append text
      for (unsigned int i=0; i<output.size(); i++)
      {
         textCtrl->AppendText(wxString(output[i].c_str()));
         textCtrl->AppendText("");
      }

      textCtrl->AppendText
         ("========================================================\n\n");

      fileCount++;
   }

   if (fileCount == 0)
   {
      textCtrl->AppendText("** There is no report file to compare.\n\n");
      MessageInterface::ShowMessage("** There is no files to compare.\n");
   }
   else
   {
      // show summary report of compare
      textCtrl->AppendText("The following files are different:\n\n");
      textCtrl->AppendText(summary);

      // show non-existant reports
      if (cannotOpen != "")
      {
         textCtrl->AppendText("\n\n");
         textCtrl->AppendText(cannotOpen.c_str());
      }

      textCtrl->AppendText
         ("========================================================\n\n");

      if (saveCompareResults)
         textCtrl->SaveFile(saveFileName);
   }
}


//------------------------------------------------------------------------------
// void OnGenerateTextEphemFile(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles comparing two files
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnGenerateTextEphemFile(wxCommandEvent& event)
{
   TextEphemFileDialog dlg(this);
   dlg.ShowModal();

   if (dlg.CreateEphemFile())
      RunCurrentMission();
}


//------------------------------------------------------------------------------
// void OnSashDrag(wxSashEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnSashDrag(wxSashEvent& event)
{
   int w, h;
   GetClientSize(&w, &h);

   if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
      return;

   #ifdef DEBUG_SASH_DRAG
   int newW = event.GetDragRect().width;
   int minW = theMainWin->GetMinimumSizeX();
   int maxW = theMainWin->GetMaximumSizeX();
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnSashDrag() minW=%d, maxW=%d, setting new width to %d\n",
       minW, maxW, newW);
   #endif

   theMainWin->SetDefaultSize(wxSize(event.GetDragRect().width, h));

   wxLayoutAlgorithm layout;
   layout.LayoutMDIFrame(this);

   // Leaves bits of itself behind sometimes
   GetClientWindow()->Refresh();
}


//------------------------------------------------------------------------------
// void OnMsgSashDrag(wxSashEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnMsgSashDrag(wxSashEvent& event)
{
   int w, h;
   GetClientSize(&w, &h);

   if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
      return;

   #ifdef DEBUG_SASH_DRAG
   int newH = event.GetDragRect().height;
   int minH = theMessageWin->GetMinimumSizeY();
   int maxH = theMessageWin->GetMaximumSizeY();
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnMsgSashDrag() minH=%d, maxH=%d, setting new height to %d\n",
       minH, maxH, newH);
   #endif

   theMessageWin->SetDefaultSize(wxSize(w, event.GetDragRect().height));

   wxLayoutAlgorithm layout;
   layout.LayoutMDIFrame(this);

   // Leaves bits of itself behind sometimes
   GetClientWindow()->Refresh();
}


//------------------------------------------------------------------------------
// void OnMainFrameSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles resizing of the window
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnMainFrameSize(wxSizeEvent& event)
{
   int w, h;
   GetClientSize(&w, &h);

   #ifdef DEBUG_SIZE
   MessageInterface::ShowMessage("GmatMainFrame::OnMainFrameSize() entered\n");
   MessageInterface::ShowMessage("   client size w=%d, h=%d\n", w, h);
   #endif

   // adjust new maximum SashWindow size
   if (theMessageWin != NULL && theMainWin != NULL)
   {
      theMessageWin->SetMaximumSizeY(h-20);
      theMainWin->SetMaximumSizeX(w-20);
   }

   wxLayoutAlgorithm layout;
   layout.LayoutMDIFrame(this);
   
   #ifdef DEBUG_SIZE
   MessageInterface::ShowMessage("GmatMainFrame::OnMainFrameSize() leaving\n");
   MessageInterface::ShowMessage("   client size w=%d, h=%d\n", w, h);
   #endif
}


//------------------------------------------------------------------------------
// void OnSetFocus(wxFocusEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles set focus event of the window
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnSetFocus(wxFocusEvent& event)
{
   wxYield();
   event.Skip(true);
}


//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes wxKeyEvent.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnKeyDown(wxKeyEvent &event)
{
   int keyDown = event.GetKeyCode();
   if (keyDown == WXK_ESCAPE)
      StopRunningMission();
}


//------------------------------------------------------------------------------
// void UpdateRecentMenu(bool openOn)
//------------------------------------------------------------------------------
/*
 * Enables or disables menu File->Open item
 */
//------------------------------------------------------------------------------
void GmatMainFrame::UpdateRecentMenu(wxArrayString files)
{
   ((GmatMenuBar *) theMenuBar)->UpdateRecentMenu(files);
}


//------------------------------------------------------------------------------
// void UpdateMenus(bool openOn)
//------------------------------------------------------------------------------
/*
 * Enables or disables menu File->Open item
 */
//------------------------------------------------------------------------------
void GmatMainFrame::UpdateMenus(bool openOn)
{
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      child->GetMenuBar()->Enable(MENU_FILE_OPEN_SCRIPT, openOn);
      node = node->GetNext();
   }
}


//------------------------------------------------------------------------------
// void EnableMenuAndToolBar(bool enable, bool missionRunning, bool forAnimation)
//------------------------------------------------------------------------------
/*
 * Enables menu items and tool bar icons. Usuallay menus and icons will be
 * disabled when the mission run started and enabled after the run completes.
 *
 * @param <enable> true to enable the menu and tool icons, false to disable them
 * @param <missionRunning> true if mission is running, this will toggle pause
 *                         and stop icons
 * @param <forAnimation> true if icons are for animation, this will toggle animation
 *                       play icon
 */
//------------------------------------------------------------------------------
void GmatMainFrame::EnableMenuAndToolBar(bool enable, bool missionRunning,
                                         bool forAnimation)
{
   #if DBGLVL_MENUBAR
   MessageInterface::ShowMessage
      ("GmatMainFrame::EnableMenuAndToolBar() enable=%d, missionRunning=%d, "
       "forAnimation=%d\n", enable, missionRunning, forAnimation);
   #endif

   wxToolBar *toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_RUN, enable);
   toolBar->EnableTool(TOOL_PAUSE, enable);
   toolBar->EnableTool(TOOL_STOP, enable);

   if (missionRunning)
   {
      toolBar->EnableTool(TOOL_PAUSE, !enable);
      toolBar->EnableTool(TOOL_STOP, !enable);
      toolBar->EnableTool(TOOL_SCREENSHOT, true);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_PLAY, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_STOP, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_FAST, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_SLOW, false);
   }
   else
   {
      toolBar->EnableTool(TOOL_PAUSE, false);
      toolBar->EnableTool(TOOL_STOP, false);
   }

   if (forAnimation)
      toolBar->ToggleTool(TOOL_ANIMATION_PLAY, !enable);

   toolBar->EnableTool(TOOL_CLOSE_CHILDREN, enable);
   toolBar->EnableTool(TOOL_CLOSE_CURRENT, enable);

   toolBar->EnableTool(MENU_FILE_NEW_SCRIPT, enable);
   toolBar->EnableTool(MENU_FILE_OPEN_SCRIPT, enable);
   toolBar->EnableTool(MENU_FILE_SAVE_SCRIPT, enable);
   toolBar->EnableTool(MENU_LOAD_DEFAULT_MISSION, enable);

   //-----------------------------------
   // Enable child mdi menu bar first
   //-----------------------------------
   GmatMdiChildFrame *child = (GmatMdiChildFrame*)GetActiveChild();
   if (child != NULL)
   {
      child->UpdateGuiItem(true, true);
      wxMenuBar *childMenuBar = child->GetMenuBar();

      #if DBGLVL_MENUBAR > 1
      MessageInterface::ShowMessage("   ==childMenuBar=%p\n", childMenuBar);
      #endif

      int helpIndex = childMenuBar->FindMenu("Help");
      int childMenuCount = childMenuBar->GetMenuCount();

      for (int i=0; i<childMenuCount; i++)
      {
         // Update except Help menu
         if (i != helpIndex)
            childMenuBar->EnableTop(i, enable);
      }
   }

   //-----------------------------------
   // Enable parent mdi menu bar second
   //-----------------------------------
   int parentMenuCount = theMenuBar->GetMenuCount();
   int helpIndex = theMenuBar->FindMenu("Help");

   for (int i=0; i<parentMenuCount; i++)
   {
      // Update except Help menu
      if (i != helpIndex)
         theMenuBar->EnableTop(i, enable);
   }

   //-----------------------------------
   // Always Disable parent Edit menu
   //-----------------------------------
   int editIndex = theMenuBar->FindMenu("Edit");
   theMenuBar->EnableTop(editIndex, false);
}

//------------------------------------------------------------------------------
// void OnScreenShot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScreenshot(wxCommandEvent& WXUNUSED(event))
{
   char ImageFilename[255] = {0};
   char ImagePath[255] = {0};
   char VerNum[] = {"_%03d.png"};
   char NameVer[12] = {0};
   // Retrieve an instance of the file manager
   FileManager *fm = FileManager::Instance();

   // Keep looking until we do not find an existing file
   for (int ii = 1; ii < 1000; ii++){
      strcpy(ImagePath, fm->GetPathname(FileManager::OUTPUT_PATH).c_str());
      if (!wxDirExists(ImagePath))
         wxMkdir(ImagePath);
      strcpy(ImageFilename, fm->GetFilename("SCREENSHOT_FILE").c_str());
     // Look in the output path for the screenshot file with the current version number
      sprintf(NameVer, VerNum, ii);
      strncat(ImageFilename, NameVer, strlen(NameVer));
      strcat(ImagePath, ImageFilename);
      // If the file isn't there, we've found the next file index and break
      if (!wxFileExists(ImagePath))
         break;
   }
   ScreenShotSave(ImagePath);
}

//------------------------------------------------------------------------------
// void OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
   wxString filename = mScriptFilename.c_str();
   
   InterpretScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT);
}


//------------------------------------------------------------------------------
// void OnScriptBuildAndRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildAndRun(wxCommandEvent& event)
{
   wxString filename = mScriptFilename.c_str();
   
   if (InterpretScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT))
      mRunStatus = RunCurrentMission();
}


//------------------------------------------------------------------------------
// bool OnScriptRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * @note We should have an option to clear message each run. If it runs for
 *       a long time (days, months, etc), we will not see all the output written
 *       to the message window.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptRun(wxCommandEvent& WXUNUSED(event))
{
   //MessageInterface::ShowMessage("===> GmatMainFrame::OnScriptRun()\n");
   mRunStatus = RunCurrentMission();
}


//------------------------------------------------------------------------------
// bool SetScriptFileName(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Sets current script file name.
 *
 * @param  filename  The script file name
 * @return true if script file name set to new one, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::SetScriptFileName(const std::string &filename)
{
   if (!mRunCompleted)
   {
      // We also want to log this message, so use PopupMessage()
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "GMAT is still running the mission.\n"
          "Please STOP before reading a new script.");
      
      //wxMessageBox(wxT("GMAT is still running the mission.\n"
      //                 "Please STOP before reading a new script."),
      //             wxT("GMAT Warning"));
      return false;
   }
   
   mScriptFilename = filename;
   return true;
}


//------------------------------------------------------------------------------
// bool IsActiveScriptModified()
//------------------------------------------------------------------------------
bool GmatMainFrame::IsActiveScriptModified()
{
   return true;  // for testing only
}


//------------------------------------------------------------------------------
// void RefreshActiveScript(const wxString &filename)
//------------------------------------------------------------------------------
/**
 * Refreshes active script by making active script bold face in the ResourceTree,
 * updates active script status on any opened script panel.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::RefreshActiveScript(const wxString &filename)
{
   #ifdef DEBUG_REFRESH_SCRIPT
   MessageInterface::ShowMessage
      ("GmatMainFrame::RefreshActiveScript() entered, filename='%s'\n",
       filename.c_str());
   #endif
   
   // If scripts are running batch mode, just skip
   if (GmatGlobal::Instance()->IsBatchMode())
      return;
   
   // Show active script in bold in the ResourceTree (LOJ: 2010.12.27)
   GmatAppData::Instance()->GetResourceTree()->SetActiveScript(filename);
   UpdateTitle(filename);
   
   // Update active script status on any opened script panels.
   #ifdef DEBUG_REFRESH_SCRIPT
   MessageInterface::ShowMessage
      ("GmatMainFrame::RefreshActiveScript() now updating active script status "
       "on opened script panels\n");
   #endif
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      #ifdef DEBUG_REFRESH_SCRIPT
      MessageInterface::ShowMessage("   node = %p\n", node);
      #endif
      
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      
      wxString title = child->GetTitle();
      wxString name = child->GetName();
      GmatTree::ItemType type = child->GetItemType();
      
      #ifdef DEBUG_REFRESH_SCRIPT
      MessageInterface::ShowMessage("   name = %s, type = %d\n", name.c_str(), type);
      #endif
      
      if (type >= GmatTree::BEGIN_OF_RESOURCE && type <= GmatTree::END_OF_RESOURCE)
      {
         // Check if script file is opened
         if (type == GmatTree::SCRIPT_FILE)
         {
            if (child->GetAssociatedWindow() != NULL)
            {
               if (GmatFileUtil::IsSameFileName(name.c_str(), filename.c_str()))
               {
                  #ifdef DEBUG_REFRESH_SCRIPT
                  MessageInterface::ShowMessage("   Setting the script as ACTIVE\n");
                  #endif
                  
                  // We don't wan't to reload the file, so commented out (LOJ: 2011.05.20)
                  //((GmatSavePanel*)child->GetAssociatedWindow())->ReloadFile();
                  ((GmatSavePanel*)child->GetAssociatedWindow())->UpdateScriptActiveStatus(true);
               }
               else
               {
                  #ifdef DEBUG_REFRESH_SCRIPT
                  MessageInterface::ShowMessage("   Setting the script as INACTIVE\n");
                  #endif
                  ((GmatSavePanel*)child->GetAssociatedWindow())->UpdateScriptActiveStatus(false);
               }
            }
         }
      }
      
      node = node->GetNext();
   }
   
   #ifdef DEBUG_REFRESH_SCRIPT
   MessageInterface::ShowMessage
      ("GmatMainFrame::RefreshActiveScript() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// std::string GetActiveScriptFileName();
//------------------------------------------------------------------------------
std::string GmatMainFrame::GetActiveScriptFileName()
{
   return mScriptFilename;
}


//------------------------------------------------------------------------------
// void UpdateGuiScriptSyncStatus(int guiStatus, int scriptStatus)
//------------------------------------------------------------------------------
/**
 * Calls GmatToolBar to update GUI and Script file synchronization status
 *
 * @param <guiStatus> status of GUI,
 *                       0 = no change, 1 = clean, 2 = dirty, 3 = error
 * @param <scriptStatus>  status of active script,
 *                       0 = no change, 1 = clean, 2 = dirty, 3 = error
 */
//------------------------------------------------------------------------------
void GmatMainFrame::UpdateGuiScriptSyncStatus(int guiStatus, int scriptStatus)
{
   GuiItemManager *guiManager = GuiItemManager::GetInstance();
   if (guiStatus != 0)
      guiManager->SetGuiStatus(guiStatus);
   if (scriptStatus != 0)
      guiManager->SetActiveScriptStatus(scriptStatus);
   
   ((GmatToolBar*)theToolBar)->
      UpdateGuiScriptSyncStatus(theToolBar, guiStatus, scriptStatus);
}


//------------------------------------------------------------------------------
// void OnUndo(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnUndo(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnUndo(event);
#else
   child->GetScriptTextCtrl()->Undo();
#endif
}


//------------------------------------------------------------------------------
// void OnRedo(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnRedo(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnRedo(event);
#else
   child->GetScriptTextCtrl()->Redo();
#endif
}


//------------------------------------------------------------------------------
// void OnCut(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnCut(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnCut(event);
#else
   child->GetScriptTextCtrl()->Cut();
#endif
}


//------------------------------------------------------------------------------
// void OnCopy(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnCopy(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnCopy(event);
#else
   if (child != NULL)
   {
      if (child->GetScriptTextCtrl() != NULL)
         child->GetScriptTextCtrl()->Copy();
      else
         GmatAppData::Instance()->GetMessageTextCtrl()->Copy();
   }
   else
   {
      GmatAppData::Instance()->GetMessageTextCtrl()->Copy();
   }
#endif
}


//------------------------------------------------------------------------------
// void OnPaste(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnPaste(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnPaste(event);
#else
   child->GetScriptTextCtrl()->Paste();
#endif
}


//------------------------------------------------------------------------------
// void OnComment(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnComment(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnComment(event);
#else
   wxTextCtrl *scriptTC = child->GetScriptTextCtrl();
   wxString selString = scriptTC->GetStringSelection();
   selString.Replace("\n", "\n%");
   selString = "%" + selString;

   if (selString.Last() == '%')
      selString = selString.Mid(0, selString.Length()-1);

   scriptTC->WriteText(selString);
#endif

}

//------------------------------------------------------------------------------
// void OnUncomment(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnUncomment(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnUncomment(event);
#else
   wxTextCtrl *scriptTC = child->GetScriptTextCtrl();
   wxString selString = scriptTC->GetStringSelection();

   if (selString.StartsWith("%"))  // gets rid of first %
      selString = selString.Mid(1, selString.Length()-1);

   selString.Replace("\n%", "\n");
   scriptTC->WriteText(selString);
#endif
}


//------------------------------------------------------------------------------
// void OnSelectAll(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnSelectAll(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
#ifdef __USE_STC_EDITOR__
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnSelectAll(event);
#else
   wxTextCtrl *scriptTC = child->GetScriptTextCtrl();
   scriptTC->SetSelection(-1, -1);
#endif
}


//------------------------------------------------------------------------------
// void OnFind(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnFind(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnFind(event);
#endif
}


//------------------------------------------------------------------------------
// void OnFindNext(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnFindNext(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnFindNext(event);
#endif
}


//------------------------------------------------------------------------------
// void OnReplace(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnReplace(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnFind(event);
#endif
}


//------------------------------------------------------------------------------
// void OnReplaceNext(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnReplaceNext(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnReplaceNext(event);
#endif
}


//------------------------------------------------------------------------------
// void OnGoToLine(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnGoToLine(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnGoToLine(event);
#endif
}


//------------------------------------------------------------------------------
// void OnLineNumber(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnLineNumber(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnLineNumber(event);
#endif
}


//------------------------------------------------------------------------------
// void OnIndentMore(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnIndentMore(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnIndentMore(event);
#endif
}


//------------------------------------------------------------------------------
// void OnIndentLess(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnIndentLess(wxCommandEvent& event)
{
#ifdef __USE_STC_EDITOR__
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   Editor *editor = child->GetEditor();
   if (editor)
      editor->OnIndentLess(event);
#endif
}


//------------------------------------------------------------------------------
// void OnFont(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnFont(wxCommandEvent& event)
{
   GmatAppData *gmatAppData = GmatAppData::Instance();
   wxFontData data;
   data.SetInitialFont(gmatAppData->GetFont());

   wxFontDialog dialog(this, data);
   if (dialog.ShowModal() == wxID_OK)
   {
      wxFontData retData = dialog.GetFontData();
      wxFont newFont = retData.GetChosenFont();

      // change all script windows to new font
      wxNode *node = theMdiChildren->GetFirst();
      while (node)
      {
         GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
         if ((child->GetItemType() == GmatTree::SCRIPT_FILE)   ||
             (child->GetItemType() == GmatTree::OUTPUT_REPORT)  ||
             (child->GetItemType() == GmatTree::SCRIPT_EVENT) ||
             (child->GetItemType() == GmatTree::GMAT_FUNCTION))
         {
            child->GetScriptTextCtrl()->SetFont(newFont);
         }
         node = node->GetNext();
      }

      gmatAppData->SetFont(newFont);
   }
}


//------------------------------------------------------------------------------
// void OnAnimation(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnAnimation(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   wxToolBar *toolBar = GetToolBar();

   if (child == NULL)
   {
      toolBar->ToggleTool(TOOL_ANIMATION_PLAY, false);
      return;
   }
   
   // active child is not OrbitView, just return
   if (child->GetItemType() != GmatTree::OUTPUT_ORBIT_VIEW &&
       child->GetItemType() != GmatTree::OUTPUT_GROUND_TRACK_PLOT)
   {
      toolBar->ToggleTool(TOOL_ANIMATION_PLAY, false);
      return;
   }
   
   wxString title = child->GetTitle();
   MdiChildViewFrame *frame = NULL;
   bool frameFound = false;

   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnAnimation() title=%s\n", title.c_str());
   #endif

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      if (frame && (frame->GetPlotName().IsSameAs(title)))
      {
         frameFound = true;
         break;
      }
   }

   if (!frameFound)
      return;

   #ifdef DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("===> Now start animation of %s\n", frame->GetPlotName().c_str());
   #endif
   
   if (!mAnimationEnabled)
   {
      wxString msg = "*** WARNING *** This plot data was published inside a "
         "function, so repainting or drawing animation is disabled.\n";
      MessageInterface::ShowMessage(msg.c_str());
      wxToolBar* toolBar = GetToolBar();
      toolBar->ToggleTool(TOOL_ANIMATION_PLAY, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_PLAY, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_STOP, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_FAST, false);
      toolBar->EnableTool(GmatMenu::TOOL_ANIMATION_SLOW, false);
      mAnimationEnabled = true;
      return;
   }
   
   switch (event.GetId())
   {
   case TOOL_ANIMATION_PLAY:
      frame->SetAnimationUpdateInterval(1);
      frame->SetAnimationFrameIncrement(mAnimationFrameInc);
      frame->RedrawPlot(true);
      break;
   case TOOL_ANIMATION_STOP:
      frame->SetUserInterrupt();
      break;
   case TOOL_ANIMATION_FAST:
      mAnimationFrameInc += 5;
      if (mAnimationFrameInc > 50)
         frame->SetAnimationUpdateInterval(0);
      else
         frame->SetAnimationUpdateInterval(1);
      frame->SetAnimationFrameIncrement(mAnimationFrameInc);
      break;
   case TOOL_ANIMATION_SLOW:
      mAnimationFrameInc -= 5;
      if (mAnimationFrameInc < 0)
         mAnimationFrameInc = 1;
      frame->SetAnimationFrameIncrement(mAnimationFrameInc);
      break;
   case TOOL_ANIMATION_OPTIONS:
      // This is deprecated code
      //frame->OnShowOptionDialog(event);
      break;
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// void UpdateTitle(const wxString &filename)
//------------------------------------------------------------------------------
void GmatMainFrame::UpdateTitle(const wxString &filename)
{
   wxString title;
   if (filename == "")
      title = "General Mission Analysis Tool (GMAT)";
   else
      title.Printf("%s - General Mission Analysis Tool (GMAT)", filename.c_str());

   SetTitle(title);
}


//------------------------------------------------------------------------------
// void SaveGuiToActiveScript()
//------------------------------------------------------------------------------
void GmatMainFrame::SaveGuiToActiveScript()
{
   static int backupCounter = 0;
   static wxString backupFilename;
   
   backupCounter++;
   wxString cntStr;
   cntStr.Printf("%d", backupCounter);
   wxString currFilename = mScriptFilename.c_str();
   
   //backupFilename = currFilename + "." + cntStr + ".bak";
   backupFilename = currFilename + ".bak";
   
   // Create backup file for the first time only and show message
   // instead of popup message (Bug 2409 fix)
   if (backupCounter == 1)
   {
      ::wxCopyFile(currFilename, backupFilename);
      
      #ifdef DEBUG_MAINFRAME_SAVE
      MessageInterface::ShowMessage
         ("GmatMainFrame::SaveGuiToActiveScript() Created backup file: %s\n",
          backupFilename.c_str());
      #endif
      
      MessageInterface::ShowMessage
         ("***** Old script saved to backup file \"%s\"\n", backupFilename.c_str());
   }
   
   SaveChildPositionsAndSizes();
   theGuiInterpreter->SaveScript(mScriptFilename);
}

//------------------------------------------------------------------------------
// void SaveChildPositionsAndSizes()
//------------------------------------------------------------------------------
void GmatMainFrame::SaveChildPositionsAndSizes()
{
   wxNode *node = theMdiChildren->GetFirst();

   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      child->SaveChildPositionAndSize();

      node = node->GetNext();
   }
}

//------------------------------------------------------------------------------
// void GetConfigurationData(const std::string &forItem)
//------------------------------------------------------------------------------
bool GmatMainFrame::GetConfigurationData(const std::string &forItem, Integer &x, Integer &y, Integer &w, Integer &h)
{
   bool isPresetSizeUsed = false;
   // Get configuration data associated with MissionTree or ScriptEditor here
   Real positionX = 0.0, positionY = 0.0;
   Real width     = 0.0, height    = 0.0;

   wxFileConfig *pConfig;
   pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
   std::string upperLeftString = "//";
   upperLeftString += forItem + "//UpperLeft";
   std::string sizeString = "//";
   sizeString += forItem + "//Size";

   wxString treeUpperLeft, treeSize;
   pConfig->Read(upperLeftString.c_str(), &treeUpperLeft, "false");
   pConfig->Read(sizeString.c_str(), &treeSize, "false");
   if ((treeUpperLeft.Lower() == "false") || (treeSize.Lower() == "false"))
   {
      x = -1;   // @todo - move it so it doesn't come up in the upper left corner on top of the main frame
      y = -1;
      w = -1;
      h = -1;
      return isPresetSizeUsed;
   }
   // Compute the location and size for the item
   std::stringstream upperLeft(treeUpperLeft.c_str());
   std::stringstream size(treeSize.c_str());
//   std::string xStr, yStr, wStr, hStr;  // ???????????????
   upperLeft >> positionX >> positionY;
   size      >> width     >> height;

   #ifdef DEBUG_CONFIG_DATA
   MessageInterface::ShowMessage("   config data read for %s  : x = %12.10f, y = %12.10f, w = %12.10f, h = %12.10f\n",
         forItem.c_str(), positionX, positionY, width, height);
   #endif
   Integer screenWidth  = 0;
   Integer screenHeight = 0;

   #ifdef __WXMAC__
      screenWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
      screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
   #else
      GetActualClientSize(&screenWidth, &screenHeight, true);
   #endif

   // Do we need the part with the plotCount, etc. (see GuiPlotReceiver)?????? <<<<<<<<<<<<<<<<<<<<
   #ifdef DEBUG_CONFIG_DATA
   MessageInterface::ShowMessage("   screen size  : w = %4d, h = %4d\n", screenWidth, screenHeight);
   #endif

   isPresetSizeUsed = true;

   x = (Integer) (positionX * (Real) screenWidth);
   y = (Integer) (positionY * (Real) screenHeight);
   w = (Integer) (width     * (Real) screenWidth);
   h = (Integer) (height    * (Real) screenHeight);

   #ifdef DEBUG_CONFIG_DATA
   MessageInterface::ShowMessage("   before offset: x = %4d, y = %4d, w = %4d, h = %4d\n", x, y, w,h);
   #endif

   // Since -1 is default position, change it to 0
   #ifndef __WXMAC__
      Real realW = (Real)screenWidth;
      Real realH = (Real)screenHeight;
      Integer xOffset = (Integer)((realW * 0.01) + (10000.0 / realW));
      Integer yOffset = (Integer)((realH * 0.06) + (10000.0 / realH));
      if (x == -1) x = 0;
      //else x -= xOffset;
      //y -= yOffset;
      #ifdef DEBUG_CONFIG_DATA
      MessageInterface::ShowMessage("   screen offset: x = %4d, y = %4d\n", xOffset, yOffset);
      MessageInterface::ShowMessage("   after offset : x = %4d, y = %4d\n", x, y);
      #endif
   #endif

   return isPresetSizeUsed;
}

