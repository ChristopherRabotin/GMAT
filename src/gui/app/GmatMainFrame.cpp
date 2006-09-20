//$Header$
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
#include "AchievePanel.hpp"
#include "VaryPanel.hpp"
#include "MinimizePanel.hpp"
#include "NonLinearConstraintPanel.hpp"
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
#include "InteractiveMatlabDialog.hpp"
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
#include "FileManager.hpp"
#include "FileUtil.hpp"               // for Compare()

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include "ddesetup.hpp"   // for IPC_SERVICE, IPC_TOPIC

#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/stop.xpm"
#include "bitmaps/close.xpm"
#include "bitmaps/tabclose.xpm"
#include "bitmaps/script.xpm"
#include "bitmaps/build.xpm"

//#define DEBUG_MAINFRAME 1
//#define DEBUG_MAINFRAME_CLOSE 1
//#define DEBUG_FILE_COMPARE 1

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
   EVT_MENU(MENU_PROJECT_NEW, GmatMainFrame::OnProjectNew)
   EVT_MENU(MENU_PROJECT_LOAD_DEFAULT_MISSION, GmatMainFrame::OnLoadDefaultMission)
   EVT_MENU(MENU_FILE_SAVE_SCRIPT, GmatMainFrame::OnSaveScript)
   EVT_MENU(MENU_FILE_SAVE_AS_SCRIPT, GmatMainFrame::OnSaveScriptAs)
   EVT_MENU(MENU_PROJECT_EXIT, GmatMainFrame::OnProjectExit)
   EVT_MENU(MENU_PROJECT_PREFERENCES_FONT, GmatMainFrame::OnFont)
   EVT_MENU(TOOL_RUN, GmatMainFrame::OnRun)
   EVT_MENU(TOOL_PAUSE, GmatMainFrame::OnPause)
   EVT_MENU(TOOL_STOP, GmatMainFrame::OnStop)
   EVT_MENU(TOOL_CLOSE_CHILDREN, GmatMainFrame::OnCloseChildren)
   EVT_MENU(TOOL_CLOSE_CURRENT, GmatMainFrame::OnCloseCurrent)
   
   EVT_MENU(MENU_HELP_ABOUT, GmatMainFrame::OnHelpAbout)
   EVT_MENU(MENU_SCRIPT_BUILD, GmatMainFrame::OnScriptBuild)    
//   EVT_MENU(MENU_ORBIT_FILES_GL_PLOT_TRAJ_FILE, GmatMainFrame::OnGlPlotTrajectoryFile)
//   EVT_MENU(MENU_ORBIT_FILES_XY_PLOT_TRAJ_FILE, GmatMainFrame::OnXyPlotTrajectoryFile)

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

   EVT_MENU(MENU_TOOLS_MATLAB_OPEN, GmatMainFrame::OnOpenMatlab)
   EVT_MENU(MENU_TOOLS_MATLAB_CLOSE, GmatMainFrame::OnCloseMatlab)
   EVT_MENU(MENU_TOOLS_FILE_COMPARE_NUMERIC, GmatMainFrame::OnFileCompareNumeric)
   EVT_MENU(MENU_TOOLS_FILE_COMPARE_TEXT, GmatMainFrame::OnFileCompareText)
   EVT_MENU(MENU_TOOLS_GEN_TEXT_EPHEM_FILE, GmatMainFrame::OnGenerateTextEphemFile)
//   EVT_MENU(MENU_TOOLS_MATLAB_INTERACTIVE, GmatMainFrame::OnMatlabInteractive)

   EVT_SASH_DRAGGED(ID_SASH_WINDOW, GmatMainFrame::OnSashDrag) 
   EVT_SASH_DRAGGED(ID_MSG_SASH_WINDOW, GmatMainFrame::OnMsgSashDrag) 
   
   EVT_SIZE(GmatMainFrame::OnMainFrameSize)
   EVT_CLOSE(GmatMainFrame::OnClose)
   EVT_SET_FOCUS(GmatMainFrame::OnSetFocus)
   
   EVT_MENU(MENU_SCRIPT_BUILD_OBJECT, GmatMainFrame::OnScriptBuildObject)
   EVT_MENU(MENU_SCRIPT_BUILD_AND_RUN, GmatMainFrame::OnScriptBuildAndRun)
   EVT_MENU(MENU_SCRIPT_RUN, GmatMainFrame::OnScriptRun)

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
//GmatMainFrame::GmatMainFrame(const wxString& title, const wxPoint& pos,
//                             const wxSize& size, long style)
//              : wxFrame(NULL, -1, title, pos, size, style)
GmatMainFrame::GmatMainFrame(wxWindow *parent,  const wxWindowID id,
                             const wxString& title, const wxPoint& pos, 
                             const wxSize& size, long style)
   : wxMDIParentFrame(parent, id, title, pos, size, style)
{
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() entered\n");
   #endif
   
   // set the script name
   scriptFilename = "$gmattempscript$.script";
   scriptCounter =0;

   // child frames
   trajSubframe = (MdiChildTrajFrame *)NULL;
   tsSubframe = (MdiChildTsFrame *)NULL;

   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
  
#if wxUSE_MENUS
   // create a menu bar 
   menuBar = new GmatMenuBar(0);
   SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
   // create a status bar
   //theStatusBar = CreateStatusBar(2);
   //loj: 3/20/06 int widths[] = {150, 600, 200};
   int widths[] = {150, 600, 300};
   theStatusBar = CreateStatusBar(3, wxBORDER);
   SetStatusWidths(3, widths);
   SetStatusText(_T("Welcome to GMAT!"));
#endif // wxUSE_STATUSBAR

   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::GmatMainFrame() creating ToolBar...\n");
   #endif
   
   CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL);
   InitToolBar(GetToolBar());
   
   // used to store the list of open children
   mdiChildren = new wxList();
   
   int w, h;
   GetClientSize(&w, &h);

   // A window w/sash for messages
   msgWin = new wxSashLayoutWindow(this, ID_MSG_SASH_WINDOW,
                           wxDefaultPosition, wxSize(30, 200),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
#ifdef __WXMAC__
   msgWin->SetDefaultSize(wxSize(w, (int) (0.25 * h)));
#else
   msgWin->SetDefaultSize(wxSize(w, 100));
#endif
   msgWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   msgWin->SetAlignment(wxLAYOUT_BOTTOM);
   msgWin->SetSashVisible(wxSASH_TOP, TRUE);

   // create MessageWindow and save in GmatApp for later use
   wxTextCtrl *msgTextCtrl =
      new wxTextCtrl(msgWin, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE);
   msgTextCtrl->SetMaxLength(320000);
   GmatAppData::GetMessageWindow()->Show(false);
   GmatAppData::SetMessageTextCtrl(msgTextCtrl);
   
   // A window w/sash for gmat notebook
   win = new wxSashLayoutWindow(this, ID_SASH_WINDOW,
                           wxDefaultPosition, wxSize(200, 30),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

#ifdef __WXMAC__
   //win->SetDefaultSize(wxSize(275, h));
   win->SetDefaultSize(wxSize(w, h));
#else
   win->SetDefaultSize(wxSize(200, h));
#endif
   win->SetOrientation(wxLAYOUT_VERTICAL);
   win->SetAlignment(wxLAYOUT_LEFT);
   win->SetSashVisible(wxSASH_RIGHT, TRUE);

   /*GmatNotebook *projectTree =*/ new GmatNotebook(win, -1, wxDefaultPosition,
                                wxDefaultSize, wxCLIP_CHILDREN);          
   //   new wxNotebookSizer(projectTree);
  
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
      MessageInterface::ShowMessage(e.GetMessage());
   }
   
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ~GmatMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame::~GmatMainFrame()
{
#ifdef __WXMAC__             // Need to close MATLAB on Mac
   MatlabInterface::Close();
#endif

   if (mServer)
      delete mServer;
   
   if (GmatAppData::GetMessageWindow() != NULL)
      GmatAppData::GetMessageWindow()->Close();
   
//    MessageInterface::ShowMessage("==========> Closing GmatMainFrame\n");
//    CloseAllChildren(true, true);
   
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
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateChild() item=%s\n", item->GetDesc().c_str());
   #endif
   
   GmatMdiChildFrame *newChild = NULL;

   // if child already open, just return
   if (IsChildOpen(item))
      return NULL;
   
   int dataType = item->GetDataType();

   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateChild() name=%s, dataType=%d\n",
       item->GetDesc().c_str(), dataType);
   #endif
   
   //----------------------------------------------------------------------
   // create a mdi child
   // Note: Do not change the order of ItemType in GmatTreeItemData.hpp.
   // The wrong order of dataType will not work propery.
   //----------------------------------------------------------------------
   if (dataType >= GmatTree::BEGIN_OF_RESOURCE &&
       dataType <= GmatTree::END_OF_RESOURCE)
   {
      newChild = CreateNewResource(item->GetDesc(), item->GetDesc(), dataType);
   }
   else if (dataType >= GmatTree::BEGIN_OF_COMMAND &&
            dataType <= GmatTree::END_OF_COMMAND)
   {
      newChild = CreateNewCommand(item->GetDesc(), item->GetDesc(), dataType,
                                  item->GetCommand());
   }
   else if (dataType >= GmatTree::BEGIN_OF_CONTROL &&
            dataType <= GmatTree::END_OF_CONTROL)
   {
      newChild = CreateNewControl(item->GetDesc(), item->GetDesc(), dataType,
                                  item->GetCommand());
   }
   else if (dataType >= GmatTree::BEGIN_OF_OUTPUT &&
            dataType <= GmatTree::END_OF_OUTPUT)
   {
      // Create panel if Report or Compare Report
      if (dataType == GmatTree::OUTPUT_REPORT ||
          dataType == GmatTree::COMPARE_REPORT)
         newChild = CreateNewOutput(item->GetDesc(), item->GetDesc(), dataType);
   }
   else
   {
      // do nothing
      #if DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("GmatMainFrame::CreateChild() Invalid item=%s dataType=%d entered\n",
          item->GetDesc().c_str(), dataType);
      #endif
   }

   return newChild;
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
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();

      #if DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("GmatMainFrame::IsChildOpen() title=%s\n   desc=%s\n",
          theChild->GetTitle().c_str(), item->GetDesc().c_str());
      #endif
    
      if ((theChild->GetTitle().IsSameAs(item->GetDesc().c_str())) &&
          (theChild->GetDataType() == item->GetDataType()))
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
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::GetChild() name=%s\n", name.c_str());
   #endif
   
   GmatMdiChildFrame *theChild = NULL;
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      theChild = (GmatMdiChildFrame *)node->GetData();

      #if DEBUG_MAINFRAME
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

   //loj: 1/17/06 commented out because the code is replaced by
   // RenameChild(wxString oldName, wxString newName)
   
//    wxNode *node = mdiChildren->GetFirst();
//    while (node)
//    {
//       GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();

//       #if DEBUG_MAINFRAME
//       MessageInterface::ShowMessage
//          ("GmatMainFrame::RenameChild() child %s  this %s\n",
//           theChild->GetTitle().c_str(), item->GetDesc().c_str());
//       #endif
    
//       if ((theChild->GetTitle().IsSameAs(item->GetDesc().c_str()))&&
//           (theChild->GetDataType() == item->GetDataType()))
//       {
//          theChild->SetTitle(newName);
//          return TRUE;
//       }
//       node = node->GetNext();
//    }
 
//    return FALSE;
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
   
//    wxNode *node = mdiChildren->GetFirst();
//    while (node)
//    {
//       GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();

//       #if DEBUG_MAINFRAME
//       MessageInterface::ShowMessage
//          ("GmatMainFrame::RenameChild() oldName=%s, newName=%s\n",
//           oldName.c_str(), newName.c_str());
//       #endif

//       if (theChild->GetTitle().IsSameAs(oldName))
//       {
//          theChild->SetTitle(newName);
//          return TRUE;
//       }
//       node = node->GetNext();
//    }

//    /// @todo: need to rename item in tree?

//    return FALSE;
}

//------------------------------------------------------------------------------
// bool RenameActiveChild(const wxString &newName)
//------------------------------------------------------------------------------
/**
 *
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
// void GmatMainFrame::RemoveChild(const wxString &item, int dataTpe)
//------------------------------------------------------------------------------
void GmatMainFrame::RemoveChild(const wxString &item, int dataType)
{
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      
      #if DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("GmatMainFrame::RemoveChild() title:%s\n   item:%s\n",
          theChild->GetTitle().c_str(), item.c_str());
      #endif
      
      if ((theChild->GetTitle().IsSameAs(item.c_str())) &&
           (theChild->GetDataType() == dataType))
      {
         delete theChild;
         delete node;
         break;
      }
      node = node->GetNext();
   }
}

//------------------------------------------------------------------------------
// void CloseActiveChild()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseActiveChild()
{
   GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)GetActiveChild();
   
   if (theChild != NULL)
   {
      wxCloseEvent event;
      theChild->OnClose(event);
      wxSafeYield();
   }
}


//------------------------------------------------------------------------------
// void CloseAllChildren(bool closeScriptWindow = true, bool closePlots = true,
//                       wxString excludeTitle = "")
//------------------------------------------------------------------------------
void GmatMainFrame::CloseAllChildren(bool closeScriptWindow, bool closePlots,
                                     wxString excludeTitle)
{   
   #if DEBUG_MAINFRAME_CLOSE
   MessageInterface::ShowMessage
      ("GmatMainFrame::CloseAllChildren() closeScriptWindow=%d, closePlots=%d\n   "
       "excludeTitle=%s\n", closeScriptWindow, closePlots, excludeTitle.c_str());
   #endif
   
   wxString title;
   int type;
   bool canDelete;
   
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      canDelete = false;
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      
      title = theChild->GetTitle();
      type = theChild->GetDataType();
      
      #if DEBUG_MAINFRAME_CLOSE
      MessageInterface::ShowMessage("   title=%s, type=%d\n", title.c_str(), type);
      #endif
      
      if ((type >= GmatTree::BEGIN_OF_RESOURCE && type <= GmatTree::END_OF_RESOURCE) ||
          (type >= GmatTree::BEGIN_OF_COMMAND && type <= GmatTree::END_OF_CONTROL))
      {
         // delete resource child
         if (type == GmatTree::SCRIPT_FILE)
         {
            if (closeScriptWindow)
               canDelete = true;
            else if (theChild->GetTitle() != excludeTitle)
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
            canDelete = true;
      }
      
      if (canDelete)
      {
         #if DEBUG_MAINFRAME_CLOSE
         MessageInterface::ShowMessage("   ==>deleting child=%s\n", title.c_str());
         #endif
         
         delete theChild;
         delete node;
      }
      
      node = node->GetNext();
      wxSafeYield();
   }
   wxSafeYield();
}


//------------------------------------------------------------------------------
// void GmatMainFrame::MinimizeChildren()
//------------------------------------------------------------------------------
void GmatMainFrame::MinimizeChildren()
{
   // do not need to check if script window is open
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      if (theChild->GetDataType() != GmatTree::OUTPUT_OPENGL_PLOT &&
          theChild->GetDataType() != GmatTree::OUTPUT_XY_PLOT &&
          theChild->GetDataType() != GmatTree::COMPARE_REPORT)
         theChild->Iconize(TRUE);
      node = node->GetNext();
   }

}

//------------------------------------------------------------------------------
// void GmatMainFrame::SetActiveChildDirty()
//------------------------------------------------------------------------------
void GmatMainFrame::SetActiveChildDirty(bool dirty)
{
   GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)GetActiveChild();
   
   if (theChild != NULL)
      theChild->SetDirty(dirty); 
}



//------------------------------------------------------------------------------
// void CloseCurrentProject()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseCurrentProject()
{
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::CloseCurrentProject()\n");
   #endif
   
   //close all windows
   CloseAllChildren();
   
   theGuiInterpreter->ClearResource();
   theGuiInterpreter->ClearCommandSeq();
   MessageInterface::ClearMessage();

   GmatAppData::GetResourceTree()->UpdateResource(true);
   GmatAppData::GetMissionTree()->UpdateMission(true);
}


//------------------------------------------------------------------------------
// void RunCurrentMission()
//------------------------------------------------------------------------------
void GmatMainFrame::RunCurrentMission()
{
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::RunCurrentMission() mRunPaused=%d\n", mRunPaused);
   #endif
   
   wxToolBar* toolBar = GetToolBar();
   
   
   menuBar->Enable(MENU_FILE_OPEN_SCRIPT, FALSE);
   UpdateMenus(FALSE);
   
   toolBar->EnableTool(MENU_FILE_OPEN_SCRIPT, FALSE);
   toolBar->EnableTool(TOOL_RUN, FALSE);
   toolBar->EnableTool(TOOL_PAUSE, TRUE);
   toolBar->EnableTool(TOOL_STOP, TRUE);
   
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
      theGuiInterpreter->RunMission();
   
      menuBar->Enable(MENU_FILE_OPEN_SCRIPT, TRUE);
      UpdateMenus(TRUE);
      toolBar->EnableTool(MENU_FILE_OPEN_SCRIPT, TRUE);
      toolBar->EnableTool(TOOL_RUN, TRUE);
      toolBar->EnableTool(TOOL_PAUSE, FALSE);
      toolBar->EnableTool(TOOL_STOP, FALSE);
      
      //put items in output tab
      GmatAppData::GetOutputTree()->UpdateOutput(false);
   }
} // end RunCurrentMission()


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
// void GmatMainFrame::StartServer()
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
      MessageInterface::ShowMessage("Server started.\n");
//      mServerMenu->Enable(MENU_START_SERVER, false);
//      mServerMenu->Enable(MENU_STOP_SERVER, true);
   }
   else
   {
      MessageInterface::ShowMessage("Server has already started.\n");
   }
}


//------------------------------------------------------------------------------
// void GmatMainFrame::StopServer()
//------------------------------------------------------------------------------
void GmatMainFrame::StopServer()
{
   if (mServer)
   {
      delete mServer;
      MessageInterface::ShowMessage("Server terminated.\n");
//      mServerMenu->Enable(MENU_START_SERVER, true);
//      mServerMenu->Enable(MENU_STOP_SERVER, false);
      mServer = NULL;
   }
   else
   {
      MessageInterface::ShowMessage("Server has not started.\n");
   }
}


//------------------------------------------------------------------------------
// void GmatMainFrame::OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnClose(wxCloseEvent& event)
{
   //MessageInterface::ShowMessage("===> GmatMainFrame::OnClose()\n");
   
   if (!mRunCompleted)
   {
      wxMessageBox(wxT("GMAT is still running the mission.\n"
                       "Please STOP the run before closing."),
                   wxT("GMAT Warning"));
      //ProcessCommand(TOOL_PAUSE); (loj: We don't need OnPause() here)
      return;
   }
   
   //CloseAllChildren(true, true); (loj: moved below)
   
   // prompt save
   
   if (theGuiInterpreter->HasConfigurationChanged())
   {
      wxMessageDialog *msgDlg =
         new wxMessageDialog(this,
                             "Would you like to save changes?", "Save...",
                             wxYES_NO | wxCANCEL |wxICON_QUESTION, wxDefaultPosition);
      
      int result = msgDlg->ShowModal();
      std::string oldScriptName = scriptFilename;

      if (result == wxID_CANCEL)
      {
         return;
      }
      else if (result == wxID_YES)
      {
         if (strcmp(scriptFilename.c_str(), "$gmattempscript$.script") == 0)
         {
            //open up dialog box to get the script name
            wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
                                _T("*.script"), wxSAVE );
            
            if (dialog.ShowModal() == wxID_OK)
            {
               scriptFilename = dialog.GetPath().c_str();

               if(FileExists(scriptFilename))
               {
                  MessageInterface::ShowMessage("File DE 2 - prompt");
                  if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"), 
                                   _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
                     GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
                  else
                  {
                     scriptFilename = oldScriptName;
                     return;
                  }
               }
               else
               {
                  //MessageInterface::ShowMessage("File DNE 2 - just save");
                  GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
               }
           
               GmatAppData::GetResourceTree()->AddScriptItem(scriptFilename.c_str());
               GmatAppData::GetResourceTree()->UpdateResource(false);
            }
         }
         else if (result == wxID_NO)
         {
            GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
         }
         
         //event.Skip();
      }
   }
   else
   {
      wxMessageDialog *msgDlg =
         new wxMessageDialog(this,
                             "Do you really want to exit?", "Exiting...",
                             wxYES_NO |wxICON_QUESTION, wxDefaultPosition);
      
      int result = msgDlg->ShowModal();
      
      if (result == wxID_NO)
         return;
   }

   CloseAllChildren(true, true);

   // Let Moderator::Finalize() clear all resource and commands (loj: 6/13/06)
//    theGuiInterpreter->ClearResource();
//    theGuiInterpreter->ClearCommandSeq();
   
   event.Skip();
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
   // ask user to continue because changes will be lost
   if (wxMessageBox(_T("Changes will be lost.\nDo you still want to continue?"), 
         _T("Please confirm"),
         wxICON_QUESTION | wxYES_NO) != wxYES)
   {
      return;
   }

   CloseCurrentProject();
   scriptFilename = "$gmattempscript$.script";
   theGuiInterpreter->LoadDefaultMission();

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
void GmatMainFrame::OnSaveScript(wxCommandEvent& WXUNUSED(event))
{
   std::string oldScriptName = scriptFilename;
   
   if (strcmp(scriptFilename.c_str(), "$gmattempscript$.script") == 0)
   {
      //open up dialog box to get the script name
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.script"), wxSAVE );
    
      if (dialog.ShowModal() == wxID_OK)
      {
         scriptFilename = dialog.GetPath().c_str();
         if(FileExists(scriptFilename))
         {
            MessageInterface::ShowMessage("File DE 3 - prompt");
            if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"), 
                             _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
               GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
            else
            {
               scriptFilename = oldScriptName;
               return;
            }
         }
         else
         {
            //MessageInterface::ShowMessage("File DNE 3 - just save");
            GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
         }
         
         GmatAppData::GetResourceTree()->AddScriptItem(scriptFilename.c_str());
         GmatAppData::GetResourceTree()->UpdateResource(false);
      }
   }
   else
   {
      GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
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
   //open up dialog box to get the script name
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.script"), wxSAVE );
   std::string oldScriptName = scriptFilename;
    
   if (dialog.ShowModal() == wxID_OK)
   {
      scriptFilename = dialog.GetPath().c_str();
      
      if(FileExists(scriptFilename))
      {
           MessageInterface::ShowMessage("File DE - prompt");
           if (wxMessageBox(_T("File already exists.\nDo you want to overwrite?"), 
               _T("Please confirm"), wxICON_QUESTION | wxYES_NO) == wxYES)
                   {
                      GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
                   }
           else
              scriptFilename = oldScriptName;
      }
      else
      {
          MessageInterface::ShowMessage("File DNE - just save");
          GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
      }
   }
}

bool GmatMainFrame::FileExists(std::string scriptFilename)
{
  FILE * pFile;
  pFile = fopen (scriptFilename.c_str(),"rt+");
  if (pFile!=NULL)
  {
    fclose (pFile);
    return true;
  }
  else
    return false;

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
   RunCurrentMission();
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
  
   menuBar->Enable(MENU_FILE_OPEN_SCRIPT, FALSE);
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
   wxToolBar* toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_STOP, FALSE);
   wxYield();
   
   theGuiInterpreter->ChangeRunState("Stop");
   mRunPaused = false;
   
   menuBar->Enable(MENU_FILE_OPEN_SCRIPT, TRUE);
   UpdateMenus(TRUE);
   toolBar->EnableTool(MENU_FILE_OPEN_SCRIPT, TRUE);
   toolBar->EnableTool(TOOL_RUN, TRUE);
}


//------------------------------------------------------------------------------
// void OnCloseChildren(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles closing all open children.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnCloseChildren(wxCommandEvent& WXUNUSED(event))
{
   CloseAllChildren();
   wxSafeYield();
}

//------------------------------------------------------------------------------
// void OnCloseCurrent(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles closing all open children.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnCloseCurrent(wxCommandEvent& WXUNUSED(event))
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
   wxString msg;
   msg.Printf(_T("General Mission Analysis Tool.\n")
               _T("Uses %s\n\nBuild Date: %s %s"), wxVERSION_STRING,
               __DATE__, __TIME__);      

   wxMessageBox(msg, _T("About GMAT"), wxOK | wxICON_INFORMATION, this);
}

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
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::InitToolBar() entered\n");
   #endif
   wxBitmap* bitmaps[15];
   
   bitmaps[0] = new wxBitmap(new_xpm);
   bitmaps[1] = new wxBitmap(open_xpm);
   bitmaps[2] = new wxBitmap(save_xpm);
   bitmaps[3] = new wxBitmap(copy_xpm);
   bitmaps[4] = new wxBitmap(cut_xpm);
   bitmaps[5] = new wxBitmap(paste_xpm);
   bitmaps[6] = new wxBitmap(print_xpm);
   bitmaps[7] = new wxBitmap(help_xpm);
   bitmaps[8] = new wxBitmap(play_xpm);
   bitmaps[9] = new wxBitmap(pause_xpm);
   bitmaps[10] = new wxBitmap(stop_xpm);
   bitmaps[11] = new wxBitmap(close_xpm);
   bitmaps[12] = new wxBitmap(tabclose_xpm);
   bitmaps[13] = new wxBitmap(script_xpm);
   bitmaps[14] = new wxBitmap(build_xpm);
   
  
   // add project tools
   toolBar->AddTool(MENU_FILE_NEW_SCRIPT, _T("New"), *bitmaps[0], _T("New Script"));
   toolBar->AddTool(MENU_FILE_OPEN_SCRIPT, _T("Open"), *bitmaps[1], _T("Open Script"));
   toolBar->AddTool(MENU_FILE_SAVE_SCRIPT, _T("Save"), *bitmaps[2], _T("Save to Script"));
   toolBar->AddSeparator();
   
   toolBar->AddTool(MENU_PROJECT_LOAD_DEFAULT_MISSION, _T("Default"), *bitmaps[13], 
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
   toolBar->AddTool(TOOL_CLOSE_CURRENT, _T("Close this Child"), *bitmaps[12], _T("Close this Child"));
   toolBar->AddSeparator();
   
   // add help tool
   toolBar->AddTool(MENU_HELP_ABOUT, _T("Help"), *bitmaps[7], _T("Help"));
   toolBar->AddSeparator();
   
   // now realize to make tools appear
   toolBar->Realize();
   
   //loj: Why separators are not showing on Windows?

   // disable tools
   toolBar->EnableTool(3, FALSE); // copy
   toolBar->EnableTool(4, FALSE); // cut
   toolBar->EnableTool(5, FALSE); // paste
   toolBar->EnableTool(6, FALSE); // print
   
   toolBar->EnableTool(TOOL_PAUSE, FALSE);
   toolBar->EnableTool(TOOL_STOP, FALSE);
   
   for (int i = 0; i < 15; i++)
   {
      delete bitmaps[i];
   }
   
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::InitToolBar() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewResource(const wxString &title,
//                                   const wxString &name, int dataType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewResource(const wxString &title,
                                 const wxString &name, int dataType)
{
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, dataType);

   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);

   switch (dataType)
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
   case GmatTree::FORMATION_FOLDER:
      sizer->Add(new FormationSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::UNIVERSE_FOLDER:
      sizer->Add(new UniversePanel(scrolledWin), 0, wxGROW|wxALL, 0);
      break;
      //case dataType == GmatTree::BODY:
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
      if (theGuiInterpreter->GetParameter(name.c_str())->GetTypeName() == "Variable" ||
          theGuiInterpreter->GetParameter(name.c_str())->GetTypeName() == "String")
      {
         sizer->Add(new ParameterSetupPanel(scrolledWin, name), 0, wxGROW|wxALL, 0);
      }
      else if (theGuiInterpreter->GetParameter(name.c_str())->GetTypeName() == "Array")
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

   // list of open children
   mdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
#ifndef __WXMSW__
   newChild->Show();
#endif

   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewCommand(const wxString &title,
//                                   const wxString &name, int dataType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewCommand(const wxString &title,
                                const wxString &name, int dataType,
                                GmatCommand *cmd)
{
   //MessageInterface::ShowMessage
   //   ("===> title=%s, name=%s, dataType=%d\n", title.c_str(), name.c_str(),
   //    dataType);
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, dataType);

   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);

   switch (dataType)
   {
   case GmatTree::PROPAGATE_COMMAND:
      sizer->Add(new PropagatePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::MANEUVER_COMMAND:
      sizer->Add(new ManeuverPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::BEGIN_FINITE_BURN_COMMAND:
      sizer->Add(new BeginFiniteBurnPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::END_FINITE_BURN_COMMAND:
      sizer->Add(new EndFiniteBurnPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::TARGET_COMMAND:
      sizer->Add(new TargetPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::ACHIEVE_COMMAND:
      sizer->Add(new AchievePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::VARY_COMMAND:
      sizer->Add(new VaryPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SAVE_COMMAND:
      sizer->Add(new SavePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::REPORT_COMMAND:
      sizer->Add(new ReportPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::TOGGLE_COMMAND:
      sizer->Add(new TogglePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::CALL_FUNCTION_COMMAND:
      sizer->Add(new CallFunctionPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::MINIMIZE_COMMAND:
      sizer->Add(new MinimizePanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::NON_LINEAR_CONSTRAINT_COMMAND:
      sizer->Add(new NonLinearConstraintPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   case GmatTree::SCRIPT_COMMAND:
   {
      ScriptEventPanel *scriptEventPanel = new ScriptEventPanel(scrolledWin, cmd);
      sizer->Add(scriptEventPanel, 0, wxGROW|wxALL, 0);
      newChild->SetScriptTextCtrl(scriptEventPanel->mFileContentsTextCtrl);
//      sizer->Add(new ScriptEventPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      break;
   }
      //case GmatTree::ASSIGNMENT_COMMAND:
      //sizer->Add(new AssignmentPanel(scrolledWin, cmd), 0, wxGROW|wxALL, 0);
      //break;
   default:
      MessageInterface::ShowMessage("===> returning NULL\n");
      return NULL;
   }
   
   scrolledWin->SetScrollRate(5, 5);
   scrolledWin->SetAutoLayout(TRUE);
   scrolledWin->SetSizer(sizer);
   sizer->Fit(scrolledWin);
   sizer->SetSizeHints(scrolledWin);
   
   // list of open children
   mdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif

   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewControl(const wxString &title,
//                                   const wxString &name, int dataType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewControl(const wxString &title,
                                const wxString &name, int dataType,
                                GmatCommand *cmd)
{
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, dataType);
   
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (dataType)
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

   // list of open children
   mdiChildren->Append(newChild);

   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif
   
   return newChild;
}


//------------------------------------------------------------------------------
// GmatMdiChildFrame* CreateNewOutput(const wxString &title,
//                                    const wxString &name, int dataType)
//------------------------------------------------------------------------------
GmatMdiChildFrame*
GmatMainFrame::CreateNewOutput(const wxString &title,
                               const wxString &name, int dataType)
{
   //MessageInterface::ShowMessage
   //   ("GmatMainFrame::CreateNewOutput() title=%s, name=%s, dataType=%d\n",
   //    title.c_str(), name.c_str(), dataType);
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   
   GmatMdiChildFrame *newChild =
      new GmatMdiChildFrame(this, title, name, dataType);
   
   wxScrolledWindow *scrolledWin = new wxScrolledWindow(newChild);
   
   switch (dataType)
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
   mdiChildren->Append(newChild);
   
   // djc: Under linux, force the new child to display
   #ifndef __WXMSW__
      newChild->Show();
   #endif
   
   return newChild;
}


//------------------------------------------------------------------------------
// bool InterpretScript(const wxString &filename)
//------------------------------------------------------------------------------
bool GmatMainFrame::InterpretScript(const wxString &filename)
{
   bool status = false;
   try
   {
      // If successfully interpreted, set status to true
      if (GmatAppData::GetGuiInterpreter()->
          InterpretScript(std::string(filename.c_str())))
      {
         status = true;
      }  
      else
      {
         wxLogError
            ("Error occurred during parsing.\nPlease check the syntax and try again\n");
         wxLog::FlushActive();
      } 
      
      // Always refresh the gui      
      CloseAllChildren(false, true, filename);
     
      // Update ResourceTree and MissionTree
      GmatAppData::GetResourceTree()->UpdateResource(true);
      GmatAppData::GetMissionTree()->UpdateMission(true);
      
   }
   catch (BaseException &e)
   {
      wxLogError(e.GetMessage().c_str());
      wxLog::FlushActive();
      MessageInterface::ShowMessage(e.GetMessage());
   }
   
   return status;
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
   name.Printf("Script%d.script", ++scriptCounter);

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
       mdiChildren->Append(newChild);

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
   
   if (GmatAppData::GetResourceTree()->wasChildAdded())
   {
      if (theGuiInterpreter->HasConfigurationChanged())
      {
          // need to save new file name because it gets overwritten in save
          std::string tmpFilename = scriptFilename;
          
          // ask user to continue because changes will be lost
          //loj:if (wxMessageBox(_T("Changes will be lost.\nDo you still want to save?"), 
          if (wxMessageBox(_T("Changes will be lost.\nDo you want to save?"), 
             _T("Please confirm"),
             wxICON_QUESTION | wxYES_NO) == wxYES)
          {
             OnSaveScriptAs(event);
          }
          
          scriptFilename = tmpFilename;
      }
      
      wxString statusText;
      statusText.Printf("%s - General Mission Analysis Tool (GMAT)", scriptFilename.c_str());       
       
      // let's try building the script
      SetStatusText("", 1);
      SetTitle(statusText);
      InterpretScript(scriptFilename.c_str());
   }
}

//------------------------------------------------------------------------------
// void OnScriptBuild(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles building script file from objects
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuild(wxCommandEvent& WXUNUSED(event))
{
   //loj: temp code
   GmatAppData::GetGuiInterpreter()->
      SaveScript("$gmattempscript$.script");
}

//------------------------------------------------------------------------------
// void OnGlPlotTrajectoryFile(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening trajectory file and draws 3D OpenGl plot.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnGlPlotTrajectoryFile(wxCommandEvent& WXUNUSED(event))
{
//   if (MdiGlPlot::mdiParentGlFrame == NULL)
//   {
//      MdiGlPlot::mdiParentGlFrame =
//         new MdiParentGlFrame((wxFrame *)NULL, -1, _T("MDI OpenGL Window"),
//                              wxPoint(300, 200), wxSize(600, 500),
//                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
//   }
//
//   // Give it an icon
//#ifdef __WXMSW__
//   MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon(_T("mdi_icn")));
//#else
//   MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon(mondrian_xpm));
//#endif
//
//   MdiGlPlot::mdiParentGlFrame->Show(TRUE);
//   //SetTopWindow(MdiGlPlot::mdiParentGlFrame);
}

//------------------------------------------------------------------------------
// void OnXyPlotTrajectoryFile(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening trajectory file and draws XY plot. It draws position againt
 * time.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnXyPlotTrajectoryFile(wxCommandEvent& WXUNUSED(event))
{
//   if (MdiXyPlot::mdiParentXyFrame == NULL)
//   {
//      MdiXyPlot::mdiParentXyFrame =
//         new MdiParentXyFrame((wxFrame *)NULL, -1, _T("MDI XY Window"),
//                              wxPoint(300, 200), wxSize(700, 600),
//                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
//   }

   // Give it an icon
//#ifdef __WXMSW__
//   MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon(_T("mdi_icn")));
//#else
//   MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon(mondrian_xpm));
//#endif
//
//   MdiXyPlot::mdiParentXyFrame->Show(TRUE);
   //SetTopWindow(MdiXyPlot::mdiParentXyFrame);
}

//------------------------------------------------------------------------------
// void OnStartServer(wxCommandEvent& WXUNUSED(event))
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
// void OnMatlabInteractive(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles interactivly evaluating a matlab expression
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnMatlabInteractive(wxCommandEvent& event)
{
   InteractiveMatlabDialog interactiveMatlabDlg(this);
   interactiveMatlabDlg.ShowModal();
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

   #if DEBUG_FILE_COMPARE
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
      
      textFrame = GmatAppData::GetMainFrame()->CreateChild(compareItem);
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

   #if DEBUG_FILE_COMPARE
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
      
      textFrame = GmatAppData::GetMainFrame()->CreateChild(compareItem);
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
                                    filename2.c_str(), filename3.c_str());
      
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
      MessageInterface::ShowMessage("** There is no files to compare.\n");
   }
   else
   {
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
        
   win->SetDefaultSize(wxSize(event.GetDragRect().width, h));

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
        
   msgWin->SetDefaultSize(wxSize(w, event.GetDragRect().height));
            

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
   //#if DEBUG_MAINFRAME
   //MessageInterface::ShowMessage("GmatMainFrame::OnSetFocus() entered\n");
   //#endif


//   wxObject *obj = event.GetEventObject();
//   bool isGmatMdiChild = obj->IsKindOf(CLASSINFO(wxMDIChildFrame));
//
//   if (isGmatMdiChild)
//   {
//      // need to find out its type
//      GmatMdiChildFrame *childFrame = (GmatMdiChildFrame *)obj;
//
//      if (childFrame->GetDataType() == GmatTree::SCRIPT_FILE)
//         MessageInterface::ShowMessage("ScriptFile brought into focus\n");
//      // set the menu
//   }

   //loj: GmatMainFrame is not getting Focus when iconized
   
   wxYield();
   
//    MessageInterface::ShowMessage
//       ("GmatMainFrame::OnSetFocus()  IsIconized=%d, IsEnabled=%d\n",
//        IsIconized(), IsEnabled());
   
   event.Skip(true);
}


//------------------------------------------------------------------------------
// void UpdateMenus(bool openOn)
//------------------------------------------------------------------------------
void GmatMainFrame::UpdateMenus(bool openOn)
{
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      theChild->menuBar->Enable(MENU_FILE_OPEN_SCRIPT, openOn);
      node = node->GetNext();
   }

}



//------------------------------------------------------------------------------
// void OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
   wxString filename = ((GmatMdiChildFrame *)GetActiveChild())->GetTitle();
   //wxLogStatus(GmatAppData::GetMainFrame(), "script:%s", filename.c_str());
   SetStatusText("", 1);
   SetTitle(filename +" - General Mission Analysis Tool (GMAT)");
   InterpretScript(filename);
}


//------------------------------------------------------------------------------
// void OnScriptBuildAndRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildAndRun(wxCommandEvent& event)
{
   //MessageInterface::ShowMessage("====> GmatMainFrame::OnScriptBuildAndRun()\n");
   
   //loj: 3/14 wxString filename = ((GmatMdiChildFrame *)GetActiveChild())->GetTitle();
   wxString filename = scriptFilename.c_str();
   SetStatusText("", 1);
   SetTitle(filename +" - General Mission Analysis Tool (GMAT)");
   
   if (InterpretScript(filename))
      OnRun(event);
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
   RunCurrentMission();
}


//------------------------------------------------------------------------------
// void OnUndo(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnUndo(wxCommandEvent& event)
{
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   theChild->GetScriptTextCtrl()->Undo();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnRedo(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnRedo(wxCommandEvent& event)
{
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   theChild->GetScriptTextCtrl()->Redo();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnCut(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnCut(wxCommandEvent& event)
{
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   theChild->GetScriptTextCtrl()->Cut();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnCopy(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnCopy(wxCommandEvent& event)
{
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   theChild->GetScriptTextCtrl()->Copy();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnPaste(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnPaste(wxCommandEvent& event)
{
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   theChild->GetScriptTextCtrl()->Paste();
//   theSaveButton->Enable(true);
}

//------------------------------------------------------------------------------
// void OnComment(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnComment(wxCommandEvent& event)
{
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   wxTextCtrl *scriptTC = theChild->GetScriptTextCtrl();
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
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   wxTextCtrl *scriptTC = theChild->GetScriptTextCtrl();
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
   GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
   wxTextCtrl *scriptTC = theChild->GetScriptTextCtrl();
   scriptTC->SetSelection(-1, -1);
}


//------------------------------------------------------------------------------
// void OnFont(wxCommandEvent& event)
//------------------------------------------------------------------------------
void GmatMainFrame::OnFont(wxCommandEvent& event)
{
//  GmatMdiChildFrame* theChild = (GmatMdiChildFrame *)GetActiveChild();
//  wxTextCtrl *scriptTC = theChild->GetScriptTextCtrl();

  wxFontData data;
  data.SetInitialFont(GmatAppData::GetFont());
//  data.SetColour(canvasTextColour);

  wxFontDialog dialog(this, &data);
  if (dialog.ShowModal() == wxID_OK)
  {
    wxFontData retData = dialog.GetFontData();
    wxFont newFont = retData.GetChosenFont();

    // change all script windows to new font
    wxNode *node = mdiChildren->GetFirst();
    while (node)
    {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
      if ((theChild->GetDataType() == GmatTree::SCRIPT_FILE)   ||
         (theChild->GetDataType() == GmatTree::OUTPUT_REPORT)  ||
         (theChild->GetDataType() == GmatTree::SCRIPT_COMMAND) ||
         (theChild->GetDataType() == GmatTree::GMAT_FUNCTION))
      {
         theChild->GetScriptTextCtrl()->SetFont(newFont);
      }
      node = node->GetNext();
    }

    GmatAppData::SetFont(newFont);
  }
}

//------------------------------------------------------------------------------
// void SetScriptFileName(std::string filename)
//------------------------------------------------------------------------------
void GmatMainFrame::SetScriptFileName(std::string filename)
{
    scriptFilename = filename;
}

