//$Header$
//------------------------------------------------------------------------------
//                              MdiChildTrajFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares MdiChildTrajFrame for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildTrajFrame_hpp
#define MdiChildTrajFrame_hpp

#include "TrajPlotCanvas.hpp"
#include "gmatdefs.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class MdiChildTrajFrame: public wxMDIChildFrame
{
public:
   TrajPlotCanvas *mCanvas;
   wxMenu         *mViewOptionMenu;

   bool mIsMainFrame;
   
   MdiChildTrajFrame(wxMDIParentFrame *parent, bool isMainFrame,
                     const wxString& plotName, const wxString& title,
                     const wxPoint& pos, const wxSize& size,
                     const long style);
   ~MdiChildTrajFrame();

   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);
   void OnZoomIn(wxCommandEvent& event);
   void OnZoomOut(wxCommandEvent& event);
   void OnShowWireFrame(wxCommandEvent& event);
   void OnShowEquatorialPlane(wxCommandEvent& event);
   void OnHelpView(wxCommandEvent& event);
   void OnQuit(wxCommandEvent& event);
    
   void OnActivate(wxActivateEvent& event);
   void OnSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
   void OnClose(wxCloseEvent& event);
   
   void UpdateSpacecraft(const Real &time, const RealArray &posX,
                         const RealArray &posY, const RealArray &posZ,
                         const UnsignedIntArray &color,
                         bool updateCanvas);
   void RefreshPlot();
   void DeletePlot();

   // getter
   wxString GetPlotName() {return mPlotName;}
   bool GetDrawWireFrame() {return mDrawWireFrame;}
   bool GetOverlapPlot() {return mOverlapPlot;}
   
   // setter
   void SetPlotName(const wxString &str) {mPlotName = str;}
   void SetDrawWireFrame(bool flag);
   void SetOverlapPlot(bool overlap);

protected:
   wxString mPlotName;
   bool mDrawWireFrame;
   bool mOverlapPlot;
   
   DECLARE_EVENT_TABLE();
};
#endif
