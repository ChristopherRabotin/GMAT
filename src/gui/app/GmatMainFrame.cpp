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
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiXyPlotData.hpp"
#include "GmatNotebook.hpp"

#include "GmatTreeItemData.hpp"
#include "SolarSystemWindow.hpp"
#include "SpacecraftPanel.hpp"
#include "UniversePanel.hpp"
#include "PropagationConfigPanel.hpp"
#include "PropagateCommandPanel.hpp"
#include "ImpulsiveBurnSetupPanel.hpp"
#include "DCSetupPanel.hpp"
#include "ManeuverSetupPanel.hpp"
#include "XyPlotSetupPanel.hpp"
#include "OpenGlPlotSetupPanel.hpp"
#include "ReportFileSetupPanel.hpp"
#include "MessageInterface.hpp"
#include "SolverGoalsPanel.hpp"
#include "SolverVariablesPanel.hpp"
#include "SolverEventPanel.hpp"
#include "ParameterSetupPanel.hpp"
#include "ConditionalStatementPanel.hpp"
#include "ForLoopPanel.hpp"
#include "GmatMdiChildFrame.hpp"
#include "FormationSetupPanel.hpp"

#include <wx/gdicmn.h>

//loj: 8/5/04 moved from GmatMainFrame.hpp
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/run.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/stop.xpm"
#include "bitmaps/close.xpm"
#include "bitmaps/script.xpm"

#define DEBUG_MAINFRAME 0

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
   EVT_MENU(TOOL_RUN, GmatMainFrame::OnRun)
   EVT_MENU(TOOL_STOP, GmatMainFrame::OnStop)
   EVT_MENU(MENU_HELP_ABOUT, GmatMainFrame::OnHelpAbout)
   EVT_MENU(TOOL_CLOSE_TABS, GmatMainFrame::OnCloseTabs)
   EVT_MENU(MENU_SCRIPT_OPEN_EDITOR, GmatMainFrame::OnScriptOpenEditor)    
   EVT_MENU(MENU_SCRIPT_BUILD, GmatMainFrame::OnScriptBuild)    
   EVT_MENU(MENU_ORBIT_FILES_GL_PLOT_TRAJ_FILE, GmatMainFrame::OnGlPlotTrajectoryFile)    
   EVT_MENU(MENU_ORBIT_FILES_XY_PLOT_TRAJ_FILE, GmatMainFrame::OnXyPlotTrajectoryFile) 

   EVT_MENU(MENU_FILE_NEW_SCRIPT, GmatMainFrame::OnScriptOpenNewEditor)
   EVT_MENU(MENU_FILE_OPEN_SCRIPT, GmatMainFrame::OnScriptOpenFileEditor)
   
   EVT_SASH_DRAGGED(ID_SASH_WINDOW, GmatMainFrame::OnSashDrag) 
   EVT_SASH_DRAGGED(ID_MSG_SASH_WINDOW, GmatMainFrame::OnMsgSashDrag) 

   EVT_SIZE(GmatMainFrame::OnSize)
   EVT_SET_FOCUS(GmatMainFrame::OnFocus)
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
//GmatMainFrame::GmatMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
//                             long style)
//              : wxFrame(NULL, -1, title, pos, size, style)
GmatMainFrame::GmatMainFrame(wxWindow *parent,
                             const wxWindowID id,
                             const wxString& title, 
                             const wxPoint& pos, 
                             const wxSize& size,
                             long style)
   : wxMDIParentFrame(parent, id, title, pos, size, 
                      style | wxNO_FULL_REPAINT_ON_RESIZE)
{
#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() entered\n");
#endif
   
   // set the script name
   scriptFilename = "$gmattempscript$.script";
   
   mDocManager = (wxDocManager *) NULL;
//   GmatSplitterWindow *splitter;
//   GmatNotebook *leftTabs;

   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
  
#if wxUSE_MENUS
   // create a menu bar 
   SetMenuBar(CreateMainMenu());
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
   // create a status bar
   CreateStatusBar(2);
   SetStatusText(_T("Welcome to GMAT!"));
#endif // wxUSE_STATUSBAR

#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() creating ToolBar...\n");
#endif
   
   CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
   InitToolBar(GetToolBar());
   
   // used to store the list of open children
   mdiChildren = new wxList();
   
   int w, h;
   GetClientSize(&w, &h);

   // A window w/sash for messages
   msgWin = new wxSashLayoutWindow(this, ID_MSG_SASH_WINDOW,
                           wxDefaultPosition, wxSize(30, 200),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

   msgWin->SetDefaultSize(wxSize(w, 100));
   msgWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   msgWin->SetAlignment(wxLAYOUT_BOTTOM);
   msgWin->SetSashVisible(wxSASH_TOP, TRUE);

   // create MessageWindow and save in GmatApp for later use
   wxTextCtrl *msgTextCtrl = new wxTextCtrl(msgWin, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE);
   GmatAppData::GetMessageWindow()->Show(false);
   GmatAppData::SetMessageTextCtrl(msgTextCtrl);
   
   // A window w/sash for gmat notebook
   win = new wxSashLayoutWindow(this, ID_SASH_WINDOW,
                           wxDefaultPosition, wxSize(200, 30),
                           wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

   win->SetDefaultSize(wxSize(200, h));
   win->SetOrientation(wxLAYOUT_VERTICAL);
   win->SetAlignment(wxLAYOUT_LEFT);
   win->SetSashVisible(wxSASH_RIGHT, TRUE);

   GmatNotebook *projectTree = new GmatNotebook(win, -1, wxDefaultPosition,
                                wxDefaultSize, wxCLIP_CHILDREN);          
   new wxNotebookSizer( projectTree );  
  
//   theChild->Maximize();
   
   // set the main frame, because there will no longer be right notebook
   GmatAppData::SetMainFrame(this);

   // set the flag to say mdi is not open
   scriptMdiShown = false;

#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() exiting\n");
#endif
}

//------------------------------------------------------------------------------
// ~GmatMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame::~GmatMainFrame()
{
   if (GmatAppData::GetMessageWindow() != NULL)
      GmatAppData::GetMessageWindow()->Close();

   if (MdiGlPlot::mdiParentGlFrame != NULL)
      MdiGlPlot::mdiParentGlFrame->Close();
    
   if (MdiXyPlot::mdiParentXyFrame != NULL)
      MdiXyPlot::mdiParentXyFrame->Close();
}

//------------------------------------------------------------------------------
// void CreateChild(GmatTreeItemData *item)
//------------------------------------------------------------------------------
/**
 * Adds a page to notebook based on tree item type.
 *
 * @param <item> input GmatTreeItemData.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::CreateChild(GmatTreeItemData *item)
{
   
   // GmatAppData::GetMainNotebook()->CreatePage(item);
   
   wxGridSizer *sizer = new wxGridSizer( 1, 0, 0 );
   panel = NULL;
   GmatMdiChildFrame *newChild = NULL;

   if (!IsChildOpen(item))
   {
      int dataType = item->GetDataType();

      // curPages->Append( (wxObject *) item);
      if ((dataType == GmatTree::DEFAULT_SPACECRAFT )  ||
          (dataType == GmatTree::CREATED_SPACECRAFT ))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new SpacecraftPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == GmatTree::DEFAULT_FORMATION_FOLDER) ||
               (dataType == GmatTree::CREATED_FORMATION_FOLDER))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new FormationSetupPanel(panel, item->GetDesc()), 0, wxGROW|wxALL, 0 );         
      }   
      else if (dataType == GmatTree::UNIVERSE_FOLDER)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new UniversePanel(panel), 0, wxGROW|wxALL, 0 );
      }
      // invisible for build 2
      // else if ((dataType == GmatTree::DEFAULT_BODY)   ||
      //           (dataType == GmatTree::CREATED_BODY))
      // {
      //     sizer->Add( new SolarSystemWindow(panel), 0, wxGROW|wxALL, 0 );      
      // }
      else if ((dataType == GmatTree::DEFAULT_IMPULSIVE_BURN )  ||
               (dataType == GmatTree::CREATED_IMPULSIVE_BURN ))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new ImpulsiveBurnSetupPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == GmatTree::DEFAULT_PROPAGATOR)   ||
               (dataType == GmatTree::CREATED_PROPAGATOR))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new PropagationConfigPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == GmatTree::DEFAULT_DIFF_CORR)   ||
               (dataType == GmatTree::CREATED_DIFF_CORR))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new DCSetupPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == GmatTree::DEFAULT_REPORT_FILE)   ||
               (dataType == GmatTree::CREATED_REPORT_FILE))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new ReportFileSetupPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == GmatTree::DEFAULT_XY_PLOT)   ||
               (dataType == GmatTree::CREATED_XY_PLOT))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new XyPlotSetupPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );          
      }
      else if ((dataType == GmatTree::DEFAULT_OPENGL_PLOT)   ||
               (dataType == GmatTree::CREATED_OPENGL_PLOT))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new OpenGlPlotSetupPanel(panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::MISSION_SEQ_COMMAND)
      {
         return;
      }
      else if (dataType == GmatTree::DEFAULT_PROPAGATE_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);
         
#if DEBUG_MAINFRAME
         MessageInterface::ShowMessage
            ("GmatMainNotebook::CreatePage() creating PropagateCommand\n");
#endif
         sizer->Add( new PropagateCommandPanel(panel, item->GetDesc(), 
                                               item->GetCommand()),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::PROPAGATE_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new PropagateCommandPanel(panel, item->GetDesc(), item->GetCommand()),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::MANEUVER_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add( new ManeuverSetupPanel(panel, item->GetCommand()),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::TARGET_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add ( new SolverEventPanel (panel, item->GetDesc(), item->GetCommand()),
                      0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::VIEW_SOLVER_GOALS)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);              
         sizer->Add ( new SolverGoalsPanel (panel),
                      0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::VIEW_SOLVER_VARIABLES)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add ( new SolverVariablesPanel (panel),
                      0, wxGROW|wxALL, 0 );
      }
      else if ((dataType == GmatTree::DEFAULT_VARIABLE) ||
               (dataType == GmatTree::CREATED_VARIABLE))
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add (new ParameterSetupPanel (panel, item->GetDesc()),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::IF_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add (new ConditionalStatementPanel (panel, "IF"),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::WHILE_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add (new ConditionalStatementPanel (panel, "WHILE"),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::DO_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add (new ConditionalStatementPanel (panel, "DO WHILE"),
                     0, wxGROW|wxALL, 0 );
      }
      else if (dataType == GmatTree::FOR_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          wxMAXIMIZE  | wxDEFAULT_FRAME_STYLE);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add (new ForLoopPanel (panel),
                     0, wxGROW|wxALL, 0 );
      }
      else
      {
         // if no panel set up then just exit function
         // instead of opening blank panel
         return;
      }
         
      // maximize window
      //newChild->Maximize();

      panel->SetScrollRate( 5, 5 );
      panel->SetAutoLayout( TRUE );
      panel->SetSizer( sizer );
      sizer->Fit( panel );
      sizer->SetSizeHints( panel );

      // list of open children
      mdiChildren->Append(newChild);
   }
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
         ("GmatMainFrame::IsChildOpen() child %s  this %s\n",
          theChild->GetTitle().c_str(), item->GetDesc().c_str());
#endif
    
      if (theChild->GetTitle().IsSameAs(item->GetDesc().c_str()))
      {
         // move child to the front
         theChild->Activate();
         return TRUE;   
      }
      node = node->GetNext();
   }
 
   return FALSE;
}

//------------------------------------------------------------------------------
// bool RenameChild(GmatTreeItemData *item, wxString newName)
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
bool GmatMainFrame::RenameChild(GmatTreeItemData *item, wxString newName)
{
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();

#if DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("GmatMainFrame::GetChild() child %s  this %s\n",
          theChild->GetTitle().c_str(), item->GetDesc().c_str());
#endif
    
      if (theChild->GetTitle().IsSameAs(item->GetDesc().c_str()))
      {
         theChild->SetTitle(newName);
         return TRUE;   
      }
      node = node->GetNext();
   }
 
   return FALSE;
}

//------------------------------------------------------------------------------
// void GmatMainFrame::RemoveChild(wxString *item)
//------------------------------------------------------------------------------
void GmatMainFrame::RemoveChild(wxString item)
{
   wxNode *node = mdiChildren->GetFirst();
   while (node)
   {
      GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
#if DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("GmatMainFrame::RemoveChild() child %s  this %s\n",
          theChild->GetTitle().c_str(), item.c_str());
#endif
      if (theChild->GetTitle().IsSameAs(item.c_str()))
      {
         delete theChild;
         delete node;
         break;
      }
      node = node->GetNext();
   }
}

//------------------------------------------------------------------------------
// void GmatMainFrame::CloseActiveChild()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseActiveChild()
{
   GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)GetActiveChild();
   
   theChild->OnClose();
}

//------------------------------------------------------------------------------
// void GmatMainFrame::CloseAllChildren()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseAllChildren()
{
  wxNode *node = mdiChildren->GetFirst();
  while (node)
  {
    GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
    delete theChild;
    delete node;
    node = node->GetNext();
  }
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
   theGuiInterpreter->ClearResource();
   theGuiInterpreter->ClearCommandSeq();
   MessageInterface::ClearMessage();

   // close plot window on new project
   if (MdiGlPlot::mdiParentGlFrame != NULL)
      MdiGlPlot::mdiParentGlFrame->Close();
    
   if (MdiXyPlot::mdiParentXyFrame != NULL)
      MdiXyPlot::mdiParentXyFrame->Close();
    
   GmatAppData::GetResourceTree()->UpdateResource(true); //loj: 6/29/04 added true
   GmatAppData::GetMissionTree()->UpdateMission(true); //loj: 6/29/04 added true
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

   theGuiInterpreter->ClearResource();
   theGuiInterpreter->ClearCommandSeq();
    MessageInterface::ClearMessage();
    
   //close all windows
   CloseAllChildren();
       
   //close plot window on new project
   if (MdiGlPlot::mdiParentGlFrame != NULL)
      MdiGlPlot::mdiParentGlFrame->Close();
    
   if (MdiXyPlot::mdiParentXyFrame != NULL)
      MdiXyPlot::mdiParentXyFrame->Close();

   theGuiInterpreter->LoadDefaultMission();

   GmatAppData::GetResourceTree()->UpdateResource(true); //loj: 6/29/04 added true
   GmatAppData::GetMissionTree()->UpdateMission(true); //loj: 6/29/04 added true
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
   if (strcmp(scriptFilename.c_str(), "$gmattempscript$.script") == 0)
   {
      //open up dialog box to get the script name
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.script"));
    
      if (dialog.ShowModal() == wxID_OK)
      {
        scriptFilename = dialog.GetPath().c_str();
        GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
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
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.script"));
    
   if (dialog.ShowModal() == wxID_OK)
   {
      scriptFilename = dialog.GetPath().c_str();
      GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
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
   //loj: 8/16/04 added toolBar enable/disable
   wxToolBar* toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_RUN, FALSE);
   toolBar->EnableTool(TOOL_STOP, TRUE);
   wxYield();
   SetFocus();
   
   theGuiInterpreter->RunMission();
   
   toolBar->EnableTool(TOOL_RUN, TRUE);
   toolBar->EnableTool(TOOL_STOP, FALSE);
}

//loj: 8/17/04 added
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
   
   toolBar->EnableTool(TOOL_RUN, TRUE);
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
   msg.Printf( _T("Goddard Mission Analysis Tool.\n")
               _T("Uses %s\n\nBuild Date: %s %s"), wxVERSION_STRING,
               __DATE__, __TIME__);      

   wxMessageBox(msg, _T("About GMAT"), wxOK | wxICON_INFORMATION, this);
}

//------------------------------------------------------------------------------
// void OnCloseTabs(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles close tabs command from the tool bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnCloseTabs(wxCommandEvent& WXUNUSED(event))
{
//   rightTabs->ClosePage();
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
   wxBitmap* bitmaps[13];

   bitmaps[0] = new wxBitmap( new_xpm );
   bitmaps[1] = new wxBitmap( open_xpm );
   bitmaps[2] = new wxBitmap( save_xpm );
   bitmaps[3] = new wxBitmap( copy_xpm );
   bitmaps[4] = new wxBitmap( cut_xpm );
   bitmaps[5] = new wxBitmap( paste_xpm );
   bitmaps[6] = new wxBitmap( print_xpm );
   bitmaps[7] = new wxBitmap( help_xpm );
   bitmaps[8] = new wxBitmap( run_xpm );
   bitmaps[9] = new wxBitmap( pause_xpm );
   bitmaps[10] = new wxBitmap( stop_xpm );
   bitmaps[11] = new wxBitmap( close_xpm );
   bitmaps[12] = new wxBitmap( script_xpm );

   int width = 24;
   int currentX = 5;
   
//   toolBar->AddTool( MENU_PROJECT_NEW, *(bitmaps[0]), wxNullBitmap, FALSE, currentX, -1,
//                     (wxObject *) NULL, _T("New project"));
   toolBar->AddTool( MENU_FILE_NEW_SCRIPT, *(bitmaps[0]), 
                     wxNullBitmap, FALSE, currentX, -1,
                     (wxObject *) NULL, _T("New Script"));
   currentX += width + 5;
   toolBar->AddTool(MENU_FILE_OPEN_SCRIPT, *bitmaps[1], wxNullBitmap, 
                     FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Open Script"));
   currentX += width + 5;
   toolBar->AddTool(MENU_FILE_SAVE_SCRIPT, *bitmaps[2], wxNullBitmap, 
                     FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Save Script"));
   currentX += width + 5;
   toolBar->AddSeparator();

   toolBar->AddTool(MENU_PROJECT_LOAD_DEFAULT_MISSION, *bitmaps[12], 
                    wxNullBitmap, FALSE,
                    currentX, -1, (wxObject *) NULL, _T("Default Project"));
   currentX += width + 5;
   toolBar->AddSeparator();

   
   toolBar->AddTool(3, *bitmaps[3], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Copy"));
   currentX += width + 5;
   toolBar->AddTool(4, *bitmaps[4], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Cut"));
   currentX += width + 5;
   toolBar->AddTool(5, *bitmaps[5], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Paste"));
   currentX += width + 5;
   toolBar->AddSeparator();
   toolBar->AddTool(6, *bitmaps[6], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Print"));
   currentX += width + 5;
   toolBar->AddSeparator();

   toolBar->AddTool(TOOL_RUN, *bitmaps[8], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Run"));
   toolBar->AddTool(TOOL_PAUSE, *bitmaps[9], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Pause"));
   toolBar->AddTool(TOOL_STOP, *bitmaps[10], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Stop"));

//   toolBar->AddSeparator();
//   toolBar->AddTool(TOOL_CLOSE_TABS, *bitmaps[11], wxNullBitmap, FALSE,
//                    currentX, -1, (wxObject *) NULL, _T("Close Current Tab"));
   toolBar->AddSeparator();
   toolBar->AddTool(MENU_HELP_ABOUT, *bitmaps[7], wxNullBitmap, FALSE,
                    currentX, -1, (wxObject *) NULL, _T("Help"));

   toolBar->Realize();

   toolBar->EnableTool(3, FALSE); // copy
   toolBar->EnableTool(4, FALSE); // cut
   toolBar->EnableTool(5, FALSE); // paste
   toolBar->EnableTool(6, FALSE); // print
   
   toolBar->EnableTool(TOOL_PAUSE, FALSE);
   toolBar->EnableTool(TOOL_STOP, FALSE);
   
   wxColour toolBarBackground = *wxLIGHT_GREY;
   toolBar->SetBackgroundColour(toolBarBackground);

   int i;
   for (i = 0; i < 13; i++)
   {
      delete bitmaps[i];
   }
   
#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::InitToolBar() exiting\n");
#endif
}

//------------------------------------------------------------------------------
// wxMenuBar *CreateMainMenu()
//------------------------------------------------------------------------------
/**
 * Adds items to the menu.
 *
 * @return Menu bar.
 */
//------------------------------------------------------------------------------
wxMenuBar *GmatMainFrame::CreateMainMenu()
{
   wxMenuBar *menuBar = new wxMenuBar;
   wxMenu *fileMenu = new wxMenu;
   wxMenu *editMenu = new wxMenu;
   wxMenu *parametersMenu = new wxMenu;
   wxMenu *orbitFileMenu = new wxMenu;
//   wxMenu *variablesMenu = new wxMenu;
//   wxMenu *viewsMenu = new wxMenu;
   wxMenu *toolsMenu = new wxMenu;
   wxMenu *helpMenu = new wxMenu;
//   wxMenu *scriptMenu = new wxMenu;
 
   wxMenu *propagatorMenu;
//   wxMenu *openMenu, *saveMenu, *saveAsMenu, *propagatorMenu;
   
//   fileMenu->Append(MENU_PROJECT_NEW, wxT("New Project"), wxT(""), FALSE);
//   fileMenu->Append(MENU_PROJECT_LOAD_DEFAULT_MISSION, wxT("Load Default Mission"), wxT(""), FALSE);

   fileMenu->Append(MENU_FILE_NEW_SCRIPT, wxT("New Script"));  
   fileMenu->Append(MENU_FILE_OPEN_SCRIPT, wxT("Open Script"), wxT(""), FALSE);  
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT, wxT("Save Script"), wxT(""), FALSE);  
   fileMenu->Append(MENU_FILE_SAVE_AS_SCRIPT, wxT("Save Script As"), 
                     wxT(""), FALSE);  

//   openMenu = new wxMenu;
//   openMenu->Append(MENU_PROJECT_OPEN_BINARY, wxT("Binary"), wxT(""), FALSE);
//   openMenu->Append(MENU_PROJECT_OPEN_ASCII, wxT("ASCII"), wxT(""), FALSE);
//   fileMenu->Append(MENU_PROJECT_OPEN, wxT("Open Project"), openMenu, wxT(""));
//   
//   saveMenu = new wxMenu;
//   saveMenu->Append(MENU_PROJECT_SAVE_BINARY, wxT("Binary"), wxT(""), FALSE);
//   saveMenu->Append(MENU_PROJECT_SAVE_ASCII, wxT("ASCII"), wxT(""), FALSE);
//   fileMenu->Append(MENU_PROJECT_SAVE, wxT("Save Project"), saveMenu, wxT(""));
//   
//   saveAsMenu = new wxMenu;
//   saveAsMenu->Append(MENU_PROJECT_SAVE_AS_BINARY, wxT("Binary"), wxT(""),
//                      FALSE);
//   saveAsMenu->Append(MENU_PROJECT_SAVE_AS_ASCII, wxT("ASCII"), wxT(""),
//                      FALSE);
//   fileMenu->Append(MENU_PROJECT_SAVE_AS, wxT("Save Project As"), saveAsMenu,
//                    wxT(""));
   
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_LOAD_DEFAULT_MISSION, wxT("Default Project"), 
                     wxT(""), FALSE);   
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_PREFERENCES, wxT("Preferences"), wxT(""),
                    FALSE);
   fileMenu->Append(MENU_SET_PATH_AND_LOG, wxT("Set File Paths and Log Level"),
                    wxT(""), FALSE);
   fileMenu->Append(MENU_INFORMATION, wxT("Information"), wxT(""), FALSE);

   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_PRINT, wxT("Print"), wxT(""), FALSE);
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_EXIT, wxT("Exit"), wxT(""), FALSE);
   
   fileMenu->Enable(MENU_PROJECT_PREFERENCES, FALSE);
   fileMenu->Enable(MENU_SET_PATH_AND_LOG, FALSE);
   fileMenu->Enable(MENU_INFORMATION, FALSE);
   fileMenu->Enable(MENU_PROJECT_PRINT, FALSE);


//   scriptMenu->Append(MENU_SCRIPT_OPEN_EDITOR, wxT("Open Editor"), wxT(""), FALSE);
//   scriptMenu->Append(MENU_SCRIPT_BUILD, wxT("Build Script from Object"), wxT(""), FALSE);
    
   editMenu->Append(MENU_EDIT_CUT, wxT("Cut"), wxT(""), FALSE);
   editMenu->Append(MENU_EDIT_COPY, wxT("Copy"), wxT(""), FALSE);
   editMenu->Append(MENU_EDIT_PASTE, wxT("Paste"), wxT(""), FALSE);
   editMenu->AppendSeparator();
   editMenu->Append(MENU_EDIT_RESOURCES, wxT("Resources"), wxT(""), FALSE);
   editMenu->Append(MENU_EDIT_MISSION, wxT("Mission"), wxT(""), FALSE);

   editMenu->Enable(MENU_EDIT_CUT, FALSE);
   editMenu->Enable(MENU_EDIT_COPY, FALSE);
   editMenu->Enable(MENU_EDIT_PASTE, FALSE);
   editMenu->Enable(MENU_EDIT_RESOURCES, FALSE);
   editMenu->Enable(MENU_EDIT_MISSION, FALSE);
        
   parametersMenu->Append(MENU_PARAMETERS_PROP_CONFIG,
                          wxT("Propagation Configuration"), wxT(""), FALSE);
    
   propagatorMenu = new wxMenu;
   parametersMenu->Append(MENU_PARAMETERS_PROPAGATOR, wxT("Propagator"),
                          propagatorMenu, wxT(""));
                           
   parametersMenu->Append(MENU_PARAMETERS_LAUNCH_MODEL, wxT("Launch Model"),
                          wxT(""), FALSE);
   parametersMenu->Append(MENU_PARAMETERS_INJECTION_BURN_MODEL,
                          wxT("Injection Burn Model"), wxT(""), FALSE);
   parametersMenu->Append(MENU_PARAMETERS_SOLAR_RAD, wxT("Solar Radiation"),
                          wxT(""), FALSE);
   parametersMenu->Append(MENU_PARAMETERS_ORBIT_INFO,
                          wxT("Setup Orbit Information"), wxT(""), FALSE);
   parametersMenu->Append(MENU_PARAMETERS_ATTITUDE_MODES,
                          wxT("Setup Attitude Modes"), wxT(""), FALSE);
   parametersMenu->Append(MENU_PARAMETERS_SOLAR_SAILS,
                          wxT("Setup Solar Sails"), wxT(""), FALSE);
   parametersMenu->Append(MENU_PARAMETERS_SOLAR_ELEC_CONV,
                          wxT("Setup Solar Electric Conversion"), wxT(""),
                          FALSE);

   parametersMenu->Enable(MENU_PARAMETERS_PROP_CONFIG, FALSE);     
   parametersMenu->Enable(MENU_PARAMETERS_PROPAGATOR, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_LAUNCH_MODEL, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_INJECTION_BURN_MODEL, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_SOLAR_RAD, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_ORBIT_INFO, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_ATTITUDE_MODES, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_SOLAR_SAILS, FALSE);
   parametersMenu->Enable(MENU_PARAMETERS_SOLAR_ELEC_CONV, FALSE);
                     
   orbitFileMenu->Append(MENU_ORBIT_FILES_GL_PLOT_TRAJ_FILE,
                         wxT("Read/OpenGl Plot Trajectory File"), wxT(""), FALSE);
   orbitFileMenu->Append(MENU_ORBIT_FILES_XY_PLOT_TRAJ_FILE,
                         wxT("Read/XY Plot Trajectory File (time vs position)"), wxT(""), FALSE);
   orbitFileMenu->Append(MENU_ORBIT_FILES_EPHEM_FILE,
                         wxT("Read/Plot Ephemeris File"), wxT(""), FALSE);
   
   orbitFileMenu->Enable(MENU_ORBIT_FILES_GL_PLOT_TRAJ_FILE, FALSE);
   orbitFileMenu->Enable(MENU_ORBIT_FILES_XY_PLOT_TRAJ_FILE, FALSE);
   orbitFileMenu->Enable(MENU_ORBIT_FILES_EPHEM_FILE, FALSE);
      
//   variablesMenu->Append(MENU_VARIABLES_CREATE, wxT("Create"), wxT(""), FALSE); 
//   variablesMenu->Append(MENU_VARIABLES_EVALUATE, wxT("Evaluate"), wxT(""),
//                         FALSE);
//
//   viewsMenu->Append(MENU_VIEWS_COORD_FRAME, wxT("Coordinate Frame"), wxT(""),
//                     FALSE);
//   viewsMenu->AppendSeparator();
//   viewsMenu->Append(MENU_VIEWS_TARG_OUTPUT, wxT("Targeter Output"), wxT(""),
//                     FALSE);
//   viewsMenu->AppendSeparator();
//   viewsMenu->Append(MENU_VIEWS_CASCADE, wxT("Cascade Plots"), wxT(""), FALSE); 
//   viewsMenu->Append(MENU_VIEWS_TILE, wxT("Tile Plots"), wxT(""), FALSE); 
//   viewsMenu->AppendSeparator();
//   viewsMenu->Append(MENU_VIEWS_CLEAR, wxT("Clear Plots"), wxT(""), FALSE); 
//   viewsMenu->Append(MENU_VIEWS_MIN, wxT("Minimize Plots"), wxT(""), FALSE); 
//   viewsMenu->Append(MENU_VIEWS_RESTORE, wxT("Restore Plots"), wxT(""), FALSE); 
//   viewsMenu->Append(MENU_VIEWS_CLOSE, wxT("Close Plots"), wxT(""), FALSE); 

   toolsMenu->Append(MENU_TOOLS_SWINGBY, wxT("Swingby"), wxT(""), FALSE); 

   toolsMenu->Enable(MENU_TOOLS_SWINGBY, FALSE);

    
   helpMenu->Append(MENU_HELP_TOPICS, wxT("Topics"), wxT(""), FALSE);
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_ABOUT, wxT("About"), wxT(""), FALSE);
 
   helpMenu->Enable(MENU_HELP_TOPICS, FALSE);
  
   menuBar->Append(fileMenu, wxT("File"));
//   menuBar->Append(scriptMenu, wxT("Script"));
   menuBar->Append(editMenu, wxT("Edit"));
   menuBar->Append(parametersMenu, wxT("Parameters"));
   menuBar->Append(orbitFileMenu, wxT("Orbit Files"));
   //menuBar->Append(variablesMenu, wxT("Variables"));
   //menuBar->Append(viewsMenu, wxT("Views"));
   menuBar->Append(toolsMenu, wxT("Tools"));
    
   menuBar->Append(helpMenu, wxT("Help"));
    
   return menuBar;
}

//------------------------------------------------------------------------------
// wxMenuBar* CreateScriptWindowMenu(const std::string &docType)
//------------------------------------------------------------------------------
wxMenuBar* GmatMainFrame::CreateScriptWindowMenu(const std::string &docType)
{
   // Make a menubar
   wxMenu *fileMenu = new wxMenu;
   wxMenu *editMenu = (wxMenu *) NULL;
    
   fileMenu->Append(wxID_NEW, _T("&New..."));
   fileMenu->Append(wxID_OPEN, _T("&Open..."));

   if (docType == "sdi")
   {
      fileMenu->Append(wxID_CLOSE, _T("&Close"));
      fileMenu->Append(wxID_SAVE, _T("&Save"));
      fileMenu->Append(wxID_SAVEAS, _T("Save &As..."));
      fileMenu->AppendSeparator();
      fileMenu->Append(wxID_PRINT, _T("&Print..."));
      fileMenu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
      fileMenu->Append(wxID_PREVIEW, _T("Print Pre&view"));
 
      editMenu = new wxMenu;
      editMenu->Append(wxID_UNDO, _T("&Undo"));
      editMenu->Append(wxID_REDO, _T("&Redo"));
      editMenu->AppendSeparator();
      //editMenu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
    
      docMainFrame->editMenu = editMenu;
      fileMenu->AppendSeparator();
   }
    
   fileMenu->Append(wxID_EXIT, _T("E&xit"));
    
   // A nice touch: a history of files visited. Use this menu.
   mDocManager->FileHistoryUseMenu(fileMenu);
    
   //wxMenu *help_menu = new wxMenu;
   //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
   wxMenuBar *menuBar = new wxMenuBar;
    
   menuBar->Append(fileMenu, _T("&File"));
    
   if (editMenu)
      menuBar->Append(editMenu, _T("&Edit"));
    
   //menuBar->Append(help_menu, _T("&Help"));

   return menuBar;
}

//------------------------------------------------------------------------------
// void OnScriptOpenEditor(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptOpenEditor(wxCommandEvent& WXUNUSED(event))
{
   //----------------------------------------------------------------
   //not MAC mode - Create MDI frame
   //----------------------------------------------------------------
#if !defined __WXMAC__
   // Create a document manager
   mDocManager = new wxDocManager;

   // Create a template relating text documents to their views
   // use MdiTextDocument
   mDocTemplate = 
      new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                        _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                        CLASSINFO(MdiTextDocument), CLASSINFO(MdiTextEditView));
    
   // Create the main frame window    
   //loj: pass "this" so that this frame closes when the main frame closes
   mdiDocMainFrame =
      new MdiDocViewFrame(mDocManager, this, _T("Script Window (MDI)"),
                          wxPoint(0, 0), wxSize(600, 500),
                          (wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE));

   // Give it an icon (this is ignored in MDI mode: uses resources)
   mdiDocMainFrame->SetIcon(wxIcon(_T("doc")));
    
   // Make a menubar
   wxMenuBar *menuBar = CreateScriptWindowMenu("mdi");
       
   // Associate the menu bar with the frame
   mdiDocMainFrame->SetMenuBar(menuBar);
    
   mdiDocMainFrame->Centre(wxBOTH);
   mdiDocMainFrame->Show(TRUE);
   
   // set flag to say that mdi is open
   scriptMdiShown = true;
  
   //loj:compile error:
   //SetTopWindow(mdiDocMainFrame);
   //----------------------------------------------------------------
#else
   //----------------------------------------------------------------
   //other mode - Create SDI frame
   //----------------------------------------------------------------
   // Create a document manager
   mDocManager = new wxDocManager;

   // Create a template relating text documents to their views
   mDocTemplate = 
      new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                        _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                        CLASSINFO(TextDocument), CLASSINFO(TextEditView));
    
   // Create the main frame window    
   // pass "this" so that this frame closes when the main frame closes
   docMainFrame =
      new DocViewFrame(mDocManager, this, -1, _T("Script Window"),
                       wxPoint(0, 0), wxSize(600, 500), wxDEFAULT_FRAME_STYLE);
        
   // Make a menubar
   wxMenuBar *menuBar = CreateScriptWindowMenu("sdi");
       
   // Associate the menu bar with the frame
   docMainFrame->SetMenuBar(menuBar);
    
   docMainFrame->Centre(wxBOTH);
   docMainFrame->Show(TRUE);
    
   //loj:compile error:
   //SetTopWindow(docMainFrame);
#endif
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
   if (MdiGlPlot::mdiParentGlFrame == NULL)
   {
      MdiGlPlot::mdiParentGlFrame = 
         new MdiParentGlFrame((wxFrame *)NULL, -1, _T("MDI OpenGL Window"),
                              wxPoint(300, 200), wxSize(600, 500),
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
   }
    
   // Give it an icon
#ifdef __WXMSW__
   MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon(_T("mdi_icn")));
#else
   MdiGlPlot::mdiParentGlFrame->SetIcon(wxIcon( mondrian_xpm ));
#endif
    
   MdiGlPlot::mdiParentGlFrame->Show(TRUE);
   //SetTopWindow(MdiGlPlot::mdiParentGlFrame);
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
   if (MdiXyPlot::mdiParentXyFrame == NULL)
   {
      MdiXyPlot::mdiParentXyFrame = 
         new MdiParentXyFrame((wxFrame *)NULL, -1, _T("MDI XY Window"),
                              wxPoint(300, 200), wxSize(700, 600),
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
   }
    
   // Give it an icon
#ifdef __WXMSW__
   MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon(_T("mdi_icn")));
#else
   MdiXyPlot::mdiParentXyFrame->SetIcon(wxIcon( mondrian_xpm ));
#endif
    
   MdiXyPlot::mdiParentXyFrame->Show(TRUE);
   //SetTopWindow(MdiXyPlot::mdiParentXyFrame);
}

//------------------------------------------------------------------------------
// void OnScriptOpenEditor(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptOpenNewEditor(wxCommandEvent& event)
{
   if (scriptMdiShown == false)
      OnScriptOpenEditor(event);
//   else
//      mdiDocMainFrame->Iconize(false);
   
   if (mDocManager != NULL)
      mDocManager->OnFileNew(event);
}

//------------------------------------------------------------------------------
// void OnScriptOpenEditor(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptOpenFileEditor(wxCommandEvent& event)
{
   if (scriptMdiShown == false)
     OnScriptOpenEditor(event);

   if (mDocManager != NULL)
      mDocManager->OnFileOpen(event);
}

//------------------------------------------------------------------------------
// wxToolBar* GmatMainFrame::GetMainFrameToolBar()
//------------------------------------------------------------------------------
wxToolBar* GmatMainFrame::GetMainFrameToolBar()
{
   return GetToolBar();
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


//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles resizing of the window
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnSize(wxSizeEvent& event)
{
   wxLayoutAlgorithm layout;
   layout.LayoutMDIFrame(this);
}

//------------------------------------------------------------------------------
// void OnFocus(wxFocusEvent& event)
//------------------------------------------------------------------------------
/**
 * Handles focus event of the window
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void GmatMainFrame::OnFocus(wxFocusEvent& event)
{
#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::OnFocus() entered\n");
#endif
   
   wxYield();
   event.Skip(true);
}

