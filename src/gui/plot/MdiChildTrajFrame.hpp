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
#include "OpenGlOptionDialog.hpp"

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"

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
                     const long style, SolarSystem *solarSystem = NULL);
   ~MdiChildTrajFrame();

   // getters
   wxString GetPlotName() {return mPlotName;}
   bool GetOverlapPlot() {return mOverlapPlot;}
   bool GetDrawWireFrame();
   bool GetDrawEqPlane();
   bool GetDrawEcPlane();
   bool GetDrawEcLine();
   unsigned int GetEqPlaneColor();
   unsigned int GetEcPlaneColor();
   unsigned int GetEcLineColor();
   float GetDistance();
   int   GetGotoBodyId();
   
   // setters
   void SetPlotName(const wxString &name);
   void SetOverlapPlot(bool overlap);
   void SetDrawWireFrame(bool flag);
   void SetDrawEqPlane(bool flag);
   void SetDrawEcPlane(bool flag);
   void SetDrawEcLine(bool flag);
   void SetEqPlaneColor(unsigned int color);
   void SetEcPlaneColor(unsigned int color);
   void SetEcLineColor(unsigned int color);
   void SetDistance(float dist);
   void SetGotoBodyName(const wxString &bodyName);
   void SetCoordSystem(CoordinateSystem *cs); //loj: 1/28/05 Added

   // actions
   void UpdatePlot();
   
   void OnClearPlot(wxCommandEvent& event);
   void OnChangeTitle(wxCommandEvent& event);
   void OnShowDefaultView(wxCommandEvent& event);
   void OnZoomIn(wxCommandEvent& event);
   void OnZoomOut(wxCommandEvent& event);
   
   void OnShowOptionDialog(wxCommandEvent& event);
   void OnDrawWireFrame(wxCommandEvent& event);
   void OnDrawEqPlane(wxCommandEvent& event);
   
   void OnAddBody(wxCommandEvent& event);
   void OnGotoStdBody(wxCommandEvent& event);
   void OnGotoOtherBody(wxCommandEvent& event);
   
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

protected:

   OpenGlOptionDialog *mOptionDialog;
   
   wxString mPlotName;
   wxString mPlotTitle;
   bool mOverlapPlot;

   wxArrayString mBodyNames;
   UnsignedIntArray mBodyColors;

   wxMenu *mViewOptionMenu;
   wxMenu *mViewMenu;
   
   wxMenu* CreateGotoBodyMenu();
   int GetMenuId(const wxString &body);
   
   DECLARE_EVENT_TABLE();
};
#endif
