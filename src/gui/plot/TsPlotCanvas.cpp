// $Header$
//------------------------------------------------------------------------------
//                              TsPlotCanvas
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotCanvas window.
 */
//------------------------------------------------------------------------------


#include "TsPlotCanvas.hpp"
#include "MessageInterface.hpp"
#include "TsPlotOptionsDialog.hpp"
 
#include <cmath>
#include <fstream>
#include <algorithm>


// Define the static data
int  TsPlotCanvas::xSensitivity = 5;
int  TsPlotCanvas::ySensitivity = 5;
int  TsPlotCanvas::defaultLineWidth = 1;
bool TsPlotCanvas::defaultLabels = false;

//#define DEBUG_TS_CANVAS 1
//#define DEBUG_INTERFACE 1
//#define DEBUG_TS_CANVAS 1


BEGIN_EVENT_TABLE(TsPlotCanvas, wxWindow)
   EVT_PAINT         (TsPlotCanvas::OnPaint)
   EVT_SIZE          (TsPlotCanvas::OnSize)
   EVT_MOUSE_EVENTS  (TsPlotCanvas::OnMouseEvent)
   EVT_MENU          (ID_TOGGLE_GRID, TsPlotCanvas::ToggleGrid)
   EVT_MENU          (ID_TOGGLE_LEGEND, TsPlotCanvas::ToggleLegend)
   EVT_MENU          (ID_PLOT_DETAILS, TsPlotCanvas::SetOptions)
   EVT_MENU          (ID_PLOT_SAVE, TsPlotCanvas::SaveData)
END_EVENT_TABLE()


// Define a constructor for my canvas
TsPlotCanvas::TsPlotCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
                           const wxSize& size, long style, 
                           const wxString& name) :
   wxWindow       (parent, -1, pos, size, style),
   left           (80),
   right          (30),
   top            (20),
   bottom         (50),
   titleSize      (10),
   labelSize      (10),
   legendSize     (8),
   axisLabelSize  (12),
   plotPens       (NULL),
   plotDependent  (NULL),
   xDataName      ("X Data"),
   filename       ("PlotData.txt"),
   plotTitle      (name),
   xLabel         (""),
   yLabel         (""),
   plotXMin       (1e99), 
   plotXMax       (-1e99), 
   plotYMin       (1e99), 
   plotYMax       (-1e99),
   xticks         (6),
   yticks         (4),
   xMinorTicks    (5),
   yMinorTicks    (5),
   tickSize       (10),
   minorTickSize  (6),
   xMin           (1e99),
   xMax           (-1e99), 
   yMin           (1e99), 
   yMax           (-1e99),
   xName          ("X"), 
   yName          ("Y"),
   hasData        (false),
   rescaled       (true),
   zoomed         (false),
   zooming        (false),
   movingLegend   (false),
   dataUpdated    (false),
   overrideXMin   (false),
   overrideXMax   (false),
   overrideYMin   (false),
   overrideYMax   (false),
//   automaticXMin  (true),
//   automaticXMax  (true),
//   automaticYMin  (true),
//   automaticYMax  (true),
   userXMin       (0.0),
   userXMax       (8600.0),
   userYMin       (-4000.0),
   userYMax       (7000.0),
   showTitle      (false),
   labelAxes      (false),
   hasGrid        (true),
   hasLegend      (true),
   initializeLegendLoc (true),
   xLabelPrecision(8), 
   yLabelPrecision(6),
   legendColumns  (1)
{
   wxPaintDC dc(this);

   labelFont = dc.GetFont();
   labelFont.SetPointSize(labelSize);
   
   titleFont = *wxSWISS_FONT; //dc.GetFont();
   titleFont.SetPointSize(titleSize); 
   titleFont.SetWeight(wxBOLD);
   
   axisFont = *wxSWISS_FONT;
   axisFont.SetPointSize(axisLabelSize); 
   
   
   backgnd = GetBackgroundColour();
   plotColor.Set(0xff, 0xff, 0xff);
   
   gridColor = backgnd;
   gridPen.SetStyle(wxDOT);
   gridPen.SetColour(gridColor);
   
   legendColor.Set(0xff, 0xff, 0xef);
   legendFont = *wxSMALL_FONT;
   legendFont.SetPointSize(legendSize);
   
   legendRect.x = left-5;
   legendRect.y = top-5;
}


//==============================================================================
// wx Message Handlers
//==============================================================================

void TsPlotCanvas::OnPaint(wxPaintEvent& ev)
{
   // On linux, this line floods the processor with messages.  So for
   // platforms that are not using GTK, refresh here
   #ifndef __WXGTK__
      wxWindow::Refresh(false);
   #endif
   
   wxPaintDC dc(this);
   wxCoord w, h;
   dc.GetSize(&w, &h);

   // If the legend is turned on, be sure it can be seen
   if (legendRect.x > w)
      legendRect.x = w - 5;
   if (legendRect.y > h)
      legendRect.y = h - 5;

   bool drawAll = false;

   wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
   if (!dataUpdated)
      drawAll = true;

   Refresh(dc, drawAll);
}


void TsPlotCanvas::OnSize(wxSizeEvent& ev)
{
   initializeLegendLoc = true;
}


void TsPlotCanvas::OnMouseEvent(wxMouseEvent& event)
{
   bool changed = false;

   wxClientDC dc(this);
   PrepareDC(dc);
   static int oldX, oldY;

   wxPoint pt(event.GetLogicalPosition(dc));
   
   if (event.RightDown())
   {
      wxMenu menu;
      menu.AppendCheckItem(ID_TOGGLE_GRID, "Toggle Grid");
      menu.AppendCheckItem(ID_TOGGLE_LEGEND, "Toggle Legend");
      menu.AppendSeparator();
      menu.Append(ID_PLOT_DETAILS, "Set Plot Options...");
      menu.AppendSeparator();
      menu.Append(ID_PLOT_SAVE, "Save Plot...");
      
      menu.Check(ID_TOGGLE_GRID, hasGrid);
      menu.Check(ID_TOGGLE_LEGEND, hasLegend);
      
      #ifdef __WXMSW__
         // Windows seems to need a location here.
         PopupMenu(&menu, pt);
      #else
         PopupMenu(&menu);
      #endif
   }
   else if (event.LeftDClick())
   {
      changed = true;
      UnZoom();
   }
   else
   {
      if (event.LeftDown())
      {
         mouseRect.x = pt.x;
         mouseRect.y = pt.y;
         oldX = pt.x;
         oldY = pt.y;

         // First check to see if the user is dragging the legend
         if (hasLegend)
         {
            if (legendRect.wxRECT_CONTAINS(pt))
               movingLegend = true;
         }
         if (!movingLegend)
         {
            #ifdef DEBUG_INTERFACE
               MessageInterface::ShowMessage("Down at [%ld  %ld]\n",
                  mouseRect.x, mouseRect.y);
            #endif
               
            if (plotArea.wxRECT_CONTAINS(pt))
               zooming = true;
         }
      }
      if (event.Dragging())
      {
         int logfun = dc.GetLogicalFunction();
         dc.SetLogicalFunction(wxINVERT);
         if (zooming)
         {
            dc.DrawLine(mouseRect.x, mouseRect.y, mouseRect.x, oldY);
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, mouseRect.y);
            dc.DrawLine(oldX, mouseRect.y, oldX, oldY);
            dc.DrawLine(mouseRect.x, oldY, oldX, oldY);
            oldX = pt.x;
            oldY = pt.y;
            dc.DrawLine(mouseRect.x, mouseRect.y, mouseRect.x, oldY);
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, mouseRect.y);
            dc.DrawLine(oldX, mouseRect.y, oldX, oldY);
            dc.DrawLine(mouseRect.x, oldY, oldX, oldY);
         }
         else if (movingLegend)
         {
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, oldY);
            oldX = pt.x;
            oldY = pt.y;
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, oldY);
         }
         dc.SetLogicalFunction(logfun);
      }
      if (event.LeftUp() && (movingLegend || zooming))
      {
         bool wentRight = true, wentDown = true;

         if (mouseRect.x <= pt.x)
            mouseRect.width = pt.x - mouseRect.x;
         else
         {
            mouseRect.width = mouseRect.x - pt.x;
            mouseRect.x = pt.x;
            wentRight = false;
         }
         if (mouseRect.y <= pt.y)
            mouseRect.height = pt.y - mouseRect.y;
         else
         {
            mouseRect.height = mouseRect.y - pt.y;
            mouseRect.y = pt.y;
            wentDown = false;
         }

         #ifdef DEBUG_INTERFACE
            MessageInterface::ShowMessage("Up at [%ld  %ld]; ", pt.x, pt.y);
            MessageInterface::ShowMessage("Rect is %ld by %ld\n",
               mouseRect.width, mouseRect.height);
            MessageInterface::ShowMessage("Rectangle properties: [%ld %ld], "
               "%ld by %ld\n", region.x, region.y, region.width, region.height);
         #endif

         if (movingLegend)
         {
            if (wentRight)
               legendRect.x += mouseRect.width;
            else
               legendRect.x -= mouseRect.width;

            if (wentDown)
               legendRect.y += mouseRect.height;
            else
               legendRect.y -= mouseRect.height;

            changed = true;
         }

         if (zooming)
         {
            // First clear the rubberband, in case no zoom is made
            int logfun = dc.GetLogicalFunction();
            dc.SetLogicalFunction(wxINVERT);
            dc.DrawLine(mouseRect.x, mouseRect.y, mouseRect.x, oldY);
            dc.DrawLine(mouseRect.x, mouseRect.y, oldX, mouseRect.y);
            dc.DrawLine(oldX, mouseRect.y, oldX, oldY);
            dc.DrawLine(mouseRect.x, oldY, oldX, oldY);
            dc.SetLogicalFunction(logfun);
            
            if ((mouseRect.width > xSensitivity) &&
                (mouseRect.height > ySensitivity))
            {
               Zoom(mouseRect);
               changed = true;
            }
         }
      }
   }

   if (event.LeftUp() && (movingLegend || zooming))
   {
      zooming = false;
      movingLegend = false;
   }

   if (changed)
   {
      // Tell curves to draw everything
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
         (*i)->lastPointPlotted = 0;

      Refresh(dc, true);
   }
}


//==============================================================================
// Core plotting methods
//==============================================================================

void TsPlotCanvas::Refresh(wxDC &dc, bool drawAll)
{
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::Refresh() datasize=%d, drawAll=%d\n", data.size(), drawAll);
   #endif
   
   wxCoord w, h;
   dc.GetSize(&w, &h);
   
   if (showTitle)
      top = 30;
   else
      top = 20;

   Rescale(dc);

   // Set region colors
   if (rescaled || drawAll)
   {   
      dc.SetBackground(wxBrush(backgnd, wxTRANSPARENT));
      dc.Clear();
   
      dc.SetClippingRegion(left, top, (w-(left+right)), (h-(top+bottom)));
      dc.SetBackground(wxBrush(plotColor, wxTRANSPARENT));
      dc.Clear();
      dc.DestroyClippingRegion();
      
      plotArea.x      = left;
      plotArea.y      = top;
      plotArea.width  = w - (left + right);
      plotArea.height = h - (top + bottom);
   }
   
   // Set curves to plot all data when needed
   if ((!rescaled && drawAll) || zoomed)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
         (*i)->lastPointPlotted = 0;

   // Now draw everything
   wxBEGIN_DRAWING

   if (rescaled || drawAll)
   {   
      DrawAxes(dc);
      DrawLabels(dc);
   }
   
   if (hasGrid && (rescaled || drawAll))
      DrawGrid(dc);

   PlotData(dc);

   if (rescaled || drawAll)
   {
      if (hasLegend)
         DrawLegend(dc);
   }
   
   wxEND_DRAWING
   dataUpdated = false;
   
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage("TsPlotCanvas::Refresh() leaving\n");
   #endif
}


void TsPlotCanvas::DrawGrid(wxDC &dc)
{
   wxCoord w, h;
   dc.GetSize(&w, &h);

   int x0  = left,
       y0  = h-(bottom),
       xm  = w-right,
       ym  = top;

   dc.SetPen(gridPen);
   
//   if (xGridLoc.size() > 0)
//      for (unsigned int i = 0; i <= xGridLoc.size(); ++i)
   if (xticks > 0)
      for (int i = 0; i <= xticks; ++i)
         if ((xGridLoc[i] != x0) && (xGridLoc[i] != xm))
            dc.DrawLine(xGridLoc[i], y0 - tickSize, xGridLoc[i], ym + tickSize);
//   if (yGridLoc.size() > 0)
//      for (unsigned int i = 0; i <= yGridLoc.size(); ++i)
   if (yticks > 0)
      for (int i = 0; i <= yticks; ++i)
         if ((yGridLoc[i] != y0) && (yGridLoc[i] != ym))
            dc.DrawLine(xm - tickSize, yGridLoc[i], x0+tickSize, yGridLoc[i]);
}


void TsPlotCanvas::DrawLegend(wxDC &dc)
{
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::DrawLegend() names.size=%d\n", names.size());
   #endif
   
   int j = 0, h = 16, w = 16, labelCount = (int)names.size();
   wxString label;
   int xloc, yloc, rowCount = 1, colCount;

   if (legendColumns > 0)
   {
      rowCount = (labelCount + legendColumns - 1) / legendColumns;
      colCount = legendColumns;
   }
   else
      colCount = labelCount;

   wxColour textFore = dc.GetTextForeground();

   dc.SetFont(legendFont);

   // Find widest text extent
   int maxw = 0;
   for (j = 0; j < labelCount; ++j)
   {
      dc.GetTextExtent(names[j].c_str(), &w, &h);
      if (w > maxw)
         maxw = w;
   }

   legendRect.height = (h+1)*rowCount + 8;
   legendRect.width = (maxw + 10) * colCount;
   
   if ((maxw > 0) && (initializeLegendLoc))
   { 
      legendRect.x = plotArea.x + plotArea.width - legendRect.width + 5;
      legendRect.y = plotArea.y - 5;
      initializeLegendLoc = false;  // Turn off static legend location
   }

   dc.SetClippingRegion(legendRect);
   dc.SetBackground(wxBrush(legendColor, wxTRANSPARENT));
   dc.Clear();

   dc.SetPen(*wxBLACK_PEN);
   dc.DrawLine(legendRect.x + 1, legendRect.y + 1,
               legendRect.x + 1, legendRect.y + legendRect.height-2);
   dc.DrawLine(legendRect.x + 1, legendRect.y + 1,
               legendRect.x + legendRect.width - 2, legendRect.y + 1);
   dc.DrawLine(legendRect.x + legendRect.width - 2, legendRect.y + 1,
               legendRect.x + legendRect.width - 2, 
               legendRect.y + legendRect.height - 2);
   dc.DrawLine(legendRect.x + 1, legendRect.y + legendRect.height - 2,
               legendRect.x + legendRect.width - 2, 
               legendRect.y + legendRect.height - 2);

   for (j = 0; j < labelCount; ++j)
   {
      dc.SetTextForeground(plotPens[j].GetColour());
      label = _T(names[j].c_str());
      xloc = legendRect.x + 6;
      yloc = legendRect.y + (h+1)*j + 4;
      dc.DrawText(label, xloc, yloc);
   }
   
   dc.SetTextForeground(textFore);
   dc.DestroyClippingRegion();
}


//==============================================================================
// Data manipulation methods
//==============================================================================

void TsPlotCanvas::SetDataName(const std::string &dataName)
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Adding data named %s\n", dataName.c_str());
   #endif
   
   names.push_back(dataName);
}


void TsPlotCanvas::SetLabel(const std::string &dataName, 
                            const PlotComponents which)
{
   switch (which)
   {
      case PLOT_TITLE:
         #ifdef DEBUG_LABELS
            MessageInterface::ShowMessage("Plot title is %s\n", 
               dataName.c_str());
         #endif
         plotTitle = dataName;
         yLabel = "";
         xLabel = "";
         break;
      
      case X_LABEL:
         #ifdef DEBUG_LABELS
            MessageInterface::ShowMessage("X label is %s\n", dataName.c_str());
         #endif
         xLabel = dataName;
         break;
      
      case Y_LABEL:
         #ifdef DEBUG_LABELS
            MessageInterface::ShowMessage("Y label is %s\n", dataName.c_str());
         #endif
         yLabel = dataName;
         break;
      
      default:
         break;
   }
}


void TsPlotCanvas::AddData(TsPlotCurve *curve, wxColour startColor)
{
   data.push_back(curve);
   penUpLocations.push_back(curve->GetPenUpLocations());

   unsigned int varCount = (unsigned int)data.size();
   wxPen *newPens = new wxPen[varCount];
   
   for (unsigned int i = 0; i < varCount-1; ++i)
      newPens[i] = plotPens[i];
      
   // Defaults for the first 6 (if there are that many)
   int penID = varCount - 1;
   
   switch (penID)
   {
      case 0:
         newPens[0].SetColour(220, 0, 0);
         break;
         
      case 1:
         newPens[1].SetColour(0, 220, 0);
         break;

      case 2:
         newPens[2].SetColour(0, 0, 220);
         break;

      case 3:
         newPens[3].SetColour(220, 220, 0);
         break;

      case 4:
         newPens[4].SetColour(220, 0, 220);
         break;

      case 5:
         newPens[5].SetColour(0, 220, 220);
         break;
         
      default:
         newPens[penID].SetColour(0, 0, 0);  // Black for the rest
   }

   newPens[penID].SetWidth(defaultLineWidth);

   if (plotPens)
      delete [] plotPens;
      
   if (startColor != *wxWHITE)
      newPens[penID].SetColour(startColor);
   
   plotPens = newPens;
   
   curve->SetColour(plotPens[penID].GetColour());
}


void TsPlotCanvas::DataUpdate(bool tf)
{
   dataUpdated = tf;
}


TsPlotCurve* TsPlotCanvas::GetPlotCurve(int index)
{
   if ((index < (int)data.size()) && (index >= 0))
      return data[index];
   return NULL;
}

void TsPlotCanvas::DeletePlotCurve(int index)
{
   if ((index < (int)data.size()) && (index >= 0))
   {
      TsPlotCurve *curve = GetPlotCurve(index);
      std::vector<TsPlotCurve*>::iterator iter =
         find(data.begin(), data.end(), curve);
      data.erase(iter);
      delete curve;
   }

   //loj: 6/15/05
   // We dont keep track of deleted names, so just clear
   names.clear();

   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::DeletePlotCurve() datasize=%d, index=%d\n",
       index, data.size());
   #endif
}

int TsPlotCanvas::GetCurveCount()
{
   return (int)data.size();
}


void TsPlotCanvas::DumpData(const std::string &fn)
{
   std::ofstream outfile;
   std::string outname = filename;
   if (fn != "")
      outname = fn;
      
   if (outname != "")
   {
      outfile.open(outname.c_str());
      outfile.precision(15);
      
      outfile << plotTitle << std::endl 
              << xLabel << "   " << yLabel << std::endl;
      
      /// @todo Determine a better format to write out the plot data
      int curveNum = 0;
      for (std::vector<TsPlotCurve*>::iterator i = data.begin();
           i != data.end(); ++i)
      {
         int j;
         outfile << names[curveNum++] << "\n";
         for (j = 0; j < (int)((*i)->abscissa.size()); ++j)
            outfile << (*i)->abscissa[j] << ", " << (*i)->ordinate[j] << "\n";
         outfile << "\n";
      }
      outfile.close();
   }
}


//------------------------------------------------------------------------------
// void wxPlotWindow::ClearAllCurveData()
//------------------------------------------------------------------------------
void TsPlotCanvas::ClearAllCurveData()
{
   for (std::vector<TsPlotCurve*>::iterator i = data.begin();
        i != data.end(); ++i)
      (*i)->Clear();

   plotXMin =  1e99; 
   plotXMax = -1e99; 
   plotYMin =  1e99; 
   plotYMax = -1e99;
   xMin     =  1e99;
   xMax     = -1e99; 
   yMin     =  1e99; 
   yMax     = -1e99;

   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage(
      "TsPlotCanvas::ClearAllCurveData() clearing dc\n");
   #endif

   //loj: 6/15/05 Added
   // Clear plot area and redraw other area
   wxClientDC dc(this);
   
   dc.SetClippingRegion(plotArea.x, plotArea.y, plotArea.width, 
      plotArea.height);
   dc.SetBackground(wxBrush(plotColor, wxTRANSPARENT));
   dc.Clear();
   dc.DestroyClippingRegion();
   
   // Now draw other area
   wxBEGIN_DRAWING

   DrawAxes(dc);
   DrawLabels(dc);
   
   if (hasGrid)
      DrawGrid(dc);

   if (hasLegend)
      DrawLegend(dc);

   wxEND_DRAWING
   dataUpdated = false;
   
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage(
      "TsPlotCanvas::ClearAllCurveData() leaving\n");
   #endif
}


//==============================================================================
// Interface manipulation methods
//==============================================================================

void TsPlotCanvas::ShowGrid(bool show)
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Toggling hasGrid to %s\n",
         (show ? "true" : "false"));
   #endif
   hasGrid = show;
}


void TsPlotCanvas::ToggleGrid(wxCommandEvent& event)
{
   hasGrid = (hasGrid ? false : true);
   wxWindow::Refresh(true);
}


void TsPlotCanvas::ShowLegend(bool show)
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Toggling hasLegend to %s\n",
         (show ? "true" : "false"));
   #endif
   hasLegend = show;
}


void TsPlotCanvas::ToggleLegend(wxCommandEvent& event)
{
   hasLegend = (hasLegend ? false : true);
   wxWindow::Refresh(true);
}


void TsPlotCanvas::SetOptions(wxCommandEvent& event)
{
   TsPlotOptionsDialog dlg(xName, yName, this, -1, "Plot options");
   dlg.SetPlotTitle(plotTitle);
   dlg.SetXLabel(xLabel);
   dlg.SetYLabel(yLabel);
   if (data.size() > 0)
      dlg.SetWidth(data[0]->GetWidth());
   else
      dlg.SetWidth(1);
      
   dlg.SetXPrecision(xLabelPrecision);
   dlg.SetXPrecision(yLabelPrecision);
   
   dlg.SetXMin(userXMin);
   dlg.SetXMax(userXMax);
   dlg.SetYMin(userYMin);
   dlg.SetYMax(userYMax);
   
   dlg.SetXMinState(overrideXMin);
   dlg.SetXMaxState(overrideXMax);
   dlg.SetYMinState(overrideYMin);
   dlg.SetYMaxState(overrideYMax);
   
   if (dlg.ShowModal() == wxID_OK)
   {
      // Plot title
      plotTitle = dlg.GetPlotTitle();
      showTitle = (plotTitle != "");
      
      // Plot x- and y-axis label
      xLabel = dlg.GetXLabel();
      yLabel = dlg.GetYLabel();
      labelAxes = ((xLabel != "") | (yLabel != ""));

      // Line properties
      SetLineWidth(dlg.GetWidth());

      xLabelPrecision = dlg.GetXPrecision();
      yLabelPrecision = dlg.GetXPrecision();
      
      overrideXMin = dlg.GetXMinState();
      overrideXMax = dlg.GetXMaxState();
      overrideYMin = dlg.GetYMinState();
      overrideYMax = dlg.GetYMaxState();
      
      if (overrideXMin)
         userXMin = dlg.GetXMin();
      else
         plotXMin = 1e99;

      if (overrideXMax)
         userXMax = dlg.GetXMax();
      else
         plotXMax = -1e99;

      if (overrideYMin)
         userYMin = dlg.GetYMin();
      else
         plotYMin = 1e99;

      if (overrideYMax)
         userYMax = dlg.GetYMax();
      else
         plotYMax = -1e99;

      ResetRanges();
      
      wxClientDC dc(this);
      Refresh(dc, true);
   }
}


void TsPlotCanvas::ResetRanges()
{
   if (overrideXMin)
      plotXMin = userXMin;
   if (overrideXMax)
      plotXMax = userXMax;
   if (overrideYMin)
      plotYMin = userYMin;
   if (overrideYMax)
      plotYMax = userYMax;
}


void TsPlotCanvas::SaveData(wxCommandEvent& event)
{
   wxFileDialog dlg(this, "Select save file name", "", "PlotData.txt", "*.*", 
      wxFILE_SAVE_FLAG | wxFILE_OVERWRITE_FLAG);
      
   if (dlg.ShowModal() == wxID_OK)
   {
      std::string filename;
      filename = dlg.GetPath().c_str();
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Selected file name: %s", 
            filename.c_str());
      #endif
      DumpData(filename);
   }
}


void TsPlotCanvas::Zoom(wxRect &region)
{
   wxClientDC dc(this);
   wxCoord w, h;
   dc.GetSize(&w, &h);
   wxCoord wid = w - right, ht = h - bottom;
   
   if (region.GetX() < left)
      region.SetX(left);
   if (region.GetX()+region.GetWidth() > wid)
      region.SetWidth(wid-region.GetX());

   if (region.GetY() < top)
      region.SetY(top);
   if (region.GetY()+region.GetHeight() > ht)
      region.SetHeight(ht-region.GetY());

   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Zoom to [%d %d; %d %d]!\n"
                                    "BoundBox is [%d %d; %d %d]",
         region.GetX(), region.GetY(), region.GetX()+region.GetWidth(),
         region.GetY()+region.GetHeight(), left, top, wid, ht);
   #endif


   zoomXMin = GetActualXValue(region.x);
   zoomXMax = GetActualXValue(region.x + region.width);
   zoomYMin = GetActualYValue(region.y + region.height);
   zoomYMax = GetActualYValue(region.y);

   zoomed = true;
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Zoom region is [%lf %lf; %lf %lf]!\n",
         zoomXMin, zoomXMax, zoomYMin, zoomYMax);
   #endif
}


void TsPlotCanvas::UnZoom()
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Unzoom!\n");
   #endif
   zoomed = false;
}


void TsPlotCanvas::PenUp(int index)
{
   if (index == -1)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); 
           i != data.end(); ++i)
         (*i)->PenUp();   
   else
      if ((index >= 0) && (data.size() < (unsigned int)index))
         data[index]->PenUp();

   for (std::vector<TsPlotCurve *>::iterator curve = data.begin(); 
        curve != data.end(); ++curve)
      (*curve)->lastPointPlotted = 0;
      
   // Finally, refresh the display
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::PenDown(int index)
{
   if (index == -1)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); 
           i != data.end(); ++i)
         (*i)->PenDown();   
   else
      if ((index >= 0) && (data.size() < (unsigned int)index))
         data[index]->PenDown();

   for (std::vector<TsPlotCurve *>::iterator curve = data.begin(); 
        curve != data.end(); ++curve)
      (*curve)->lastPointPlotted = 0;

   // Finally, refresh the display
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::SetLineWidth(int w,  int lineId)
{
   if (lineId == -1)
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); 
           i != data.end(); ++i)
         (*i)->SetWidth(w);
   else
      if (lineId < (int)data.size())
         data[lineId]->SetWidth(w);

   // Finally, refresh the display
   for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
      (*i)->lastPointPlotted = 0;
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::SetLineStyle(int ls, int lineId)
{
   if (lineId == -1)
   {
      for (std::vector<TsPlotCurve *>::iterator i = data.begin(); 
           i != data.end(); ++i)
         (*i)->SetStyle(ls);
   }
   else
      if (lineId < (int)data.size())
         data[lineId]->SetStyle(ls);

   // Finally, refresh the display
   for (std::vector<TsPlotCurve *>::iterator i = data.begin(); i != data.end(); ++i)
      (*i)->lastPointPlotted = 0;
   wxClientDC dc(this);
   Refresh(dc, true);
}


void TsPlotCanvas::SetAxisLimit(std::string axisEnd, bool automatic, 
                                double value)
{
   if (axisEnd == "xMinimum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideXMin)
         {
            overrideXMin = false;
            plotXMin = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideXMin = true;
         plotXMin = value;
         ResetRanges();
      }
   }
   else if (axisEnd == "xMaximum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideXMax)
         {
            overrideXMax = false;
            plotXMax = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideXMax = true;
         plotXMax = value;
         ResetRanges();
      }
   }
   else if (axisEnd == "yMinimum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideYMin)
         {
            overrideYMin = false;
            plotYMin = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideYMin = true;
         plotYMin = value;
         ResetRanges();
      }
   }
   else if (axisEnd == "yMaximum")
   {
      if (automatic)
      {
         // Only reset if override was previously used
         if (overrideYMax)
         {
            overrideYMax = false;
            plotYMax = 1e99;
            ResetRanges();
         }
      }
      else
      {
         overrideYMax = true;
         plotYMax = value;
         ResetRanges();
      }
   }
   else
   {
      MessageInterface::ShowMessage("Axis override failed; specify axes using "
         "the names \"xMinimum\", \"xMaximum\", \"yMinimum\", or "
         "\"yMaximum\".\n");
   }
}


//==============================================================================
// Helper methods
//==============================================================================

int TsPlotCanvas::GetXLocation(double val, double val2)
{
   return (int)(left + (val - currentXMin) * xScale);
}


int TsPlotCanvas::GetYLocation(double val, double val2)
{
   return (int)(top + (currentYMax - val) * yScale);
}


double TsPlotCanvas::GetActualXValue(int x, int y)
{
   return currentXMin + (x - left) / xScale;
}


double TsPlotCanvas::GetActualYValue(int y, int x)
{
   return currentYMax - (y - top) / yScale;
}
