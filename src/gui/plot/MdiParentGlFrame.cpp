//$Header$
//------------------------------------------------------------------------------
//                              MdiParentGlFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Implements MdiParentGlFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#include "MdiParentGlFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "MdiChildTrajFrame.hpp"
#include "TrajPlotCanvas.hpp"

#include "wx/toolbar.h"

// bitmap buttons for the toolbar
#include "bitmaps/open.xpm"
#include "bitmaps/zoomin.xpm"
#include "bitmaps/zoomout.xpm"

BEGIN_EVENT_TABLE(MdiParentGlFrame, wxMDIParentFrame)
    EVT_MENU(GmatPlot::MDI_OPEN_TRAJECTORY_FILE, MdiParentGlFrame::OnOpenTrajectoryFile)
    EVT_MENU(GmatPlot::MDI_QUIT, MdiParentGlFrame::OnQuit)

    EVT_CLOSE(MdiParentGlFrame::OnClose)
    EVT_SIZE(MdiParentGlFrame::OnSize)
END_EVENT_TABLE()

// Define constructor
//------------------------------------------------------------------------------
// MdiParentGlFrame::MdiParentGlFrame(wxWindow *parent, ...)
//------------------------------------------------------------------------------
MdiParentGlFrame::MdiParentGlFrame(wxWindow *parent,
                                   const wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   const long style)
    : wxMDIParentFrame(parent, id, title, pos, size,
                       style | wxNO_FULL_REPAINT_ON_RESIZE)
{
    
//      textWindow = new wxTextCtrl(this, -1, _T("A help window"),
//                                  wxDefaultPosition, wxDefaultSize,
//                                  wxTE_MULTILINE | wxSUNKEN_BORDER);

    // file menu
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(GmatPlot::MDI_OPEN_TRAJECTORY_FILE, _T("Open &Trajectory File\tCtrl-T"),
                      _T("Open a trajectory file"));
    fileMenu->Append(GmatPlot::MDI_QUIT, _T("&Exit\tAlt-X"), _T("Quit the program"));

    // help menu
    wxMenu *helpMenu = new wxMenu;

    // menu bar
    wxMenuBar *menuBar = new wxMenuBar;

    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

    // status bar
    CreateStatusBar();
    
    // tool bar
    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());

    // Accelerators
    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_CTRL, (int) 'O', GmatPlot::MDI_OPEN_TRAJECTORY_FILE);
    entries[1].Set(wxACCEL_CTRL, (int) 'X', GmatPlot::MDI_QUIT);
    wxAcceleratorTable accel(2, entries);
    SetAcceleratorTable(accel);
}

//------------------------------------------------------------------------------
// void UpdateUI()
//------------------------------------------------------------------------------
void MdiParentGlFrame::UpdateUI()
{
    if (MdiPlot::gs_nFrames > 0)
    {
        GetToolBar()->EnableTool(GmatPlot::MDI_ZOOM_IN, TRUE);
        GetToolBar()->EnableTool(GmatPlot::MDI_ZOOM_OUT, TRUE);
        subframe->GetMenuBar()->GetMenu(subframe->mViewMenuIndex)->
            Enable(GmatPlot::MDI_ZOOM_IN,  TRUE);
        subframe->GetMenuBar()->GetMenu(subframe->mViewMenuIndex)->
            Enable(GmatPlot::MDI_ZOOM_OUT, TRUE);
    }
    else
    {
        GetToolBar()->EnableTool(GmatPlot::MDI_ZOOM_IN, FALSE);
        GetToolBar()->EnableTool(GmatPlot::MDI_ZOOM_OUT, FALSE);
    }
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiParentGlFrame::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);

//      textWindow->SetSize(0, 0, 200, h);
    //GetClientWindow()->SetSize(200, 0, w - 200, h);
    GetClientWindow()->SetSize(0, 0, w, h);

    // FIXME: On wxX11, we need the MDI frame to process this
    // event, but on other platforms this should not
    // be done.
#ifdef __WXUNIVERSAL__   
    event.Skip();
#endif
}

//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void MdiParentGlFrame::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() && (MdiPlot::gs_nFrames > 0) )
    {
//          wxString msg;
//          msg.Printf(_T("%d windows still open, close anyhow?"), MdiPlot::gs_nFrames);
//          if ( wxMessageBox(msg, _T("Please confirm"),
//                            wxICON_QUESTION | wxYES_NO) != wxYES )
//          {
//              event.Veto();
//              return;
//          }
    }
    MdiPlot::mdiParentGlFrame = NULL;
    event.Skip();
}

//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiParentGlFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

//------------------------------------------------------------------------------
// void OnOpenTrajectoryFile(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void MdiParentGlFrame::OnOpenTrajectoryFile(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog fileDialog(this,
                            _T("Open Text Trajectory File"),
                            _T(""),
                            _T(""),
                            _T("text trajectory file (*.txt)|*.txt")
                            );

    fileDialog.SetDirectory(wxGetCwd());

    if (fileDialog.ShowModal() == wxID_OK)
    {
//          wxString info;
//          info.Printf(_T("Full file name: %s\n")
//                      _T("Path: %s\n")
//                      _T("Name: %s"),
//                      fileDialog.GetPath().c_str(),
//                      fileDialog.GetDirectory().c_str(),
//                      fileDialog.GetFilename().c_str());
        
//          wxMessageDialog msgDialog(this, info, _T("Selected file"));
//          msgDialog.ShowModal();

        
        wxString trajectoryFileName = fileDialog.GetPath();
    
        // Make a frame, containing a opengl canvas
        subframe = 
            new MdiChildTrajFrame(MdiPlot::mdiParentGlFrame, _T("OpenGL Canvas Frame"),
                                  wxPoint(-1, -1), wxSize(-1, -1),
                                  wxDEFAULT_FRAME_STYLE);
    
        wxString title;
        title.Printf(_T("Canvas Frame %d"), ++MdiPlot::gs_nFrames);
        subframe->SetTitle(title);

        //-----------------------------------
        // Read text trajectory file
        //-----------------------------------
        int dataPoints = subframe->mCanvas->ReadTextTrajectory(trajectoryFileName);
        if (dataPoints > 0)
        {
            subframe->Show(TRUE);
            wxLogStatus(MdiPlot::mdiParentGlFrame,
                        wxT("Number of data points: %d"), dataPoints);
            UpdateUI();
        }
    }
}

//------------------------------------------------------------------------------
// void InitToolBar(wxToolBar* toolBar)
//------------------------------------------------------------------------------
void MdiParentGlFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap tbBitmaps[3];

    tbBitmaps[0] = wxBITMAP(open);
    tbBitmaps[1] = wxBITMAP(zoomin);
    tbBitmaps[2] = wxBITMAP(zoomout);
    
    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(GmatPlot::MDI_OPEN_TRAJECTORY_FILE,
                     tbBitmaps[0], wxNullBitmap, FALSE, -1, -1,
                     (wxObject*)NULL, _T("Open"), _T("Open an trajectory file"));
    toolBar->AddSeparator();
    
    toolBar->AddTool(GmatPlot::MDI_ZOOM_IN,
                     tbBitmaps[1], wxNullBitmap, FALSE, -1, -1,
                     (wxObject*)NULL, _T("Zoom in"), _T("Zoom in"));
    
    toolBar->AddTool(GmatPlot::MDI_ZOOM_OUT,
                     tbBitmaps[2], wxNullBitmap, FALSE, -1, -1,
                     (wxObject*)NULL, _T("Zoom out"), _T("Zoom out"));

    toolBar->Realize();
    toolBar->EnableTool(GmatPlot::MDI_ZOOM_IN, FALSE);    // must be after Realize() !
    toolBar->EnableTool(GmatPlot::MDI_ZOOM_OUT, FALSE); 

}
