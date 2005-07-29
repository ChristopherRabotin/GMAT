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
#include "ManeuverPanel.hpp"
#include "XyPlotSetupPanel.hpp"
#include "OpenGlPlotSetupPanel.hpp"
#include "ReportFileSetupPanel.hpp"
#include "MessageInterface.hpp"
#include "SolverGoalsPanel.hpp"
#include "SolverVariablesPanel.hpp"
#include "TargetPanel.hpp"
#include "AchievePanel.hpp"
#include "VaryPanel.hpp"
#include "SavePanel.hpp"
#include "TogglePanel.hpp"
#include "ParameterSetupPanel.hpp"
#include "ArraySetupPanel.hpp"
#include "IfPanel.hpp"
#include "ForLoopPanel.hpp"
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

#include <wx/gdicmn.h>
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
#include "bitmaps/script.xpm"
#include "bitmaps/build.xpm"

//#define DEBUG_MAINFRAME 1
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
   EVT_MENU(TOOL_STOP, GmatMainFrame::OnStop)
   EVT_MENU(TOOL_CLOSE_CHILDREN, GmatMainFrame::OnCloseChildren)
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
//   EVT_MENU(MENU_TOOLS_MATLAB_INTERACTIVE, GmatMainFrame::OnMatlabInteractive)

   EVT_SASH_DRAGGED(ID_SASH_WINDOW, GmatMainFrame::OnSashDrag) 
   EVT_SASH_DRAGGED(ID_MSG_SASH_WINDOW, GmatMainFrame::OnMsgSashDrag) 

   EVT_SIZE(GmatMainFrame::OnMainFrameSize)
   EVT_SET_FOCUS(GmatMainFrame::OnFocus)
   EVT_CLOSE(GmatMainFrame::OnClose)

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
   : wxMDIParentFrame(parent, id, title, pos, size, 
                      style | wxNO_FULL_REPAINT_ON_RESIZE)
{
#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() entered\n");
#endif
   
   // set the script name
   scriptFilename = "$gmattempscript$.script";
   scriptCounter =0;

   // Set frame full/reduced size
   mFullSize = size;
   mReducedSize = wxSize(400, 200);

   // child frames
   trajSubframe = (MdiChildTrajFrame *)NULL;
//    trajMainSubframe = (MdiChildTrajFrame *)NULL;
   tsSubframe = (MdiChildTsFrame *)NULL;

   //   xySubframe = (MdiChildXyFrame *)NULL;
//    xyMainSubframe = (MdiChildXyFrame *)NULL;

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

   msgWin->SetDefaultSize(wxSize(w, 100));
   msgWin->SetOrientation(wxLAYOUT_HORIZONTAL);
   msgWin->SetAlignment(wxLAYOUT_BOTTOM);
   msgWin->SetSashVisible(wxSASH_TOP, TRUE);

   // create MessageWindow and save in GmatApp for later use
   wxTextCtrl *msgTextCtrl =
      new wxTextCtrl(msgWin, -1, _T(""), wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE);
   msgTextCtrl->SetMaxLength(320000); //loj:5/20/05 Added
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

   /*GmatNotebook *projectTree =*/ new GmatNotebook(win, -1, wxDefaultPosition,
                                wxDefaultSize, wxCLIP_CHILDREN);          
//   new wxNotebookSizer(projectTree);
  
   // set the main frame, because there will no longer be right notebook
   GmatAppData::SetMainFrame(this);

   mServer = NULL;
   
#if DEBUG_MAINFRAME
   MessageInterface::ShowMessage("GmatMainFrame::GmatMainFrame() exiting\n");
#endif
}

//------------------------------------------------------------------------------
// ~GmatMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame::~GmatMainFrame()
{
   if (mServer)
      delete mServer;
   
   if (GmatAppData::GetMessageWindow() != NULL)
      GmatAppData::GetMessageWindow()->Close();
   
//    MessageInterface::ShowMessage("==========> Closing GmatMainFrame\n");
//    CloseAllChildren(true, true);
   
   if (theGuiInterpreter)
      theGuiInterpreter->Finalize(); //loj: 7/8/05 Added
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
   #if DEBUG_MAINFRAME
   MessageInterface::ShowMessage
      ("GmatMainFrame::CreateChild() item=%s\n", item->GetDesc().c_str());
   #endif
   
   wxGridSizer *sizer = new wxGridSizer(1, 0, 0);
   panel = NULL;
   GmatMdiChildFrame *newChild = NULL;

   if (!IsChildOpen(item))
   {
      int dataType = item->GetDataType();
      long style = wxDEFAULT_FRAME_STYLE;  // | wxMAXIMIZE

      // curPages->Append((wxObject *) item);
      if (dataType == GmatTree::SPACECRAFT)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType );
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new SpacecraftPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::CELESTIAL_BODY)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType );
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new CelestialBodyPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }

      else if (dataType == GmatTree::FUELTANK)
      {
         //wxLogMessage("This panel is coming soon...");
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                            wxPoint(-1,-1), wxSize(-1,-1),
                                            style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new TankConfigPanel(panel, item->GetDesc()), 0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::THRUSTER)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                            wxPoint(-1,-1), wxSize(-1,-1),
                                            style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new ThrusterConfigPanel(panel, item->GetDesc()), 0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::FORMATION_FOLDER)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new FormationSetupPanel(panel, item->GetDesc()), 0, wxGROW|wxALL, 0);         
      }   
      else if (dataType == GmatTree::UNIVERSE_FOLDER)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new UniversePanel(panel), 0, wxGROW|wxALL, 0);
      }
      // invisible for build 2
      // else if (dataType == GmatTree::BODY)
      // {
      //     sizer->Add(new SolarSystemWindow(panel), 0, wxGROW|wxALL, 0);      
      // }
      else if (dataType == GmatTree::IMPULSIVE_BURN)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new ImpulsiveBurnSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::FINITE_BURN)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new FiniteBurnSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::PROPAGATOR)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new PropagationConfigPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::DIFF_CORR)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new DCSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::REPORT_FILE)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new ReportFileSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::XY_PLOT)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new XyPlotSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);          
      }
      else if (dataType == GmatTree::OPENGL_PLOT)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new OpenGlPlotSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::MISSION_SEQ_COMMAND)
      {
         return;
      }
      else if (dataType == GmatTree::PROPAGATE_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         
         #if DEBUG_MAINFRAME
         MessageInterface::ShowMessage
            ("GmatMainNotebook::CreatePage() creating PropagateCommand\n");
         #endif
         sizer->Add(new PropagatePanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
//        else if (dataType == GmatTree::PROPAGATE_COMMAND)
//        {
//           newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
//                                            wxPoint(-1,-1), wxSize(-1,-1),
//                                            style, dataType);
//           panel = new wxScrolledWindow(newChild);
//           sizer->Add(new PropagatePanel(panel, item->GetCommand()),
//                      0, wxGROW|wxALL, 0);
//        }
      else if (dataType == GmatTree::MANEUVER_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new ManeuverPanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::TARGET_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new TargetPanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::ACHIEVE_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);              
         sizer->Add(new AchievePanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::VARY_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new VaryPanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::SAVE_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new SavePanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::TOGGLE_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new TogglePanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::SCRIPT_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new ScriptEventPanel(panel, item->GetCommand()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::VARIABLE)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);

         if (theGuiInterpreter->GetParameter
             (std::string(item->GetDesc().c_str()))->GetTypeName() == "Variable" ||
             theGuiInterpreter->GetParameter
             (std::string(item->GetDesc().c_str()))->GetTypeName() == "String")
         {
            sizer->Add(new ParameterSetupPanel(panel, item->GetDesc()),
                       0, wxGROW|wxALL, 0);
         }
         else if (theGuiInterpreter->GetParameter
                  (std::string(item->GetDesc().c_str()))->GetTypeName() == "Array")
         {
            sizer->Add(new ArraySetupPanel(panel, item->GetDesc()),
                       0, wxGROW|wxALL, 0);
         }
         //sizer->Add(new ParameterSetupPanel(panel, item->GetDesc()),
         //           0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::GMAT_FUNCTION)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new FunctionSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::MATLAB_FUNCTION)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new MatlabFunctionSetupPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::SCRIPT_FILE)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         ScriptPanel *scriptPanel = new ScriptPanel(panel, item->GetDesc());
         sizer->Add(scriptPanel, 0, wxGROW|wxALL, 0);
         newChild->SetScriptTextCtrl(scriptPanel->mFileContentsTextCtrl);
      }
      else if (dataType == GmatTree::IF_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new IfPanel(panel, item->GetCommand()), 0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::WHILE_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         
         sizer->Add(new WhilePanel(panel, item->GetCommand()), 0, wxGROW|wxALL, 0);
      }
//      else if (dataType == GmatTree::DO_CONTROL)
//      {
//         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
//                                          wxPoint(-1,-1), wxSize(-1,-1),
//                                          style);
//         panel = new wxScrolledWindow(newChild);  
//         sizer->Add(new DoWhilePanel(panel), 0, wxGROW|wxALL, 0);
//      }
      else if (dataType == GmatTree::FOR_CONTROL)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);  
         sizer->Add(new ForLoopPanel(panel, item->GetCommand()), 0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::CALL_FUNCTION_COMMAND)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add (new CallFunctionPanel (panel, item->GetCommand()),
                     0, wxGROW|wxALL, 0 );
      }
//      else if (dataType == GmatTree::ASSIGNMENT_COMMAND)
//      {
//         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
//                                          wxPoint(-1,-1), wxSize(-1,-1),
//                                          style);
//         panel = new wxScrolledWindow(newChild);
//         sizer->Add (new AssignmentPanel (panel, item->GetCommand()),
//                     0, wxGROW|wxALL, 0 );
//      }
      else if (dataType == GmatTree::COORD_SYSTEM)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new CoordSystemConfigPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::BARYCENTER)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new BarycenterPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::LIBRATION_POINT)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new LibrationPointPanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else if (dataType == GmatTree::OUTPUT_REPORT)
      {
         newChild = new GmatMdiChildFrame(this, -1, item->GetDesc(),
                                          wxPoint(-1,-1), wxSize(-1,-1),
                                          style, item->GetDesc(), dataType);
         panel = new wxScrolledWindow(newChild);
         sizer->Add(new ReportFilePanel(panel, item->GetDesc()),
                    0, wxGROW|wxALL, 0);
      }
      else
      {
         // if no panel set up then just exit function
         // instead of opening blank panel
         return;
      }
      
      // maximize window
      //newChild->Maximize();

      if (newChild && panel)
      {
         // set the datatype, so the gmatnotebook minimize/cascade accordingly
//         newChild->SetDataType(dataType);
//         newChild->SetMenuBar(CreateMainMenu(dataType));

         panel->SetScrollRate(5, 5);
         panel->SetAutoLayout(TRUE);
         panel->SetSizer(sizer);
         sizer->Fit(panel);
         sizer->SetSizeHints(panel);

         // list of open children
         mdiChildren->Append(newChild);

         // djc: Under linux, force the new child to display
         #ifndef __WXMSW__
            newChild->Show();
         #endif
      }
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
    
      if ((theChild->GetTitle().IsSameAs(item->GetDesc().c_str()))&&
          (theChild->GetDataType() == item->GetDataType()))
      {
         theChild->SetTitle(newName);
         return TRUE;
      }
      node = node->GetNext();
   }
 
   return FALSE;
}

//------------------------------------------------------------------------------
// bool RenameChild(wxString oldName, wxString newName)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GmatMainFrame::RenameChild(wxString oldName, wxString newName)
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

      if (theChild->GetTitle().IsSameAs(oldName))
      {
         theChild->SetTitle(newName);
         return TRUE;
      }
      node = node->GetNext();
   }

   /// @todo: need to rename item in tree?

   return FALSE;
}


//------------------------------------------------------------------------------
// void GmatMainFrame::RemoveChild(wxString *item, int dataTpe)
//------------------------------------------------------------------------------
void GmatMainFrame::RemoveChild(wxString item, int dataType)
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
   wxCloseEvent event;
   theChild->OnClose(event);
}

//------------------------------------------------------------------------------
// void CloseAllChildren(bool closeScriptWindow, bool closePlots,
//                       wxString title)
//------------------------------------------------------------------------------
void GmatMainFrame::CloseAllChildren(bool closeScriptWindow, bool closePlots,
                                     wxString title)
{
   if (closeScriptWindow)
   {
      // do not need to check if script window is open
      wxNode *node = mdiChildren->GetFirst();
      while (node)
      {
         GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();
         delete theChild;
         delete node;
         node = node->GetNext();
      }
   }
   else
   {
      wxNode *node = mdiChildren->GetFirst();
      while (node)
      {
         GmatMdiChildFrame *theChild = (GmatMdiChildFrame *)node->GetData();

         if (theChild->GetTitle() != title)
         {
            delete theChild;
            delete node;
         }

         node = node->GetNext();
      }
   }

   if (closePlots)
   {
      //loj: 6/14/05
//       // close xy plots
//       for (int i=0; i<MdiXyPlot::numChildren; i++)
//       {
//          MdiChildXyFrame *frame = (MdiChildXyFrame*)(MdiXyPlot::mdiChildren[i]);
//          frame->Close(TRUE);
//       }
      
      // close ts plots
      int count = MdiTsPlot::numChildren;
      for (int i=0; i<count; ++i)
      {
         #if DEBUG_MAINFRAME
            MessageInterface::ShowMessage
               ("CloseAllChildren() MdiTsPlot::numChildren=%d\n",
                MdiTsPlot::numChildren);
         #endif
         MdiChildTsFrame *frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren[i]);
         frame->Close(TRUE);
      }
      
      // close gl plots
      #if DEBUG_MAINFRAME
      MessageInterface::ShowMessage
         ("CloseAllChildren() MdiGlPlot::numChildren=%d\n", MdiGlPlot::numChildren);
      #endif
      
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         MdiChildTrajFrame *frame = (MdiChildTrajFrame*)(MdiGlPlot::mdiChildren[i]);
         
         #if DEBUG_MAINFRAME
         MessageInterface::ShowMessage
            ("CloseAllChildren() frame[%d]=%s\n", i, frame->GetPlotName().c_str());
         #endif
         
         frame->Close(TRUE);
      }
   }
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
      if (theChild->GetDataType() != GmatTree::OUTPUT_OPENGL_PLOT)
         theChild->Iconize(TRUE);
      node = node->GetNext();
   }

}


//------------------------------------------------------------------------------
// void CloseCurrentProject()
//------------------------------------------------------------------------------
void GmatMainFrame::CloseCurrentProject()
{
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
   wxToolBar* toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_RUN, FALSE);
   toolBar->EnableTool(TOOL_STOP, TRUE);
   wxYield();
//   mFullSize = GetSize();
//   SetSize(mReducedSize);
//   SetFocus();
   
   theGuiInterpreter->RunMission();
   
   toolBar->EnableTool(TOOL_RUN, TRUE);
   toolBar->EnableTool(TOOL_STOP, FALSE);
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
   //show full size when mission run completed
   SetSize(mFullSize);
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
      mServerMenu->Enable(MENU_START_SERVER, false);
      mServerMenu->Enable(MENU_STOP_SERVER, true);
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
      mServerMenu->Enable(MENU_START_SERVER, true);
      mServerMenu->Enable(MENU_STOP_SERVER, false);
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
   CloseAllChildren(true, true);
   event.Skip();
}


//------------------------------------------------------------------------------
// wxToolBar* GmatMainFrame::GetMainFrameToolBar()
//------------------------------------------------------------------------------
wxToolBar* GmatMainFrame::GetMainFrameToolBar()
{
   return GetToolBar();
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
   if (strcmp(scriptFilename.c_str(), "$gmattempscript$.script") == 0)
   {
      //open up dialog box to get the script name
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.script"), wxSAVE );
    
      if (dialog.ShowModal() == wxID_OK)
      {
        scriptFilename = dialog.GetPath().c_str();
        GmatAppData::GetGuiInterpreter()->SaveScript(scriptFilename);
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
   wxToolBar* toolBar = GetToolBar();
   toolBar->EnableTool(TOOL_RUN, FALSE);
   toolBar->EnableTool(TOOL_STOP, TRUE);
   wxYield();
   mFullSize = GetSize();
//   SetSize(mReducedSize);
//   SetFocus();

   MinimizeChildren();
   theGuiInterpreter->RunMission();
   
   toolBar->EnableTool(TOOL_RUN, TRUE);
   toolBar->EnableTool(TOOL_STOP, FALSE);

   // put items in output tab
   GmatAppData::GetOutputTree()->UpdateOutput(false);
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
   SetSize(mFullSize);
   
   theGuiInterpreter->ChangeRunState("Stop");
   
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
   msg.Printf(_T("Goddard Mission Analysis Tool.\n")
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
   wxBitmap* bitmaps[14];

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
   bitmaps[12] = new wxBitmap(script_xpm);
   bitmaps[13] = new wxBitmap(build_xpm);

   int width = 24;
   int currentX = 5;
   
//   toolBar->AddTool(MENU_PROJECT_NEW, *(bitmaps[0]), wxNullBitmap, FALSE, currentX, -1,
//                     (wxObject *) NULL, _T("New project"));
   toolBar->AddTool(MENU_FILE_NEW_SCRIPT, *(bitmaps[0]), 
                     wxNullBitmap, FALSE, currentX, -1,
                     (wxObject *) NULL, _T("New Script"));
   currentX += width + 5;
   toolBar->AddTool(MENU_FILE_OPEN_SCRIPT, *bitmaps[1], wxNullBitmap, 
                     FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Open Script"));
   currentX += width + 5;
   toolBar->AddTool(MENU_FILE_SAVE_SCRIPT, *bitmaps[2], wxNullBitmap, 
                     FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Save to Script"));
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
//   toolBar->AddTool(TOOL_BUILD, *bitmaps[13], wxNullBitmap, FALSE, currentX, -1,
//                    (wxObject *) NULL, _T("Build"));
//   toolBar->AddSeparator();
   toolBar->AddTool(TOOL_RUN, *bitmaps[8], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Run"));
   toolBar->AddTool(TOOL_PAUSE, *bitmaps[9], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Pause"));
   toolBar->AddTool(TOOL_STOP, *bitmaps[10], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Stop"));

   toolBar->AddSeparator();
   toolBar->AddTool(TOOL_CLOSE_CHILDREN, *bitmaps[11], wxNullBitmap, FALSE,
                    currentX, -1, (wxObject *) NULL, _T("Close All"));
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
   
//   wxColour toolBarBackground = *wxLIGHT_GREY;
//   toolBar->SetBackgroundColour(toolBarBackground);

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
// wxMenuBar *CreateMainMenu(int dataType)
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
   wxMenu *toolsMenu = new wxMenu;
   wxMenu *helpMenu = new wxMenu;
 
   fileMenu->Append(MENU_FILE_NEW_SCRIPT, wxT("&New Script"));  
   fileMenu->Append(MENU_FILE_OPEN_SCRIPT, wxT("&Open Script"), wxT(""), FALSE);  
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT, wxT("&Save to Script"), wxT(""), FALSE);
   fileMenu->Append(MENU_FILE_SAVE_AS_SCRIPT, wxT("Save to Script As"),
                     wxT(""), FALSE);  

   fileMenu->AppendSeparator();
   fileMenu->Append(TOOL_CLOSE_CHILDREN, wxT("Close All"),
                     wxT(""), FALSE);

   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_LOAD_DEFAULT_MISSION, wxT("Default Project"), 
                     wxT(""), FALSE);   
   fileMenu->AppendSeparator();

   wxMenu *prefMenu = new wxMenu;
   prefMenu->Append(MENU_PROJECT_PREFERENCES_FONT, wxT("Font"));

   fileMenu->Append(MENU_PROJECT_PREFERENCES,
                        wxT("Preferences"), prefMenu, wxT(""));

   fileMenu->Append(MENU_SET_PATH_AND_LOG, wxT("Set File Paths and Log Level"),
                    wxT(""), FALSE);
   fileMenu->Append(MENU_INFORMATION, wxT("Information"), wxT(""), FALSE);

   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_PRINT, wxT("Print"), wxT(""), FALSE);
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_EXIT, wxT("Exit"), wxT(""), FALSE);
   
   fileMenu->Enable(MENU_SET_PATH_AND_LOG, FALSE);
   fileMenu->Enable(MENU_INFORMATION, FALSE);
   fileMenu->Enable(MENU_PROJECT_PRINT, FALSE);
   menuBar->Append(fileMenu, wxT("&File"));

   editMenu->Append(MENU_EDIT_RESOURCES, wxT("Resources"), wxT(""), FALSE);
   editMenu->Append(MENU_EDIT_MISSION, wxT("Mission"), wxT(""), FALSE);

   editMenu->Enable(MENU_EDIT_RESOURCES, FALSE);
   editMenu->Enable(MENU_EDIT_MISSION, FALSE);
   menuBar->Append(editMenu, wxT("Edit"));
   
   // Tools
   toolsMenu->Append(MENU_TOOLS_SWINGBY, wxT("Swingby"), wxT(""), FALSE); 
   toolsMenu->Enable(MENU_TOOLS_SWINGBY, FALSE);

   wxMenu *matlabMenu = new wxMenu;
   matlabMenu->Append(MENU_TOOLS_MATLAB_OPEN, wxT("Open"),
                          wxT(""), FALSE);
   matlabMenu->Append(MENU_TOOLS_MATLAB_CLOSE, wxT("Close"),
                          wxT(""), FALSE);
//   matlabMenu->AppendSeparator();
//   matlabMenu->Append(MENU_TOOLS_MATLAB_INTERACTIVE, wxT("Interact"),
//                          wxT(""), FALSE);
//   matlabMenu->Enable(MENU_TOOLS_MATLAB_INTERACTIVE, FALSE);


   toolsMenu->Append(MENU_TOOLS_MATLAB, wxT("Matlab"), matlabMenu, wxT(""));
   menuBar->Append(toolsMenu, wxT("Tools"));

   // Server
   mServerMenu = new wxMenu;
   mServerMenu->Append(MENU_START_SERVER, _T("Start"), _T("Start server"));
   mServerMenu->Append(MENU_STOP_SERVER, _T("Stop"), _T("Stop server"));
   menuBar->Append(mServerMenu, wxT("Server"));

   // Help
   helpMenu->Append(MENU_HELP_TOPICS, wxT("Topics"), wxT(""), FALSE);
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_ABOUT, wxT("About"), wxT(""), FALSE);
 
   helpMenu->Enable(MENU_HELP_TOPICS, FALSE);
   menuBar->Append(helpMenu, wxT("Help"));
    
   return menuBar;
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
   panel = NULL;
   GmatMdiChildFrame *newChild = new GmatMdiChildFrame(this, -1, name,
                    wxPoint(-1,-1), wxSize(-1,-1), wxDEFAULT_FRAME_STYLE, "",
                    GmatTree::SCRIPT_FILE);
   panel = new wxScrolledWindow(newChild);
   ScriptPanel *scriptPanel = new ScriptPanel(panel, "");
   sizer->Add(scriptPanel, 0, wxGROW|wxALL, 0);
   newChild->SetScriptTextCtrl(scriptPanel->mFileContentsTextCtrl);

   if (newChild && panel)
   {
       panel->SetScrollRate(5, 5);
       panel->SetAutoLayout(TRUE);
       panel->SetSizer(sizer);
       sizer->Fit(panel);
       sizer->SetSizeHints(panel);

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
   wxYield();
   event.Skip(true);
}

//------------------------------------------------------------------------------
// bool OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
   wxString filename = ((GmatMdiChildFrame *)GetActiveChild())->GetTitle();

   /*bool status = */GmatAppData::GetGuiInterpreter()->
      InterpretScript(std::string(filename.c_str()));

   //close the open windows
   GmatAppData::GetMainFrame()->CloseAllChildren(false, false, filename);

   // Update ResourceTree and MissionTree
   GmatAppData::GetResourceTree()->UpdateResource(true);
   GmatAppData::GetMissionTree()->UpdateMission(true);

//   return status;
}

//------------------------------------------------------------------------------
// bool OnScriptBuildAndRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void GmatMainFrame::OnScriptBuildAndRun(wxCommandEvent& event)
{
//   bool status = false;

//   status = OnScriptBuildObject(event);
   OnScriptBuildObject(event);
   OnRun(event);
   
//
//   status = GmatAppData::GetGuiInterpreter()->
//      InterpretScript(std::string(filename.c_str()));
//
//   if (status)
//   {
//      //close the open windows
//      GmatAppData::GetMainFrame()->CloseAllChildren(false, filename);
//
//      // Update ResourceTree
//      GmatAppData::GetResourceTree()->UpdateResource(true);
//      GmatAppData::GetMissionTree()->UpdateMission(true);
//
//      //loj: 9/24/04 added so that it enables Red(stop) button on the main frame
//      GmatAppData::GetMainFrame()->RunCurrentMission();
//
//      //loj: 3/17/04 Should I close all plot window?
//      //status = GmatAppData::GetGuiInterpreter()->RunScript();
//   }

//   return status;
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
   //MessageInterface::ClearMessage();

   //loj: 3/17/04 Should I close all plot window?

   //loj: 9/24/04 added so that it enables Red(stop) button on the main frame
   GmatAppData::GetMainFrame()->RunCurrentMission();

   //bool status = GmatAppData::GetGuiInterpreter()->RunScript();

//   return true;
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
      if (theChild->GetDataType() == GmatTree::SCRIPT_FILE)
      {
         theChild->GetScriptTextCtrl()->SetFont(newFont);
      }
      node = node->GetNext();
    }

    GmatAppData::SetFont(newFont);
  }
}

