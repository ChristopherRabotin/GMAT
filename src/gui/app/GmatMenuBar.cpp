//$Header$
//------------------------------------------------------------------------------
//                             GmatMenuBar
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Allison Greene
// Created: 2006/05/09
/**
 * This class provides the menu bar for the main frame.
 */
//------------------------------------------------------------------------------
#include "GmatMenuBar.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MdiTsPlotData.hpp"
#include "MessageInterface.hpp"

// for more GL view option menu
//#define _SHOW_GL_VIEW_MENU__

//#define DEBUG_MENUBAR 1

using namespace GmatMenu;
//------------------------------------------------------------------------------
// EVENT_TABLE(GmatMenuBar, wxMenuBar)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatMenuBar, wxMenuBar)
   EVT_MENU(GmatPlot::MDI_GL_SHOW_DEFAULT_VIEW, MdiChildTrajFrame::OnShowDefaultView)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatMenuBar( long style)
//------------------------------------------------------------------------------
GmatMenuBar::GmatMenuBar(int dataType, long style)
   : wxMenuBar(style)
{
  CreateMenu(dataType);
}

//------------------------------------------------------------------------------
// void CreateMenu(int dataType)
//------------------------------------------------------------------------------
/**
 * Adds items to the menu.
 *
 */
//------------------------------------------------------------------------------
void GmatMenuBar::CreateMenu(int dataType)
{
   //-------------------------------------------------------
   // File menu
   //-------------------------------------------------------
   wxMenu *fileMenu = new wxMenu;
   fileMenu->Append(MENU_FILE_NEW_SCRIPT, wxT("&New Script"));  
   fileMenu->Append(MENU_FILE_OPEN_SCRIPT, wxT("&Open Script"), wxT(""), FALSE);  

   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
   {
      // chances are that script will be running when plot is first opened...
      fileMenu->Enable(MENU_FILE_OPEN_SCRIPT, FALSE);
      fileMenu->Append(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, _T("&Open Trajectory File"));
      fileMenu->Enable(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, false);
   }
   
   if (dataType == GmatTree::OUTPUT_XY_PLOT)
   {
       fileMenu->Append(GmatPlot::MDI_TS_OPEN_PLOT_FILE, _T("&Open XY Plot File"));
   }
   
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT, wxT("&Save to Script"), wxT(""), FALSE);
   fileMenu->Append(MENU_FILE_SAVE_AS_SCRIPT, wxT("Save to Script As"),
                     wxT(""), FALSE);  

   fileMenu->AppendSeparator();
   fileMenu->Append(TOOL_CLOSE_CHILDREN, wxT("Close All"),
                     wxT(""), FALSE);
                     
   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
      fileMenu->Append(GmatPlot::MDI_GL_CHILD_QUIT, _T("&Close Plot"), _T("Close this window"));
   else if (dataType == GmatTree::OUTPUT_XY_PLOT)
      fileMenu->Append(GmatPlot::MDI_TS_CHILD_QUIT, _T("&Close"),
         _T("Close this window")); 
   else
      fileMenu->Append(TOOL_CLOSE_CURRENT, wxT("Close this Child"),
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
   this->Append(fileMenu, wxT("&File"));

   //-------------------------------------------------------
   // Edit menu
   //-------------------------------------------------------
   wxMenu *editMenu = new wxMenu;

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
      editMenu->Append(MENU_EDIT_COMMENT, wxT("Comment\tCtrl-T"), wxT(""), FALSE);
      editMenu->Append(MENU_EDIT_UNCOMMENT, wxT("Uncomment\tCtrl-M"), wxT(""), FALSE);
      editMenu->AppendSeparator();
   }

   editMenu->Append(MENU_EDIT_RESOURCES, wxT("Resources"), wxT(""), FALSE);
   editMenu->Append(MENU_EDIT_MISSION, wxT("Mission"), wxT(""), FALSE);

   editMenu->Enable(MENU_EDIT_RESOURCES, FALSE);
   editMenu->Enable(MENU_EDIT_MISSION, FALSE);
   this->Append(editMenu, wxT("Edit"));
   
   //-------------------------------------------------------
   // Script menu
   //-------------------------------------------------------
   if (dataType == GmatTree::SCRIPT_FILE)
   {
      wxMenu *scriptMenu = new wxMenu;
      scriptMenu->Append(MENU_SCRIPT_BUILD_OBJECT,
                         _T("&Build Object"));
      scriptMenu->Append(MENU_SCRIPT_BUILD_AND_RUN,
                         _T("&Build and Run"));
      scriptMenu->Append(MENU_SCRIPT_RUN, _T("&Run"));
      this->Append(scriptMenu, wxT("Script"));
   }
   
   //-------------------------------------------------------
   // Plot menu
   //-------------------------------------------------------
   if (dataType == GmatTree::OUTPUT_OPENGL_PLOT)
   {
      #ifdef __SHOW_PLOT_MENU__
      // Plot menu
      wxMenu *plotMenu = new wxMenu;      
      plotMenu->Append(GmatPlot::MDI_GL_CLEAR_PLOT, _T("Clear Plot"));
      plotMenu->AppendSeparator();
      plotMenu->Append(GmatPlot::MDI_GL_CHANGE_TITLE, _T("Change &title..."));
      this->Append(plotMenu, wxT("Plot"));
      #endif
      
      
      // View menu
      wxMenu *viewMenu = new wxMenu;
      viewMenu->Append(GmatPlot::MDI_GL_SHOW_OPTION_PANEL,
                       _T("Show View Option Dialog"),
                       _T("Show view option dialog"), wxITEM_CHECK);
      
      #ifdef __SHOW_GL_VIEW_MENU__
      wxMenu *viewOptionMenu = new wxMenu;
      wxMenuItem *item =
         new wxMenuItem(viewMenu, GmatPlot::MDI_GL_VIEW_OPTION, _T("Option"),
                        _T("Show bodies in wire frame"), wxITEM_NORMAL, viewOptionMenu);
      viewOptionMenu->Append(GmatPlot::MDI_GL_SHOW_WIRE_FRAME,
                              _T("Show Wire Frame"),
                              _T("Show bodies in wire frame"), wxITEM_CHECK);
      viewOptionMenu->Append(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE,
                              _T("Show Equatorial Plane"),
                              _T("Show equatorial plane lines"), wxITEM_CHECK);
      
      viewOptionMenu->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, true);
      
      viewMenu->Append(item);
      #endif
      
      this->Append(viewMenu, wxT("View"));
   }  
   
   if (dataType == GmatTree::OUTPUT_XY_PLOT)
   {
      #if __SHOW_PLOT_MENU__
      // Plot menu
      wxMenu *plotMenu = new wxMenu;
    
      plotMenu->Append(GmatPlot::MDI_TS_CLEAR_PLOT, _T("Clear Plot"));
      plotMenu->AppendSeparator();
      plotMenu->Append(GmatPlot::MDI_TS_CHANGE_TITLE, _T("Change &title..."));
      #endif
      
      #if __SHOW_XY_VIEW_MENU__
      // View menu
      wxMenu *viewMenu = new wxMenu;
      viewMenu->Append(GmatPlot::MDI_TS_SHOW_DEFAULT_VIEW, _T("Reset\tCtrl-R"),
                       _("Reset to default view"));
      viewMenu->AppendSeparator();
      
      // View Option submenu
      wxMenu *viewOptionMenu = new wxMenu;
      wxMenuItem *item =
         new wxMenuItem(viewMenu, GmatPlot::MDI_TS_VIEW_OPTION, _T("Option"),
                        _T("view options"), wxITEM_NORMAL, viewOptionMenu);
      viewOptionMenu->Append(GmatPlot::MDI_TS_DRAW_GRID,
                              _T("Draw Grid"),
                              _T("Draw Grid"), wxITEM_CHECK);
      viewOptionMenu->Append(GmatPlot::MDI_TS_DRAW_DOTTED_LINE,
                              _T("Draw dotted line"),
                              _T("Draw dotted line"), wxITEM_CHECK);
      
      viewOptionMenu->Check(GmatPlot::MDI_TS_DRAW_DOTTED_LINE, false);
      
      viewMenu->Append(item);
      #endif
      
      
      #if __SHOW_PLOT_MENU__
      this->Append(plotMenu, _T("&Plot"));
      #endif
      
      #ifdef __SHOW_XY_VIEW_MENU__
      this->Append(viewMenu, _T("&View"));
      #endif
   } 
   
   //-------------------------------------------------------
   // Tools menu
   //-------------------------------------------------------
   wxMenu *toolsMenu = new wxMenu;
   toolsMenu->Append(MENU_TOOLS_FILE_COMPARE_NUMERIC, wxT("Compare Numeric Valus"), wxT(""));
   toolsMenu->Append(MENU_TOOLS_FILE_COMPARE_TEXT, wxT("Compare Text Lines"), wxT(""));
   toolsMenu->Append(MENU_TOOLS_GEN_TEXT_EPHEM_FILE, wxT("Generate Text Ephemeris File"), wxT(""));
   
   this->Append(toolsMenu, wxT("Tools"));

   //-------------------------------------------------------
   // Help menu
   //-------------------------------------------------------
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append(MENU_HELP_TOPICS, wxT("Topics"), wxT(""), FALSE);
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_ABOUT, wxT("About"), wxT(""), FALSE);
 
   helpMenu->Enable(MENU_HELP_TOPICS, FALSE);
   this->Append(helpMenu, wxT("Help"));
}

