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
#include "GmatMainFrame.hpp"
#include "ViewTextFrame.hpp"
#include "BatchRunFromGui.hpp"
#include "ConsoleAppException.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "TextDocument.hpp"
#include "MdiTextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "GmatAppData.hpp"

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
BEGIN_EVENT_TABLE(GmatMainFrame, wxFrame)
    EVT_MENU(MENU_PROJECT_EXIT, GmatMainFrame::OnProjectExit)
    EVT_MENU(MENU_HELP_ABOUT, GmatMainFrame::OnHelpAbout)
    EVT_MENU(TOOL_CLOSE_TABS, GmatMainFrame::OnCloseTabs)
    //EVT_MENU(MENU_DEMO_BATCH_RUN, GmatMainFrame::OnDemoBatchRun)    
    EVT_MENU(MENU_SCRIPT_OPEN_EDITOR, GmatMainFrame::OnScriptOpenEditor)    
    EVT_MENU(MENU_SCRIPT_BUILD, GmatMainFrame::OnScriptBuild)    
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
GmatMainFrame::GmatMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
                             long style)
              : wxFrame(NULL, -1, title, pos, size, style)

{
    mDocManager = (wxDocManager *) NULL;
    GmatSplitterWindow *splitter;
    GmatNotebook *leftTabs;
    
#if wxUSE_MENUS
    // create a menu bar 
    SetMenuBar(CreateMainMenu());
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to GMAT!"));
#endif // wxUSE_STATUSBAR

    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());
  
    splitter = new GmatSplitterWindow(this);

    // create the tabs for Resources, Mission, Output
    leftTabs = new GmatNotebook( splitter, -1, wxDefaultPosition,
                                 wxDefaultSize, wxCLIP_CHILDREN);

    rightTabs = new GmatMainNotebook( splitter, -1, wxDefaultPosition,
                                      wxDefaultSize, wxCLIP_CHILDREN);

    // add the left and right side to splitter
    leftTabs->SetMainNotebook(rightTabs);
    splitter->SplitVertically( leftTabs, rightTabs, 175 );
}

//------------------------------------------------------------------------------
// ~GmatMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame::~GmatMainFrame()
{
    if (GmatAppData::GetMessageWindow() != NULL)
        GmatAppData::GetMessageWindow()->Close();
}

//-------------------------------
// private methods
//-------------------------------

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
                _T("Uses %s"), wxVERSION_STRING);

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
   rightTabs->ClosePage();
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
    wxBitmap* bitmaps[12];

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

    int width = 24;
    int currentX = 5;

    toolBar->AddTool( 0, *(bitmaps[0]), wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("New file"));
    currentX += width + 5;
    toolBar->AddTool(1, *bitmaps[1], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Open file"));
    currentX += width + 5;
    toolBar->AddTool(2, *bitmaps[2], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Save file"));
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
    toolBar->AddTool(7, *bitmaps[8], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Run"));
    toolBar->AddTool(8, *bitmaps[9], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Pause"));
    toolBar->AddTool(9, *bitmaps[10], wxNullBitmap, FALSE, currentX, -1,
                    (wxObject *) NULL, _T("Stop"));
    toolBar->AddSeparator();
    toolBar->AddTool(TOOL_CLOSE_TABS, *bitmaps[11], wxNullBitmap, FALSE,
                    currentX, -1, (wxObject *) NULL, _T("Close Current Tab"));
    toolBar->AddSeparator();
    toolBar->AddTool(MENU_HELP_ABOUT, *bitmaps[7], wxNullBitmap, FALSE,
                     currentX, -1, (wxObject *) NULL, _T("Help"));

    toolBar->Realize();

    int i;
    for (i = 0; i < 12; i++)
    {
        delete bitmaps[i];
    }
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
    wxMenu *fileMenu = new wxMenu,
           *editMenu = new wxMenu,
           *parametersMenu = new wxMenu,
           *orbitFileMenu = new wxMenu,
           *variablesMenu = new wxMenu,
           *viewsMenu = new wxMenu,
           *toolsMenu = new wxMenu,
           *helpMenu = new wxMenu;
    //loj: added
    //wxMenu *demoMenu = new wxMenu;
    wxMenu *scriptMenu = new wxMenu;
    
    wxMenu *openMenu, *saveMenu, *saveAsMenu, *propagatorMenu;
    
    fileMenu->Append(MENU_PROJECT_NEW, wxT("New Project"), wxT(""), FALSE);
    
    openMenu = new wxMenu;
    openMenu->Append(MENU_PROJECT_OPEN_BINARY, wxT("Binary"), wxT(""), FALSE);
    openMenu->Append(MENU_PROJECT_OPEN_ASCII, wxT("ASCII"), wxT(""), FALSE);
    fileMenu->Append(MENU_PROJECT_OPEN, wxT("Open Project"), openMenu, wxT(""));

    saveMenu = new wxMenu;
    saveMenu->Append(MENU_PROJECT_SAVE_BINARY, wxT("Binary"), wxT(""), FALSE);
    saveMenu->Append(MENU_PROJECT_SAVE_ASCII, wxT("ASCII"), wxT(""), FALSE);
    fileMenu->Append(MENU_PROJECT_SAVE, wxT("Save Project"), saveMenu, wxT(""));
    
    saveAsMenu = new wxMenu;
    saveAsMenu->Append(MENU_PROJECT_SAVE_AS_BINARY, wxT("Binary"), wxT(""),
                       FALSE);
    saveAsMenu->Append(MENU_PROJECT_SAVE_AS_ASCII, wxT("ASCII"), wxT(""),
                       FALSE);
    fileMenu->Append(MENU_PROJECT_SAVE_AS, wxT("Save Project As"), saveAsMenu,
                       wxT(""));
   
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

    scriptMenu->Append(MENU_SCRIPT_OPEN_EDITOR, wxT("Open Editor"), wxT(""), FALSE);
    scriptMenu->Append(MENU_SCRIPT_BUILD, wxT("Build Script from Object"), wxT(""), FALSE);
    
    editMenu->Append(MENU_EDIT_CUT, wxT("Cut"), wxT(""), FALSE);
    editMenu->Append(MENU_EDIT_COPY, wxT("Copy"), wxT(""), FALSE);
    editMenu->Append(MENU_EDIT_PASTE, wxT("Paste"), wxT(""), FALSE);
    editMenu->AppendSeparator();
    editMenu->Append(MENU_EDIT_RESOURCES, wxT("Resources"), wxT(""), FALSE);
    editMenu->Append(MENU_EDIT_MISSION, wxT("Mission"), wxT(""), FALSE);
  
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

    orbitFileMenu->Append(MENU_ORBIT_FILES_TRAJ_FILE,
                          wxT("Read/Plot Trajectory File"), wxT(""), FALSE);
    orbitFileMenu->Append(MENU_ORBIT_FILES_EPHEM_FILE,
                          wxT("Read/Plot Ephemeris File"), wxT(""), FALSE);
    
    variablesMenu->Append(MENU_VARIABLES_CREATE, wxT("Create"), wxT(""), FALSE); 
    variablesMenu->Append(MENU_VARIABLES_EVALUATE, wxT("Evaluate"), wxT(""),
                          FALSE);

    viewsMenu->Append(MENU_VIEWS_COORD_FRAME, wxT("Coordinate Frame"), wxT(""),
                      FALSE);
    viewsMenu->AppendSeparator();
    viewsMenu->Append(MENU_VIEWS_TARG_OUTPUT, wxT("Targeter Output"), wxT(""),
                      FALSE);
    viewsMenu->AppendSeparator();
    viewsMenu->Append(MENU_VIEWS_CASCADE, wxT("Cascade Plots"), wxT(""), FALSE); 
    viewsMenu->Append(MENU_VIEWS_TILE, wxT("Tile Plots"), wxT(""), FALSE); 
    viewsMenu->AppendSeparator();
    viewsMenu->Append(MENU_VIEWS_CLEAR, wxT("Clear Plots"), wxT(""), FALSE); 
    viewsMenu->Append(MENU_VIEWS_MIN, wxT("Minimize Plots"), wxT(""), FALSE); 
    viewsMenu->Append(MENU_VIEWS_RESTORE, wxT("Restore Plots"), wxT(""), FALSE); 
    viewsMenu->Append(MENU_VIEWS_CLOSE, wxT("Close Plots"), wxT(""), FALSE); 

    toolsMenu->Append(MENU_TOOLS_SWINGBY, wxT("Swingby"), wxT(""), FALSE); 
    
    helpMenu->Append(MENU_HELP_TOPICS, wxT("Topics"), wxT(""), FALSE);
    //helpMenu->Append(MENU_HELP_DEMOS, wxT("Demos"), wxT(""), FALSE);
    helpMenu->AppendSeparator();
    helpMenu->Append(MENU_HELP_ABOUT, wxT("About"), wxT(""), FALSE);

    //loj: added
    //demoMenu->Append(MENU_DEMO_BATCH_RUN, wxT("Batch Run"), wxT(""), FALSE);
    
    menuBar->Append(fileMenu, wxT("File"));
    menuBar->Append(scriptMenu, wxT("Script"));
    menuBar->Append(editMenu, wxT("Edit"));
    menuBar->Append(parametersMenu, wxT("Parameters"));
    menuBar->Append(orbitFileMenu, wxT("Orbit Files"));
    menuBar->Append(variablesMenu, wxT("Variables"));
    menuBar->Append(viewsMenu, wxT("Views"));
    menuBar->Append(toolsMenu, wxT("Tools"));
    //loj: added
    //menuBar->Append(demoMenu, wxT("Demo"));
    
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

//loj: added
//  //------------------------------------------------------------------------------
//  // void OnDemoBatchRun(wxCommandEvent& WXUNUSED(event))
//  //------------------------------------------------------------------------------
//  /**
//   * Handles demo output from the menu bar.
//   *
//   * @param <event> input event.
//   */
//  //------------------------------------------------------------------------------
//  void GmatMainFrame::OnDemoBatchRun(wxCommandEvent& WXUNUSED(event))
//  {
//      mTextFrame = new ViewTextFrame(this, _T("Demo Batch Run"), 50, 50, 450, 340);
//      mTextFrame->Show(true);

//      GmatAppData::theMessageWindow->ClearText();
    
//      try
//      {
//          BatchRunFromGui batchMode;
//          batchMode.Run();
//          StringArray output = batchMode.GetTextBuffer();
        
//          int size = output.size();
//          for(int i=0; i<size; i++)
//              mTextFrame->AppendText(wxString(output[i].c_str()));

//      }
//      catch (BaseException &e)
//      {
//          mTextFrame->AppendText("*** Some Exception occurred in BatchRunFromGui! \n");
//          mTextFrame->AppendText(wxString(e.GetMessage().c_str()));
//      }
//      catch (...)
//      {
//          mTextFrame->AppendText("*** Some other Error occurred in BatchRunFromGui! ");
//      }
//  }

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

    //Windows mode
    //----------------------------------------------------------------
#ifdef __WXMSW__
    // Create a document manager
    mDocManager = new wxDocManager;

    // Create a template relating text documents to their views
    mDocTemplate = 
        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                          CLASSINFO(TextDocument), CLASSINFO(MdiTextEditView));
    
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
    
    //loj:compile error:
    //SetTopWindow(mdiDocMainFrame);
    //----------------------------------------------------------------
#else
    // Create a document manager
    mDocManager = new wxDocManager;

    // Create a template relating text documents to their views
    mDocTemplate = 
        new wxDocTemplate(mDocManager, _T("Text"), _T("*.script"),
                          _T(""), _T("script"), _T("Text Doc"), _T("Text View"),
                          CLASSINFO(TextDocument), CLASSINFO(TextEditView));
    
    // Create the main frame window    
    //loj: pass "this" so that this frame closes when the main frame closes
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
    bool status = GmatAppData::GetGuiInterpreter()->
        SaveScript("$gmattempscript$.script");
}
