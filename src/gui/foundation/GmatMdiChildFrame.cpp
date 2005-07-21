//$Header$
//------------------------------------------------------------------------------
//                             GmatNotebook
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2003/08/28
/**
 * This class provides the layout of a mdi child frame
 */
//------------------------------------------------------------------------------
#include "GmatMdiChildFrame.hpp"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"

#include "GuiItemManager.hpp"

using namespace GmatMenu;

//------------------------------
// event tables for wxWindows
//------------------------------

//------------------------------------------------------------------------------
// EVENT_TABLE(GmatMdiChildFrame, wxMDIChildFrame)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatMdiChildFrame, wxMDIChildFrame)
   EVT_CLOSE(GmatMdiChildFrame::OnClose) 
END_EVENT_TABLE()

GmatMdiChildFrame::GmatMdiChildFrame(wxMDIParentFrame* parent, 
                       wxWindowID id, 
                       const wxString& title, 
                       const wxPoint& pos, 
                       const wxSize& size, 
                       long style, 
                       const wxString& name,
                       const int type)
                 :wxMDIChildFrame(parent, id, title, pos, size, style, name)
{
   dataType = type;
   this->title.Printf("%s", title.c_str());
   SetMenuBar(CreateMenu(dataType));
}

GmatMdiChildFrame::~GmatMdiChildFrame()
{
}


void GmatMdiChildFrame::OnClose(wxCloseEvent &event)
{
   // check if window is dirty?
  
   // remove from list of frames
   GmatAppData::GetMainFrame()->RemoveChild(title, dataType);

//   event.Skip();
}

#ifdef __WXMAC__
void GmatMdiChildFrame::SetTitle(wxString newTitle)
{
//   SetTitle(newTitle);
   title = newTitle;
}

wxString GmatMdiChildFrame::GetTitle()
{
   return title;
}
#endif

int GmatMdiChildFrame::GetDataType()
{
   return dataType;
}

//------------------------------------------------------------------------------
// wxMenuBar *CreateMenu(int dataType)
//------------------------------------------------------------------------------
/**
 * Adds items to the menu.
 *
 * @return Menu bar.
 */
//------------------------------------------------------------------------------
wxMenuBar *GmatMdiChildFrame::CreateMenu(int dataType)
{
   wxMenuBar *menuBar = new wxMenuBar;
   wxMenu *fileMenu = new wxMenu;
   wxMenu *editMenu = new wxMenu;
   wxMenu *toolsMenu = new wxMenu;
   wxMenu *helpMenu = new wxMenu;

   fileMenu->Append(MENU_FILE_NEW_SCRIPT, wxT("New Script"));
   fileMenu->Append(MENU_FILE_OPEN_SCRIPT, wxT("Open Script"), wxT(""), FALSE);

   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
   {
      fileMenu->Append(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, _T("&Open Trajectory File"));
      fileMenu->Enable(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, false);
   }
//   if (dataType == GmatTree::OUTPUT_XY_PLOT)
//      fileMenu->Append(GmatPlot::MDI_TS_OPEN_PLOT_FILE, _T("&Open XY Plot File"));

   fileMenu->Append(MENU_FILE_SAVE_SCRIPT, wxT("Save to Script"), wxT(""), FALSE);
   fileMenu->Append(MENU_FILE_SAVE_AS_SCRIPT, wxT("Save to Script As"),
                     wxT(""), FALSE);

   fileMenu->AppendSeparator();
   fileMenu->Append(TOOL_CLOSE_CHILDREN, wxT("Close All"),
                     wxT(""), FALSE);

   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
      fileMenu->Append(GmatPlot::MDI_GL_CHILD_QUIT, _T("&Close Plot"), _T("Close this window"));
//   if (dataType == GmatTree::OUTPUT_XY_PLOT)
//      fileMenu->Append(GmatPlot::MDI_TS_CHILD_QUIT, _T("&Close Plot"),
//         _T("Close this window"));


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

//   fileMenu->Enable(MENU_PROJECT_PREFERENCES, FALSE);
   fileMenu->Enable(MENU_SET_PATH_AND_LOG, FALSE);
   fileMenu->Enable(MENU_INFORMATION, FALSE);
   fileMenu->Enable(MENU_PROJECT_PRINT, FALSE);
   menuBar->Append(fileMenu, wxT("File"));

   if (dataType == GmatTree::SCRIPT_FILE)
   {
      editMenu->Append(MENU_EDIT_UNDO, wxT("Undo\tCtrl-Z"), wxT(""), FALSE);
      editMenu->Append(MENU_EDIT_REDO, wxT("Redo\tShift-Ctrl-Z"), wxT(""), FALSE);
      editMenu->AppendSeparator();
      editMenu->Append(MENU_EDIT_CUT, wxT("Cut\tCtrl-X"), wxT(""), FALSE);
      editMenu->Append(MENU_EDIT_COPY, wxT("Copy\tCtrl-C"), wxT(""), FALSE);
      editMenu->Append(MENU_EDIT_PASTE, wxT("Paste\tCtrl-V"), wxT(""), FALSE);
      editMenu->AppendSeparator();
      editMenu->Append(MENU_EDIT_SELECT_ALL, wxT("Select All\tCtrl-A"), wxT(""), FALSE);
      editMenu->AppendSeparator();
      editMenu->Append(MENU_EDIT_COMMENT, wxT("Comment"), wxT(""), FALSE);
      editMenu->Append(MENU_EDIT_UNCOMMENT, wxT("Uncomment"), wxT(""), FALSE);
      editMenu->AppendSeparator();
   }
   
   editMenu->Append(MENU_EDIT_RESOURCES, wxT("Resources"), wxT(""), FALSE);
   editMenu->Append(MENU_EDIT_MISSION, wxT("Mission"), wxT(""), FALSE);

   //   editMenu->Enable(MENU_EDIT_CUT, FALSE);
   //   editMenu->Enable(MENU_EDIT_COPY, FALSE);
   //   editMenu->Enable(MENU_EDIT_PASTE, FALSE);
   editMenu->Enable(MENU_EDIT_RESOURCES, FALSE);
   editMenu->Enable(MENU_EDIT_MISSION, FALSE);
   menuBar->Append(editMenu, wxT("Edit"));

   // create and add script menu
   if (dataType == GmatTree::SCRIPT_FILE)
   {
      wxMenu *scriptMenu = new wxMenu;
      scriptMenu->Append(MENU_SCRIPT_BUILD_OBJECT,
            _T("&Build Object"));
      scriptMenu->Append(MENU_SCRIPT_BUILD_AND_RUN,
            _T("&Build and Run"));
      scriptMenu->Append(MENU_SCRIPT_RUN, _T("&Run"));
      menuBar->Append(scriptMenu, wxT("Script"));
   }

   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
   {
      // Plot menu
      wxMenu *plotMenu = new wxMenu;

      plotMenu->Append(GmatPlot::MDI_GL_CLEAR_PLOT, _T("Clear Plot"));
      plotMenu->AppendSeparator();
      plotMenu->Append(GmatPlot::MDI_GL_CHANGE_TITLE, _T("Change &title..."));
      menuBar->Append(plotMenu, wxT("Plot"));

      // View menu
//      wxMenu *mViewMenu = new wxMenu;
      mViewMenu = new wxMenu;
      mViewMenu->Append(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, _T("Default\tCtrl-R"),
                       _("Reset to default view"));
      mViewMenu->Append(GmatPlot::MDI_GL_ZOOM_IN, _T("Zoom &in\tCtrl-I"), _("Zoom in"));
      mViewMenu->Append(GmatPlot::MDI_GL_ZOOM_OUT, _T("Zoom &out\tCtrl-O"), _("Zoom out"));
      mViewMenu->AppendSeparator();

      // View Option submenu
      mViewMenu->Append(GmatPlot::MDI_GL_SHOW_OPTION_PANEL,
                        _T("Show View Option Dialog"),
                        _T("Show view option dialog"), wxITEM_CHECK);

      mViewOptionMenu = new wxMenu;
      wxMenuItem *item =
         new wxMenuItem(mViewMenu, GmatPlot::MDI_GL_VIEW_OPTION, _T("Option"),
                        _T("Show bodies in wire frame"), wxITEM_NORMAL, mViewOptionMenu);
      mViewOptionMenu->Append(GmatPlot::MDI_GL_SHOW_WIRE_FRAME,
                             _T("Show Wire Frame"),
                             _T("Show bodies in wire frame"), wxITEM_CHECK);
      mViewOptionMenu->Append(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE,
                             _T("Show Equatorial Plane"),
                             _T("Show equatorial plane lines"), wxITEM_CHECK);

      mViewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, true);

      mViewMenu->Append(item);

      // Animation menu
      mViewMenu->AppendSeparator();
      mViewMenu->Append(GmatPlot::MDI_GL_VIEW_ANIMATION, _T("Animation"));
      menuBar->Append(mViewMenu, wxT("View"));
   }

   //loj: 6/14/05 Changed MDI_XY_ to MDI_TS_
   if (dataType == GmatTree::OUTPUT_XY_PLOT)
   {
      // Plot menu
 //     wxMenu *plotMenu = new wxMenu;

//      plotMenu->Append(GmatPlot::MDI_TS_CLEAR_PLOT, _T("Clear Plot"));
//      plotMenu->AppendSeparator();
//      plotMenu->Append(GmatPlot::MDI_TS_CHANGE_TITLE, _T("Change &title..."));
//
//      // View menu
 //     wxMenu *viewMenu = new wxMenu;
//      viewMenu->Append(GmatPlot::MDI_TS_SHOW_DEFAULT_VIEW, _T("Reset\tCtrl-R"),
//                       _("Reset to default view"));
//      viewMenu->AppendSeparator();
//
//      // View Option submenu
//      mViewOptionMenu = new wxMenu;
//      wxMenuItem *item =
//         new wxMenuItem(viewMenu, GmatPlot::MDI_TS_VIEW_OPTION, _T("Option"),
//                        _T("view options"), wxITEM_NORMAL, mViewOptionMenu);
//      mViewOptionMenu->Append(GmatPlot::MDI_TS_DRAW_GRID,
//                             _T("Draw Grid"),
//                             _T("Draw Grid"), wxITEM_CHECK);
//      mViewOptionMenu->Append(GmatPlot::MDI_TS_DRAW_DOTTED_LINE,
//                             _T("Draw dotted line"),
//                             _T("Draw dotted line"), wxITEM_CHECK);
//
//      mViewOptionMenu->Check(GmatPlot::MDI_TS_DRAW_DOTTED_LINE, false);
//
//      viewMenu->Append(item);


      // Help menu
//      wxMenu *helpMenu = new wxMenu;
//      helpMenu->Append(GmatPlot::MDI_TS_HELP_VIEW, _T("View"), _T("View mouse control"));

//      menuBar->Append(plotMenu, wxT("Plot"));
//      menuBar->Append(viewMenu, wxT("View"));
//      menuBar->Append(mViewMenu, wxT("View"));
   }

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
   wxMenu *mServerMenu = new wxMenu;
   mServerMenu->Append(MENU_START_SERVER, _T("Start"), _T("Start server"));
   mServerMenu->Append(MENU_STOP_SERVER, _T("Stop"), _T("Stop server"));
   menuBar->Append(mServerMenu, wxT("Server"));

   // Help
   helpMenu->Append(MENU_HELP_TOPICS, wxT("Topics"), wxT(""), FALSE);
   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
      helpMenu->Append(GmatPlot::MDI_GL_HELP_VIEW, _T("Plot View"), _T("View mouse control"));
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_ABOUT, wxT("About"), wxT(""), FALSE);

   helpMenu->Enable(MENU_HELP_TOPICS, FALSE);
   menuBar->Append(helpMenu, wxT("Help"));

   return menuBar;
}

