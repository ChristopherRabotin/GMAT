//$Id$
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
#include "GmatTreeItemData.hpp"
#include "GmatMainFrame.hpp"     // for namespace GmatMenu
#include "MdiGlPlotData.hpp"     // for MDI_GL_*
#include "MdiTsPlotData.hpp"     // for MDI_TS_*
#include "MessageInterface.hpp"


#define __ADD_CLOSE_TO_WINDOW_MENU__

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
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatMenuBar(GmatTree::ItemType itemType, wxMenu *windowMenu,
//             wxMenu *newWinMenu, long style)
//------------------------------------------------------------------------------
GmatMenuBar::GmatMenuBar(GmatTree::ItemType itemType, wxMenu *windowMenu,
                         long style)
   : wxMenuBar(style)
{
   CreateMenu(itemType, windowMenu);
}


//------------------------------------------------------------------------------
// void CreateMenu(GmatTree::ItemType itemType, wxMenu *windowMenu)
//------------------------------------------------------------------------------
/**
 * Adds items to the menu.
 *
 * @param <itemType> input item type from GmatTree::ItemType
 * @param <windowMenu> input Window menu pointer. This is available on Windows only
 *
 */
//------------------------------------------------------------------------------
void GmatMenuBar::CreateMenu(GmatTree::ItemType itemType, wxMenu *windowMenu)
{
   #ifdef DEBUG_MENUBAR
   MessageInterface::ShowMessage
      ("GmatMenuBar::CreateMenu() itemType=%d, windowMenu=%p\n",
       itemType, windowMenu);
   #endif
   
   //-----------------------------------------------------------------
   // File menu
   //-----------------------------------------------------------------
   wxMenu *fileMenu = new wxMenu;
   fileMenu->Append(MENU_FILE_NEW_SCRIPT, wxT("&New Script"), wxT(""));
   fileMenu->Append(MENU_FILE_OPEN_SCRIPT, wxT("&Open Script"), wxT(""));
   
   if (itemType == GmatTree::OUTPUT_OPENGL_PLOT)
   {
      fileMenu->Append(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, _T("&Open Trajectory File"));
      fileMenu->Enable(GmatPlot::MDI_GL_OPEN_TRAJECTORY_FILE, false);
   }
   
   if (itemType == GmatTree::OUTPUT_XY_PLOT)
   {
       fileMenu->Append(GmatPlot::MDI_TS_OPEN_PLOT_FILE, _T("&Open XY Plot File"));
   }
   
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT, wxT("&Save to Script"), wxT(""));
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT_AS, wxT("Save to Script As"), wxT(""));
   
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_LOAD_DEFAULT_MISSION, wxT("Default Project"), wxT(""));   
   fileMenu->Append(MENU_EMPTY_PROJECT, wxT("Empty Project"), wxT(""));   
   fileMenu->AppendSeparator();
   
   wxMenu *prefMenu = new wxMenu;
   prefMenu->Append(MENU_PROJECT_PREFERENCES_FONT, wxT("Font"));
   
   fileMenu->Append(MENU_PROJECT_PREFERENCES, wxT("Preferences"), prefMenu, wxT(""));
   
   fileMenu->Append(MENU_SET_PATH_AND_LOG, wxT("Set File Paths"), wxT(""));
   fileMenu->Append(MENU_INFORMATION, wxT("Information"), wxT(""));
   
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_FILE_PRINT, wxT("Print"), wxT(""));
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_EXIT, wxT("Exit"), wxT(""));
   #ifdef __WXMAC__
       wxApp::s_macExitMenuItemId = MENU_PROJECT_EXIT;
   #endif
   
   //fileMenu->Enable(MENU_SET_PATH_AND_LOG, FALSE);
   fileMenu->Enable(MENU_INFORMATION, FALSE);
   fileMenu->Enable(MENU_FILE_PRINT, FALSE);
   this->Append(fileMenu, wxT("&File"));
   
   //-----------------------------------------------------------------
   // Edit menu
   //-----------------------------------------------------------------
   wxMenu *editMenu = new wxMenu;

   if (itemType == GmatTree::SCRIPT_FILE)
   {
      editMenu->Append(MENU_EDIT_UNDO, wxT("Undo\tCtrl-Z"), wxT(""));
      editMenu->Append(MENU_EDIT_REDO, wxT("Redo\tShift-Ctrl-Z"), wxT(""));
      editMenu->AppendSeparator();
      editMenu->Append(MENU_EDIT_CUT, wxT("Cut\tCtrl-X"), wxT(""));
      editMenu->Append(MENU_EDIT_COPY, wxT("Copy\tCtrl-C"), wxT(""));
      editMenu->Append(MENU_EDIT_PASTE, wxT("Paste\tCtrl-V"), wxT(""));
      editMenu->AppendSeparator();
      editMenu->Append(MENU_EDIT_SELECT_ALL, wxT("Select All\tCtrl-A"), wxT(""));
      editMenu->AppendSeparator();
      editMenu->Append(MENU_EDIT_COMMENT, wxT("Comment\tCtrl-T"), wxT(""));
      editMenu->Append(MENU_EDIT_UNCOMMENT, wxT("Uncomment\tCtrl-M"), wxT(""));
      editMenu->AppendSeparator();
   }
   
   editMenu->Append(MENU_EDIT_RESOURCES, wxT("Resources"), wxT(""));
   editMenu->Append(MENU_EDIT_MISSION, wxT("Mission"), wxT(""));

   editMenu->Enable(MENU_EDIT_RESOURCES, FALSE);
   editMenu->Enable(MENU_EDIT_MISSION, FALSE);
   this->Append(editMenu, wxT("Edit"));
   
   //-----------------------------------------------------------------
   // Script menu
   //-----------------------------------------------------------------
   if (itemType == GmatTree::SCRIPT_FILE)
   {
      wxMenu *scriptMenu = new wxMenu;
      scriptMenu->Append(MENU_SCRIPT_BUILD_OBJECT,
                         _T("&Build Object"));
      scriptMenu->Append(MENU_SCRIPT_BUILD_AND_RUN,
                         _T("&Build and Run"));
      scriptMenu->Append(MENU_SCRIPT_RUN, _T("&Run"));
      this->Append(scriptMenu, wxT("Script"));
   }
   
   //-----------------------------------------------------------------
   // Tools menu
   //-----------------------------------------------------------------
   wxMenu *toolsMenu = new wxMenu;
   toolsMenu->Append(MENU_TOOLS_FILE_COMPARE_NUMERIC, wxT("Compare Numeric Valus"), wxT(""));
   toolsMenu->Append(MENU_TOOLS_FILE_COMPARE_TEXT, wxT("Compare Text Lines"), wxT(""));
   toolsMenu->Append(MENU_TOOLS_GEN_TEXT_EPHEM_FILE, wxT("Generate Text Ephemeris File"), wxT(""));
   
   this->Append(toolsMenu, wxT("Tools"));
   
   
   //-----------------------------------------------------------------
   // Help menu
   //-----------------------------------------------------------------
   
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append(MENU_HELP_TOPICS, wxT("Topics"), wxT(""));
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_ABOUT, wxT("About"), wxT(""));
   
   helpMenu->Enable(MENU_HELP_TOPICS, FALSE);
   this->Append(helpMenu, wxT("Help"));
   
   //-----------------------------------------------------------------
   // Window menu
   //-----------------------------------------------------------------
   // @note: In order for system Window menu to work, do not call
   // SetMenuBar() from GmatMdiChildFrame after theMenuBar is created.
   #ifdef __ADD_CLOSE_TO_WINDOW_MENU__
   //-------------------------------------------------------
   // If on Windows, use Window menu from MdiParenentFrame
   // otherwise, create Window menu
   //-------------------------------------------------------
   
   wxMenu *winMenu = (wxMenu*)NULL;
   bool createWindowMenu = true;
   bool prependClose = false;
   
   #ifdef __WXMSW__
   if (windowMenu != NULL)
   {
      createWindowMenu = false;
      winMenu = windowMenu;
      if (winMenu->FindItem("Close All") == wxNOT_FOUND)
         prependClose = true;
   }
   #endif
   
   #ifdef DEBUG_MENUBAR
   MessageInterface::ShowMessage
      ("==> GmatMenuBar::CreateMenu() this=%p, winMenu=%p, createWindowMenu=%d, "
       "prependClose=%d\n", this, winMenu, createWindowMenu, prependClose);
   #endif
   
   if (createWindowMenu)
   {
      //MessageInterface::ShowMessage("===> creating Window menu\n");
      winMenu = new wxMenu;
      winMenu->Append(TOOL_CLOSE_CHILDREN, wxT("Close All"), wxT(""));
      winMenu->Append(TOOL_CLOSE_CURRENT, wxT("Close"), wxT(""));
      
      // Insert before Help menu
      int helpPos = FindMenu("Help");
      this->Insert(helpPos, winMenu, wxT("Window"));
   }
   else if (prependClose)
   {
      //MessageInterface::ShowMessage("===> prepending Close menu item\n");
      winMenu->PrependSeparator();
      winMenu->Prepend(TOOL_CLOSE_CURRENT, wxT("Close"), wxT(""));      
      winMenu->Prepend(TOOL_CLOSE_CHILDREN, wxT("Close All"), wxT(""));
   }
   #endif
   
}

