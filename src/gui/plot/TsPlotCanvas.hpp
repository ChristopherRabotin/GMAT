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


#ifndef _TSPLOTCANVAS_HPP_
#define _TSPLOTCANVAS_HPP_

#include <wx/wx.h>
#include <vector>
#include <string>

#include "TsPlotDefs.hpp"
#include "TsPlotCurve.hpp"


class TsPlotCanvas : public wxWindow
{
public:
   enum PlotComponents {
      PLOT_TITLE = 1024,
      X_LABEL,
      Y_LABEL,
      PLOT_AREA,
      XAXIS,
      YAXIS,
      YAXIS2,
      ID_TOGGLE_GRID,
      ID_TOGGLE_LEGEND,
      ID_PLOT_DETAILS,
      ID_PLOT_SAVE
   };

public:
   TsPlotCanvas(wxWindow* parent, wxWindowID id = -1, 
                const wxPoint& pos = wxPoint(1,1),
                const wxSize& size = wxDefaultSize, 
                long style = wxTAB_TRAVERSAL,
                const wxString& name = "");

   virtual void OnPaint(wxPaintEvent& ev);
   virtual void OnSize(wxSizeEvent& ev);
   void OnMouseEvent(wxMouseEvent& event);
   
   void Refresh(wxDC &dc, bool drawAll);

   void DumpData(const std::string &fn);
   void SetLabel(const std::string &dataName, const PlotComponents which);
   void SetDataName(const std::string &dataName);
   void AddData(TsPlotCurve *curve, wxColour startColor = *wxWHITE);
   void DataUpdate(bool tf = true);
   int  GetCurveCount();
   void ShowGrid(bool show);
   void ShowLegend(bool show = true);
   
   void ClearAllCurveData();

   TsPlotCurve*   GetPlotCurve(int index);
   void DeletePlotCurve(int index);
   
   void PenUp(int index = -1);
   void PenDown(int index = -1);
   
protected:
   /// borders
   int left, right, top, bottom;
   /// fonts
   wxFont titleFont, labelFont, axisFont, legendFont;
   /// font sizes
   int titleSize, labelSize, legendSize;
   /// Colors
   wxColour backgnd, plotColor, legendColor;
   wxColour gridColor;
   wxPen gridPen;
   
   int axisLabelSize;
   
   std::vector<TsPlotCurve *> data;
   std::vector<const std::vector<int>*> penUpLocations;
   
   wxPen *plotPens;
   std::vector <int> *plotDependent;
   std::vector <std::string> names;
   std::string xDataName;
   std::string filename;
   std::string plotTitle;
   std::string xLabel;
   std::string yLabel;
   double plotXMin;
   double plotXMax;
   double plotYMin;
   double plotYMax;
   double currentXMin;
   double currentXMax;
   double currentYMin;
   double currentYMax;
   double zoomXMin;
   double zoomXMax;
   double zoomYMin;
   double zoomYMax;
   double xScale;
   double yScale;
   int xticks;
   int yticks;
   int xMinorTicks;
   int yMinorTicks;
   std::vector <int> xGridLoc;
   std::vector <int> yGridLoc;
   int tickSize;
   int minorTickSize;
   int varCount;
   
   double xMin, xMax, yMin, yMax;
   
   int currentId;
   bool hasData;
   bool rescaled;
   bool zoomed;
   bool zooming;
   bool movingLegend;
   bool dataUpdated;
   
   // Appearance parms
   bool showTitle;
   bool labelAxes;
   bool hasGrid;
   bool hasLegend;
   bool initializeLegendLoc;

   long zoomLeft, zoomTop, zoomWidth, zoomHeight;
   
   virtual void DrawAxes(wxDC &dc) = 0;
   virtual void DrawLabels(wxDC &dc) = 0;
   virtual void PlotData(wxDC &dc) = 0;
   virtual void Rescale(wxDC &dc) = 0;
   
   virtual void DrawGrid(wxDC &dc);
   virtual void ToggleGrid(wxCommandEvent& event);
   virtual void DrawLegend(wxDC &dc);
   virtual void ToggleLegend(wxCommandEvent& event);

   virtual void SetOptions(wxCommandEvent& event);
   void SaveData(wxCommandEvent& event);

   void Zoom(const wxRect &region);
   void UnZoom();
   
   int GetXLocation(double val);
   int GetYLocation(double val);
   double GetActualXValue(int x);
   double GetActualYValue(int y);

   // Statics to let us set global properties
   static int  xSensitivity, ySensitivity;
   static int  defaultLineWidth;
   static bool defaultLabels;

   /// Rectangle with screen coordinates of the plot area
   wxRect  plotArea;
   /// Rectangle used when dragging the mouse
   wxRect  mouseRect;
   /// Legend
   wxRect  legendRect;
   /// Number of columns in the legend (0 to make it 1 row)
   int legendColumns;

//private:
   DECLARE_EVENT_TABLE()
};

#endif //_TSPLOTCANVAS_HPP_
