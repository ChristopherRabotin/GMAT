//$Header$
//------------------------------------------------------------------------------
//                              MdiChildTrajFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Implements MdiChildTrajFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#include "MdiChildTrajFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "TrajPlotCanvas.hpp"

BEGIN_EVENT_TABLE(MdiChildTrajFrame, wxMDIChildFrame)
    EVT_MENU(GmatPlot::MDI_CHILD_QUIT, MdiChildTrajFrame::OnQuit)
    EVT_MENU(GmatPlot::MDI_CHANGE_TITLE, MdiChildTrajFrame::OnChangeTitle)
    EVT_MENU(GmatPlot::MDI_CLEAR_PLOT, MdiChildTrajFrame::OnClearPlot)
    EVT_MENU(GmatPlot::MDI_SHOW_DEFAULT_VIEW, MdiChildTrajFrame::OnShowDefaultView)
    EVT_MENU(GmatPlot::MDI_ZOOM_IN, MdiChildTrajFrame::OnZoomIn)
    EVT_MENU(GmatPlot::MDI_ZOOM_OUT, MdiChildTrajFrame::OnZoomOut)
    EVT_MENU(GmatPlot::MDI_SHOW_WIRE_FRAME, MdiChildTrajFrame::OnShowWireFrame)
    EVT_MENU(GmatPlot::MDI_SHOW_EQUATORIAL_PLANE, MdiChildTrajFrame::OnShowEquatorialPlane)
    EVT_MENU(GmatPlot::MDI_HELP_VIEW, MdiChildTrajFrame::OnHelpView)

    EVT_ACTIVATE(MdiChildTrajFrame::OnActivate)
    EVT_SIZE(MdiChildTrajFrame::OnSize)
    EVT_MOVE(MdiChildTrajFrame::OnMove)
    EVT_CLOSE(MdiChildTrajFrame::OnClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildTrajFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildTrajFrame::MdiChildTrajFrame(wxMDIParentFrame *parent, bool isMainFrame,
                                     const wxString& title, const wxPoint& pos,
                                     const wxSize& size, const long style)
    : wxMDIChildFrame(parent, -1, title, pos, size,
                      style | wxNO_FULL_REPAINT_ON_RESIZE)
{
    mCanvas = (TrajPlotCanvas *) NULL;
    mIsMainFrame = isMainFrame;
    MdiGlPlot::mdiChildren.Append(this);
    
    // Give it an icon
#ifdef __WXMSW__
    SetIcon(wxIcon(_T("chrt_icn")));
#else
    SetIcon(wxIcon( mondrian_xpm ));
#endif

    // File menu
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(GmatPlot::MDI_OPEN_TRAJECTORY_FILE, _T("&Open Trajectory File"));
    fileMenu->Append(GmatPlot::MDI_QUIT, _T("&Exit"));

    // Plot menu
    wxMenu *plotMenu = new wxMenu;

    plotMenu->Append(GmatPlot::MDI_CLEAR_PLOT, _T("Clear Plot"));
    plotMenu->Append(GmatPlot::MDI_CHILD_QUIT, _T("&Close"), _T("Close this window"));
    plotMenu->AppendSeparator();
    plotMenu->Append(GmatPlot::MDI_CHANGE_TITLE, _T("Change &title..."));

    // View menu
    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(GmatPlot::MDI_SHOW_DEFAULT_VIEW, _T("Reset\tCtrl-R"),
                     _("Reset to default view"));
    viewMenu->Append(GmatPlot::MDI_ZOOM_IN, _T("Zoom &in\tCtrl-I"), _("Zoom in"));
    viewMenu->Append(GmatPlot::MDI_ZOOM_OUT, _T("Zoom &out\tCtrl-O"), _("Zoom out"));
    viewMenu->AppendSeparator();

    // View Option submenu
    wxMenu *viewOptionMenu = new wxMenu;
    wxMenuItem *item =
        new wxMenuItem(viewMenu, GmatPlot::MDI_VIEW_OPTION, _T("Option"),
                       _T("Show bodies in wire frame"), wxITEM_NORMAL, viewOptionMenu);
    viewOptionMenu->Append(GmatPlot::MDI_SHOW_WIRE_FRAME,
                           _T("Show Wire Frame"),
                           _T("Show bodies in wire frame"), wxITEM_CHECK);
    viewOptionMenu->Append(GmatPlot::MDI_SHOW_EQUATORIAL_PLANE,
                           _T("Show Equatorial Plane"),
                           _T("Show equatorial plane lines"), wxITEM_CHECK);

    viewOptionMenu->Check(GmatPlot::MDI_SHOW_EQUATORIAL_PLANE, true);
        
    viewMenu->Append(item);

    // Help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(GmatPlot::MDI_HELP_VIEW, _T("View"), _T("View mouse control"));

    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;

    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(plotMenu, _T("&Plot"));
    menuBar->Append(viewMenu, _T("&View"));
    menuBar->Append(helpMenu, _T("&Help"));

    // Associate the menu bar with the frame
    SetMenuBar(menuBar);

    // status bar
    //CreateStatusBar();
    //SetStatusText(title);

    // Create GLCanvas
    int width, height;
    GetClientSize(&width, &height);
    TrajPlotCanvas *canvas =
        new TrajPlotCanvas(this, -1, wxPoint(0, 0), wxSize(width, height));

    mCanvas = canvas;
    
    // this should work for MDI frames as well as for normal ones
    SetSizeHints(100, 100);
}

//------------------------------------------------------------------------------
// ~MdiChildTrajFrame()
//------------------------------------------------------------------------------
MdiChildTrajFrame::~MdiChildTrajFrame()
{
    MdiGlPlot::mdiChildren.DeleteObject(this);
}

//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
//#if wxUSE_TEXTDLG
    static wxString s_title = _T("Canvas Frame");

    wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                       _T(""),
                                       s_title,
                                       GetParent()->GetParent());
    if ( !title )
        return;

    s_title = title;
    SetTitle(s_title);
//#endif
}

//------------------------------------------------------------------------------
// void OnClearPlot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
    if (mCanvas)
        mCanvas->ClearPlot();
}

//------------------------------------------------------------------------------
// void OnShowDefaultView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowDefaultView(wxCommandEvent& event)
{
    if (mCanvas)
        mCanvas->ShowDefaultView();
}

//------------------------------------------------------------------------------
// void OnZoomIn(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnZoomIn(wxCommandEvent& event)
{
    if (mCanvas)
        mCanvas->ZoomIn();
}

//------------------------------------------------------------------------------
// void OnZoomOut(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnZoomOut(wxCommandEvent& event)
{
    if (mCanvas)
        mCanvas->ZoomOut();
}

//------------------------------------------------------------------------------
// void OnShowWireFrame(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowWireFrame(wxCommandEvent& event)
{
    if (mCanvas)
        mCanvas->ShowWireFrame(event.IsChecked());
}

//------------------------------------------------------------------------------
// void OnShowEquatorialPlane(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnShowEquatorialPlane(wxCommandEvent& event)
{
    if (mCanvas)
        mCanvas->ShowEquatorialPlane(event.IsChecked());
}

//------------------------------------------------------------------------------
// void OnHelpView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnHelpView(wxCommandEvent& event)
{
    wxMessageBox(_T("Use Left mouse button to rotate \n"
                    "Right mouse button to zoom \n"
                    "Shift left mouse button to translate"),
                 _T("Help View"),
                 wxOK | wxICON_INFORMATION, this);
}

//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() && mCanvas )
    {
        mCanvas->SetFocus();
    }
}

//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnMove(wxMoveEvent& event)
{
    // VZ: here everything is totally wrong under MSW, the positions are
    //     different and both wrong (pos2 is off by 2 pixels for me which seems
    //     to be the width of the MDI canvas border)
    wxPoint pos1 = event.GetPosition(),
            pos2 = GetPosition();
    wxLogStatus(MdiGlPlot::mdiParentGlFrame,
                wxT("position from event: (%d, %d), from frame (%d, %d)"),
                pos1.x, pos1.y, pos2.x, pos2.y);

    event.Skip();
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnSize(wxSizeEvent& event)
{
    // VZ: under MSW the size event carries the client size (quite
    //     unexpectedly) *except* for the very first one which has the full
    //     size... what should it really be? TODO: check under wxGTK
    wxSize size1 = event.GetSize(),
           size2 = GetSize(),
           size3 = GetClientSize();
    wxLogStatus(MdiGlPlot::mdiParentGlFrame,
                wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
                size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
    
    event.Skip();
}

//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::OnClose(wxCloseEvent& event)
{
    MdiGlPlot::numChildFrames--;
    
    if (mIsMainFrame)
        MdiGlPlot::mdiParentGlFrame->mainSubframe = NULL;
    
    if (MdiGlPlot::numChildFrames == 0)
        MdiGlPlot::mdiParentGlFrame->subframe = NULL;
    
    MdiGlPlot::mdiParentGlFrame->UpdateUI();
    event.Skip();
}

//------------------------------------------------------------------------------
// void UpdateSpacecraft(const Real &time, const Real &posX,
//                       const Real &posY, const Real &posZ)
//------------------------------------------------------------------------------
void MdiChildTrajFrame::UpdateSpacecraft(const Real &time, const Real &posX,
                                         const Real &posY, const Real &posZ)
{
    if (mCanvas)
    {
        mCanvas->SetFocus();
        mCanvas->UpdateSpacecraft(time, posX, posY, posZ);
        Update();
    }
}

