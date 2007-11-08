//$Id$
//------------------------------------------------------------------------------
//                              GmatMainFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
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
//
/**
 * This class provides the main frame for GMAT.
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "GmatMainFrame.hpp"
#include "ViewTextFrame.hpp"
#include "GmatAppData.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiTsPlotData.hpp"
#include "GmatNotebook.hpp"
#include "GmatMenuBar.hpp"

#include "GmatTreeItemData.hpp"
#include "SolarSystemWindow.hpp"
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
#include "ManeuverPanel.hpp"
#include "BeginFiniteBurnPanel.hpp"
#include "EndFiniteBurnPanel.hpp"
#include "XyPlotSetupPanel.hpp"
#include "OpenGlPlotSetupPanel.hpp"
#include "ReportFileSetupPanel.hpp"
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
#include "ParameterSetupPanel.hpp"
#include "ArraySetupPanel.hpp"
#include "IfPanel.hpp"
#include "ForPanel.hpp"
#include "WhilePanel.hpp"
#include "DoWhilePanel.hpp"
#include "GmatMdiChildFrame.hpp"
#include "FormationSetupPanel.hpp"
#include "CallFunctionPanel.hpp"
#include "CoordSystemConfigPanel.hpp"
#include "FunctionSetupPanel.hpp"
#include "MatlabFunctionSetupPanel.hpp"
#include "AssignmentPanel.hpp"
#include "ScriptEventPanel.hpp"
#include "ScriptPanel.hpp"
#include "ReportFilePanel.hpp"
#include "BarycenterPanel.hpp"
#include "LibrationPointPanel.hpp"
#include "CelestialBodyPanel.hpp"
#include "CompareReportPanel.hpp"
#include "CompareFilesDialog.hpp"
#include "CompareTextDialog.hpp"
#include "TextEphemFileDialog.hpp"
#include "AboutDialog.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"               // for Compare()

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include "ddesetup.hpp"   // for IPC_SERVICE, IPC_TOPIC

#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/tool_save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/tool_stop.xpm"
#include "bitmaps/close.xpm"
#include "bitmaps/tabclose.xpm"
#include "bitmaps/script.xpm"
#include "bitmaps/build.xpm"

#include "bitmaps/animation_play.xpm"
#include "bitmaps/animation_stop.xpm"
#include "bitmaps/animation_fast.xpm"
#include "bitmaps/animation_slow.xpm"
#include "bitmaps/animation_options.xpm"

#define __USE_CHILD_BEST_SIZE__


//#define DEBUG_MAINFRAME
//#define DEBUG_MAINFRAME_CLOSE
//#define DEBUG_MAINFRAME_SAVE
//#define DEBUG_FILE_COMPARE
//#define DEBUG_SERVER
//#define DEBUG_CREATE_CHILD
//#define DEBUG_REMOVE_CHILD

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
   //EVT_MENU(MENU_PROJECT_NEW, GmatMainFrame::OnProjectNew)
   EVT_MENU(MENU_LOAD_DEFAULT_MISSION, GmatMainFrame::OnLoadDefaultMission)
   EVT_MENU(MENU_FILE_SAVE_SCRIPT, GmatMainFrame::OnSaveScript)
   EVT_MENU(MENU_FILE_SAVE_SCRIPT_AS, GmatMainFrame::OnSaveScriptAs)
   EVT_MENU(MENU_PROJECT_EXIT, GmatMainFrame::OnProjectExit)
   EVT_MENU(MENU_PROJECT_PREFERENCES_FONT, GmatMainFrame::OnFont)
   EVT_MENU(TOOL_RUN, GmatMainFrame::OnRun)
   EVT_MENU(TOOL_PAUSE, GmatMainFrame::OnPause)
   EVT_MENU(TOOL_STOP, GmatMainFrame::OnStop)
   EVT_MENU(TOOL_CLOSE_CHILDREN, GmatMainFrame::OnCloseAll)
   EVT_MENU(TOOL_CLOSE_CURRENT, GmatMainFrame::OnCloseActive)
   
   EVT_MENU(MENU_HELP_ABOUT, GmatMainFrame::OnHelpAbout)
   
   EVT_MENU(MENU_FILE_NEW_SCRIPT, GmatMainFrame::OnNewScript)
   EVT_MENU(MENU_FILE_OPEN_SCRIPT, GmatMainFrame::OnOpenScript)
   
   EVT_MENU(MENU_EDIT_UNDO, GmatMainFrame::OnUndo)
   EVT_MENU(MENU_EDIT_REDO, GmatMainFrame::OnRedo)
   EVT_MENU(MENU_EDIT_COPY, GmatMainFrame::OnCopy)
   EVT_MENU(MENU_EDIT_CUT, GmatMainFrame::OnCut)
   EVT_MENU(MENU_EDIT_PASTE, GmatMainFrame::OnPaste)
   EVT_MENU(MENU_EDIT_COMMENT, GmatMainFrame::OnComment)
   EVT_MENU(MENU_EDIT_UNCOMMENT, GmatMainFrame::OnUncomment)
   EVT_MENU(MENU_EDIT_SELECT_ALL, GmatMainFrame::OnSelectAll)
   
   EVT_MENU(MENU_START_SERVER, GmatMainFrame::OnStartServer)
   EVT_MENU(MENU_STOP_SERVER, GmatMainFrame::OnStopServer)
   
   EVT_MENU(MENU_MATLAB_OPEN, GmatMainFrame::OnOpenMatlab)
   EVT_MENU(MENU_MATLAB_CLOSE, GmatMainFrame::OnCloseMatlab)
   EVT_MENU(MENU_TOOLS_FILE_COMPARE_NUMERIC, GmatMainFrame::OnFileCompareNumeric)
   EVT_MENU(MENU_TOOLS_FILE_COMPARE_TEXT, GmatMainFrame::OnFileCompareText)
   EVT_MENU(MENU_TOOLS_GEN_TEXT_EPHEM_FILE, GmatMainFrame::OnGenerateTextEphemFile)
   
   EVT_SASH_DRAGGED(ID_SASH_WINDOW, GmatMainFrame::OnSashDrag) 
   EVT_SASH_DRAGGED(ID_MSG_SASH_WINDOW, GmatMainFrame::OnMsgSashDrag) 
   
   EVT_SIZE(GmatMainFrame::OnMainFrameSize)
   EVT_CLOSE(GmatMainFrame::OnClose)
   EVT_SET_FOCUS(GmatMainFrame::OnSetFocus)
   EVT_KEY_DOWN(GmatMainFrame::OnKeyDown)
   
   EVT_MENU(MENU_SCRIPT_BUILD_OBJECT, GmatMainFrame::OnScriptBuildObject)
   EVT_MENU(MENU_SCRIPT_BUILD_AND_RUN, GmatMainFrame::OnScriptBuildAndRun)
   EVT_MENU(MENU_SCRIPT_RUN, GmatMainFrame::OnScriptRun)
   
   EVT_MENU_RANGE(TOOL_ANIMATION_PLAY, TOOL_ANIMATION_OPTIONS, GmatMainFrame::OnAnimation)

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
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() entered\n");
   #endif
   
   // set the script name
   mScriptFilename = "$gmattempscript$.script";
   mScriptCounter = 0;
   mAnimationFrameInc = 1;
   mInterpretFailed = false;
   mExitWithoutConfirm = false;
   mRunStatus = 0;
   
   // child frames
   trajSubframe = (MdiChildTrajFrame *)NULL;
   tsSubframe = (MdiChildTsFrame *)NULL;

   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   
#if wxUSE_MENUS
   // create a menu bar
   // pass Window menu if Windows
   #ifdef __WXMSW__
   theMenuBar = new GmatMenuBar(GmatTree::UNKNOWN_ITEM, GetWindowMenu());
   #else
   theMenuBar = new GmatMenuBar(GmatTree::UNKNOWN_ITEM, NULL);
   #endif
   
   SetMenuBar(theMenuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
   // create a status bar
   int widths[] = {150, 600, 300};
   theStatusBar = CreateStatusBar(3, wxBORDER);
   SetStatusWidths(3, widths);
   SetStatusText(_T("Welcome to GMAT!"));
#endif // wxUSE_STATUSBAR

   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::GmatMainFrame() creating ToolBar...\n");
   #endif
   
   // Why not showing separator with wxNO_BORDER | wxTB_HORIZONTAL ?
   //CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL);
   CreateToolBar();
   InitToolBar(GetToolBar());
   AddAnimationTools(GetToolBar());
   
   // used to store the list of open children
   theMdiChildren = new wxList();
   
   int w, h;
   GetClientSize(&w, &h);
   
   // A window w/sash for messages
   theMessageWin = new wxSashLayoutWindow(this, ID_MSG_SASH_WINDOW,
                           wxDefaultPosition, wxSize(30, 200),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
#ifdef __WXMAC__
   theMessageWin->SetDefaultSize(wxSize(w, (int) (0.25 * h)));
#else
   theMessageWin->SetDefaultSize(wxSize(w, 100));
#endif
   theMessageWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   theMessageWin->SetAlignment(wxLAYOUT_BOTTOM);
   theMessageWin->SetSashVisible(wxSASH_TOP, TRUE);

   // create MessageWindow and save in GmatApp for later use
   wxTextCtrl *msgTextCtrl =
      new wxTextCtrl(theMessageWin, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                     wxTE_MULTILINE);
   msgTextCtrl->SetMaxLength(320000);
   GmatAppData::GetMessageWindow()->Show(false);
   GmatAppData::SetMessageTextCtrl(msgTextCtrl);
   
   // A window w/sash for gmat notebook
   theMainWin = new wxSashLayoutWindow(this, ID_SASH_WINDOW,
                           wxDefaultPosition, wxSize(200, 30),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

#ifdef __WXMAC__
   //theMainWinSetDefaultSize(wxSize(275, h));
   theMainWin->SetDefaultSize(wxSize(w, h));
#else
   theMainWin->SetDefaultSize(wxSize(200, h));
   // 200 is too narrow for most linux themes
   #ifdef __LINUX__
      theMainWin->SetDefaultSize(wxSize(220, h));
   #endif
#endif
   theMainWin->SetOrientation(wxLAYOUT_VERTICAL);
   theMainWin->SetAlignment(wxLAYOUT_LEFT);
   theMainWin->SetSashVisible(wxSASH_RIGHT, TRUE);
   
   new GmatNotebook(theMainWin, -1, wxDefaultPosition,
                    wxDefaultSize, wxCLIP_CHILDREN);
   
   // set the main frame, because there will no longer be right notebook
   GmatAppData::SetMainFrame(this);
   
   mServer = NULL;
   mRunPaused = false;
   mRunCompleted = true;
   
   // Set icon if icon file is in the start up file
   FileManager *fm = FileManager::Instance();
   try
   {
      wxString iconfile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      #if defined __WXMSW__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_ICO));
      #elif defined __WXGTK__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_XPM));
      #elif defined __WXMAC__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_PICT_RESOURCE));
      #endif
   }
   catch (GmatBaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
   
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ~GmatMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame::~GmatMainFrame()
{
   // Close MATLAB connection
   MatlabInterface::Close();
   
   if (mServer)
      delete mServer;
   
   if (GmatAppData::GetMessageWindow() != NULL)
      GmatAppData::GetMessageWindow()->Close();
   
   if (theGuiInterpreter)
      theGuiInterpreter->Finalize();
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateChild(GmatTreeItemData *item)
//------------------------------------------------------------------------------
/**
 * Adds a page to notebook based on tree item type.
 *
 * @param <item> input GmatTreeItemData.
 */
//------------------------------------------------------------------------------
GmatMdiChildFrame* GmatMainFrame::CreateChild(GmatTreeItemData *item)
{
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateChild() item=%s\n", item->GetDesc().c_str());
   #endif
   
   GmatMdiChildFrame *newChild = NULL;

   // if child already open, just return
   if (IsChildOpen(item))
      return NULL;
   
   GmatTree::ItemType itemType = item->GetItemType();
   
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateChild() name=%s, itemType=%d\n",
       item->GetDesc().c_str(), itemType);
   #endif
   
   //----------------------------------------------------------------------
   // create a mdi child
   // Note: Do not change the order of ItemType in GmatTreeItemData.hpp.
   // The wrong order of itemType will not work properly.
   //----------------------------------------------------------------------
   if (itemType >= GmatTree::BEGIN_OF_RESOURCE &&
       itemType <= GmatTree::END_OF_RESOURCE)
   {
      newChild = CreateNewResource(item->GetDesc(), item->GetDesc(), itemType);
   }
   else if (itemType >= GmatTree::BEGIN_OF_COMMAND &&
            itemType <= GmatTree::END_OF_COMMAND)
   {
      newChild = CreateNewCommand(itemType, item);
   }
   else if (itemType >= GmatTree::BEGIN_OF_CONTROL &&
            itemType <= GmatTree::END_OF_CONTROL)
   {
      newChild = CreateNewControl(item->GetDesc(), item->GetDesc(), itemType,
                                  item->GetCommand());
   }
   else if (itemType >= GmatTree::BEGIN_OF_OUTPUT &&
            itemType <= GmatTree::END_OF_OUTPUT)
   {
      // Create panel if Report or Compare Report
      if (itemType == GmatTree::OUTPUT_REPORT ||
          itemType == GmatTree::COMPARE_REPORT)
         newChild = CreateNewOutput(item->GetDesc(), item->GetDesc(), itemType);
   }
   else
   {
      // do nothing
      #ifdef DEBUG_CREATE_CHILD
      MessageInterface::ShowMessage
         ("GmatMainFrame::CreateChild() Invalid item=%s itemType=%d entered\n",
          item->GetDesc().c_str(), itemType);
      #endif
   }
   
   return newChild;
}


//------------------------------------------------------------------------------
// Integer GetNumberOfChildOpen(bool incPlots = false, bool incScripts = false)
//------------------------------------------------------------------------------
Integer GmatMainFrame::GetNumberOfChildOpen(bool incPlots, bool incScripts)
{
   #ifdef DEBUG_MAINFRAME_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::GetNumberOfChildOpen() incPlots=%d, incScripts=%d\n",
       incPlots, incScripts);
   #endif
   
   Integer openCount = 0;
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      GmatTree::ItemType itemType = theChild->GetItemType();
      
      #ifdef DEBUG_MAINFRAME_CHILD
      MessageInterface::ShowMessage
         ("   itemType=%d, title=%s\n", itemType, theChild->GetTitle().c_str());
      #endif
      
      if (itemType == GmatTree::SCRIPT_FILE)
      {
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
   
   return openCount;
}


//------------------------------------------------------------------------------
// bool IsChildOpen(GmatTreeItemData *item)
//------------------------------------------------------------------------------
/**
 * Determines if page should be opened.  If the page is already opened, sets that
 * page as the selected page.
 *
 * @param <item> input GmatTreeItemData.
 *
 * @return True if page should be opened, false if it should not be opened.
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::IsChildOpen(GmatTreeItemData *item)
{
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();

      #ifdef DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("GmatMainFrame::IsChildOpen() title=%s\n   desc=%s\n",
          theChild->GetTitle().c_str(), item->GetDesc().c_str());
      #endif
    
      if ((theChild->GetTitle().IsSameAs(item->GetDesc().c_str())) &&
          (theChild->GetItemType() == item->GetItemType()))
      {
         // move child to the front
         theChild->Activate();
         theChild->Restore();
         return TRUE;   
      }
      node = node->GetNext();
   }
 
   return FALSE;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* GetChild(const wxString &name)
//------------------------------------------------------------------------------
/**
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
         ("   theChild=%s\n", theChild->GetTitle().c_str());
      #endif

      if (theChild->GetTitle().IsSameAs(name))
      {
         return theChild;
      }
      node = node->GetNext();
   }

   return NULL;
}


//------------------------------------------------------------------------------
// bool RenameChild(GmatTreeItemData *item, wxString newName)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::RenameChild(GmatTreeItemData *item, wxString newName)
{
   wxString oldName = item->GetDesc();
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
      if (theChild->GetTitle().IsSameAs(oldName))
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
       return TRUE;
    }
    
    return FALSE;    
}


//------------------------------------------------------------------------------
// void RemoveChild(const wxString &item, GmatTree::ItemType itemType,
//                  bool deleteChild = true)
//------------------------------------------------------------------------------
/*
 * Removes and deletes child frame from the list.
 *
 * @param <item> Name of the child frame
 * @param <itemType> Item type of the child frame
 * @param <deleteChild> Set to true if child frame is to be deleted
 *                      This flag is set to false if plot frame is deleted by
 *                      clicking X in the upper right corner.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::RemoveChild(const wxString &item, GmatTree::ItemType itemType,
                                bool deleteChild)
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::RemoveChild() item=%s, itemType=%d, deleteChild=%d\n",
       item.c_str(), itemType, deleteChild);
   #endif
   
   wxNode *node = theMdiChildren->GetFirst();
   bool childRemoved = false;
   
   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      
      if ((child->GetTitle().IsSameAs(item.c_str())) &&
          (child->GetItemType() == itemType))
      {
         //------------------------------------------------------
         // Notes:
         // OpenGL and XYPlot is added to theMdiChildren list
         // in this main frame and to it's own list of 
         // MdiGlPlot::mdiChildren and MdiTsPlot::mdiChildren.
         // These lists are used in the PlotInterface.
         // The count is decremented and object is deleted in the
         // destructors
         //------------------------------------------------------
         
         #ifdef DEBUG_REMOVE_CHILD
         MessageInterface::ShowMessage
            ("   removing title:%s\n   item: %s\n", child->GetTitle().c_str(),
             item.c_str());
         #endif
         
         // MdiChildTrajFrame::OnPlotClose() and MdiChildTsrame::OnPlotClose()
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
      MessageInterface::ShowMessage("   %s removed\n", item.c_str());
      #endif
      
      if (GmatAppData::GetOutputTree() != NULL)
      {
         #ifdef DEBUG_REMOVE_CHILD
         MessageInterface::ShowMessage("   calling GetOutputTree()->RemoveItem()\n");
         #endif
         
         GmatAppData::GetOutputTree()->RemoveItem(itemType, item);
      }
   }
   
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage("GmatMainFrame::RemoveChild() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void CloseChild(const wxString &item, GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
/*
 * Closes child frame of given item name and type.
 *
 * @param <item> Name of the child frame
 * @param <itemType> Item type of the child frame
 */
//------------------------------------------------------------------------------
void GmatMainFrame::CloseChild(const wxString &item, GmatTree::ItemType itemType)
{
   #ifdef DEBUG_REMOVE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CloseChild() item=%s, itemType=%d\n", item.c_str(),
       itemType);
   #endif
   
   wxNode *node = theMdiChildren->GetFirst();
   
   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      
      if ((child->GetTitle().IsSameAs(item.c_str())) &&
          (child->GetItemType() == itemType))
      {
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
//                       wxString excludeTitle = "")
//------------------------------------------------------------------------------
/*
 * Closes all mdi children frames.
 *
 * @param <closeScriptWindow> true to close script window
 * @param <closePlots> true to close all plot windows
 * @param <excludeTitle> name of the window to be excluded from closing
 * @return true if all frames is closed false otherwise
 */
//------------------------------------------------------------------------------ 
bool GmatMainFrame::CloseAllChildren(bool closeScriptWindow, bool closePlots,
                                     wxString excludeTitle)
{   
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage
      ("GmatMainFrame::CloseAllChildren() closeScriptWindow = %d, closePlots = %d"
       "\n   excludeTitle = %s\n", closeScriptWindow, closePlots, excludeTitle.c_str());
   MessageInterface::ShowMessage
      ("   Number of children = %d\n", theMdiChildren->GetCount());
   #endif
   
   wxString title;
   GmatTree::ItemType type;
   bool canDelete;
   
   wxNode *node = theMdiChildren->GetFirst();
   wxCloseEvent event;
   
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
      type = child->GetItemType();
      
      #ifdef DEBUG_MAINFRAME_CLOSE
      MessageInterface::ShowMessage("   title = %s, type = %d\n", title.c_str(), type);
      #endif
      
      // if title is not excluded
      if (child->GetTitle() != excludeTitle)
      {
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
               GmatAppData::GetOutputTree()->UpdateOutput(true);
               canDelete = true;
            }
         }
      }
      
      //--------------------------------------------------------------
      // delete chilren by child->OnClose()
      //--------------------------------------------------------------
      #ifdef __WXMSW__
      if (canDelete)
      {
         #ifdef DEBUG_MAINFRAME_CLOSE
         MessageInterface::ShowMessage("   ==> closing child = %s\n", title.c_str());
         #endif
         
         //-------------------------------------------------
         // delete child if frame can be closed
         // Note: GmatMdiChildFrame::OnClose() calls RemoveChild()
         // child->Close() will not process OnClose() correctly
         // So use OnClose(event) instead
         //-------------------------------------------------
         if (mExitWithoutConfirm)
            child->SetDirty(false);
         
         child->OnClose(event);
         
         if (!child->CanClose())
         {
            #ifdef DEBUG_MAINFRAME_CLOSE
            MessageInterface::ShowMessage("   ==> cannot close this child\n");
            #endif
            
            canDelete = false;
            return false;
         }
      }
      
      //-------------------------------------------------
      // Note: The node is deleted from RemoveChild()
      //-------------------------------------------------
      wxNode *nextNode = theMdiChildren->GetFirst();
      
      // if node is not deleted get next node
      if (!canDelete)
         nextNode = nextNode->GetNext();
      
      #ifdef DEBUG_MAINFRAME_CLOSE
      MessageInterface::ShowMessage
         ("   next node = %p, canDelete = %d\n", nextNode, canDelete);
      #endif
      
      if (node == nextNode)
         break;
      
      node = nextNode;
      
      //--------------------------------------------------------------
      // delete chilren need more work on platforms other than Windows
      //--------------------------------------------------------------
      #else
      
      wxNode *temp = NULL;
      if (canDelete)
      {
         #ifdef DEBUG_MAINFRAME_CLOSE
         MessageInterface::ShowMessage
            ("   ==> deleting child = %s\n", title.c_str());
         #endif
         
         delete child;
         temp = node;
      }
      
      node = node->GetNext();
      if (canDelete)
         delete temp;
      
      #endif
      //--------------------------------------------------------------
      // endif  __WXMSW__
      //--------------------------------------------------------------
   }
   
   wxSafeYield();
   return true;
}


//------------------------------------------------------------------------------
// void GmatMainFrame::MinimizeChildren()
//------------------------------------------------------------------------------
void GmatMainFrame::MinimizeChildren()
{
   // do not need to check if script window is open
   wxNode *node = theMdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *child = (GmatMdiChildFrame *)node->GetData();
      if (child->GetItemType() != GmatTree::OUTPUT_OPENGL_PLOT &&
          child->GetItemType() != GmatTree::OUTPUT_XY_PLOT &&
          child->GetItemType() != GmatTree::COMPARE_REPORT)
         child->Iconize(TRUE);
      node = node->GetNext();
   }

}


//------------------------------------------------------------------------------
// void GmatMainFrame::SetActiveChildDirty()
//------------------------------------------------------------------------------
void GmatMainFrame::SetActiveChildDirty(bool dirty)
{
   GmatMdiChildFrame *child = (GmatMdiChildFrame *)GetActiveChild();
   
   if (child != NULL)
      child->SetDirty(dirty); 
}


//------------------------------------------------------------------------------
// void CloseCurrentProject()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseCurrentProject()
{
   #ifdef DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage("GmatMainFrame::CloseCurrentProject()\n");
   #endif
   
   // close all windows
   CloseAllChildren();
   
   // update title and status bar
   wxString statusText;
   statusText.Printf("GMAT - General Mission Analysis Tool");
   SetStatusText("", 1);
   UpdateTitle();
   
   // clear trees, message window
   theGuiInterpreter->ClearResource();
   theGuiInterpreter->ClearCommandSeq();
   MessageInterface::ClearMessage();
   
   GmatAppData::GetResourceTree()->UpdateResource(true);
   GmatAppData::GetMissionTree()->UpdateMission(true);
   GmatAppData::GetOutputTree()->UpdateOutput(true);
}


//------------------------------------------------------------------------------
// bool InterpretScript(const wxString &filename, bool readBack = false,
//                      const wxString &savePath = "", bool openScript = true,
//                      bool multScripts = false)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <readBack> true will read scripts, save, and read back in
 * @param <newPath> new path to be used for saving scripts
 * @param <openScript> true if script file to be opened on error
 * @param <multScripts> true if running scripts from the folder
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::InterpretScript(const wxString &filename, bool readBack,
                                    const wxString &savePath, bool openScript,
                                    bool multScripts)
{
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::InterpretScript()\n   filename=%s, readBack=%d\n   "
       "savePath=%s openScript=%d, multScripts=%d\n", filename.c_str(), readBack,
       savePath.c_str(), openScript, multScripts);
   #endif
   
   UpdateTitle(filename);
   
   bool success = false;
   
   // Always refresh the gui before new scritpes are read
   CloseAllChildren(false, true, filename);
   GmatAppData::GetResourceTree()->ClearResource(false);
   GmatAppData::GetMissionTree()->ClearMission();
   GmatAppData::GetOutputTree()->UpdateOutput(true);
   
   // let's try building the script, Moderator::InterpretScript() will
   // clear all resource and commands
   try
   {
      // If successfully interpreted, set status to true
      if (theGuiInterpreter->
          InterpretScript(filename.c_str(), readBack, savePath.c_str()))
      {
         success = true;
      }  
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Errors were found in the script named \"%s\".\n"
             "Please fix all errors listed in message window.\n", filename.c_str());
         
         theGuiInterpreter->ClearResource();
         theGuiInterpreter->ClearCommandSeq();
      }
      
      if (success)
      {
         // Update ResourceTree and MissionTree
         GmatAppData::GetResourceTree()->UpdateResource(true);
         GmatAppData::GetMissionTree()->UpdateMission(true);
         
         // if not running script folder, clear status
         if (!multScripts)
            SetStatusText("", 1);
      }
      else
      {
         SetStatusText("Errors were Found in the Script!!", 1);
         
         // open script editor      
         if (openScript)
            OpenScript();
      }      
   }
   catch (BaseException &e)
   {
      wxLogError(e.GetFullMessage().c_str());
      wxLog::FlushActive();
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
   
   mInterpretFailed = !success;
   
   return success;
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
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::RunCurrentMission() mRunPaused=%d\n", mRunPaused);
   #endif
   
   Integer retval = 1;
   
   if (mInterpretFailed)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Errors were found in the script named \"%s\".\n"
          "Please fix all errors listed in message window before running "
          "the mission.\n", mScriptFilename.c_str());
      
      return 0;
   }
   
   EnableMenuAndToolBar(false, true);
   
   wxYield();
   SetFocus();
   
   mRunCompleted = false;
   
   if (mRunPaused)
   {
      mRunPaused = false;
      
      MessageInterface::ShowMessage("Execution resumed.\n");
      theGuiInterpreter->ChangeRunState("Resume");
   }
   else
   {
      MinimizeChildren();
      retval = theGuiInterpreter->RunMission();
      
      if (retval != 1 && mServer)
         StopServer(); // stop server if running to avoid getting callback staus
                       // when run stopped by user
      
      EnableMenuAndToolBar(true, true);
      
      //put items in output tab
      GmatAppData::GetOutputTree()->UpdateOutput(false);
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
   wxYield();
}


//------------------------------------------------------------------------------
// void StartServer()
//------------------------------------------------------------------------------
void GmatMainFrame::StartServer()
{
   if (!mServer)
   {
      // service name (DDE classes) or port number (TCP/IP based classes)
      wxString service = IPC_SERVICE;
      
      // Create a new server
      mServer = new GmatServer;
      mServer->Create(service);
      
      //#ifdef DEBUG_SERVER
      MessageInterface::ShowMessage("Server started.\n");
      //#endif
      
      //mServerMenu->Enable(MENU_START_SERVER, false);
      //mServerMenu->Enable(MENU_STOP_SERVER, true);
   }
   else
   {
      //#ifdef DEBUG_SERVER
      MessageInterface::ShowMessage("Server has already started.\n");
      //#endif
   }
}


//------------------------------------------------------------------------------
// void StopServer()
//------------------------------------------------------------------------------
void GmatMainFrame::StopServer()
{
   if (mServer)
   {
      delete mServer;
      
      //#ifdef DEBUG_SERVER
      MessageInterface::ShowMessage("Server terminated.\n");
      //#endif
      
      mServer = NULL;
   }
   else
   {
      //#ifdef DEBUG_SERVER
      MessageInterface::ShowMessage("Server has not started.\n");
      //#endif
   }
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnClose(wxCloseEvent& event)
{
   //MessageInterface::ShowMessage("===> GmatMainFrame::OnClose()\n");
   
   if (!mRunCompleted)
   {
      wxMessageBox(wxT("GMAT is still running the mission.\n"
                       "Please STOP the run before closing."),
                   wxT("GMAT Warning"));
      event.Veto();
      return;
   }
   
   // close all child windows first
   if (CloseAllChildren(true, true))
   {
      ShowSaveMessage();
      event.Skip();
   }
   else
   {
      event.Veto();
   }   
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

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void InitToolBar(wxToolBar* toolBar)
//------------------------------------------------------------------------------
/**
 * Adds bitmaps to tool bar.
 *
 * @param <toolBar> input tool bar.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::InitToolBar(wxToolBar* toolBar)
{
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::InitToolBar() entered\n");
   #endif

   const int NUM_ICONS = 15;
   wxBitmap* bitmaps[NUM_ICONS];
   
   bitmaps[0] = new wxBitmap(new_xpm);
   bitmaps[1] = new wxBitmap(open_xpm);
   bitmaps[2] = new wxBitmap(tool_save_xpm);
   bitmaps[3] = new wxBitmap(copy_xpm);
   bitmaps[4] = new wxBitmap(cut_xpm);
   bitmaps[5] = new wxBitmap(paste_xpm);
   bitmaps[6] = new wxBitmap(print_xpm);
   bitmaps[7] = new wxBitmap(help_xpm);
   bitmaps[8] = new wxBitmap(play_xpm);
   bitmaps[9] = new wxBitmap(pause_xpm);
   bitmaps[10] = new wxBitmap(tool_stop_xpm);
   bitmaps[11] = new wxBitmap(close_xpm);
   bitmaps[12] = new wxBitmap(tabclose_xpm);
   bitmaps[13] = new wxBitmap(script_xpm);
   bitmaps[14] = new wxBitmap(build_xpm);
   
   toolBar->SetToolBitmapSize(wxSize(16,15));
   
   // recale to default size of 16x15
   for (int i=0; i<NUM_ICONS; i++)
   {
      wxImage image = bitmaps[i]->ConvertToImage();
      image = image.Rescale(16, 15);
      *bitmaps[i] = wxBitmap(image);
   }
   
   // add project tools
   toolBar->AddTool(MENU_FILE_NEW_SCRIPT, _T("New"), *bitmaps[0], _T("New Script"));
   toolBar->AddTool(MENU_FILE_OPEN_SCRIPT, _T("Open"), *bitmaps[1], _T("Open Script"));
   toolBar->AddTool(MENU_FILE_SAVE_SCRIPT, _T("Save"), *bitmaps[2], _T("Save to Script"));
   toolBar->AddSeparator();
   
   toolBar->AddTool(MENU_LOAD_DEFAULT_MISSION, _T("Default"), *bitmaps[13], 
                    _T("Default Project"));
   toolBar->AddSeparator();
   
   // add edit tools
   toolBar->AddTool(3, _T("Copy"), *bitmaps[3], _T("Copy"));
   toolBar->AddTool(4, _T("Cut"), *bitmaps[4], _T("Cut"));
   toolBar->AddTool(5, _T("Paste"), *bitmaps[5], _T("Paste"));
   toolBar->AddSeparator();
   
   // add print tool
   toolBar->AddTool(6, _T("Print"), *bitmaps[6], _T("Print"));
   toolBar->AddSeparator();
   
   // add run tools
   toolBar->AddTool(TOOL_RUN, _T("Run"), *bitmaps[8], _T("Run"));
   toolBar->AddTool(TOOL_PAUSE, _T("Pause"), *bitmaps[9], _T("Pause"));
   toolBar->AddTool(TOOL_STOP, _T("Stop"), *bitmaps[10], _T("Stop"));
   toolBar->AddSeparator();
   
   // add close window tool
   toolBar->AddTool(TOOL_CLOSE_CHILDREN, _T("Close"), *bitmaps[11], _T("Close All"));
   toolBar->AddTool(TOOL_CLOSE_CURRENT, _T("Close this Child"), *bitmaps[12],
                    _T("Close this Child"));
   toolBar->AddSeparator();
   
   // add help tool
   toolBar->AddTool(MENU_HELP_ABOUT, _T("Help"), *bitmaps[7], _T("Help"));
   toolBar->AddSeparator();
   
   // now realize to make tools appear
   toolBar->Realize();
   
   // disable tools
   toolBar->EnableTool(3, FALSE); // copy
   toolBar->EnableTool(4, FALSE); // cut
   toolBar->EnableTool(5, FALSE); // paste
   toolBar->EnableTool(6, FALSE); // print
   
   toolBar->EnableTool(TOOL_PAUSE, FALSE);
   toolBar->EnableTool(TOOL_STOP, FALSE);
   
   for (int i = 0; i < NUM_ICONS; i++)
      delete bitmaps[i];
   
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::InitToolBar() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddAnimationTools(wxToolBar* toolBar)
//------------------------------------------------------------------------------
/**
 * Adds animation tool icons to tool bar.
 *
 * @param <toolBar> input tool bar.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::AddAnimationTools(wxToolBar* toolBar)
{
   #ifdef DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::AddAnimationTools() entered\n");
   #endif
      
   
   const int NUM_ICONS = 5;
   wxBitmap* bitmaps[NUM_ICONS];
   
   bitmaps[0] = new wxBitmap(animation_play_xpm);
   bitmaps[1] = new wxBitmap(animation_stop_xpm);
   bitmaps[2] = new wxBitmap(animation_fast_xpm);
   bitmaps[3] = new wxBitmap(animation_slow_xpm);
   bitmaps[4] = new wxBitmap(animation_options_xpm);
   
   // recale to default size of 16x15
   for (int i=0; i<NUM_ICONS; i++)
   {
      wxImage image = bitmaps[i]->ConvertToImage();
      image = image.Rescale(16, 15);
      *bitmaps[i] = wxBitmap(image);
   }

   toolBar->AddSeparator();
   toolBar->AddSeparator();
   
   // How do I put spacing between tools
   //toolBar->SetToolSeparation(50); // Why this doesn't set spacing?
   //toolBar->SetToolPacking(10);    // What will this do?
   //toolBar->SetMargins(50, 2);
   //int currentX = 400;
   //toolBar->AddTool(TOOL_ANIMATION_PLAY, *bitmaps[0], wxNullBitmap, false, currentX, -1,
   //                 (wxObject*) NULL, "Start Animation");
   
   toolBar->AddTool(TOOL_ANIMATION_PLAY, _T("AnimationPlay"), *bitmaps[0],
                    _T("Start Animation"), wxITEM_CHECK);
   toolBar->AddTool(TOOL_ANIMATION_STOP, _T("AnimationStop"), *bitmaps[1],
                    _T("Stop Animation"));
   toolBar->AddTool(TOOL_ANIMATION_FAST, _T("AnimationFast"), *bitmaps[2],
                    _T("Faster Animation"));
   toolBar->AddTool(TOOL_ANIMATION_SLOW, _T("AnimationSlow"), *bitmaps[3],
                    _T("Slower Animation"));
   toolBar->AddTool(TOOL_ANIMATION_OPTIONS, _T("AnimationOptions"), *bitmaps[4],
                    _T("Show Animation Options"));
   
   // now realize to make tools appear
   toolBar->Realize();
   
   for (int i = 0; i < NUM_ICONS; i++)
      delete bitmaps[i];
}


//------------------------------------------------------------------------------
// void ShowSaveMessage()
//------------------------------------------------------------------------------
void GmatMainFrame::ShowSaveMessage()
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
         return;
      }
      else if (result == wxID_NO)
      {
         // If we decided to ignore any changes made to panel later,
         // just uncomment this
         //mExitWithoutConfirm = true;
      }
      else if (result == wxID_YES)
      {
         bool scriptSaved = false;
         
         if (strcmp(mScriptFilename.c_str(), "$gmattempscript$.script") == 0)
         {
            scriptSaved = SaveScriptAs();
         }
         else
         {
            theGuiInterpreter->SaveScript(mScriptFilename);
            scriptSaved = true;
         }
         
         if (scriptSaved)
         {
            MessageInterface::PopupMessage
               (Gmat::INFO_, "Script saved to \"%s\"\n", mScriptFilename.c_str());
         }
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
         return;
      #endif
   }
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
   
   return scriptSaved;
}


//------------------------------------------------------------------------------
// void OpenScript()
//------------------------------------------------------------------------------
void GmatMainFrame::OpenScript()
{
   GmatTreeItemData *scriptItem =
      new GmatTreeItemData(mScriptFilename.c_str(), GmatTree::SCRIPT_FILE);
   
   CreateChild(scriptItem);
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
   mScriptFilename = "$gmattempscript$.script";
   theGuiInterpreter->LoadDefaultMission();
   mInterpretFailed = false;
   
   // Update trees
   GmatAppData::GetResourceTree()->UpdateResource(true);
   GmatAppData::GetMissionTree()->UpdateMission(true);
   GmatAppData::GetOutputTree()->UpdateOutput(true);
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
   
   if (strcmp(mScriptFilename.c_str(), "$gmattempscript$.script") == 0)
   {
      scriptSaved = SaveScriptAs();
      if (scriptSaved)
      {
         GmatAppData::GetResourceTree()->AddScriptItem(mScriptFilename.c_str());
         GmatAppData::GetResourceTree()->UpdateResource(false);
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
         // Create backup file
         wxString currFilename = mScriptFilename.c_str();
         wxString backupFilename = currFilename + ".bak";
         ::wxCopyFile(currFilename, backupFilename);
         
         #ifdef DEBUG_MAINFRAME_SAVE
         MessageInterface::ShowMessage
            ("GmatMainFrame::OnSaveScript() Created backup file: %s\n",
             backupFilename.c_str());
         #endif
         
         theGuiInterpreter->SaveScript(mScriptFilename);
         scriptSaved = true;
      }
   }
   
   if (scriptSaved)
   {
      UpdateTitle(mScriptFilename.c_str());

      #ifdef __CONFIRM_SAVE__
      MessageInterface::PopupMessage
         (Gmat::INFO_, "Scrpt saved to \"%s\"\n", mScriptFilename.c_str());
      #endif
      
      #ifdef __CLOSE_CHILDREN_AFTER_SAVE__
      CloseAllChildren();
      #endif
      
   }
}


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
      GmatAppData::GetResourceTree()->AddScriptItem(mScriptFilename.c_str());
      UpdateTitle(mScriptFilename.c_str());
   }
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
   CloseAllChildren();
   wxSafeYield();
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
// GmatMdiChildFrame* CreateNewResource(const wxString &title, const wxString &name
//                                      GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewResource(const wxString &title, const wxString &name,
                                 GmatTree::ItemType itemType)
{
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateNewResource() title=%s, name=%s, itemType=%d\n",
       title.c_str(), name.c_str(), itemType);
   #endif
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);   
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, title, name, itemType);   
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
   case GmatTree::SPACECRAFT:
      sizer->Add(new SpacecraftPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::CELESTIAL_BODY:
      sizer->Add(new CelestialBodyPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::FUELTANK:
      sizer->Add(new TankConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::THRUSTER:
      sizer->Add(new ThrusterConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::FORMATION:
      sizer->Add(new FormationSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::UNIVERSE_FOLDER:
      sizer->Add(new UniversePanel(scrolledWin), 0, wxGROW|wxALL, 0);
      break;
      //case itemType == GmatTree::BODY:
      //sizer->Add(new SolarSystemWindow(scrolledWin), 0, wxGROW|wxALL, 0);      
      //break;
   case GmatTree::IMPULSIVE_BURN:
      sizer->Add(new ImpulsiveBurnSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::FINITE_BURN:
      sizer->Add(new FiniteBurnSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::PROPAGATOR:
      sizer->Add(new PropagationConfigPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::DIFF_CORR:
      sizer->Add(new DCSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SQP:
      sizer->Add(new SQPSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::REPORT_FILE:
      sizer->Add(new ReportFileSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::XY_PLOT:
      sizer->Add(new XyPlotSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::OPENGL_PLOT:
      sizer->Add(new OpenGlPlotSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::VARIABLE:
      if (theGuiInterpreter->GetConfiguredObject(name.c_str())->GetTypeName() == "Variable" ||
          theGuiInterpreter->GetConfiguredObject(name.c_str())->GetTypeName() == "String")
      {
         sizer->Add(new ParameterSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      }
      else if (theGuiInterpreter->GetConfiguredObject(name.c_str())->GetTypeName() == "Array")
      {
         sizer->Add(new ArraySetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      }
      break;
   case GmatTree::GMAT_FUNCTION:
      {
         FunctionSetupPanel *functPanel = new FunctionSetupPanel(scrolledWin, name);
         sizer->Add(functPanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(functPanel->mFileContentsTextCtrl);
         break;
      }
   case GmatTree::MATLAB_FUNCTION:
      sizer->Add(new MatlabFunctionSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SCRIPT_FILE:
      {
         ScriptPanel *scriptPanel = new ScriptPanel(scrolledWin, name);
         sizer->Add(scriptPanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(scriptPanel->mFileContentsTextCtrl);
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
   wxString title = item->GetDesc();
   wxString name = item->GetDesc();
   GmatCommand *cmd = item->GetCommand();
   
   #ifdef DEBUG_CREATE_CHILD
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateNewCommand() title=%s, name=%s, itemType=%d\n",
       title.c_str(), name.c_str(), itemType);
   #endif
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, itemType);
   
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
      sizer->Add(new TogglePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
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
      newChild->SetScriptTextCtrl(scriptEventPanel->mFileContentsTextCtrl);
      break;
   }
   case GmatTree::ASSIGNMENT:
      sizer->Add(new AssignmentPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   default:
      MessageInterface::ShowMessage("===> returning NULL\n");
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
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, itemType);
   
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
      break;
   case GmatTree::IF_CONTROL:
      sizer->Add(new IfPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
      //case GmatTree::ELSE_IF_CONTROL:
      //sizer->Add(new ElseIfPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      //break;
   case GmatTree::FOR_CONTROL:
      sizer->Add(new ForPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::WHILE_CONTROL:
      sizer->Add(new WhilePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
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
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, itemType);
   
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (itemType)
   {
   case GmatTree::OUTPUT_REPORT:
      {
         ReportFilePanel *reportPanel = new ReportFilePanel(scrolledWin, name);
         sizer->Add(reportPanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(reportPanel->mFileContentsTextCtrl);
         break;
      }
   case GmatTree::COMPARE_REPORT:
      {
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
//   GmatAppData::GetResourceTree()->OnNewScript();
   wxString name;
   name.Printf("Script%d.script", ++mScriptCounter);

   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, name, name, GmatTree::SCRIPT_FILE);
   
   //panel = new wxScrolledWindow(newChild);
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   ScriptPanel *scriptPanel = new ScriptPanel(scrolledWin, "");
   sizer->Add(scriptPanel, 0, wxGROW|wxALL, 0);
   newChild->SetScriptTextCtrl(scriptPanel->mFileContentsTextCtrl);
   
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
   GmatAppData::GetResourceTree()->OnAddScript(event);
   
   if (GmatAppData::GetResourceTree()->WasScriptAdded())
   {
      #ifdef DEBUG_MAINFRAME_OPEN
      MessageInterface::ShowMessage
         ("GmatMainFrame::OnOpenScript() mInterpretFailed=%d, "
          "HasConfigurationChanged=%d\n", mInterpretFailed,
          theGuiInterpreter->HasConfigurationChanged());
      #endif
      
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
      
      SetStatusText("", 1);
      InterpretScript(mScriptFilename.c_str());
   }
}


//------------------------------------------------------------------------------
// void OnStartServer(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles starting server from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnStartServer(wxCommandEvent& event)
{
   StartServer();
}

//------------------------------------------------------------------------------
// void OnStopServer(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles terminating server from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnStopServer(wxCommandEvent& event)
{
   StopServer();
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
   MatlabInterface::Open();
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
   MatlabInterface::Close();
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

   #ifdef DEBUG_FILE_COMPARE
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnFileCompareText() baseDir=%s\n   "
       "compDirs[0]=%s\n", baseDir.c_str(), compDirs[0].c_str());
   MessageInterface::ShowMessage
      ("   numDirsToCompare=%d, numFilesToCompare=%d\n", numDirsToCompare,
       numFilesToCompare);
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
   
   //How do I specify multiple file ext?
   bool cont = dir.GetFirst(&filename);
   while (cont)
   {
      if (filename.Contains(".report") || filename.Contains(".txt") ||
          filename.Contains(".data") || filename.Contains(".script"))
      {
         filepath = baseDir + "/" + filename;
         
         // remove any backup files
         if (filename.Last() == 't' || filename.Last() == 'a')
            baseFileNameArray.push_back(filepath.c_str());
      }
      
      cont = dir.GetNext(&filename);
   }
   
   wxString tempStr;
   int fileCount = 0;
   wxString baseFileName;
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
      wxFileName filename(baseFileName);
      
      wxArrayString compareNames;
      
      for (int j=0; j<numDirsToCompare; j++)
         compareNames.Add(filename.GetFullName());
      
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
   
   theMessageWin->SetDefaultSize(wxSize(w, event.GetDragRect().height));
   
   
   wxLayoutAlgorithm layout;
   layout.LayoutMDIFrame(this);
   
   // Leaves bits of itself behind sometimes
   GetClientWindow()->Refresh();
}


// ------------------------------------------------------------------------------
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
   //MessageInterface::ShowMessage("===> GmatMainFrame::OnMainFrameSize() called\n");
   
   wxLayoutAlgorithm layout;
   layout.LayoutMDIFrame(this);
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
// void UpdateMenus(bool openOn)
//------------------------------------------------------------------------------
/*
 * Enanbles or disables menu File->Open item
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
 * @param <missionRunning> true if missin is running, this will toggle pause
 *                         and stop icons
 * @param <forAnimation> true if icons are for animation, this will toggle animation
 *                       play icon
 */
//------------------------------------------------------------------------------
void GmatMainFrame::EnableMenuAndToolBar(bool enable, bool missionRunning,
                                         bool forAnimation)
{
   wxToolBar *toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_RUN, enable);
   toolBar->EnableTool(TOOL_PAUSE, enable);
   toolBar->EnableTool(TOOL_STOP, enable);
   
   if (missionRunning)
   {
      toolBar->EnableTool(TOOL_PAUSE, !enable);
      toolBar->EnableTool(TOOL_STOP, !enable);
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
      wxMenuBar *childMenuBar = child->GetMenuBar();
      int helpIndex = childMenuBar->FindMenu("Help");
      int childMenuCount = childMenuBar->GetMenuCount();
      
      for (int i=0; i<childMenuCount; i++)
      {
         // Update except Help menu
         if (i != helpIndex)
            childMenuBar->EnableTop(i, enable);
      }
//       // wcs 2007.08.16 crashes the Mac app
//       #ifndef __WXMAC__
//       childMenuBar->Update();
//       #endif
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
   
//    // wcs 2007.08.16 crashes the Mac app
//    #ifndef __WXMAC__
//    theMenuBar->Update();
//    #endif
   
}


//------------------------------------------------------------------------------
// void OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
   wxString filename = ((GmatMdiChildFrame *)GetActiveChild())->GetTitle();
   
   InterpretScript(filename);
}


//------------------------------------------------------------------------------
// void OnScriptBuildAndRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildAndRun(wxCommandEvent& event)
{
   //MessageInterface::ShowMessage("====> GmatMainFrame::OnScriptBuildAndRun()\n");
   
   wxString filename = mScriptFilename.c_str();
   
   if (InterpretScript(filename))      
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
   //MessageInterface::ShowMessage("====> GmatMainFrame::OnScriptRun()\n");
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
      wxMessageBox(wxT("GMAT is still running the mission.\n"
                       "Please STOP before reading a new script."),
                   wxT("GMAT Warning"));
      return false;
   }
   
   mScriptFilename = filename;
   return true;
}


//------------------------------------------------------------------------------
// void OnUndo(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnUndo(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   child->GetScriptTextCtrl()->Undo();
//   theSaveButton->Enable(true);
}


//------------------------------------------------------------------------------
// void OnRedo(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnRedo(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   child->GetScriptTextCtrl()->Redo();
//   theSaveButton->Enable(true);
}


//------------------------------------------------------------------------------
// void OnCut(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnCut(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   child->GetScriptTextCtrl()->Cut();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnCopy(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnCopy(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   child->GetScriptTextCtrl()->Copy();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnPaste(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnPaste(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   child->GetScriptTextCtrl()->Paste();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnComment(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnComment(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   wxTextCtrl *scriptTC = child->GetScriptTextCtrl();
   wxString selString = scriptTC->GetStringSelection();
   selString.Replace("\n", "\n%");
   selString = "%" + selString;
   
   if (selString.Last() == '%')
      selString = selString.Mid(0, selString.Length()-1);

   scriptTC->WriteText(selString);

}

//------------------------------------------------------------------------------
// void OnUncomment(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnUncomment(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   wxTextCtrl *scriptTC = child->GetScriptTextCtrl();
   wxString selString = scriptTC->GetStringSelection();

   if (selString.StartsWith("%"))  // gets rid of first %
      selString = selString.Mid(1, selString.Length()-1);

   selString.Replace("\n%", "\n");
   scriptTC->WriteText(selString);
}

//------------------------------------------------------------------------------
// void OnSelectAll(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnSelectAll(wxCommandEvent& event)
{
   GmatMdiChildFrame* child = (GmatMdiChildFrame *)GetActiveChild();
   wxTextCtrl *scriptTC = child->GetScriptTextCtrl();
   scriptTC->SetSelection(-1, -1);
}


//------------------------------------------------------------------------------
// void OnFont(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnFont(wxCommandEvent& event)
{
  wxFontData data;
  data.SetInitialFont(GmatAppData::GetFont());
  
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
    
    GmatAppData::SetFont(newFont);
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
   
   // active child is not OpenGL, just return
   if (child->GetItemType() != GmatTree::OUTPUT_OPENGL_PLOT)
   {
      toolBar->ToggleTool(TOOL_ANIMATION_PLAY, false);
      return;
   }
   
   wxString title = child->GetTitle();
   MdiChildTrajFrame *frame = NULL;
   bool frameFound = false;
   
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("GmatMainFrame::OnAnimation() title=%s\n", title.c_str());
   #endif
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      if (frame && (frame->GetPlotName().IsSameAs(title)))
      {
         frameFound = true;
         break;
      }
   }
   
   if (!frameFound)
      return;
   
   #if DEBUG_ANIMATION
   MessageInterface::ShowMessage
      ("===> Now start animation of %s\n", frame->GetPlotName().c_str());
   #endif
   
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
      frame->OnShowOptionDialog(event);
      break;
   default:
      break;
   }
}


