//$Header$
//------------------------------------------------------------------------------
//                              MdiChildXyFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/01/20
/**
 * Implements MdiChildXyFrame class for xy plot.
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiChildXyFrame.hpp"
#include "MdiXyPlotData.hpp"
#include "XyPlotCurve.hpp"

#include <fstream>    // for ifstream (plot input file)

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/gdicmn.h" // for color

#include "MessageInterface.hpp"

BEGIN_EVENT_TABLE(MdiChildXyFrame, wxMDIChildFrame)
    EVT_MENU(GmatPlot::MDI_XY_CHILD_QUIT, MdiChildXyFrame::OnQuit)
    EVT_MENU(GmatPlot::MDI_XY_CHANGE_TITLE, MdiChildXyFrame::OnChangeTitle)
    EVT_MENU(GmatPlot::MDI_XY_CLEAR_PLOT, MdiChildXyFrame::OnClearPlot)
    EVT_MENU(GmatPlot::MDI_XY_SHOW_DEFAULT_VIEW, MdiChildXyFrame::OnShowDefaultView)
    EVT_MENU(GmatPlot::MDI_XY_DRAW_GRID, MdiChildXyFrame::OnDrawGrid)
    EVT_MENU(GmatPlot::MDI_XY_DRAW_DOTTED_LINE, MdiChildXyFrame::OnDrawDottedLine)
    EVT_MENU(GmatPlot::MDI_XY_HELP_VIEW, MdiChildXyFrame::OnHelpView)

    EVT_PLOT_CLICKED(-1, MdiChildXyFrame::OnPlotClick)
    EVT_ACTIVATE(MdiChildXyFrame::OnActivate)
    EVT_SIZE(MdiChildXyFrame::OnSize)
    EVT_MOVE(MdiChildXyFrame::OnMove)
    EVT_CLOSE(MdiChildXyFrame::OnClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildXyFrame(wxMDIParentFrame *parent, const wxString &plotName,
//                 const wxString& title, const wxString& xAxisTitle,
//                 const wxString& yAxisTitle, const wxPoint& pos,
//                 const wxSize& size, const long style)
//------------------------------------------------------------------------------
MdiChildXyFrame::MdiChildXyFrame(wxMDIParentFrame *parent, const wxString &plotName,
                                 const wxString& title, const wxString& xAxisTitle,
                                 const wxString& yAxisTitle, const wxPoint& pos,
                                 const wxSize& size, const long style)
    : wxMDIChildFrame(parent, -1, title, pos, size,
                      style | wxNO_FULL_REPAINT_ON_RESIZE)
{
    mXyPlot = (wxPlotWindow *) NULL;
    mPlotName = plotName;
    mPlotTitle = title;
    mXAxisTitle = xAxisTitle;
    mYAxisTitle = yAxisTitle;

    for (int i=0; i<MAX_NUM_CURVE; i++)
        mHasFirstXSet[i] = false;
    
    MessageInterface::ShowMessage("MdiChildXyFrame::MdiChildXyFrame() "
                                  "X Axis Title = %s  Y Axis Title = %s\n",
                                  xAxisTitle.c_str(), yAxisTitle.c_str());
    
    MdiXyPlot::mdiChildren.Append(this);
    
    // Give it an icon
#ifdef __WXMSW__
    SetIcon(wxIcon(_T("chrt_icn")));
#else
    SetIcon(wxIcon( mondrian_xpm ));
#endif

    // File menu
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(GmatPlot::MDI_XY_OPEN_PLOT_FILE, _T("&Open XY Plot File"));
    fileMenu->Append(GmatPlot::MDI_XY_QUIT, _T("&Exit"));

    // Plot menu
    wxMenu *plotMenu = new wxMenu;

    plotMenu->Append(GmatPlot::MDI_XY_CLEAR_PLOT, _T("Clear Plot"));
    plotMenu->Append(GmatPlot::MDI_XY_CHILD_QUIT, _T("&Close"), _T("Close this window"));
    plotMenu->AppendSeparator();
    plotMenu->Append(GmatPlot::MDI_XY_CHANGE_TITLE, _T("Change &title..."));

    // View menu
    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(GmatPlot::MDI_XY_SHOW_DEFAULT_VIEW, _T("Reset\tCtrl-R"),
                     _("Reset to default view"));
    viewMenu->AppendSeparator();

    // View Option submenu
    wxMenu *viewOptionMenu = new wxMenu;
    wxMenuItem *item =
        new wxMenuItem(viewMenu, GmatPlot::MDI_XY_VIEW_OPTION, _T("Option"),
                       _T("view options"), wxITEM_NORMAL, viewOptionMenu);
    viewOptionMenu->Append(GmatPlot::MDI_XY_DRAW_GRID,
                           _T("Draw Grids"),
                           _T("Draw Grids"), wxITEM_CHECK);
    viewOptionMenu->Append(GmatPlot::MDI_XY_DRAW_DOTTED_LINE,
                           _T("Draw dotted line"),
                           _T("Draw dotted line"), wxITEM_CHECK);

    viewOptionMenu->Check(GmatPlot::MDI_XY_DRAW_DOTTED_LINE, false);
        
    viewMenu->Append(item);

    // Help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(GmatPlot::MDI_XY_HELP_VIEW, _T("View"), _T("View mouse control"));

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

    // Create XyPlotFrame
    int width, height;
    GetClientSize(&width, &height);
    wxPlotWindow *frame =
        new wxPlotWindow(this, -1, wxPoint(0, 0), wxSize(width, height), wxPLOT_DEFAULT);

    mXyPlot = frame;

    // Set units per X value
    mXyPlot->SetUnitsPerValue(0.001); //loj: use this for A1Mjd time only. how about others?

    // Create log window
    //loj: 2/24/04 MdiChildXyFrame::OnPlotClick() calls wxLogMessage(),
    // so used wxLogStatus() instead
    // If I don't have this, it doesn't scroll
    //mLogTextCtrl = new wxTextCtrl( this, -1, "",
    //                               wxPoint(0,0), wxSize(100,20), wxTE_MULTILINE );
    //loj: 2/23/04 wxLog *oldLog = wxLog::SetActiveTarget( new wxLogTextCtrl( mLogTextCtrl ) );
    //delete oldLog;
    
    wxPanel *panel = new wxPanel(this, -1, wxPoint(0,0), wxSize(100,30));
    panel->SetBackgroundColour(*wxWHITE);
    
    wxStaticText *titleText = new wxStaticText(panel, -1, wxT(xAxisTitle + " vs " + yAxisTitle));
    
    wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(titleText, 0, wxALIGN_CENTER | wxALL, 5);
    panel->SetSizer(panelSizer);
    
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    
    //loj: If mLogTextCtrl or panel is not added, I had to resize the frame to see the plot
    //topSizer->Add( mLogTextCtrl, 0, wxEXPAND );
    topSizer->Add(panel, 0, wxALIGN_CENTER | wxEXPAND);
    topSizer->Add(mXyPlot, 1, wxALIGN_CENTER |wxEXPAND);
    
    SetAutoLayout( TRUE );
    SetSizer( topSizer );
            
    // this should work for MDI frames as well as for normal ones
    SetSizeHints(100, 100);
}

//------------------------------------------------------------------------------
// ~MdiChildXyFrame()
//------------------------------------------------------------------------------
MdiChildXyFrame::~MdiChildXyFrame()
{
    MdiXyPlot::mdiChildren.DeleteObject(this);
}

//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
//#if wxUSE_TEXTDLG
    static wxString s_title = _T("Plot Frame");

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
void MdiChildXyFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
//      if (mXyPlot)
//          mXyPlot->ClearPlot();
}

//------------------------------------------------------------------------------
// void OnShowDefaultView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnShowDefaultView(wxCommandEvent& event)
{
//      if (mXyPlot)
//          mXyPlot->ShowDefaultView();
}

//------------------------------------------------------------------------------
// void OnDrawGrid(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnDrawGrid(wxCommandEvent& event)
{
//      if (mXyPlot)
//          mXyPlot->DrawGrid(event.IsChecked());
}

//------------------------------------------------------------------------------
// void OnDrawDottedLine(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnDrawDottedLine(wxCommandEvent& event)
{
//      if (mXyPlot)
//          mXyPlot->DrawDottedLine(event.IsChecked());
}

//------------------------------------------------------------------------------
// void OnHelpView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnHelpView(wxCommandEvent& event)
{
    wxMessageBox(_T("Use Left mouse button to rotate \n"
                    "Right mouse button to zoom \n"
                    "Shift left mouse button to translate"),
                 _T("Help View"),
                 wxOK | wxICON_INFORMATION, this);
}

//------------------------------------------------------------------------------
// void OnPlotClick(wxPlotEvent &event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnPlotClick(wxPlotEvent &event)
{
    if (mXyPlot)
    {
        XyPlotCurve *curve = (XyPlotCurve*)event.GetCurve();
        double x = (event.GetPosition() * mXyPlot->GetUnitsPerValue()) +
            curve->GetFirstX();
        double y = event.GetCurve()->GetY( event.GetPosition() );
        
        //MessageInterface::ShowMessage("MdiChildXyFrame::OnPlotClick() xpos = %d  "
        //                              "firstx = %f  unitspervalue = %g\n", event.GetPosition(),
        //                              curve->GetFirstX(), mXyPlot->GetUnitsPerValue());
        
        wxString info;
        info.Printf("%s: %5.3f  %s: %f\n", GetXAxisTitle().c_str(), x,
                    curve->GetCurveTitle().c_str(), y);
        
        //loj: 2/26/04 changed to wxLogStatus
        wxLogStatus(MdiXyPlot::mdiParentXyFrame, info);
        
        //mLogTextCtrl->AppendText(info);
        
        //wxLogMessage(GetXAxisTitle() + ": %5.3f, " +
        //             curve->GetCurveTitle() + ": %f", x, y);
    }
}

//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnActivate(wxActivateEvent& event)
{
    if ( event.GetActive() && mXyPlot )
    {
        mXyPlot->SetFocus();
    }
}

//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnMove(wxMoveEvent& event)
{
    // VZ: here everything is totally wrong under MSW, the positions are
    //     different and both wrong (pos2 is off by 2 pixels for me which seems
    //     to be the width of the MDI canvas border)
    
//      wxPoint pos1 = event.GetPosition(),
//              pos2 = GetPosition();
//      wxLogStatus(MdiXyPlot::mdiChildXyFrame,
//                  wxT("position from event: (%d, %d), from frame (%d, %d)"),
//                  pos1.x, pos1.y, pos2.x, pos2.y);

    event.Skip();
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnSize(wxSizeEvent& event)
{
    // VZ: under MSW the size event carries the client size (quite
    //     unexpectedly) *except* for the very first one which has the full
    //     size... what should it really be? TODO: check under wxGTK
    
//      wxSize size1 = event.GetSize(),
//             size2 = GetSize(),
//             size3 = GetClientSize();
//      wxLogStatus(MdiXyPlot::mdiChildXyFrame,
//                  wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
//                  size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);

    event.Skip();
}

//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent& event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnClose(wxCloseEvent& event)
{
    MdiXyPlot::numChildren--;
        
    if (MdiXyPlot::numChildren == 0)
        MdiXyPlot::mdiParentXyFrame->subframe = NULL;
    
    event.Skip();
}

//------------------------------------------------------------------------------   
// int ReadXyPlotFile(const wxString &filename)
//------------------------------------------------------------------------------   
int MdiChildXyFrame::ReadXyPlotFile(const wxString &filename)
{
    std::ifstream inStream;  // input data stream
    double tempData[7]; //loj: time, x, y, z, vx, vy, vz for build 2
    int numData = 0;
    double startTime;
    
    if (filename != "")
    {       
        inStream.open(filename.c_str());
        if (inStream.is_open())
        {
            XyPlotCurve *xCurve = new XyPlotCurve(0, -40000.0, 40000.0, "Position X");
            XyPlotCurve *yCurve = new XyPlotCurve(0, -40000.0, 40000.0, "Position Y");
            XyPlotCurve *zCurve = new XyPlotCurve(0, -40000.0, 40000.0, "Position Z");

            // set pen color
            xCurve->SetPenNormal(*wxRED_PEN);
            yCurve->SetPenNormal(*wxGREEN_PEN);
            zCurve->SetPenNormal(*wxCYAN_PEN);
            
            xCurve->SetPenSelected(*wxBLACK_PEN);
            yCurve->SetPenSelected(*wxBLACK_PEN);
            zCurve->SetPenSelected(*wxBLACK_PEN);
            
            // read 1st line to get start time
            for (int i=0; i<7; i++)
                inStream >> tempData[i];

            startTime = tempData[0];

            // set startX (time)
            xCurve->SetFirstX(startTime);
            yCurve->SetFirstX(startTime);
            zCurve->SetFirstX(startTime);
            
            // set time, X, Y, Z
            xCurve->AddData(0.0, tempData[1]); // time, X
            yCurve->AddData(0.0, tempData[2]); // time, Y
            zCurve->AddData(0.0, tempData[3]); // time, Z
            numData++;

            while(!inStream.eof())
            {
                // read time, X, Y, Z, Vx, Vy, Vz
                for (int i=0; i<7; i++)
                    inStream >> tempData[i];

                // set time, X, Y, Z
                xCurve->AddData(tempData[0]-startTime, tempData[1]); // time, X
                yCurve->AddData(tempData[0]-startTime, tempData[2]); // time, Y
                zCurve->AddData(tempData[0]-startTime, tempData[3]); // time, Z

                numData++;
            }
            
            //loj: use this for A1Mjd time only
            //loj: 21545.xxx
            mXyPlot->SetUnitsPerValue(0.001);

            mXyPlot->Add(xCurve);
            mXyPlot->Add(yCurve);
            mXyPlot->Add(zCurve);

        }

        Update();
    }

    return numData;
}

//------------------------------------------------------------------------------   
// void AddPlotCurve(int curveNum, int yOffset, double yMin, double yMax,
//                   const wxString &curveTitle, const wxString &penColorName)
//------------------------------------------------------------------------------   
void MdiChildXyFrame::AddPlotCurve(int curveNum, int yOffset, double yMin, double yMax,
                                   const wxString &curveTitle,
                                   const wxString &penColorName)
{
    MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() yMin = %f, yMax = %f\n",
                                  yMin, yMax);
    
    mHasFirstXSet[curveNum] = false;
    
    // Create XyPlotCurve
    XyPlotCurve *curve = new XyPlotCurve(yOffset, yMin, yMax, curveTitle);
    
    MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() curve title = %s\n",
                                  curve->GetCurveTitle().c_str());
    
    // Find the color
    wxColour *color = wxTheColourDatabase->FindColour(penColorName);
    if (color == NULL)
    {
        // Set normal pen to black dashed pen
        curve->SetPenNormal(*wxBLACK_DASHED_PEN);
        MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() color is NULL... \n");
    }
    else
    {
        wxPen *pen = wxThePenList->FindOrCreatePen(*color, 1, wxSOLID); //loj: check width of 1
        curve->SetPenNormal(*pen);
        MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() found color ... \n");
    }

    //loj: Set selected pen to black for now (build2)
    curve->SetPenSelected(*wxBLACK_PEN);

    MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() adding curve... \n");

    if (mXyPlot != NULL)
    {
        mXyPlot->Add(curve);
        MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() curve count = %d\n",
                                      mXyPlot->GetCount());
    }
    else
        MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() mXyPlot is NULL... \n");
        

}

//------------------------------------------------------------------------------
// void AddDataPoints(int curveNum, double xData, double yData)
//------------------------------------------------------------------------------
/*
 * Updates XY plot curve.
 *
 * @param <curveNum> curve number
 * @param <xData> x value to be updated
 * @param <yData> y value to be updated
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::AddDataPoints(int curveNum, double xData, double yData)
{
    if (mXyPlot)
    {
        //MessageInterface::ShowMessage("MdiChildXyFrame::AddDataPoints() X = %f  "
        //                              "Y = %f\n", xData, yData);
        
        XyPlotCurve *curve = (XyPlotCurve*)(mXyPlot->GetAt(curveNum));
        
        if (!mHasFirstXSet[curveNum])
        {
            curve->SetFirstX(xData);
            mHasFirstXSet[curveNum] = true;
        }
    
        curve->AddData((xData - curve->GetFirstX()), yData); //loj: should I check for curve title?
    }
}

//------------------------------------------------------------------------------
// void RedrawCurve()
//------------------------------------------------------------------------------
/*
 * Redraws XY plot curve.
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::RedrawCurve()
{
    if (mXyPlot)
    {
        mXyPlot->SetFocus();
        mXyPlot->RedrawEverything(); // need to draw everything
        Update();
    }
}

