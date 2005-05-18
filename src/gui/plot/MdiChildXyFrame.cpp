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
#include "GmatAppData.hpp"
#include "RealUtilities.hpp" // for Abs(), Min(), Max()
#include <fstream>           // for ifstream (plot input file)

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/gdicmn.h"       // for color

#include "RgbColor.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_XY_MDI_FRAME 1
//#define DEBUG_RENAME 1

BEGIN_EVENT_TABLE(MdiChildXyFrame, wxMDIChildFrame)
   EVT_MENU(GmatPlot::MDI_XY_OPEN_PLOT_FILE, MdiChildXyFrame::OnOpenXyPlotFile)
   EVT_MENU(GmatPlot::MDI_XY_CHILD_QUIT, MdiChildXyFrame::OnQuit)
   EVT_MENU(GmatPlot::MDI_XY_CHANGE_TITLE, MdiChildXyFrame::OnChangeTitle)
   EVT_MENU(GmatPlot::MDI_XY_CLEAR_PLOT, MdiChildXyFrame::OnClearPlot)
   EVT_MENU(GmatPlot::MDI_XY_SHOW_DEFAULT_VIEW, MdiChildXyFrame::OnShowDefaultView)
   EVT_MENU(GmatPlot::MDI_XY_DRAW_GRID, MdiChildXyFrame::OnDrawGrid)
   EVT_MENU(GmatPlot::MDI_XY_DRAW_DOTTED_LINE, MdiChildXyFrame::OnDrawDottedLine)

   EVT_PLOT_CLICKED(-1, MdiChildXyFrame::OnPlotClick)
   EVT_ACTIVATE(MdiChildXyFrame::OnActivate)
   EVT_SIZE(MdiChildXyFrame::OnSize)
   EVT_MOVE(MdiChildXyFrame::OnMove)
   EVT_CLOSE(MdiChildXyFrame::OnClose)
   END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildXyFrame(wxMDIParentFrame *parent, bool isMainFrame,
//                 const wxString &plotName, const wxString& plotTitle,
//                 const wxString& xAxisTitle, const wxString& yAxisTitle,
//                 const wxPoint& pos, const wxSize& size, const long style)
//------------------------------------------------------------------------------
MdiChildXyFrame::MdiChildXyFrame(wxMDIParentFrame *parent, bool isMainFrame,
                                 const wxString &plotName, const wxString& plotTitle,
                                 const wxString& xAxisTitle, const wxString& yAxisTitle,
                                 const wxPoint& pos, const wxSize& size, const long style)
   : wxMDIChildFrame(parent, -1, plotName, pos, size,
                     style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mXyPlot = (wxPlotWindow *) NULL;
   mIsMainFrame = isMainFrame;
   mPlotName = plotName;
   mPlotTitle = plotTitle;
   mXAxisTitle = xAxisTitle;
   mYAxisTitle = yAxisTitle;

   for (int i=0; i<MAX_NUM_CURVE; i++)
      mHasFirstXSet[i] = false;
    
   //      MessageInterface::ShowMessage("MdiChildXyFrame::MdiChildXyFrame() "
   //                                    "X Axis Title = %s  Y Axis Title = %s "
   //                                    "isMainFrame = %d\n",
   //                                    xAxisTitle.c_str(), yAxisTitle.c_str(),
   //                                    isMainFrame);
    
   MdiXyPlot::mdiChildren.Append(this);
    
   // Give it an icon
#ifdef __WXMSW__
   SetIcon(wxIcon(_T("chrt_icn")));
#else
   SetIcon(wxIcon( mondrian_xpm ));
#endif

   // Associate the menu bar with the frame
   SetMenuBar(GmatAppData::GetMainFrame()->CreateMainMenu(GmatTree::OUTPUT_XY_PLOT));

   // status bar
   //CreateStatusBar();
   //SetStatusText(title);

   // Create XyPlotFrame
   int width, height;
   GetClientSize(&width, &height);
   wxPlotWindow *frame =
      new wxPlotWindow(this, -1, wxPoint(0, 0), wxSize(width, height), wxPLOT_DEFAULT,
                       plotTitle);

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

   //      //loj: 3/11/04 moved to XyPlotWindow constructor
   //      //================================================================
   //      wxPanel *panel = new wxPanel(this, -1, wxPoint(0,0), wxSize(100,30));
   //      panel->SetBackgroundColour(*wxLIGHT_GREY);
    
   //      wxStaticText *titleText = new wxStaticText(panel, -1, plotTitle);
    
   //      wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
   //      panelSizer->Add(titleText, 0, wxALIGN_CENTER | wxALL, 5);
   //      panel->SetSizer(panelSizer);
   //      //================================================================
    
   wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    
   //loj: If mLogTextCtrl or panel is not added, I had to resize the frame to see the plot
   //topSizer->Add( mLogTextCtrl, 0, wxEXPAND );
   //      topSizer->Add(panel, 0, wxALIGN_CENTER | wxEXPAND);
   topSizer->Add(mXyPlot, 1, wxALIGN_CENTER |wxEXPAND);

   SetAutoLayout( TRUE ); //loj: this is called implicitly by SetSizer()
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
   MdiXyPlot::numChildren--;
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
// bool DeletePlot()
//------------------------------------------------------------------------------
bool MdiChildXyFrame::DeletePlot()
{
//    if (mIsMainFrame)
//       GmatAppData::GetMainFrame()->xyMainSubframe->Close();
   Close(TRUE);
   return true;
}

//------------------------------------------------------------------------------
// void SetPlotTitle(const wxString &title)
//------------------------------------------------------------------------------
void MdiChildXyFrame::SetPlotTitle(const wxString &title)
{
#if DEBUG_XY_MDI_FRAME
   MessageInterface::ShowMessage("MdiChildXyFrame::SetPlotTitle() title = %s\n",
                                 title.c_str());
#endif
   
   mPlotTitle = title;

   //SetTitle(title);
   
   if (mXyPlot)
      mXyPlot->SetPlotTitle(title);
}

//------------------------------------------------------------------------------
// void ShowPlotLegend()
//------------------------------------------------------------------------------
void MdiChildXyFrame::ShowPlotLegend()
{
#if DEBUG_XY_MDI_FRAME
   MessageInterface::ShowMessage("MdiChildXyFrame::ShowPlotLegend() entered\n");
#endif
      
   if (mXyPlot)
      mXyPlot->ShowLegend();
}

//loj: 7/13/04 changed penColor type
//------------------------------------------------------------------------------   
// void AddPlotCurve(int curveIndex, int yOffset, double yMin, double yMax,
//                   const wxString &curveTitle, UnsignedInt penColor)
//------------------------------------------------------------------------------   
void MdiChildXyFrame::AddPlotCurve(int curveIndex, int yOffset, double yMin,
                                   double yMax, const wxString &curveTitle,
                                   UnsignedInt penColor)
{
#if DEBUG_XY_MDI_FRAME
   MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() yMin = %f, yMax = %f\n",
                                 yMin, yMax);
#endif
   
   mHasFirstXSet[curveIndex] = false;
    
   // Create XyPlotCurve
   XyPlotCurve *curve = new XyPlotCurve(yOffset, yMin, yMax, curveTitle);
    
#if DEBUG_XY_MDI_FRAME
   MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() curve title = %s\n",
                                 curve->GetCurveTitle().c_str());
#endif
    
   //========== new code
   RgbColor rgb = RgbColor(penColor);
   wxColour color = wxColour(rgb.Red(), rgb.Green(), rgb.Blue());
   wxPen pen = wxPen(color, 1, wxSOLID);
   curve->SetPenNormal(pen);

//     //========== old code
//     // Find the color
//     wxColour *color = wxTheColourDatabase->FindColour(penColorName);
   
//     if (color == NULL)
//     {
//        // Set normal pen to black dashed pen
//        curve->SetPenNormal(*wxBLACK_DASHED_PEN);
//        MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() color is NULL... \n");
//     }
//     else
//     {
//        wxPen *pen = wxThePenList->FindOrCreatePen(*color, 1, wxSOLID); //loj: check width of 1
//        curve->SetPenNormal(*pen);
//        //MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() found color ... \n");
//     }
//     //========== old code

   
   //loj: Set selected pen to black for now
   curve->SetPenSelected(*wxBLACK_PEN);
   //curve->SetPenSelected(*wxGREY_PEN);


   if (mXyPlot != NULL)
   {
      mXyPlot->Add(curve);
#if DEBUG_XY_MDI_FRAME
      MessageInterface::ShowMessage
         ("MdiChildXyFrame::AddPlotCurve() curve count = %d added\n",
          mXyPlot->GetCount());
#endif
   }
   else
   {
      MessageInterface::ShowMessage("MdiChildXyFrame::AddPlotCurve() mXyPlot is NULL... \n");
   }
}

//------------------------------------------------------------------------------   
// void DeleteAllPlotCurves()
//------------------------------------------------------------------------------   
void MdiChildXyFrame::DeleteAllPlotCurves()
{
   //MessageInterface::ShowMessage("MdiChildXyFrame::DeleteAllPlotCurve() entered \n");
                                 
   if (mXyPlot != NULL)
   {
      while (mXyPlot->GetCount() > 0)
      {
         wxPlotCurve* curve = mXyPlot->GetAt(0);
         mXyPlot->Delete(curve);
         //MessageInterface::ShowMessage("MdiChildXyFrame::DeleteAllPlotCurve() curveCount = %d\n",
         //                              mXyPlot->GetCount());
      }
        
      for (int i=0; i<MAX_NUM_CURVE; i++)
         mHasFirstXSet[i] = false;

   }
   else
   {
      MessageInterface::ShowMessage("MdiChildXyFrame::DeletePlotCurve() mXyPlot is NULL... \n");
   }
}

//------------------------------------------------------------------------------   
// void DeletePlotCurve(int curveIndex)
//------------------------------------------------------------------------------   
void MdiChildXyFrame::DeletePlotCurve(int curveIndex)
{
   //MessageInterface::ShowMessage("MdiChildXyFrame::DeletePlotCurve() curveIndex = %d\n",
   //                              curveIndex);
    
   if (mXyPlot != NULL)
   {
      wxPlotCurve* curve = mXyPlot->GetAt(curveIndex);
      mXyPlot->Delete(curve);
      mHasFirstXSet[curveIndex] = false;
   }
   else
   {
      MessageInterface::ShowMessage("MdiChildXyFrame::DeletePlotCurve() mXyPlot is NULL... \n");
   }
}

//------------------------------------------------------------------------------
// void AddDataPoints(int curveIndex, double xData, double yData)
//------------------------------------------------------------------------------
/*
 * Updates XY plot curve.
 *
 * @param <curveIndex> curve number
 * @param <xData> x value to be updated
 * @param <yData> y value to be updated
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::AddDataPoints(int curveIndex, double xData, double yData)
{
   if (mXyPlot)
   {
      //MessageInterface::ShowMessage("MdiChildXyFrame::AddDataPoints() curveIndex = %d "
      //                              "X = %f  Y = %f\n", curveIndex, xData, yData);
        
      XyPlotCurve *curve = (XyPlotCurve*)(mXyPlot->GetAt(curveIndex));
      //MessageInterface::ShowMessage("MdiChildXyFrame::AddDataPoints() curveTitle = %s\n",
      //                              curve->GetCurveTitle().c_str());
        
      if (!mHasFirstXSet[curveIndex])
      {
         curve->SetFirstX(xData);
         mHasFirstXSet[curveIndex] = true;
      }
    
      curve->AddData((xData - curve->GetFirstX()), yData); //loj: should I check for curve title?
   }
}

//------------------------------------------------------------------------------
// void ClearPlotData()
//------------------------------------------------------------------------------
/*
 * Clears XY plot data.
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::ClearPlotData()
{
   if (mXyPlot)
   {
      mXyPlot->ClearAllCurveData();
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
      AdjustYScale(); //loj: 7/23/04 added
      
      //mXyPlot->SetFocus(); // If SetFocus() is called OpenGl plot flickers
      //mXyPlot->RedrawEverything();
      mXyPlot->RedrawPlotArea(); // 4/29/04 redraw plot area only since X/Y axis don't change
      //mXyPlot->RedrawXAxis();
      mXyPlot->ZoomOut();
      Update(); // need Update to show plot as it runs
      //Refresh(true); // doesn't update the frame
   }
}

//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void MdiChildXyFrame::SetPlotName(const wxString &name)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage("MdiChildXyFrame::SetPlotName() name=%s\n",
                                 name.c_str());
#endif
   mPlotName = name;
   SetTitle(name);
}

//------------------------------------------------------------------------------
// void SetShowGrid(bool show)
//------------------------------------------------------------------------------
/*
 * Sets show grid menu option
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::SetShowGrid(bool show)
{
   if (mXyPlot)
   {
      //loj: 5/18/05 Why this failing when this frame is close?
//       wxMenu *viewOptionMenu = GmatAppData::GetMainFrame()->GetXyViewOptionMenu();
//       viewOptionMenu->Check(GmatPlot::MDI_XY_DRAW_GRID, show);
      
      mXyPlot->SetShowGrid(show);
   }
}

//------------------------------------------------------------------------------
// void ResetZoom()
//------------------------------------------------------------------------------
/*
 * Resets plot zoom value
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::ResetZoom()
{
   if (mXyPlot)
   {
      mXyPlot->ResetZoom();
   }
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
   if (mXyPlot)
   {
      //static wxString s_title = _T("Plot Frame");
      wxString oldTitle = mXyPlot->GetPlotTitle();

      wxString newTitle = wxGetTextFromUser(_T("Enter the new title"),
                                         _T(""),
                                         oldTitle,
                                         GetParent()->GetParent());
      if ( !newTitle )
         return;

      //s_title = title;
      //SetTitle(s_title);
      //SetTitle(newTitle);//loj: 11/19/04 - commented out
      
      mXyPlot->SetPlotTitle(newTitle);
   }
}

//------------------------------------------------------------------------------
// void OnClearPlot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
   //if (mXyPlot)
   //   mXyPlot->ClearPlot();
}

//------------------------------------------------------------------------------
// void OnShowDefaultView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnShowDefaultView(wxCommandEvent& event)
{
   //if (mXyPlot)
   //   mXyPlot->ShowDefaultView();
}

//------------------------------------------------------------------------------
// void OnDrawGrid(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnDrawGrid(wxCommandEvent& event)
{
   //loj: 7/20/04 added
   if (mXyPlot)
   {
      mXyPlot->SetShowGrid(event.IsChecked());
      mXyPlot->RedrawPlotArea();
   }
}

//------------------------------------------------------------------------------
// void OnDrawDottedLine(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnDrawDottedLine(wxCommandEvent& event)
{
   //if (mXyPlot)
   //   mXyPlot->DrawDottedLine(event.IsChecked());
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
      //info.Printf("%s: %5.3f  %s: %f\n", GetXAxisTitle().c_str(), x,
      //            curve->GetCurveTitle().c_str(), y);
      info.Printf("%s: %5.3f  %s: %g\n", GetXAxisTitle().c_str(), x, //loj: 12/10/04 changed to %g
                  curve->GetCurveTitle().c_str(), y);
        
      //loj: 2/26/04 changed to wxLogStatus
      wxLogStatus(GmatAppData::GetMainFrame(), info);
        
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
//    MdiXyPlot::numChildren--; //loj: 5/18/05 moved to destructor
   
//    if (mIsMainFrame)
//       GmatAppData::GetMainFrame()->xyMainSubframe = NULL;
   
//    if (MdiXyPlot::numChildren == 0)
//       GmatAppData::GetMainFrame()->xySubframe = NULL;
   
   event.Skip();
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void AdjustYScale()
//------------------------------------------------------------------------------
/*
 * Automaticlly adjusts y scale to y minimum and maximum value
 */
//------------------------------------------------------------------------------
void MdiChildXyFrame::AdjustYScale()
{
   using namespace GmatMathUtil;
   
   double ymin = GetYMin();
   double ymax = GetYMax();
   double yMaxScale = Max(Abs(ymin), Abs(ymax));
   double yMargin = yMaxScale * 0.1;
   
#if DEBUG_XY_MDI_FRAME
   MessageInterface::ShowMessage
      ("MdiChildXyFrame::AdjustYScale() ymin=%f ymax=%f yMaxScale=%f yMargin=%f\n",
       ymin, ymax, yMaxScale, yMargin);
#endif

   for (unsigned int i=0; i<mXyPlot->GetCount(); i++)
   {
      mXyPlot->GetAt(i)->SetStartY(-yMaxScale - yMargin);
      mXyPlot->GetAt(i)->SetEndY(yMaxScale + yMargin);
   }
}

//------------------------------------------------------------------------------
// double GetYMin()
//------------------------------------------------------------------------------
/*
 * Returns minimum y value of all curves
 */
//------------------------------------------------------------------------------
double MdiChildXyFrame::GetYMin()
{
   double minVal = -123456789.0; //loj: return some other value?
   
   if (mXyPlot)
   {
      std::vector<double> yMinVals;
      std::vector<double>::iterator pos;
   
      for (unsigned int i=0; i<mXyPlot->GetCount(); i++)
      {
         yMinVals.push_back(mXyPlot->GetAt(i)->GetYMin());
      }

#if DEBUG_XY_MDI_FRAME
      MessageInterface::ShowMessage
         ("MdiChildXyFrame::GetYMin() yMinVals.size()=%d\n",
          yMinVals.size());;
#endif
      
      if (yMinVals.size() == 1)
      {
         minVal = yMinVals[0];
      }
      else if (yMinVals.size() >= 2)
      {
         pos = min_element(yMinVals.begin(), yMinVals.end());
         minVal = *pos;
      }
   }

   return minVal;
}

//------------------------------------------------------------------------------
// double GetYMax()
//------------------------------------------------------------------------------
/*
 * Returns minimum y value of all curves
 */
//------------------------------------------------------------------------------
double MdiChildXyFrame::GetYMax()
{
   double maxVal = 123456789.0; //loj: return some other value?
   
   if (mXyPlot)
   {
      std::vector<double> yMaxVals;
      std::vector<double>::iterator pos;
   
      for (unsigned int i=0; i<mXyPlot->GetCount(); i++)
      {
         yMaxVals.push_back(mXyPlot->GetAt(i)->GetYMax());
      }

#if DEBUG_XY_MDI_FRAME
      MessageInterface::ShowMessage
         ("MdiChildXyFrame::GetYMax() yMaxVals.size()=%d\n",
          yMaxVals.size());;
#endif
      
      if (yMaxVals.size() == 1)
      {
         maxVal = yMaxVals[0];
      }
      else if (yMaxVals.size() >= 2)
      {
         pos = max_element(yMaxVals.begin(), yMaxVals.end());
         maxVal = *pos;
      }
   }

   return maxVal;
}

//------------------------------------------------------------------------------
// void OnOpenXyPlotFile(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void MdiChildXyFrame::OnOpenXyPlotFile(wxCommandEvent& WXUNUSED(event) )
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
        GmatAppData::GetMainFrame()->xySubframe =
            new MdiChildXyFrame(GmatAppData::GetMainFrame(), false, //loj: 3/10/04 added false
                                _T("XYPlotFile"),
                                _T("XY Plot File"),
                                _T("Time"),
                                _T("Spacecraft Position X,Y,Z"),
                                wxPoint(-1, -1), wxSize(500, 400), //loj: 2/27/04 wxSize(-1, -1),
                                wxDEFAULT_FRAME_STYLE);

        ++MdiXyPlot::numChildren;
        GmatAppData::GetMainFrame()->xySubframe->SetPlotName("XYPlotFile"
                  + MdiXyPlot::numChildren);
        GmatAppData::GetMainFrame()->xySubframe->SetTitle(xyPlotFileName);

        //-----------------------------------
        // Read text XY Plot file
        //-----------------------------------
        int dataPoints = GmatAppData::GetMainFrame()->xySubframe->ReadXyPlotFile(xyPlotFileName);
        if (dataPoints > 0)
        {
            GmatAppData::GetMainFrame()->xySubframe->Show(TRUE);
            wxLogStatus(GmatAppData::GetMainFrame(),
                        wxT("Number of lines read: %d"), dataPoints);
        }
    }
}
