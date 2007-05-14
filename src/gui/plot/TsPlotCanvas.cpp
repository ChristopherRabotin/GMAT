// $Header$
//------------------------------------------------------------------------------
//                              TsPlotCanvas
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotCanvas window.
 *
 * @note The TsPlotCanvas subsystem is a generic plotting package developed by
 *       Thinking Systems using company resources.  Because the package was
 *       developed independently from the GMAT project, it uses native C++
 *       data types (e.g. int rather than Integer, double rather than Real), and
 *       in that respect will look different from other pieces of the GMAT code
 *       base.
 */
//------------------------------------------------------------------------------


#include "TsPlotCanvas.hpp"
#include "MessageInterface.hpp"
 
#include <cmath>
#include <fstream>


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
   hasData        (false),
   rescaled       (true),
   zoomed         (false),
   zooming        (false),
   movingLegend   (false),
   dataUpdated    (false),
   showTitle      (false),
   labelAxes      (false),
   hasGrid        (true),
   hasLegend      (true),
   initializeLegendLoc (true),
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
      menu.Append(ID_PLOT_SAVE, "Save Plot");
      
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
            #ifdef __USE_WX280__
            if (legendRect.Contains(pt))
            #else
            if (legendRect.Inside(pt))
            #endif
               movingLegend = true;
         }
         if (!movingLegend)
         {
            #ifdef DEBUG_INTERFACE
               MessageInterface::ShowMessage("Down at [%ld  %ld]\n",
                  mouseRect.x, mouseRect.y);
            #endif
               
            #ifdef __USE_WX280__
            if (plotArea.Contains(pt))
            #else
            if (plotArea.Inside(pt))
            #endif
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
         else
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
            //MessageInterface::ShowMessage("Rectangle properties: [%ld %ld], "
            //   "%ld by %ld\n", region.x, region.y, region.width, region.height);
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
            
            #ifdef __USE_WX280__
            if (plotArea.Contains(pt))
            #else
            if (plotArea.Inside(pt))
            #endif
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

   //MessageInterface::ShowMessage("===> Rescale(dc)\n");
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
   //wx280deprecated->dc.BeginDrawing();

   if (rescaled || drawAll)
   {   
      DrawAxes(dc);
      DrawLabels(dc);
   }
   
   if (hasGrid && (rescaled || drawAll))
      DrawGrid(dc);

//   if (hasData)
      PlotData(dc);

   if (rescaled || drawAll)
   {
      if (hasLegend)
         DrawLegend(dc);
   }
   
   //wx280deprecated->dc.EndDrawing();
   dataUpdated = false;
   
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage("TsPlotCanvas::Refresh() leaving\n");
   #endif
}


void TsPlotCanvas::Rescale(wxDC &dc)
{
   bool isNotFirstPoint = false;
   wxCoord w, h;
   dc.GetSize(&w, &h);

   w -= (left+right);
   h -= (top+bottom);

   double delx;
   double dely;

   if (!zoomed)
   {
      std::vector<TsPlotCurve *>::iterator i = data.begin();
      if (i != data.end())
      {
         xMin = (*i)->minX;
         xMax = (*i)->maxX;
         yMin = (*i)->minY;
         yMax = (*i)->maxY;

         // Look through the PlotCurves for ranges
         for (++i; i != data.end(); ++i)
         {
            if ((*i)->minX < xMin)
               xMin = (*i)->minX;
            if ((*i)->maxX > xMax)
               xMax = (*i)->maxX;
            if ((*i)->minY < yMin)
               yMin = (*i)->minY;
            if ((*i)->maxY > yMax)
               yMax = (*i)->maxY;

            if ((*i)->abscissa.size() > 1)
            {
               isNotFirstPoint = true;
               #if DEBUG_TS_CANVAS
                  MessageInterface::ShowMessage(
                     "TsPlotCanvas::Rescale() datasize=%d, %s\n", 
                     data.size(),  
                     (isNotFirstPoint?"is not First Point":"is First Point"));
               #endif
            }
         }
      }
      delx = (xMax - xMin)*0.1;
      dely = (yMax - yMin)*0.1;
      rescaled = false;
      
      //MessageInterface::ShowMessage
      //   ("===> no-zoom: xMin=%g, xMax=%g, yMin=%g, yMax=%g\n",
      //    xMin, xMax, yMin, yMax);
      
      if (xMax > plotXMax)
      {
         plotXMax = xMax + delx;
         rescaled = true;
      }
      
      if (xMin < plotXMin)
      {
         #if DEBUG_TS_CANVAS
            MessageInterface::ShowMessage(
               "TsPlotCanvas::Rescale() xMin(%lf) < plotXMin(%lf) new value: ",
               xMin, plotXMin);
         #endif
         plotXMin = xMin - delx;
         rescaled = true;
         #if DEBUG_TS_CANVAS
            MessageInterface::ShowMessage("%lf\n", plotXMin);
         #endif
      }

      if (yMin < plotYMin)
      {
         plotYMin = yMin - dely;
         rescaled = true;
      }

      if (yMax > plotYMax)
      {
         plotYMax = yMax + dely;
         rescaled = true;
      }

      if (rescaled)
      {
         for (i = data.begin(); i != data.end(); ++i)
            (*i)->lastPointPlotted = 0;
      }

      if (plotXMin == plotXMax)
         delx = 1.0;
      else
         delx = plotXMax - plotXMin;

      if (plotYMin == plotYMax)
         dely = 1.0;
      else
         dely = plotYMax - plotYMin;
      
      currentXMin = plotXMin;
      currentXMax = plotXMax;
      currentYMin = plotYMin;
      currentYMax = plotYMax;
      
      //MessageInterface::ShowMessage
      //   ("===> no-zoom: currentXMin=%g, currentXMax=%g, currentYMin=%g, "
      //    "currentYMax=%g\n", currentXMin, currentXMax, currentYMin, currentYMax);
   }
   else
   {
      xMin = zoomXMin;
      xMax = zoomXMax;
      yMin = zoomYMin;
      yMax = zoomYMax;

      delx = (xMax - xMin);
      dely = (yMax - yMin);

      currentXMin = zoomXMin;
      currentXMax = zoomXMax;
      currentYMin = zoomYMin;
      currentYMax = zoomYMax;
      
      //MessageInterface::ShowMessage
      //   ("===>    zoom: currentXMin=%f, currentXMax=%f, currentYMin=%f,"
      //    "currentYMax=%f\n", currentXMin, currentXMax, currentYMin, currentYMax);
      
      rescaled = true;
   }
   
   xScale = w / delx;
   yScale = h / dely;
   
   //MessageInterface::ShowMessage
   //   ("===> xScale=%g, yScale=%g, w=%d, h=%d, delx=%g, dely=%g\n",
   //    xScale, yScale, w, h, delx, dely);
}


void TsPlotCanvas::DrawAxes(wxDC &dc)
{
   wxCoord w, h;
   dc.GetSize(&w, &h);
   
   // Draw the frame
   dc.SetPen(*wxBLACK_PEN);
   
   int wid = w-(left+right), 
       ht  = h - (top+bottom), 
       x0  = left, 
       y0  = h-(bottom), 
       xm  = w-right, 
       ym  = top;

   dc.DrawLine(x0, y0, x0, ym);
   dc.DrawLine(xm, y0, xm, ym);
   dc.DrawLine(x0, y0, xm, y0);
   dc.DrawLine(x0, ym, xm, ym);

   // Draw the axes with labels  
   int idx, idy;
   double dx, dy, dxval, dyval, fval;

   dx = wid / (double)xticks;
   dy = ht  / (double)yticks;
   dxval = (currentXMax-currentXMin) / (double)xticks;
   dyval = (currentYMax-currentYMin) / (double)yticks;
   
   wxString label;
   
   if ((int)xGridLoc.size() != xticks+1)
   {
      xGridLoc.clear();
      for (int i = 0; i <= xticks; ++i)
         xGridLoc.push_back(0);
   }
   if ((int)yGridLoc.size() != yticks+1)
   {
      xGridLoc.clear();
      for (int i = 0; i <= yticks; ++i)
         yGridLoc.push_back(0);
   }

   dc.SetFont(labelFont);

   // x-Axis first
   double delta, step, logStep, factor, div, start;
   delta = currentXMax - currentXMin;
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage
         ("***XMin = %lf, XMax = %lf, delta=%lf\n", currentXMin, currentXMax, delta);
   #endif
   step = delta / (xticks+1.0);
   if (delta > 0.0)
   {
      logStep = (int)log10(step);
      factor = pow(10.0, logStep-2);
      delta = ((int)(step / factor)) * factor;
      div = (int)(currentXMin / delta);
      if (currentXMin > 0.0)
         div += 1.0;
      start = div * delta;

      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage(
            "step = %lf, logStep = %lf, factor = %lf, div = %lf\n", step, 
            logStep, factor, div);
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start,
            delta);
      #endif
   }
   else
   {
      start = currentXMin;
      delta = step;
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start, 
            delta);
      #endif
   }
   
   // causing VC++ run time error, so just return
   // @todo resolve this run time error later
   #ifdef _MSC_VER
   if (delta < 0)
      return;
   #endif
   
   for (int i = 0; i <= xticks; ++i)
   {
      idx = (int)(i * dx + 0.5);
      fval = start + i * delta;  //currentXMin + i*dxval;
      if (fabs(fval) < 1e-9) fval = 0.0;

      xGridLoc[i] = GetXLocation(fval);  //x0 + idx;
      dc.DrawLine(xGridLoc[i], y0, xGridLoc[i], y0 - tickSize);
      dc.DrawLine(xGridLoc[i], ym, xGridLoc[i], ym + tickSize);
      label.Clear();
      label << fval;
      dc.GetTextExtent(label, &w, &h);
      dc.DrawText(label, xGridLoc[i] - w/2, y0 + h/2);
   }
   
   delta = currentYMax - currentYMin;
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("***YMin = %lf, YMax = %lf, delta=%g\n", currentYMin,
         currentYMax, delta);
   #endif
   step = delta / (yticks+1.0);
   if (delta > 0.0)
   {
      logStep = (int)log10(step);
      factor = pow(10.0, logStep-2);
      delta = ((int)(step / factor)) * factor;
      div = (int)(currentYMin / delta);
      if (currentYMin > 0.0)
         div += 1.0;
      start = div * delta;

      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage(
            "step = %lf, logStep = %lf, factor = %lf, div = %lf\n",
            step, logStep, factor, div);
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start, 
            delta);
      #endif
   }
   else
   {
      start = currentYMin;
      delta = step;
      #ifdef DEBUG_INTERFACE
         MessageInterface::ShowMessage("Start = %lf, delta = %lf\n", start, 
            delta);
      #endif
   }
   
   // causing VC++ run time error, so just return
   // @todo resolve this run time error later
   #ifdef _MSC_VER
   if (delta < 0)
      return;
   #endif
   
   for (int i = 0; i <= yticks; ++i)
   {
      idy = (int)(i * dy + 0.5);
      fval = start + i*delta;
      if (fabs(fval) < 1e-9) fval = 0.0;
      yGridLoc[i] = GetYLocation(fval);
      dc.DrawLine(xm, yGridLoc[i], xm - tickSize, yGridLoc[i]);
      dc.DrawLine(x0, yGridLoc[i], x0 + tickSize, yGridLoc[i]);
      label.Clear();
      label << fval;
      dc.GetTextExtent(label, &w, &h);
      dc.DrawText(label, x0 - w - 4, yGridLoc[i] - h/2);
   }
}


void TsPlotCanvas::DrawLabels(wxDC &dc)
{
   wxCoord wid, w, ht, h;
   dc.GetSize(&wid, &ht);
   
   int xCenter = (wid + left - right)/ 2, yCenter = (ht + top - bottom)/ 2;

   // Build default labels if they are not set already
   if ((xLabel == "") && defaultLabels)
      xLabel = "X Data";
      
   if ((yLabel == "") && defaultLabels)
   {
      if (names.size() != 0)
      {
         for (std::vector<std::string>::iterator i = names.begin(); 
              i < names.end(); ++i)
         {
            if (i != names.begin())
               yLabel += ", ";
            yLabel += *i;
         }
      }
      else
      {
         yLabel = "Y Data";
      }
   }
   
   if (showTitle)
   {
      if (plotTitle == "")
         plotTitle = yLabel + " vs " + xLabel;

      // Draw the title
      dc.SetFont(titleFont);
      wxString label;
      wxString title = _T(plotTitle.c_str());
      dc.GetTextExtent(title, &w, &h);
      int xloc = xCenter - w / 2;
      int yloc = (top - h) / 2;
      dc.DrawText(title, xloc, yloc);
   }

   if (labelAxes)
   {
      // Add x-axis label
      dc.SetFont(axisFont);
      wxString title = _T(xLabel.c_str());
      dc.GetTextExtent(title, &w, &h);
      int xloc = xCenter - w / 2;
      int yloc = ht - bottom / 2;
      dc.DrawText(title, xloc, yloc);

      // Add y-axis label
      title = _T(yLabel.c_str());
      dc.GetTextExtent(title, &w, &h);
      xloc = h / 2;
      yloc = yCenter + w / 2;
      dc.DrawRotatedText(title, xloc, yloc, 90.0);
   }
}


void TsPlotCanvas::DrawGrid(wxDC &dc)
{
   // VC++ causing run time error here
#ifndef _MSC_VER
   wxCoord w, h;
   dc.GetSize(&w, &h);

   int x0  = left,
       y0  = h-(bottom),
       xm  = w-right,
       ym  = top;

   dc.SetPen(gridPen);
   
   //MessageInterface::ShowMessage
   //   ("===> TsPlotCanvas::DrawGrid() xGridLoc.size()=%d, yGridLoc.size()=%d\n",
   //    xGridLoc.size(), yGridLoc.size());
   
   // VC++ causing run time error
   // @todo resolve this run time error later
   if (xGridLoc.size() > 0)
      for (unsigned int i = 0; i <= xGridLoc.size(); ++i)
         if ((xGridLoc[i] != x0) && (xGridLoc[i] != xm))
            dc.DrawLine(xGridLoc[i], y0 - tickSize, xGridLoc[i], ym + tickSize);
   if (yGridLoc.size() > 0)
      for (unsigned int i = 0; i <= yGridLoc.size(); ++i)
         if ((yGridLoc[i] != y0) && (yGridLoc[i] != ym))
            dc.DrawLine(xm - tickSize, yGridLoc[i], x0+tickSize, yGridLoc[i]);
#endif
}


void TsPlotCanvas::PlotData(wxDC &dc)
{
   wxCoord w, h;
   dc.GetSize(&w, &h);

   dc.SetClippingRegion(left, top, (w-(left+right)), (h-(top+bottom)));
   
   if (data.size() > 0)
   {
      int n = 0;
      int pupLoc = -1, pupIndex = -1, locCount = 0;
      const std::vector<int> *pups;
      
      for (std::vector<TsPlotCurve *>::iterator curve = data.begin(); 
           curve != data.end(); ++curve)
      {
         pups = (*curve)->GetPenUpLocations(); // penUpLocations(n);
         locCount = pups->size();
         if (locCount > 0)
         {
            pupIndex = 0;
            pupLoc = (*pups)[pupIndex];
         }
         
         if ((*curve)->abscissa.size() > 0)
         {
            dc.SetPen(plotPens[n]);

            if ((unsigned int)pupLoc < (*curve)->lastPointPlotted)
            {
               // Get the next penup
               ++pupIndex;
               if (pupIndex < locCount)
                  pupLoc = (*pups)[pupIndex];
            }
            
            int j;
            for (j = (*curve)->lastPointPlotted;
                 j < (int)((*curve)->abscissa.size())-1; ++j)
            {
               if (j != pupLoc)
                  dc.DrawLine(
                     int(left+((*curve)->abscissa[j]-currentXMin)*xScale + 0.5),
                     int(top + (currentYMax-(*curve)->ordinate[j])*yScale + 0.5),
                     int(left+((*curve)->abscissa[j+1]-currentXMin)*xScale + 0.5),
                     int(top + (currentYMax-(*curve)->ordinate[j+1])*yScale+0.5));
               else
               {
                  // Get the next penup
                  ++pupIndex;
                  if (pupIndex < locCount)
                     pupLoc = (*pups)[pupIndex];
               }
            }
            (*curve)->lastPointPlotted = j-1;
         }
         ++n;
      }
   }
   dc.DestroyClippingRegion();
}


void TsPlotCanvas::DrawLegend(wxDC &dc)
{
   #if DEBUG_TS_CANVAS
   MessageInterface::ShowMessage
      ("TsPlotCanvas::DrawLegend() names.size=%d\n", names.size());
   #endif
   
   int j = 0, h, w, labelCount = (int)names.size();
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

   unsigned int varCount = data.size();
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
   //wx280deprecated->dc.BeginDrawing();

   DrawAxes(dc);
   DrawLabels(dc);
   
   if (hasGrid)
      DrawGrid(dc);

   if (hasLegend)
      DrawLegend(dc);

   //wx280deprecated->dc.EndDrawing();
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
   wxMessageDialog dlg(this, "Plot options here");
   dlg.ShowModal();
}


void TsPlotCanvas::SaveData(wxCommandEvent& event)
{
   DumpData("");
}


void TsPlotCanvas::Zoom(const wxRect &region)
{
   #ifdef DEBUG_INTERFACE
      MessageInterface::ShowMessage("Zoom to [%d %d; %d %d]!\n",
         region.GetX(), region.GetY(), region.GetX()+region.GetWidth(),
         region.GetY()+region.GetHeight());
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
//   
//   wxPaintDC dc(this);
//   Refresh(dc, true);
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
}


//==============================================================================
// Helper methods
//==============================================================================

int TsPlotCanvas::GetXLocation(double val)
{
   return (int)(left + (val - currentXMin) * xScale);
}


int TsPlotCanvas::GetYLocation(double val)
{
   return (int)(top + (currentYMax - val) * yScale);
}


double TsPlotCanvas::GetActualXValue(int x)
{
   return currentXMin + (x - left) / xScale;
}


double TsPlotCanvas::GetActualYValue(int y)
{
   return currentYMax - (y - top) / yScale;
}
