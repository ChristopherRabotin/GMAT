//$Header$
//------------------------------------------------------------------------------
//                              MdiParentXyFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/01/21
/**
 * Implements MdiParentXyFrame for XY plot.
 */
//------------------------------------------------------------------------------
#include "MdiParentXyFrame.hpp"
#include "MdiXyPlotData.hpp"
#include "MdiChildXyFrame.hpp"

#include "wx/toolbar.h"

// bitmap buttons for the toolbar
#include "bitmaps/open.xpm"

BEGIN_EVENT_TABLE(MdiParentXyFrame, wxMDIParentFrame)
    EVT_MENU(GmatPlot::MDI_XY_OPEN_PLOT_FILE, MdiParentXyFrame::OnOpenXyPlotFile)
    EVT_MENU(GmatPlot::MDI_XY_QUIT, MdiParentXyFrame::OnQuit)

    EVT_CLOSE(MdiParentXyFrame::OnClose)
    EVT_SIZE(MdiParentXyFrame::OnSize)
END_EVENT_TABLE()

// Define constructor
//------------------------------------------------------------------------------
// MdiParentXyFrame::MdiParentXyFrame(wxWindow *parent, ...)
//------------------------------------------------------------------------------
MdiParentXyFrame::MdiParentXyFrame(wxWindow *parent,
                                   const wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   const long style)
    : wxMDIParentFrame(parent, id, title, pos, size,
                       style | wxNO_FULL_REPAINT_ON_RESIZE)
{
    
    // child frames
    subframe = (MdiChildXyFrame *)NULL;
    mainSubframe = (MdiChildXyFrame *)NULL;
    
    // file menu
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(GmatPlot::MDI_XY_OPEN_PLOT_FILE, _T("Open &XY Plot File\tCtrl-T"),
                     _T("Open a xy plot file"));
    fileMenu->Append(GmatPlot::MDI_XY_QUIT, _T("&Exit\tAlt-X"), _T("Quit the program"));

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
    entries[0].Set(wxACCEL_CTRL, (int) 'O', GmatPlot::MDI_XY_OPEN_PLOT_FILE);
    entries[1].Set(wxACCEL_CTRL, (int) 'X', GmatPlot::MDI_XY_QUIT);
    wxAcceleratorTable accel(2, entries);
    SetAcceleratorTable(accel);
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiParentXyFrame::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);

    //textWindow->SetSize(0, 0, 200, h);
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
void MdiParentXyFrame::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto() && (MdiXyPlot::numChildren > 0) )
    {
//          wxString msg;
//          msg.Printf(_T("%d windows still open, close anyhow?"), MdiXyPlot::numChildren);
//          if ( wxMessageBox(msg, _T("Please confirm"),
//                            wxICON_QUESTION | wxYES_NO) != wxYES )
//          {
//              event.Veto();
//              return;
//          }
    }
    MdiXyPlot::mdiParentXyFrame = NULL;
    MdiXyPlot::numChildren = 0;
    event.Skip();
}

//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiParentXyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

//------------------------------------------------------------------------------
// void OnOpenXyPlotFile(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void MdiParentXyFrame::OnOpenXyPlotFile(wxCommandEvent& WXUNUSED(event) )
{
    wxFileDialog fileDialog(this,
                            _T("Open Text XY Plot File"),
                            _T(""),
                            _T(""),
                            _T("text XY Plot file (*.txt)|*.txt")
                            );

    fileDialog.SetDirectory(wxGetCwd());

    if (fileDialog.ShowModal() == wxID_OK)
    {
        wxString xyPlotFileName = fileDialog.GetPath();
    
        // Make a frame, containing a wxPlotWindow
        subframe = 
            new MdiChildXyFrame(MdiXyPlot::mdiParentXyFrame,
                                _T("XYPlotFile"),                                
                                _T("XY Plot File"),
                                _T("Time"),
                                _T("Spacecraft Position X,Y,Z"),
                                wxPoint(-1, -1), wxSize(-1, -1),
                                wxDEFAULT_FRAME_STYLE);
        
        ++MdiXyPlot::numChildren;
        subframe->SetPlotName("XYPlotFile" + MdiXyPlot::numChildren);
        subframe->SetTitle(xyPlotFileName);

        //-----------------------------------
        // Read text XY Plot file
        //-----------------------------------
        int dataPoints = subframe->ReadXyPlotFile(xyPlotFileName);
        if (dataPoints > 0)
        {
            subframe->Show(TRUE);
            wxLogStatus(MdiXyPlot::mdiParentXyFrame,
                        wxT("Number of lines read: %d"), dataPoints);
        }
    }
}

//------------------------------------------------------------------------------
// void InitToolBar(wxToolBar* toolBar)
//------------------------------------------------------------------------------
void MdiParentXyFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap tbBitmaps[1];

    tbBitmaps[0] = wxBITMAP(open);

    toolBar->SetMargins(5, 5);
    toolBar->SetToolBitmapSize(wxSize(16, 16));

    toolBar->AddTool(GmatPlot::MDI_XY_OPEN_PLOT_FILE,
                     tbBitmaps[0], wxNullBitmap, FALSE, -1, -1,
                     (wxObject*)NULL, _T("Open"), _T("Open an XY plot file"));
    
    //toolBar->AddSeparator();
    
    toolBar->Realize();

}
