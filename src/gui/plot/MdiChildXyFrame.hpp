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
 * Declares MdiChildXyFrame class for xy plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildXyFrame_hpp
#define MdiChildXyFrame_hpp

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "gmatdefs.hpp"     // for UnsignedInt
#include "XyPlotWindow.hpp" // for wxPlotWindow

class MdiChildXyFrame: public wxMDIChildFrame
{
public:
   wxPlotWindow   *mXyPlot;
   wxTextCtrl     *mLogTextCtrl;
   wxMenu         *mViewOptionMenu;
   
   bool mIsMainFrame;
        
   MdiChildXyFrame(wxMDIParentFrame *parent, bool isMainFrame,
                   const wxString &plotName, const wxString& plotTitle,
                   const wxString& xAxisTitle, const wxString& yAxisTitle,
                   const wxPoint& pos, const wxSize& size, const long style);
   ~MdiChildXyFrame();

   int  ReadXyPlotFile(const wxString &filename);
   bool DeletePlot();
   void SetPlotTitle(const wxString &title);
   void ShowPlotLegend(); //loj: 7/14/04 added
   void AddPlotCurve(int curveIndex, int yOffset, double yMin,
                     double yMax, const wxString &curveTitle,
                     UnsignedInt penColor); //loj: 7/13/04 changed from wxString penColor
   void DeleteAllPlotCurves();
   void DeletePlotCurve(int curveIndex);
   void AddDataPoints(int curveIndex, double xData, double yData);
   void ClearPlotData();
   void RedrawCurve();
    
   // getter
   wxString GetPlotName() {return mPlotName;}
   wxString GetPlotTitle() {return mPlotTitle;}
   wxString GetXAxisTitle() {return mXAxisTitle;}
   wxString GetYAxisTitle() {return mYAxisTitle;}
   int GetCurveCount() {return mXyPlot->GetCount();}
   bool GetShowGrid() {return mXyPlot->GetShowGrid();}
   
   // setter
   void SetPlotName(const wxString &str) {mPlotName = str;}
   void SetXAxisTitle(const wxString &str) {mXAxisTitle = str;}
   void SetYAxisTitle(const wxString &str) {mYAxisTitle = str;}
   void SetShowGrid(bool show);
   void ResetZoom(); //loj: 7/27/04 added
   
   // menu events
   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);
   void OnDrawGrid(wxCommandEvent& event);
   void OnDrawDottedLine(wxCommandEvent& event);
   void OnHelpView(wxCommandEvent& event);
   void OnQuit(wxCommandEvent& event);
   
   // window events
   void OnPlotClick(wxPlotEvent &event);
   void OnActivate(wxActivateEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
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
