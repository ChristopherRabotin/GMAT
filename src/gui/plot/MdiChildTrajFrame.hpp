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
   
   //loj: 5/6/04 added drawWireFrame
   void UpdateSpacecraft(const Real &time, const Real &posX,
                         const Real &posY, const Real &posZ,
                         const UnsignedInt orbitColor,
                         const UnsignedInt targetColor,
                         bool updateCanvas, bool drawWireFrame = false);
   bool DeletePlot();

   // getter
   wxString GetPlotName() {return mPlotName;}
   
   // setter
   void SetPlotName(const wxString &str) {mPlotName = str;}

protected:
   wxString mPlotName;

   DECLARE_EVENT_TABLE();
};
#endif
