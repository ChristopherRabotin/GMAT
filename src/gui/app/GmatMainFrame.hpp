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

#include <stdio.h>

#include "ViewTextFrame.hpp"
#include "GuiInterpreter.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatServer.hpp"
#include "MatlabInterface.hpp"
#include "MdiChildTrajFrame.hpp"
#include "MdiChildTsFrame.hpp"

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
   
   GmatMdiChildFrame* CreateChild(GmatTreeItemData *item);
   GmatMdiChildFrame* GetChild(const wxString &name);
   bool IsChildOpen(GmatTreeItemData *item);
   bool RenameChild(GmatTreeItemData *item, wxString newName);
   bool RenameChild(const wxString &oldName, const wxString &newName);
   bool RenameActiveChild(const wxString &newName);
   void RemoveChild(const wxString &item, int dataType);
   void CloseActiveChild();
   void CloseAllChildren(bool closeScriptWindow = true, bool closePlots = true,
                         wxString excludeTitle = "");
   void MinimizeChildren();
   void SetActiveChildDirty(bool dirty);
   void CloseCurrentProject();
   bool InterpretScript(const wxString &filename, bool readBack = false,
                        const wxString &savePath = "", bool openScript = true,
                        bool multScripts = false);
   void RunCurrentMission();
   void NotifyRunCompleted();
   void ProcessPendingEvent();
   void StartServer();
   void StopServer();
   wxToolBar* GetMainFrameToolBar();
   wxStatusBar* GetMainFrameStatusBar();
   
   //void UpdateUI();
   //void OnSize(wxSizeEvent& event);
   void OnClose(wxCloseEvent& event);
   //void OnQuit(wxCommandEvent& event);
   //void OnOpenTrajectoryFile(wxCommandEvent& event);
   //void OnZoomIn(wxCommandEvent& event);
   //void OnZoomOut(wxCommandEvent& event);
   void UpdateMenus(bool openOn);
   
   void OnScriptBuildObject(wxCommandEvent& WXUNUSED(event));
   void OnScriptBuildAndRun(wxCommandEvent& event);
   void OnScriptRun(wxCommandEvent& WXUNUSED(event));
   
   void SetScriptFileName(const std::string &filename);
   
   MdiChildTrajFrame *trajSubframe;
   MdiChildTsFrame *tsSubframe;
   wxList *mdiChildren;

protected:

private:
   int scriptCounter;
   bool mRunPaused;
   bool mRunCompleted;
   bool mInterpretFailed;
   
   GmatServer *mServer;
   std::string mScriptFilename;
   GuiInterpreter *theGuiInterpreter;
   
   wxSashLayoutWindow* win;
   wxSashLayoutWindow* msgWin;
   wxScrolledWindow *mScrolledWin;

   ViewTextFrame *mTextFrame;
   wxMenu *mServerMenu;
   wxStatusBar *theStatusBar;
   GmatMenuBar *menuBar;   
   
   GmatMdiChildFrame* CreateNewResource(const wxString &title,
                                        const wxString &name, int dataType);
   GmatMdiChildFrame* CreateNewCommand(int dataType, GmatTreeItemData *item);
   GmatMdiChildFrame* CreateNewControl(const wxString &title,
                                       const wxString &name, int dataType,
                                       GmatCommand *cmd);
   GmatMdiChildFrame* CreateNewOutput(const wxString &title,
                                      const wxString &name, int dataType);
   
   void InitToolBar(wxToolBar* toolBar);
   bool SaveScriptAs();
   void OpenScript();
   
   // event handling
   DECLARE_EVENT_TABLE();
   void OnProjectNew(wxCommandEvent &event);
   void OnLoadDefaultMission(wxCommandEvent &event);
   void OnSaveScriptAs(wxCommandEvent &event);
   void OnSaveScript(wxCommandEvent &event);
   void OnProjectExit(wxCommandEvent &event);
   void OnRun(wxCommandEvent &event);
   void OnPause(wxCommandEvent &event);
   void OnStop(wxCommandEvent &event);
   void OnCloseChildren(wxCommandEvent &event);
   void OnCloseCurrent(wxCommandEvent &event);
   void OnHelpAbout(wxCommandEvent &event);

   void OnNewScript(wxCommandEvent &event);
   void OnOpenScript(wxCommandEvent &event);

   void OnUndo(wxCommandEvent& event);
   void OnRedo(wxCommandEvent& event);
   void OnCut(wxCommandEvent& event);
   void OnCopy(wxCommandEvent& event);
   void OnPaste(wxCommandEvent& event);
   void OnComment(wxCommandEvent& event);
   void OnUncomment(wxCommandEvent& event);
   void OnFont(wxCommandEvent& event);
   void OnSelectAll(wxCommandEvent& event);
   
   void OnGlPlotTrajectoryFile(wxCommandEvent &event);
   void OnXyPlotTrajectoryFile(wxCommandEvent &event);
      
   void OnStartServer(wxCommandEvent& event);
   void OnStopServer(wxCommandEvent& event);

   void OnOpenMatlab(wxCommandEvent& event);
   void OnCloseMatlab(wxCommandEvent& event);
   void OnMatlabInteractive(wxCommandEvent& WXUNUSED(event));
   void OnFileCompareNumeric(wxCommandEvent& event);
   void OnFileCompareText(wxCommandEvent& event);
   void OnGenerateTextEphemFile(wxCommandEvent& event);
   
   void OnSashDrag(wxSashEvent &event);
   void OnMsgSashDrag(wxSashEvent &event);
   void OnMainFrameSize(wxSizeEvent &event);
   void OnSetFocus(wxFocusEvent &event);
};

namespace GmatMenu
{
   // IDs for the controls and the menu commands
   enum
   {
      MENU_PROJECT_NEW = 10000,
      MENU_PROJECT_LOAD_DEFAULT_MISSION,
      MENU_PROJECT_PRINT,
      MENU_PROJECT_PREFERENCES,
      MENU_PROJECT_PREFERENCES_FONT,
      MENU_PROJECT_EXIT,
      MENU_SET_PATH_AND_LOG,
      MENU_INFORMATION,

      MENU_FILE_OPEN_SCRIPT,
      MENU_FILE_NEW_SCRIPT,
      MENU_FILE_SAVE_SCRIPT,
      MENU_FILE_SAVE_AS_SCRIPT,

      MENU_EDIT_UNDO,
      MENU_EDIT_REDO,
      MENU_EDIT_CUT,
      MENU_EDIT_COPY,
      MENU_EDIT_PASTE,
      MENU_EDIT_COMMENT,
      MENU_EDIT_UNCOMMENT,
      MENU_EDIT_SELECT_ALL,

      MENU_EDIT_RESOURCES,
      MENU_EDIT_MISSION,
      
      MENU_SCRIPT_OPEN_EDITOR,
      MENU_SCRIPT_BUILD_OBJECT,
      MENU_SCRIPT_RUN,
      MENU_SCRIPT_BUILD_AND_RUN,

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
      MENU_TOOLS_FILE_COMPARE_NUMERIC,
      MENU_TOOLS_FILE_COMPARE_TEXT,
      MENU_TOOLS_GEN_TEXT_EPHEM_FILE,
      
      MENU_HELP_TOPICS,

      TOOL_RUN,
      TOOL_PAUSE,
      TOOL_RESUME,
      TOOL_STOP,
      TOOL_BUILD,

      TOOL_CLOSE_CHILDREN,
      TOOL_CLOSE_CURRENT,
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
