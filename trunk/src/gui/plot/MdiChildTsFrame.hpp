//$Header$
//------------------------------------------------------------------------------
//                              MdiChildTsFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Darrel Conway
// Created: 2005/05/05
/**
 * Declares MdiChildTsFrame class for ts plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildTsFrame_hpp
#define MdiChildTsFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "TsPlotXYCanvas.hpp"

// // For compilers that support precompilation, includes "wx/wx.h".
// #include "wx/wxprec.h"

// #ifdef __BORLANDC__
// #pragma hdrstop
// #endif

// #ifndef WX_PRECOMP
// #include "wx/wx.h"
// #include "wx/mdi.h"
// #endif

#include "gmatdefs.hpp"     // for UnsignedInt

//class MdiChildTsFrame: public wxMDIChildFrame
class MdiChildTsFrame: public GmatMdiChildFrame
{
public:
   TsPlotCanvas   *mXyPlot;
   wxTextCtrl     *mLogTextCtrl;
   wxMenu         *mViewOptionMenu;
   
   bool mIsMainFrame;
   
   MdiChildTsFrame(wxMDIParentFrame *parent, bool isMainFrame,
                   const wxString &plotName, const wxString& plotTitle,
                   const wxString& xAxisTitle, const wxString& yAxisTitle,
                   const wxPoint& pos, const wxSize& size, const long style);
   virtual ~MdiChildTsFrame();
   
   int  ReadXyPlotFile(const wxString &filename);
   bool DeletePlot();
   void SetPlotTitle(const wxString &title);
   void ShowPlotLegend();
   void AddPlotCurve(int curveIndex, int yOffset, double yMin,
                     double yMax, const wxString &curveTitle,
                     UnsignedInt penColor);
   void DeleteAllPlotCurves();
   void DeletePlotCurve(int curveIndex);
   void AddDataPoints(int curveIndex, double xData, double yData);
   void ClearPlotData();
   void PenUp();
   void PenDown();
   void RedrawCurve();
    
   // getter
   wxString GetPlotName() {return mPlotName;}
   wxString GetPlotTitle() {return mPlotTitle;}
   wxString GetXAxisTitle() {return mXAxisTitle;}
   wxString GetYAxisTitle() {return mYAxisTitle;}
   int GetCurveCount() {return mXyPlot->GetCurveCount();}
   //bool GetShowGrid() {return mXyPlot->GetShowGrid();}
   
   // setter
   void SetPlotName(const wxString &name);
   void SetXAxisTitle(const wxString &title) {mXAxisTitle = title;}
   void SetYAxisTitle(const wxString &title) {mYAxisTitle = title;}
   void SetShowGrid(bool show);
   void ResetZoom();
   
   // menu events
   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);
   void OnDrawGrid(wxCommandEvent& event);
   void OnDrawDottedLine(wxCommandEvent& event);
   void OnHelpView(wxCommandEvent& event);
   void OnQuit(wxCommandEvent& event);
   void OnOpenXyPlotFile(wxCommandEvent& event);

   // window events
   //void OnPlotClick(wxPlotEvent &event);
   void OnActivate(wxActivateEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
   void OnPlotClose(wxCloseEvent& event);
   void OnClose(wxCloseEvent& event);
   
   
protected:
   
   void AdjustYScale();
   double GetYMin();
   double GetYMax();
   
   static const int MAX_NUM_CURVE = 6;
    
   wxString mPlotName;
   wxString mPlotTitle;
   wxString mXAxisTitle;
   wxString mYAxisTitle;
   bool mHasFirstXSet[MAX_NUM_CURVE];
    
   DECLARE_EVENT_TABLE()
};
#endif
