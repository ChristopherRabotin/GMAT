//$Header$
//------------------------------------------------------------------------------
//                              GmatApp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/08
//
// 11/24/2003 - D. Conway, Thinking Systems, Inc.
// Changes:
//  - Added test for Unix envirnment, and set the size if the main frame in that
//    environment, so that the Linux build (and, presumably, Unix builds) would
//    appear reasonably sized.
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"

#include "GmatMainFrame.hpp"
#include "ViewTextFrame.hpp"
#include "GmatApp.hpp"
#include "GmatAppData.hpp"
#include "Moderator.hpp"
#include "DocViewFrame.hpp"
#include "TextEditView.hpp"
#include "MdiDocViewFrame.hpp"
#include "MdiTextEditView.hpp"

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/splash.h"
#include "wx/image.h"

#include "bitmaps/splash.xpm"

DocViewFrame *docMainFrame = (DocViewFrame *) NULL;
MdiDocViewFrame *mdiDocMainFrame = (MdiDocViewFrame *) NULL;

// In single window mode, don't have any child windows; use
// main window.

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. GmatApp and
// not wxApp)

IMPLEMENT_APP(GmatApp)
    
//------------------------------------------------------------------------------
// GmatApp(void)
//------------------------------------------------------------------------------
GmatApp::GmatApp(void)
{
    theModerator = (Moderator *)NULL;
}

//------------------------------------------------------------------------------
// OnInit()
//------------------------------------------------------------------------------
/**
 * The execution of Main program starts here.
 */
//------------------------------------------------------------------------------
bool GmatApp::OnInit()
{
    bool status = false;
    
    // create MessageWindow and save in GmatApp for later use
    GmatAppData::theMessageWindow =
        new ViewTextFrame((wxFrame *)NULL, _T("Message Window"),
                          20, 20, 600, 350, "Permanent");
//    GmatAppData::theMessageWindow->SetMaxLength(6000000);
    GmatAppData::theMessageWindow->Show(false);

    // create the Moderator - GMAT executive
    theModerator = Moderator::Instance();

    // initialize the moderator
    if (theModerator->Initialize(true))
    {
        // get GuiInterpreter
        GmatAppData::SetGuiInterpreter(theModerator->GetGuiInterpreter());

        // Make default size larger for Linux
        wxSize size = ((wxUSE_UNIX != 1) ? wxDefaultSize : wxSize(800, 600));
        
        // create the main application window
//        theMainFrame =
//            new GmatMainFrame(_T("GMAT - Goddard Mission Analysis Tool"),
//                              wxDefaultPosition, size,
//                              wxDEFAULT_FRAME_STYLE);

         //testing the splash screen
        wxBitmap bitmap = wxBitmap( splash_xpm );

        new wxSplashScreen(bitmap,
                wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                6000, NULL, -1, wxDefaultPosition, wxSize(100, 100),
                wxSIMPLE_BORDER|wxSTAY_ON_TOP);

        wxYield();

        theMainFrame =
            new GmatMainFrame((wxFrame *)NULL, -1,
                              _T("GMAT - Goddard Mission Analysis Tool"),
                              wxDefaultPosition, size,
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

        // and show it (the frames, unlike simple controls, are not shown when
        // created initially)
        theMainFrame->CenterOnScreen(wxBOTH);
        theMainFrame->Show(true);
        
        status = true;
    }
    else
    {
        // show error messages
        {
            wxBusyCursor bc;
            wxLogWarning(wxT("The Moderator failed to initialize."));
            
            // and if ~wxBusyCursor doesn't do it, then call it manually
            wxYield();
        }

        //loj: How do I change the title?
        wxLogError(wxT("The error occurred during the initialization.  GMAT will exit"));
        wxLog::FlushActive();
        status = false;
    }

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    
    return status;
}

//------------------------------------------------------------------------------
// OnExit()
//------------------------------------------------------------------------------
int GmatApp::OnExit()
{
    return 0;
}

//------------------------------------------------------------------------------
// wxFrame* CreateSdiChildFrame(wxDocument *doc, wxView *view, bool isCanvas)
//------------------------------------------------------------------------------
wxFrame* GmatApp::CreateSdiChildFrame(wxDocument *doc, wxView *view, bool isCanvas,
                                      bool isScript)
{
    // Make a child frame
    wxDocChildFrame *subframe =
        new wxDocChildFrame(doc, view, GetMainFrame(), -1, _T("Child Frame"),
                            wxPoint(10, 10), wxSize(500, 400),
                            wxDEFAULT_FRAME_STYLE);
    
#ifdef __WXMSW__
    subframe->SetIcon(wxString(isCanvas ? _T("chart") : _T("notepad")));
#endif
    
    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    
    file_menu->Append(wxID_NEW, _T("&New..."));
    file_menu->Append(wxID_OPEN, _T("&Open..."));
    file_menu->Append(wxID_CLOSE, _T("&Close"));
    file_menu->Append(wxID_SAVE, _T("&Save"));
    file_menu->Append(wxID_SAVEAS, _T("Save &As..."));

    if (isCanvas)
    {
        file_menu->AppendSeparator();
        file_menu->Append(wxID_PRINT, _T("&Print..."));
        file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
        file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"));
    }

    wxMenu *scriptMenu = (wxMenu *) NULL;
    if (isScript)
    {
        scriptMenu = new wxMenu;
        scriptMenu->Append(GmatScript::MENU_SCRIPT_BUILD_OBJECT, _T("&Build Object"));
        scriptMenu->Append(GmatScript::MENU_SCRIPT_RUN, _T("&Run"));
    }
    
    wxMenu *edit_menu = (wxMenu *) NULL;
    
    if (isCanvas)
    {
        edit_menu = new wxMenu;
        edit_menu->Append(wxID_UNDO, _T("&Undo"));
        edit_menu->Append(wxID_REDO, _T("&Redo"));
        edit_menu->AppendSeparator();
        //edit_menu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
        
        doc->GetCommandProcessor()->SetEditMenu(edit_menu);
    }
    
    //wxMenu *help_menu = new wxMenu;
    //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
    wxMenuBar *menu_bar = new wxMenuBar;
    
    menu_bar->Append(file_menu, _T("&File"));

    if (isScript)
        menu_bar->Append(scriptMenu, _T("&Script"));
 
    if (isCanvas)
        menu_bar->Append(edit_menu, _T("&Edit"));
    
    //menu_bar->Append(help_menu, _T("&Help"));
    
    // Associate the menu bar with the frame
    subframe->SetMenuBar(menu_bar);
    
    subframe->Centre(wxBOTH);
    SetTopWindow(subframe);

    return subframe;
}

//------------------------------------------------------------------------------
// wxMDIChildFrame* CreateMdiChildFrame(wxDocument *doc, wxView *view, bool isCanvas)
//------------------------------------------------------------------------------
wxMDIChildFrame* GmatApp::CreateMdiChildFrame(wxDocument *doc, wxView *view, bool isCanvas,
                                              bool isScript)
{
    // Make a MDI child frame
    int width, height;
    GetMdiMainFrame()->GetClientSize(&width, &height);
    
    wxDocMDIChildFrame *subframe =
        new wxDocMDIChildFrame(doc, view, GetMdiMainFrame(), -1, _T("Child Frame"),
                               //wxPoint(10, 10), wxSize(500, 400),
                               wxPoint(0, 0), wxSize(width, height),
                               wxDEFAULT_FRAME_STYLE |
                               wxNO_FULL_REPAINT_ON_RESIZE);
    
#ifdef __WXMSW__
    subframe->SetIcon(wxString(isCanvas ? _T("chart") : _T("notepad")));
#endif
    
    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    
    file_menu->Append(wxID_NEW, _T("&New..."));
    file_menu->Append(wxID_OPEN, _T("&Open..."));
    file_menu->Append(wxID_CLOSE, _T("&Close"));
    file_menu->Append(wxID_SAVE, _T("&Save"));
    file_menu->Append(wxID_SAVEAS, _T("Save &As..."));

    if (isCanvas)
    {
        file_menu->AppendSeparator();
        file_menu->Append(wxID_PRINT, _T("&Print..."));
        file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
        file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"));
    }
    
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _T("E&xit"));

    wxMenu *scriptMenu = (wxMenu *) NULL;
    if (isScript)
    {
        scriptMenu = new wxMenu;
        scriptMenu->Append(GmatScript::MENU_SCRIPT_BUILD_OBJECT, _T("&Build Object"));
        scriptMenu->Append(GmatScript::MENU_SCRIPT_BUILD_AND_RUN, _T("&Build and Run"));
        scriptMenu->Append(GmatScript::MENU_SCRIPT_RUN, _T("&Run"));
    }
    
    wxMenu *edit_menu = (wxMenu *) NULL;
    
    if (isCanvas)
    {
        edit_menu = new wxMenu;
        edit_menu->Append(wxID_UNDO, _T("&Undo"));
        edit_menu->Append(wxID_REDO, _T("&Redo"));
        edit_menu->AppendSeparator();
        //edit_menu->Append(DOCVIEW_CUT, _T("&Cut last segment"));
        
        doc->GetCommandProcessor()->SetEditMenu(edit_menu);
    }
    
    //wxMenu *help_menu = new wxMenu;
    //help_menu->Append(DOCVIEW_ABOUT, _T("&About"));
    
    wxMenuBar *menu_bar = new wxMenuBar;
    
    menu_bar->Append(file_menu, _T("&File"));

    if (isScript)
        menu_bar->Append(scriptMenu, _T("&Script"));
 
    if (isCanvas)
        menu_bar->Append(edit_menu, _T("&Edit"));
    
    //menu_bar->Append(help_menu, _T("&Help"));
    
    // Associate the menu bar with the frame
    subframe->SetMenuBar(menu_bar);
    
    subframe->Centre(wxBOTH);
    SetTopWindow(subframe);

    return subframe;
}

IMPLEMENT_DYNAMIC_CLASS(TextEditView, wxView)

//------------------------------------------------------------------------------
// bool TextEditView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
//------------------------------------------------------------------------------
bool TextEditView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
    //loj: getting compilation error on wxGetApp() if I put this code in
    //     TextEditView.cpp
    //loj: isScript is hardcoded here temporarily
    frame = wxGetApp().CreateSdiChildFrame(doc, this, false, true);
    
    int width, height;
    frame->GetClientSize(&width, &height);
    textsw = new TextSubFrame(this, frame, wxPoint(0, 0), wxSize(width, height),
                              wxTE_MULTILINE);
    frame->SetTitle(_T("TextEditView"));
    
#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(-1, -1, x, y);
#endif
    
    frame->Show(TRUE);
    Activate(TRUE);
    
    return TRUE;
}

IMPLEMENT_DYNAMIC_CLASS(MdiTextEditView, wxView)

//------------------------------------------------------------------------------
// bool MdiTextEditView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
//------------------------------------------------------------------------------
bool MdiTextEditView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
    //loj: getting compilation error on wxGetApp() if I put this code in
    //     MdiTextEditView.cpp
    //loj: isScript is hardcoded here temporarily
    
    frame = wxGetApp().CreateMdiChildFrame(doc, this, false, true);
    
    int width, height;
    frame->GetClientSize(&width, &height);
    textsw = new MdiTextSubFrame(this, frame, wxPoint(0, 0), wxSize(width, height),
                                 wxTE_MULTILINE);
    frame->SetTitle(_T("MdiTextEditView"));

#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(-1, -1, x, y);
#endif
    
    frame->Show(TRUE);
    Activate(TRUE);
    
    return TRUE;
}
