//$Header$
//------------------------------------------------------------------------------
//                              GmatMainFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
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
#ifndef GmatMainFrame_hpp
#define GmatMainFrame_hpp

#include "gmatwxdefs.hpp"

#include "ViewTextFrame.hpp"
#include "GmatSplitterWindow.hpp"
#include "GuiInterpreter.hpp"
#include "GmatNotebook.hpp"
#include "GmatMainNotebook.hpp"

#include "wx/notebook.h"
#include "wx/toolbar.h"
#include "wx/docview.h"

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

class GmatMainFrame : public wxFrame
{
public:
    // constructors
    GmatMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
                  long style);
    ~GmatMainFrame();

protected:
private:
    GuiInterpreter *theGuiInterpreter;

    wxDocManager *mDocManager;
    wxDocTemplate *mDocTemplate;
    ViewTextFrame *mTextFrame;
    GmatMainNotebook *rightTabs;
    
    void InitToolBar(wxToolBar* toolBar);
    wxMenuBar* CreateMainMenu();
    wxMenuBar* CreateScriptWindowMenu(const std::string &docType);
    
    // event handling
    DECLARE_EVENT_TABLE();
    void OnProjectNew(wxCommandEvent& WXUNUSED(event));
    void OnProjectExit(wxCommandEvent& WXUNUSED(event));
    void OnRun(wxCommandEvent& WXUNUSED(event));
    void OnHelpAbout(wxCommandEvent& WXUNUSED(event));
    void OnCloseTabs(wxCommandEvent& WXUNUSED(event));

    void OnScriptOpenEditor(wxCommandEvent& WXUNUSED(event));
    void OnScriptBuild(wxCommandEvent& WXUNUSED(event));

    void OnGlPlotTrajectoryFile(wxCommandEvent& WXUNUSED(event));
    void OnXyPlotTrajectoryFile(wxCommandEvent& WXUNUSED(event));
    
    // IDs for the controls and the menu commands
    enum
    {
        MENU_PROJECT_NEW = 10000,
        MENU_PROJECT_OPEN,
        MENU_PROJECT_OPEN_BINARY,
        MENU_PROJECT_OPEN_ASCII,
        MENU_PROJECT_SAVE,
        MENU_PROJECT_SAVE_BINARY,
        MENU_PROJECT_SAVE_ASCII,
        MENU_PROJECT_SAVE_AS,
        MENU_PROJECT_SAVE_AS_BINARY,
        MENU_PROJECT_SAVE_AS_ASCII,
        MENU_PROJECT_PRINT,
        MENU_PROJECT_PREFERENCES,
        MENU_PROJECT_EXIT,
        MENU_SET_PATH_AND_LOG,
        MENU_INFORMATION,

        MENU_SCRIPT_OPEN_EDITOR,
        MENU_SCRIPT_BUILD,
        
        MENU_EDIT_CUT,
        MENU_EDIT_COPY,
        MENU_EDIT_PASTE,
        MENU_EDIT_RESOURCES,
        MENU_EDIT_MISSION,
    
        MENU_PARAMETERS_PROP_CONFIG,
        MENU_PARAMETERS_PROPAGATOR,
        MENU_PARAMETERS_LAUNCH_MODEL,
        MENU_PARAMETERS_INJECTION_BURN_MODEL,
        MENU_PARAMETERS_SOLAR_RAD,
        MENU_PARAMETERS_ORBIT_INFO,
        MENU_PARAMETERS_ATTITUDE_MODES,
        MENU_PARAMETERS_SOLAR_SAILS,
        MENU_PARAMETERS_SOLAR_ELEC_CONV,
    
        MENU_ORBIT_FILES_GL_PLOT_TRAJ_FILE,
        MENU_ORBIT_FILES_XY_PLOT_TRAJ_FILE,
        MENU_ORBIT_FILES_EPHEM_FILE,
    
        MENU_VARIABLES_CREATE,
        MENU_VARIABLES_EVALUATE,
    
        MENU_VIEWS_COORD_FRAME,
        MENU_VIEWS_TARG_OUTPUT,
        MENU_VIEWS_CASCADE,
        MENU_VIEWS_TILE,
        MENU_VIEWS_CLEAR,
        MENU_VIEWS_MIN,
        MENU_VIEWS_RESTORE,
        MENU_VIEWS_CLOSE,
    
        MENU_TOOLS_SWINGBY,

        MENU_HELP_TOPICS,
        
        TOOL_RUN,
        TOOL_PAUSE,
        TOOL_RESUME,
        TOOL_STOP,
        
        // it is important for the id corresponding to the "About" command to have
        // this standard value as otherwise it won't be handled properly under Mac
        // (where it is special and put into the "Apple" menu)    
        MENU_HELP_ABOUT = wxID_ABOUT,
        TOOL_CLOSE_TABS,
    };
};

#endif // GmatMainFrame_hpp







