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
                     const long style, const wxString &csName = "",
                     SolarSystem *solarSys = NULL);
   ~MdiChildTrajFrame();
   
   // getters
   wxString GetPlotName() {return mPlotName;}
   bool GetOverlapPlot() {return mOverlapPlot;}
   bool GetUseViewPointInfo();
   bool GetUsePerspectiveMode();
   bool GetDrawWireFrame();
   bool GetDrawEqPlane();
   bool GetDrawEcPlane();
   bool GetDrawEcLine();
   bool GetDrawAxes();
   bool GetRotateAboutXY();
   unsigned int GetEqPlaneColor();
   unsigned int GetEcPlaneColor();
   unsigned int GetEcLineColor();
   float GetDistance();
   int   GetGotoBodyId();
   wxString GetDesiredCoordSysName();
   CoordinateSystem* GetDesiredCoordSystem();
   
   // setters
   void SetPlotName(const wxString &name);
   void ResetShowViewOption();
   void SetOverlapPlot(bool overlap);
   void SetUseViewPointInfo(bool flag);
   void SetUsePerspectiveMode(bool flag);
   void SetDrawWireFrame(bool flag);
   void SetDrawEqPlane(bool flag);
   void SetDrawEcPlane(bool flag);
   void SetDrawEcLine(bool flag);
   void SetDrawAxes(bool flag);
   void SetRotateAboutXY(bool flag);
   void SetEqPlaneColor(unsigned int color);
   void SetEcPlaneColor(unsigned int color);
   void SetEcLineColor(unsigned int color);
   void SetDistance(float dist);
   void SetGotoBodyName(const wxString &bodyName);
   void SetDesiredCoordSystem(const wxString &csName);
   void SetDesiredCoordSystem(CoordinateSystem *cs);
   
   // actions
   void DrawInOtherCoordSystem(const wxString &csName);
   void DrawInOtherCoordSystem(CoordinateSystem *cs);
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
   void OnTrajSize(wxSizeEvent& event);
   void OnMove(wxMoveEvent& event);
   void OnClose(wxCloseEvent& event);
   
   void SetGlViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                        SpacePoint *vdObj, Real vsFactor,
                        const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                        const Rvector3 &vdVec, bool usevpRefVec,
                        bool usevpVec, bool usevdVec);
   
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
