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
#include "GuiInterpreter.hpp"
//#include "GmatMainNotebook.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatServer.hpp"
#include "MatlabInterface.hpp"

#include "wx/notebook.h"
#include "wx/toolbar.h"
#include "wx/docview.h"
#include "wx/laywin.h"
#include "wx/textctrl.h"



class GmatMainFrame : public wxMDIParentFrame
{
public:
   // constructors
   GmatMainFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
                 const wxPoint& pos, const wxSize& size, const long style);
   ~GmatMainFrame();
   void CreateChild(GmatTreeItemData *item);
   bool IsChildOpen(GmatTreeItemData *item);
   bool RenameChild(GmatTreeItemData *item, wxString newName);
   void RemoveChild(wxString item);
   void CloseActiveChild();
   void CloseAllChildren();
   void MinimizeChildren(int selection);
   void CloseCurrentProject();
   void RunCurrentMission();
   void NotifyRunCompleted(); //loj: 10/28/04 added
   void StartServer();
   void StopServer();
   wxToolBar* GetMainFrameToolBar();

   // public data
   bool scriptMdiShown;
    
protected:
private:
   
   GmatServer *mServer;
   std::string scriptFilename;
   GuiInterpreter *theGuiInterpreter;
   
   wxSize mFullSize;    //loj: 10/28/04 added
   wxSize mReducedSize; //loj: 10/28/04 added
   
   wxSashLayoutWindow* win;
   wxSashLayoutWindow* msgWin;
   wxScrolledWindow *panel;
   wxList *mdiChildren;
   
   wxDocManager *mDocManager;
   wxDocTemplate *mDocTemplate;
   ViewTextFrame *mTextFrame;
   //GmatMainNotebook *rightTabs;
   
   void InitToolBar(wxToolBar* toolBar);
   wxMenuBar* CreateMainMenu();
   wxMenuBar* CreateScriptWindowMenu(const std::string &docType);
   
   // event handling
   DECLARE_EVENT_TABLE();
   void OnProjectNew(wxCommandEvent &event);
   void OnLoadDefaultMission(wxCommandEvent &event);
   void OnSaveScriptAs(wxCommandEvent &event);
   void OnSaveScript(wxCommandEvent &event);
   void OnProjectExit(wxCommandEvent &event);
   void OnRun(wxCommandEvent &event);
   void OnStop(wxCommandEvent &event);
   void OnHelpAbout(wxCommandEvent &event);
   void OnCloseTabs(wxCommandEvent &event);
   
   void OnScriptOpenEditor(wxCommandEvent &event);
   void OnScriptBuild(wxCommandEvent &event);
   
   void OnScriptOpenNewEditor(wxCommandEvent &event);
   void OnScriptOpenFileEditor(wxCommandEvent &event);
   
   void OnGlPlotTrajectoryFile(wxCommandEvent &event);
   void OnXyPlotTrajectoryFile(wxCommandEvent &event);
   
   //void OnNewScript(wxCommandEvent &event);
   
   void OnStartServer(wxCommandEvent& event);
   void OnStopServer(wxCommandEvent& event);

   void OnOpenMatlab(wxCommandEvent& event);
   void OnCloseMatlab(wxCommandEvent& event);
   void OnMatlabInteractive(wxCommandEvent& WXUNUSED(event));
   
   void OnSashDrag(wxSashEvent &event);
   void OnMsgSashDrag(wxSashEvent &event);
   void OnSize(wxSizeEvent &event);
   void OnFocus(wxFocusEvent &event);

   // IDs for the controls and the menu commands
   enum
   {
      MENU_PROJECT_NEW = 10000,
      MENU_PROJECT_LOAD_DEFAULT_MISSION,
      MENU_PROJECT_PRINT,
      MENU_PROJECT_PREFERENCES,
      MENU_PROJECT_EXIT,
      MENU_SET_PATH_AND_LOG,
      MENU_INFORMATION,
        
      MENU_FILE_OPEN_SCRIPT,
      MENU_FILE_NEW_SCRIPT,
      MENU_FILE_SAVE_SCRIPT,
      MENU_FILE_SAVE_AS_SCRIPT,

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
        
      MENU_TOOLS_SWINGBY,
      MENU_TOOLS_MATLAB,
      MENU_TOOLS_MATLAB_OPEN,
      MENU_TOOLS_MATLAB_INTERACTIVE,
      MENU_TOOLS_MATLAB_CLOSE,

      MENU_HELP_TOPICS,
        
      TOOL_RUN,
      TOOL_PAUSE,
      TOOL_RESUME,
      TOOL_STOP,
        
      TOOL_CLOSE_TABS,
      TOOL_SCRIPT,
        
      MENU_START_SERVER,
      MENU_STOP_SERVER,
      
      ID_SASH_WINDOW,
      ID_MSG_SASH_WINDOW,
        
      // it is important for the id corresponding to the "About" command to have
      // this standard value as otherwise it won't be handled properly under Mac
      // (where it is special and put into the "Apple" menu)    
      MENU_HELP_ABOUT = wxID_ABOUT,
   };
};

#endif // GmatMainFrame_hpp
