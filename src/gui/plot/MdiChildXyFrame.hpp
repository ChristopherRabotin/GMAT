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

//#include "XyPlotFrame.hpp"
//#include "Rvector.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

//#include "wx/plot/plot.h"
#include "XyPlotWindow.hpp" // for wxPlotWindow

class MdiChildXyFrame: public wxMDIChildFrame
{
public:
    wxPlotWindow   *mXyPlot;
    wxTextCtrl     *mLogTextCtrl;
        
    MdiChildXyFrame(wxMDIParentFrame *parent, const wxString &plotName,
                    const wxString& title, const wxString& xAxisTitle,
                    const wxString& yAxisTitle, const wxPoint& pos,
                    const wxSize& size, const long style);
    ~MdiChildXyFrame();

    // getter
    wxString GetPlotName() {return mPlotName;}
    wxString GetPlotTitle() {return mPlotTitle;}
    wxString GetXAxisTitle() {return mXAxisTitle;}
    wxString GetYAxisTitle() {return mYAxisTitle;}
    int GetCurveCount() {return mXyPlot->GetCount();}
    
    // setter
    void SetPlotName(const wxString &str) {mPlotName = str;}
    void SetPlotTitle(const wxString &str) {mPlotTitle = str;}
    void SetXAxisTitle(const wxString &str) {mXAxisTitle = str;}
    void SetYAxisTitle(const wxString &str) {mYAxisTitle = str;}
   
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

    void AddPlotCurve(int yOffset, double yMin, double yMax,
                      const wxString &curveTitle,
                      const wxString &penColorName);
    void AddDataPoints(int curveNum, double xData, double yData);
    void RedrawCurve();
    
    int ReadXyPlotFile(const wxString &filename);
    
protected:
    
    wxString mPlotName;
    wxString mPlotTitle;
    wxString mXAxisTitle;
    wxString mYAxisTitle;
    bool mHasFirstXSet;
    
    DECLARE_EVENT_TABLE()
};
#endif
